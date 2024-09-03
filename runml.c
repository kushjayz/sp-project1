#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LINE_LEN 256
#define ASSIGNMENT_OPERATOR "<-"
#define COMMENT '#'
#define MAX_TOKENS 100

// Creating a set of enums to indentify tokens
typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_REALCONSTANT,
    TOKEN_PRINT,
    TOKEN_FUNCTION,
    TOKEN_ASSIGNMENT,
    TOKEN_OPERATOR,
    TOKEN_PARENTHESIS,
    TOKEN_END,
    TOKEN_UNKNOWN,
} TokenType;

typedef struct {
    char value[256];
    TokenType type;
} Token;

Token get_next_token(char **line);
void remove_comment(char* line);
void process_line(char* line);
bool is_parsed_program(char *line);
bool is_parsed_program_item(char *line);
bool parsed_statement(char *line);
bool parsed_function(char *line);

int main(int argc, char* argv[]) {
    char line[MAX_LINE_LEN];
    // Checking if the arguement is provided
    if(argc != 2) {
		printf("Error! File not provided!\n");
		printf("Usage: %s <program.ml> \n", argv[0]);
		return 1;
    }

    FILE* file = fopen(argv[1], "r");
    
    // If file is not opened successfully
    if (file == NULL) {
        printf("Could not open file %s.\n", argv[1]);
        return EXIT_FAILURE;
    }
    
    // Processing line by line
    while (fgets(line, sizeof(line), file)) {
        process_line(line);
    }
    
    fclose(file);
    return EXIT_SUCCESS;
}

Token get_next_token(char **line) {
    Token token;
    char *start = *line;
    char ch = *start;

    // Ignoring whitespace
    while (isspace(ch)) {
        start++;
        // Updating the starting
        ch = *start;
    }

    if (ch == '\0') {
        token.type = TOKEN_END;
        token.value[0] = '\0';
        return token;
    }

    if (islower(ch)) { // Identifier or Keyword
        char *end = start;
        while (islower(*end)) end++;
        size_t len = end - start;
        strncpy(token.value, start, len);
        if (strcmp(token.value, "print") == 0) {
            token.type = TOKEN_PRINT;
        } else if (strcmp(token.value, "function") == 0) {
                token.type = TOKEN_FUNCTION;
        } else {
                token.type = TOKEN_IDENTIFIER;
        }
        token.value[len] = '\0';
        *line = end;
        return token;
    }

    if (isdigit(ch) || (ch == '.' && isdigit(start[1]))) { // Real constant
        char *end = start;
        while (isdigit(*end) || *end == '.') end++;
        size_t len = end - start;
        strncpy(token.value, start, len);
        token.value[len] = '\0';
        token.type = TOKEN_REALCONSTANT;
        *line = end;
        return token;
    }

    if (ch == '+' || ch == '-' || ch == '*' || ch == '/') { // Operators
        token.value[0] = ch;
        token.value[1] = '\0';
        token.type = TOKEN_OPERATOR;
        *line = start + 1;
        return token;
    }

    if (ch == '(' || ch == ')') { // Parentheses
        token.value[0] = ch;
        token.value[1] = '\0';
        token.type = TOKEN_PARENTHESIS;
        *line = start + 1;
        return token;
    }

    // Handing default unknown tokens
    token.type = TOKEN_UNKNOWN;
    token.value[0] = ch;
    token.value[1] = '\0';
    *line = start + 1;
    return token;
}

// Removing comments
void remove_comment(char* line) {
    char* comment = strchr(line, COMMENT);
    if (comment != NULL) {
        *comment = '\0';
    }
}

// Function which is used to process lines in the overall program
void process_line(char* line) {
    remove_comment(line);
    printf("Processing line: %s", line);
    Token tokens[MAX_TOKENS];
    int token_count = 0;
    char *line_ptr = line;

    // Tokenize the line
    while (token_count < MAX_TOKENS) {
        Token token = get_next_token(&line_ptr);
        if (token.type == TOKEN_END) break;
        tokens[token_count++] = token;
    }

    // Print tokens
    for (int i = 0; i < token_count; i++) {
        printf("Token %d: %s (Type: %d)\n", i, tokens[i].value, tokens[i].type);
    }
    is_parsed_program(line);
}

bool is_parsed_program(char *line) {
    return is_parsed_program_item(line);
}

bool is_parsed_program_item(char *line) {
    return parsed_statement(line) || parsed_function(line);
}

bool parsed_statement(char *line) {
    return true;
}

bool parsed_function(char *line) {
    return true;
}