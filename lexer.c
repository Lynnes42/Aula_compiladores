#include "lexer.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    const char* source;
    int pos;
    int line;
    int column;
} Lexer;

static void add_token(TokenList* list, const char* type, const char* value, int line, int column) {
    if (list->size >= list->capacity) {
        list->capacity = list->capacity == 0 ? 10 : list->capacity * 2;
        list->tokens = realloc(list->tokens, sizeof(Token) * list->capacity);
    }
    list->tokens[list->size].type = strdup(type);
    list->tokens[list->size].value = strdup(value);
    list->tokens[list->size].line = line;
    list->tokens[list->size].column = column;
    list->size++;
}

static const char* KEYWORDS[] = {
    "programa", "inicio", "fim", "inteiro", "flutuante",
    "se", "entao", "senao", "fimse", "para",
    "de", "ate", "passo", "faca", "fimpara",
    "enquanto", "fimenquanto", "e", "ou", "nao",
    "leia", "escreva", "escreval", "procedimento", "retorna",
    "vazio", "div", NULL
};

static int is_keyword(const char* word) {
    for (int i = 0; KEYWORDS[i] != NULL; i++) {
        if (strcmp(word, KEYWORDS[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static void read_word(Lexer* lexer, TokenList* list) {
    int start_pos = lexer->pos;
    int start_column = lexer->column;
    
    while (isalnum(lexer->source[lexer->pos]) || lexer->source[lexer->pos] == '_') {
        lexer->pos++;
        lexer->column++;
    }
    
    int length = lexer->pos - start_pos;
    char* word = malloc(length + 1);
    strncpy(word, lexer->source + start_pos, length);
    word[length] = '\0';
    
    if (is_keyword(word)) {
        add_token(list, "KEYWORD", word, lexer->line, start_column);
    } else {
        add_token(list, "IDENTIFIER", word, lexer->line, start_column);
    }
    free(word);
}

static void read_number(Lexer* lexer, TokenList* list) {
    int start_pos = lexer->pos;
    int start_column = lexer->column;
    int is_float = 0;
    
    while (isdigit(lexer->source[lexer->pos])) {
        lexer->pos++;
        lexer->column++;
    }
    
    if (lexer->source[lexer->pos] == '.') {
        if (isdigit(lexer->source[lexer->pos + 1])) {
            is_float = 1;
            lexer->pos++;
            lexer->column++;
            while (isdigit(lexer->source[lexer->pos])) {
                lexer->pos++;
                lexer->column++;
            }
        }
    }
    
    if (lexer->source[lexer->pos] == 'e' || lexer->source[lexer->pos] == 'E') {
        is_float = 1;
        lexer->pos++;
        lexer->column++;
        if (lexer->source[lexer->pos] == '+' || lexer->source[lexer->pos] == '-') {
            lexer->pos++;
            lexer->column++;
        }
        while (isdigit(lexer->source[lexer->pos])) {
            lexer->pos++;
            lexer->column++;
        }
    }
    
    int length = lexer->pos - start_pos;
    char* value = malloc(length + 1);
    strncpy(value, lexer->source + start_pos, length);
    value[length] = '\0';
    
    if (is_float) {
        add_token(list, "FLOAT", value, lexer->line, start_column);
    } else {
        add_token(list, "INTEGER", value, lexer->line, start_column);
    }
    free(value);
}

static void read_string(Lexer* lexer, TokenList* list) {
    int start_column = lexer->column;
    lexer->pos++; // skip opening quote
    lexer->column++;
    
    int capacity = 10;
    char* value = malloc(capacity);
    int length = 0;
    
    while (lexer->source[lexer->pos] != '\0' && lexer->source[lexer->pos] != '"' && lexer->source[lexer->pos] != '\n') {
        if (lexer->source[lexer->pos] == '\\') {
            lexer->pos++;
            lexer->column++;
            if (lexer->source[lexer->pos] == '\0') break;
            
            char escaped = lexer->source[lexer->pos];
            if (escaped == 'n') escaped = '\n';
            else if (escaped == 't') escaped = '\t';
            else if (escaped == '"') escaped = '"';
            else if (escaped == '\\') escaped = '\\';
            
            if (length + 1 >= capacity) {
                capacity *= 2;
                value = realloc(value, capacity);
            }
            value[length++] = escaped;
        } else {
            if (length + 1 >= capacity) {
                capacity *= 2;
                value = realloc(value, capacity);
            }
            value[length++] = lexer->source[lexer->pos];
        }
        lexer->pos++;
        lexer->column++;
    }
    
    if (lexer->source[lexer->pos] == '"') {
        lexer->pos++;
        lexer->column++;
        value[length] = '\0';
        add_token(list, "STRING", value, lexer->line, start_column);
    } else {
        fprintf(stderr, "Error: Unterminated string at line %d, column %d\n", lexer->line, start_column);
    }
    free(value);
}

static void read_operator(Lexer* lexer, TokenList* list) {
    char c = lexer->source[lexer->pos];
    char next = lexer->source[lexer->pos + 1];
    char op[3] = {0};
    int start_column = lexer->column;
    
    if ((c == '=' && next == '=') || (c == '!' && next == '=') ||
        (c == '<' && next == '=') || (c == '>' && next == '=') ||
        (c == '&' && next == '&') || (c == '|' && next == '|')) {
        op[0] = c;
        op[1] = next;
        add_token(list, "OPERATOR", op, lexer->line, start_column);
        lexer->pos += 2;
        lexer->column += 2;
        return;
    }
    
    if (strchr("+-*/=", c)) {
        op[0] = c;
        add_token(list, "OPERATOR", op, lexer->line, start_column);
        lexer->pos++;
        lexer->column++;
        return;
    }

    if (strchr("<>", c)) {
        op[0] = c;
        add_token(list, "OPERATOR", op, lexer->line, start_column);
        lexer->pos++;
        lexer->column++;
        return;
    }
    
    if (strchr("(){}[],;", c)) {
        op[0] = c;
        add_token(list, "DELIMITER", op, lexer->line, start_column);
        lexer->pos++;
        lexer->column++;
        return;
    }
    
    fprintf(stderr, "Error: Unexpected character '%c' at line %d, column %d\n", c, lexer->line, start_column);
    lexer->pos++;
    lexer->column++;
}

static void skip_ignorable(Lexer* lexer) {
    while (lexer->source[lexer->pos] != '\0') {
        char c = lexer->source[lexer->pos];
        if (isspace(c)) {
            if (c == '\n') {
                lexer->line++;
                lexer->column = 1;
            } else {
                lexer->column++;
            }
            lexer->pos++;
        } else if (c == '/' && lexer->source[lexer->pos + 1] == '/') {
            // Single-line comment
            while (lexer->source[lexer->pos] != '\0' && lexer->source[lexer->pos] != '\n') {
                lexer->pos++;
                lexer->column++;
            }
        } else if (c == '/' && lexer->source[lexer->pos + 1] == '*') {
            // Multi-line comment
            int start_line = lexer->line;
            int start_column = lexer->column;
            lexer->pos += 2;
            lexer->column += 2;
            while (lexer->source[lexer->pos] != '\0' && 
                   !(lexer->source[lexer->pos] == '*' && lexer->source[lexer->pos+1] == '/')) {
                if (lexer->source[lexer->pos] == '\n') {
                    lexer->line++;
                    lexer->column = 1;
                } else {
                    lexer->column++;
                }
                lexer->pos++;
            }
            if (lexer->source[lexer->pos] == '\0') {
                fprintf(stderr, "Error: Unterminated comment starting at line %d, column %d\n", start_line, start_column);
                return;
            }
            lexer->pos += 2;
            lexer->column += 2;
        } else {
            break;
        }
    }
}

TokenList* tokenize(const char* source) {
    Lexer lexer = {source, 0, 1, 1};
    TokenList* list = malloc(sizeof(TokenList));
    list->tokens = NULL;
    list->size = 0;
    list->capacity = 0;
    
    while (1) {
        skip_ignorable(&lexer);
        
        if (lexer.source[lexer.pos] == '\0') {
            break;
        }
        
        char c = lexer.source[lexer.pos];
        
        if (isalpha(c) || c == '_') {
            read_word(&lexer, list);
        } else if (isdigit(c)) {
            read_number(&lexer, list);
        } else if (c == '"') {
            read_string(&lexer, list);
        } else {
            read_operator(&lexer, list);
        }
    }
    
    add_token(list, "EOF", "EOF", lexer.line, lexer.column);
    return list;
}

void free_token_list(TokenList* list) {
    if (!list) return;
    for (int i = 0; i < list->size; i++) {
        free(list->tokens[i].type);
        free(list->tokens[i].value);
    }
    free(list->tokens);
    free(list);
}
