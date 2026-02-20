#include "lexer.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

void print_tokens(TokenList* list) {
    for (int i = 0; i < list->size; i++) {
        printf("Token: type=%s, value='%s', line=%d, col=%d\n",
               list->tokens[i].type, list->tokens[i].value,
               list->tokens[i].line, list->tokens[i].column);
    }
}

void test_basic() {
    const char* source = "programa teste\ninicio\n  inteiro x = 10;\n  flutuante y = 3.14;\n  escreval(\"Olá mundo\");\nfim";
    TokenList* list = tokenize(source);
    assert(list != NULL);

    assert(strcmp(list->tokens[0].type, "KEYWORD") == 0);
    assert(strcmp(list->tokens[0].value, "programa") == 0);

    assert(strcmp(list->tokens[1].type, "IDENTIFIER") == 0);
    assert(strcmp(list->tokens[1].value, "teste") == 0);

    assert(strcmp(list->tokens[2].type, "KEYWORD") == 0);
    assert(strcmp(list->tokens[2].value, "inicio") == 0);

    assert(strcmp(list->tokens[11].type, "FLOAT") == 0);
    assert(strcmp(list->tokens[11].value, "3.14") == 0);

    assert(strcmp(list->tokens[15].type, "STRING") == 0);
    assert(strcmp(list->tokens[15].value, "Olá mundo") == 0);

    assert(strcmp(list->tokens[list->size-1].type, "EOF") == 0);

    free_token_list(list);
    printf("test_basic passed\n");
}

void test_scientific_notation() {
    const char* source = "1.2e-5 10e3 0.5E+2";
    TokenList* list = tokenize(source);
    assert(list->size == 4); // 3 numbers + EOF

    assert(strcmp(list->tokens[0].type, "FLOAT") == 0);
    assert(strcmp(list->tokens[0].value, "1.2e-5") == 0);

    assert(strcmp(list->tokens[1].type, "FLOAT") == 0);
    assert(strcmp(list->tokens[1].value, "10e3") == 0);

    assert(strcmp(list->tokens[2].type, "FLOAT") == 0);
    assert(strcmp(list->tokens[2].value, "0.5E+2") == 0);

    free_token_list(list);
    printf("test_scientific_notation passed\n");
}

void test_comments() {
    const char* source = "// line comment\nprograma /* multi\nline */ fim";
    TokenList* list = tokenize(source);
    // Tokens: programa, fim, EOF
    assert(list->size == 3);
    assert(strcmp(list->tokens[0].value, "programa") == 0);
    assert(list->tokens[0].line == 2);
    assert(strcmp(list->tokens[1].value, "fim") == 0);
    assert(list->tokens[1].line == 3);

    free_token_list(list);
    printf("test_comments passed\n");
}

void test_operators() {
    const char* source = "== != <= >= && || + - * / = ( ) { } [ ] , ; > <";
    TokenList* list = tokenize(source);
    // 21 operators/delimiters + EOF = 22
    assert(list->size == 22);
    free_token_list(list);
    printf("test_operators passed\n");
}

void test_div() {
    const char* source = "x div y";
    TokenList* list = tokenize(source);
    assert(list->size == 4);
    assert(strcmp(list->tokens[1].type, "KEYWORD") == 0);
    assert(strcmp(list->tokens[1].value, "div") == 0);
    free_token_list(list);
    printf("test_div passed\n");
}

void test_string_escapes() {
    const char* source = "\"quote: \\\" , newline: \\n\"";
    TokenList* list = tokenize(source);
    assert(list->size == 2);
    assert(strcmp(list->tokens[0].type, "STRING") == 0);
    assert(strcmp(list->tokens[0].value, "quote: \" , newline: \n") == 0);
    free_token_list(list);
    printf("test_string_escapes passed\n");
}

void test_keyword_vs_id() {
    const char* source = "inteiro inteiro_var";
    TokenList* list = tokenize(source);
    assert(list->size == 3);
    assert(strcmp(list->tokens[0].type, "KEYWORD") == 0);
    assert(strcmp(list->tokens[0].value, "inteiro") == 0);
    assert(strcmp(list->tokens[1].type, "IDENTIFIER") == 0);
    assert(strcmp(list->tokens[1].value, "inteiro_var") == 0);
    free_token_list(list);
    printf("test_keyword_vs_id passed\n");
}

void test_errors() {
    printf("--- Testing error cases (errors should be printed below) ---\n");
    const char* s1 = "\"unterminated";
    TokenList* l1 = tokenize(s1);
    free_token_list(l1);

    const char* s2 = "/* unterminated";
    TokenList* l2 = tokenize(s2);
    free_token_list(l2);

    const char* s3 = "@";
    TokenList* l3 = tokenize(s3);
    free_token_list(l3);
    printf("--- End of error cases ---\n");
}

int main() {
    test_basic();
    test_scientific_notation();
    test_comments();
    test_operators();
    test_div();
    test_string_escapes();
    test_keyword_vs_id();
    test_errors();
    printf("All tests passed!\n");
    return 0;
}
