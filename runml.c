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
    TOKEN_RETURN,
    TOKEN_ASSIGNMENT,
    TOKEN_OPERATOR,
    TOKEN_DIVIDE,
    TOKEN_MULTIPLY,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_OPEN_PARENTHESIS,
    TOKEN_CLOSED_PARENTHESIS,
    TOKEN_END,
    TOKEN_TAB,
    TOKEN_COMMA,
    TOKEN_UNKNOWN,
} TokenType;

// A struct type to store the tokentype and the value of that token
typedef struct {
    char value[256];
    TokenType type;
} Token;

Token get_next_token(char **line);
void remove_comment(char* line);
void process_line(char* line, FILE* file, bool is_body_lines);
bool is_parse_assignment(Token** tokens);
bool is_parse_factor(Token** tokens);
bool is_parse_term(Token** tokens);
bool is_parse_expression(Token** tokens);
bool is_bracketed_expression(Token** tokens);
bool is_parse_statement(Token** tokens);
bool is_parsed_program_item(Token** tokens, FILE* file);
bool is_parsed_program(Token** tokens, FILE* file);
bool is_parsed_body(Token** tokens, FILE* file);
bool is_parse_function_definition(Token** tokens, FILE* file);
bool is_parse_function_call(Token** tokens);
void report_error(const char *message);
void printToken(Token** token, char* funcName);

Token funcTokens[MAX_TOKENS];
int funcCount = 0;
int main(int argc, char* argv[]) {
    char line[MAX_LINE_LEN];
    // char next_line[MAX_LINE_LEN];
    // Checking if the arguement is provided
    if(argc != 2) {
        // printf("!Usage: %s <program.ml> \n", argv[0]);
        report_error("!Error! File not provided!\n");
    }

    FILE* file = fopen(argv[1], "r");
    
    // If file is not opened successfully
    if (file == NULL) {
        report_error("!Error: Could not open file provided!\n");
    }
    
    // Processing line by line
    while (fgets(line, sizeof(line), file)) {
        process_line(line, file, false);
    }
    
    fclose(file);
    return EXIT_SUCCESS;
}

bool is_custom_space(char c) {
    return (c == ' ' || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}


Token get_next_token(char **line) {
    Token token;
    char *start = *line;
    char ch = *start;

    // Ignoring whitespace except \t
    while (is_custom_space(ch)) {
        start++;
        // Updating the starting
        ch = *start;
    }

    if(ch == '\t') {
        token.type = TOKEN_TAB;
        token.value[0] = '\t';
        *line = start + 1;
        return token;
    }

    if (ch == '\0') {
        token.type = TOKEN_END;
        token.value[0] = '\0';
        return token;
    }

    if (ch == ',') {
        token.type = TOKEN_COMMA;
        token.value[0] = ',';
        token.value[1] = '\0';
        *line = start + 1;
        return token;
    }

    if (ch == '<') {
        char *end = start + 1;
        if (*end == '-') {
            size_t len = 2; // Length is 2 for "<-"
            strncpy(token.value, start, len); 
            token.value[len] = '\0'; 
            token.type = TOKEN_ASSIGNMENT;
            *line = end + 1; // Move the line pointer past the "<-"
            return token;
        }
    }


    if (islower(ch)) { // Identifier or Keyword
        char *end = start;
        while (islower(*end)) end++;
        size_t len = end - start;
        strncpy(token.value, start, len);
        if (strcmp(token.value, "print") == 0) {
            token.type = TOKEN_PRINT;
        } else if (strcmp(token.value, "function") == 0) {
            // check for space requirement after 'function'
            if(isspace(*end)) {
                token.type = TOKEN_FUNCTION;
                end++;
            } else {
                token.type = TOKEN_UNKNOWN;
            }
        } else if (strcmp(token.value, "return") == 0) {
                token.type = TOKEN_RETURN;
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

    switch(ch) {
        case '+': 
            token.type = TOKEN_PLUS;
            break;
        case '-':
            token.type = TOKEN_MINUS;
            break;
        case '*':
            token.type = TOKEN_MULTIPLY;
            break;
        case '/':
            token.type = TOKEN_DIVIDE;
            break;
        case '(':
            token.type = TOKEN_OPEN_PARENTHESIS;
            break;
        case ')':
            token.type = TOKEN_CLOSED_PARENTHESIS;
            break;
        default:
            token.type = TOKEN_UNKNOWN;
            break;
    }
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
void process_line(char* line, FILE* file, bool is_body_lines) {
    remove_comment(line);

    if(strlen(line) > 0) {
        Token tokens[MAX_TOKENS];
        Token* token_ptr = tokens;
        int token_count = 0;
        
        char *line_ptr = line;

        // Tokenize the line
        while (token_count < MAX_TOKENS) {
            Token token = get_next_token(&line_ptr);
            tokens[token_count++] = token;
            if (token.type == TOKEN_END) break;
        }

        if(is_body_lines){
            // // Print tokens
            for (int i = 0; i < token_count; i++) {
                printf("Token in body %d: %s (Type: %d)\n", i, tokens[i].value, tokens[i].type);
            }
        } else {
            // Print tokens
            // for (int i = 0; i < token_count; i++) {
            //     printf("Token in line %d: %s (Type: %d)\n", i, tokens[i].value, tokens[i].type);
            // }
        }

        if(is_body_lines){
            is_parsed_body(&token_ptr, file);
            return;
        }

        if(is_parsed_program(&token_ptr, file)) {
            printf("correct syntax\n");
        } else {
            printf("incorrect syntax\n");
            // report_error("Error Caused - Syntax Error\n");
        }
    }
}

bool is_parsed_body(Token** tokens, FILE* file) {
    if ((*tokens)->type == TOKEN_TAB) {
        (*tokens)++;
        while ((*tokens)->type != TOKEN_END) {
            if (!is_parse_statement(tokens)) {
                printf("Not a valid function body\n");
                return false;
            }
        }
    }
    return true;
}

bool is_parsed_program(Token** tokens, FILE* file) {
    while ((*tokens)->type != TOKEN_END) {
        if (!is_parsed_program_item(tokens, file)) {
            printf("Not a valid program\n");
            return false;
        }
    }
    return true;
}

bool is_parsed_program_item(Token** tokens, FILE* file) {
    if (is_parse_function_definition(tokens, file)) {
        return true;
    } else if (is_parse_statement(tokens)) {
        return true;
    }
    printf("Not a valid program item\n");
    return false; // No valid program item found
}

bool is_parse_function_definition(Token** tokens, FILE* file) {

    int no_statements = 0;
    char body_lines[256];
    // Check for 'function' keyword
    if ((*tokens)->type != TOKEN_FUNCTION) {
        return false;
    }
    // Consume the 'function' keyword
    (*tokens)++;

    // Check for the function name (identifier)
    if ((*tokens)->type != TOKEN_IDENTIFIER) {
        printf("Expected function name after 'function'\n");
        return false;
    } else {
        // Consume the function name (identifier)
        funcTokens[funcCount] = **tokens;
        funcCount++;
        (*tokens)++;
    }

    // Parse zero or more function parameters (identifiers)
    while ((*tokens)->type == TOKEN_IDENTIFIER) {
        // Consume each identifier
        (*tokens)++;
    }


    while (fgets(body_lines, sizeof(body_lines), file) && body_lines[0] == '\t') {
        process_line(body_lines, file, true);
        no_statements++;
    }

    if(no_statements > 0) {
        return true;  // Successfully parsed a function definition
    } else {
        return false;
    }
}

bool is_parse_statement(Token** tokens) {
    if ((*tokens)->type == TOKEN_PRINT || (*tokens)->type == TOKEN_RETURN) {
            (*tokens)++;
            if(is_parse_expression(tokens)) {
                return true;
            } else {
                printf("Not a valid expression\n");
                return false;
            }
    } else if (is_parse_assignment(tokens)) {
        return true;
    } else if (is_parse_function_call(tokens)) {
        return true;
    } else {
        printf("Not a valid statement\n");
        return false;
    }
    // else if ((*tokens)->type == TOKEN_IDENTIFIER && (*(tokens + 1))->type == TOKEN_PARENTHESIS && (*(tokens + 1))->value[0] == '(') {
    //     return is_parse_functioncall(tokens);
    // }
}

bool is_parse_function_call(Token** tokens) {
    if((*tokens)->type == TOKEN_IDENTIFIER) {
        (*tokens)++;
        if((*tokens)->type == TOKEN_OPEN_PARENTHESIS) {
            (*tokens)++;
            if (!is_parse_expression(tokens)) {
                return false;
            }
            while ((*tokens)->type == TOKEN_COMMA) {
                (*tokens)++; // Consume ","
                if (!is_parse_expression(tokens)) {
                    printf("Invalid function call\n");
                    return false; // If no valid expression after the comma, return false
                }
            }

            if((*tokens)->type == TOKEN_CLOSED_PARENTHESIS) {
                (*tokens)++;
                return true;
            } else {
                printf("Invalid function call\n");
                return false;
            }
        } else {
            printf("Invalid function call\n");
            return false;
        }
    } else {
        printf("Invalid function call\n");
        return false;
    }
}


bool is_parse_expression(Token** tokens) {
    // printToken(tokens, "is_parse_expression");
    if (!is_parse_term(tokens)) {
        printf("Is not a termed expression\n");
        return false;
    }

    // Look for "+" or "-" operator
    while ((*tokens)->type == TOKEN_PLUS || (*tokens)->type == TOKEN_MINUS) {
        // Consume the "+" or "-" operator
        (*tokens)++;
        
        // Parse the next expression after the operator
        if (!is_parse_term(tokens)) {
            printf("Expected a term after '+' or '-'\n");
            return false;
        }
    }

    return true;  // Successfully parsed the expression
}

bool is_parse_term(Token** tokens) {
    printToken(tokens, "is_parse_term");
    if (!is_parse_factor(tokens)) {
        return false;
    }

    // Look for "*" or "/" operator
    while ((*tokens)->type == TOKEN_MULTIPLY || (*tokens)->type == TOKEN_DIVIDE) {
        // Consume the "*" or "/" operator
        (*tokens)++;
        
        // Parse the next term after the operator
        if (!is_parse_factor(tokens)) {
            printf("Expected a factor after '*' or '/'\n");
            return false;
        }
    }

    return true;  // Successfully parsed the term
}

bool is_parse_factor(Token** tokens) {
    printToken(tokens,"is_parse_factor");
    if (
        (*tokens)->type == TOKEN_REALCONSTANT ||
        (*tokens)->type == TOKEN_IDENTIFIER
    ) {
        (*tokens)++;
        return true;
    } else if(is_parse_function_call(tokens)) {
        return true;
    } else if(is_bracketed_expression(tokens)) {
        return true;
    } else {
        printf("Not a valid facor\n");
        return false;
    }
}

bool is_bracketed_expression(Token** tokens) {
    // printToken(tokens,"is_bracketed_expression");
    if ((*tokens)->type == TOKEN_OPEN_PARENTHESIS) {
        (*tokens)++;
        if(is_parse_expression(tokens)){
            (*tokens)++;
            if ((*tokens)->type == TOKEN_OPEN_PARENTHESIS){
                (*tokens)++;
                return true;
            }
        }
    }
    return false;
}

bool is_parse_assignment(Token** tokens) {
    // printToken(tokens, "parseAssignment1");
    if ((*tokens)->type == TOKEN_IDENTIFIER) {
        (*tokens)++; // Consume identifier
        if ((*tokens)->type == TOKEN_ASSIGNMENT) {
            (*tokens)++; // Consume "<-"
                // printToken(tokens, "parseAssignment3");
            if (is_parse_expression(tokens)) {
                    // printToken(tokens, "parseAssignment4");
                return true;
            }
        }
    } else {
        // printToken(tokens, "parseAssignment");
        (*tokens)--;
        printf("Not a valid assignment\n");
        return false; // Not a valid assignment
    }
    printf("Not a valid assignment\n");
    (*tokens)--;
    return false;
}

void report_error(const char *message) {
    fprintf(stderr, "! %s\n", message);
    exit(EXIT_FAILURE); 
}

void printToken(Token** token, char* funcName) {
    printf("%s: %d, %s\n", funcName, (*token)->type, (*token)->value);
}