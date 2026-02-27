#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

char* read_file_content(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = malloc(length + 1);
    if (content) {
        fread(content, 1, length, file);
        content[length] = '\0';
    }
    
    fclose(file);
    return content;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return 1;
    }
    
    char* source = read_file_content(argv[1]);
    if (!source) {
        return 1;
    }
    
    TokenList* list = tokenize(source);
    if (list) {
        for (int i = 0; i < list->size; i++) {
            printf("[%d:%d] %s: '%s'\n", 
                   list->tokens[i].line, 
                   list->tokens[i].column, 
                   list->tokens[i].type, 
                   list->tokens[i].value);
        }
        free_token_list(list);
    }
    
    free(source);
    return 0;
}
