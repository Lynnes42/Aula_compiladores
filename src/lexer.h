#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>

typedef struct {
    char* type;    // e.g., "KEYWORD", "IDENTIFIER", "INTEGER", "FLOAT", "STRING"
    char* value;   // The actual lexeme (e.g., "while", "3.14")
    int line;      // Line number for debugging
    int column;    // Column number for debugging
} Token;

typedef struct {
    Token* tokens;
    int size;
    int capacity;
} TokenList;

/**
 * Tokenizes the input source string.
 * Returns a TokenList containing all recognized tokens, ending with an EOF token.
 * In case of critical failure, might return NULL or a list containing ERROR tokens.
 */
TokenList* tokenize(const char* source);

/**
 * Frees all memory associated with a TokenList, including the tokens themselves.
 */
void free_token_list(TokenList* list);

#endif // LEXER_H
