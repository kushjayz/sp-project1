//  CITS2002 Project 1 2024
//  Student1:   24205163   Kushan Sanka Jayasekera
//  Student2:   24297797   Gayathri Kasunthika Kanakaratne
//  Platform:   Linux

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LINE_LEN 256
#define ASSIGNMENT_OPERATOR "<-"
#define COMMENT '#'
#define MAX_TOKENS 100
#define MAX_VARIABLES 10

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
void process_line(char* line, FILE* r_file, FILE* w_file, bool is_body_lines);
bool is_parse_assignment(Token** tokens, FILE* w_file);
bool is_parse_factor(Token** tokens, FILE* w_file);
bool is_parse_term(Token** tokens, FILE* w_file);
bool is_parse_expression(Token** tokens, FILE* w_file);
bool is_bracketed_expression(Token** tokens, FILE* w_file);
bool is_parse_statement(Token** tokens, FILE* w_file);
bool is_parsed_program_item(Token** tokens, FILE* r_file, FILE* w_file);
bool is_parsed_program(Token** tokens, FILE* r_file, FILE* w_file);
bool is_parsed_body(Token** tokens, FILE* r_file, FILE* w_file);
bool is_parse_function_definition(Token** tokens, FILE* r_file, FILE* w_file);
bool is_parse_function_call(Token** tokens, FILE* w_file);
void report_error(const char *message);
void printToken(Token** token, char* funcName);
bool is_custom_space(char c);

Token funcTokens[MAX_TOKENS];
int funcCount = 0;
bool is_double_return = false;

int main(int argc, char* argv[]) {
    char line[MAX_LINE_LEN];

    // Checking if the arguement is provided
    if(argc != 2) {
        report_error("Error! File not provided!");
        exit(EXIT_FAILURE);
    }

    FILE* r_file = fopen(argv[1], "r");
    
    // If file is not opened successfully
    if (r_file == NULL) {
        report_error("Error: Could not open file provided!");
        exit(EXIT_FAILURE);
    }

    FILE* w_file = fopen("ml-temp.c", "w");

    if(w_file == NULL) {
        report_error("Error: Failed to write to file!");
        exit(EXIT_FAILURE);
    }

    fprintf(w_file, "#include <stdio.h>\nint main() {\n");
    // Processing line by line
    while (fgets(line, sizeof(line), r_file)) {
        process_line(line, r_file, w_file, false);
    }

    fprintf(w_file, "\n}");

    fclose(r_file);
    fclose(w_file);

    if (system("gcc -o object.o ml-temp.c") != 0) {
        fprintf(stderr, "! Error: Compilation failed\n");
        return 1;
    }

    system("./object.o");
    remove("ml-temp.c");
    remove("object.o");

    return EXIT_SUCCESS;
}

// Function which is used to process lines in the overall program
void process_line(char* line, FILE* r_file, FILE* w_file, bool is_body_lines) {
    remove_comment(line);
    if(strlen(line) > 0) {
        Token tokens[MAX_TOKENS];
        int token_count = 0;
        Token* token_ptr = tokens;
        char *line_ptr = line;

        // Tokenize the line and adding to an array
        while (token_count < MAX_TOKENS) {
            Token token = get_next_token(&line_ptr);
            if (token.type == TOKEN_UNKNOWN) {
                report_error("! Error: Unknown token encountered!");
            } 
            tokens[token_count++] = token;
            if (token.type == TOKEN_END) break;
        }
        if(is_body_lines){
            is_parsed_body(&token_ptr, r_file, w_file);
            return;
        }

        if(!is_parsed_program(&token_ptr, r_file, w_file)) {
            report_error("Invalid Syntax!");
        }
    }
}

// Removing comments by replacing # with null byte
void remove_comment(char* line) {
    char* comment = strchr(line, COMMENT);
    if (comment != NULL) {
        *comment = '\0';
    }
}

// Function which extracts all the tokens in a line
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

    // switch case for each token to update the token type
    switch(ch){
        case '\t':
            token.type = TOKEN_TAB;
            token.value[0] = '\t';
            *line = start + 1;
            return token;
        case '\0':
            token.type = TOKEN_END;
            token.value[0] = '\0';
            return token;
        case ',':
            token.type = TOKEN_COMMA;
            token.value[0] = ',';
            token.value[1] = '\0';
            *line = start + 1;
            return token;
        case '<':
            char *end = start + 1;
            if (*end == '-') {
                size_t len = 2; // Length is 2 for "<-"
                strncpy(token.value, start, len); 
                token.value[len] = '\0'; 
                token.type = TOKEN_ASSIGNMENT;
                *line = end + 1; // Move the line pointer past the "<-"
                return token;
            }
        case '+': 
            token.type = TOKEN_PLUS;
            token.value[0] = ch;
            token.value[1] = '\0';
            *line = start + 1;
            return token;
        case '-':
            token.type = TOKEN_MINUS;
            token.value[0] = ch;
            token.value[1] = '\0';
            *line = start + 1;
            return token;
        case '*':
            token.type = TOKEN_MULTIPLY;
            token.value[0] = ch;
            token.value[1] = '\0';
            *line = start + 1;
            return token;
        case '/':
            token.type = TOKEN_DIVIDE;
            token.value[0] = ch;
            token.value[1] = '\0';
            *line = start + 1;
            return token;
        case '(':
            token.type = TOKEN_OPEN_PARENTHESIS;
            token.value[0] = ch;
            token.value[1] = '\0';
            *line = start + 1;
            return token;
        case ')':
            token.type = TOKEN_CLOSED_PARENTHESIS;
            token.value[0] = ch;
            token.value[1] = '\0';
            *line = start + 1;
            return token;
        default:
        if (islower(ch)) { // Identifier or Keyword
            char *end = start;
            while (islower(*end)) end++;
            size_t len = end - start;
            strncpy(token.value, start, len);
            if (strcmp(token.value, "print") == 0) {
                // check for space requirement after 'print'
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
                    // check for space requirement after 'return'
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

    }
    token.type = TOKEN_UNKNOWN;
    token.value[0] = ch;
    token.value[1] = '\0';
    *line = start + 1;
    return token;
}

// Custom function as we cannot skip tab spaces for function body
bool is_custom_space(char c) {
    return (c == ' ' || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}

bool is_parsed_body(Token** tokens, FILE* r_file, FILE* w_file) {
    // If a tab is encountered it is still a part of the function definition
    if ((*tokens)->type == TOKEN_TAB) {
        (*tokens)++;
        while ((*tokens)->type != TOKEN_END) {
            // Loop until token ends and has to be a statement
            if (!is_parse_statement(tokens, w_file)) {
                report_error("SyntaxError-Failed to Parse Statement!");
                return false;
            }
        }
    }
    return true;
}

// Based on the BNF for parsed program according to assignment specification
bool is_parsed_program(Token** tokens, FILE* r_file, FILE* w_file) {
    while ((*tokens)->type != TOKEN_END) {
        // Loop until token ends and has to be a program item
        if (!is_parsed_program_item(tokens, r_file, w_file)) {
            report_error("SyntaxError-Failed to Parse Program!");
            return false;
        }
    }
    return true;
}

// Based on the BNF for program item  according to assignment specification
bool is_parsed_program_item(Token** tokens, FILE* r_file, FILE* w_file) {
    // Checking if it a defintion of a function
    if (is_parse_function_definition(tokens, r_file, w_file)) {
        return true;
    // Checking if it a statement
    } else if (is_parse_statement(tokens, w_file)) {
        return true;
    }
    report_error("SyntaxError-Failed to Parse Program Item!");
    return false; // No valid program item found
}

// Based on the BNF for function defintion according to assignment specification
bool is_parse_function_definition(Token** tokens, FILE* r_file, FILE* w_file) {
    int no_statements = 0;
    char body_lines[256];
    int no_parameters = 0;
    char parameters[MAX_VARIABLES][256];

    // Check for 'function' keyword
    if ((*tokens)->type != TOKEN_FUNCTION) {
        return false;
    }
    // Consume the 'function' keyword
    (*tokens)++;

    // Check for the function name (identifier)
    if ((*tokens)->type != TOKEN_IDENTIFIER) {
        report_error("SyntaxError-Expected function name!");
        return false;
    } else {
        fprintf(w_file, "double %s ", (*tokens)->value);
        // Consume the function name (identifier)
        funcTokens[funcCount] = **tokens;
        funcCount++;
        (*tokens)++;
    }

    fprintf(w_file, "(");

    // Parse zero or more function parameters (identifiers)
    while ((*tokens)->type == TOKEN_IDENTIFIER) {
        strncpy(parameters[no_parameters], (*tokens)->value, sizeof(parameters[no_parameters]) - 1);
        parameters[no_parameters][sizeof(parameters[no_parameters]) - 1] = '\0'; //Adding NULL byte at the end
        no_parameters++;
        (*tokens)++;
    }

    // looping to design function parameters dynamically
    // Ex - (1, 2, 3, 4) or (1, 2)
    for(int i = 0; i < no_parameters; i++) {
        if (i != 0) {
            fprintf(w_file, ", ");
        }
        fprintf(w_file, "double %s", parameters[i]);
    }

    fprintf(w_file, ") {\n");

    // While there is a tab space it is still a part of function definition
    while (fgets(body_lines, sizeof(body_lines), r_file) && body_lines[0] == '\t') {
        process_line(body_lines, r_file, w_file, true);
        no_statements++; // Incrementing no of body statements
    }

    fprintf(w_file, "}\n");

    if(no_statements > 0) {
        return true;  // Successfully parsed a function definition
    } else {
        report_error("SyntaxError-Expected function body!");
        return false;
    }
}

// Based on the BNF for statement according to assignment specification
bool is_parse_statement(Token** tokens, FILE* w_file) {
    bool is_print = false;
    bool is_new_line = true;
    if (strcmp((*tokens)->value, "print") == 0 || strcmp((*tokens)->value, "return") == 0) {
        if(strcmp((*tokens)->value, "print") == 0) {
            fprintf(w_file, "printf(\"%%.6f\\n\", ");
            is_print = true;
            is_new_line = false; // this 
        } else if(strcmp((*tokens)->value, "return") == 0) {
            fprintf(w_file, "return ");
        }

        (*tokens)++;
        if(is_parse_expression(tokens, w_file)) {
            if(is_print) {
                fprintf(w_file, ")");
            }
            fprintf(w_file, ";");
            if(is_new_line) {
                fprintf(w_file, "\n");
            }
            return true;
        } else {
            report_error("SyntaxError-Expression Expected after 'print' or 'return' ");
            return false;
        }

        if(strcmp((*tokens)->value, "print") == 0) {
            fprintf(w_file, "\nreturn 0.0;");
        }

    } else if (is_parse_assignment(tokens, w_file)) {
        fprintf(w_file, ";\n");
        return true;
    } else if (is_parse_function_call(tokens, w_file)) {
        fprintf(w_file, ";\n");
        return true;
    } else {
        report_error("SyntaxError-Failed to parse statement");
        return false;
    }
}

bool is_parse_function_call(Token** tokens, FILE* w_file) {
    char variable[256];
    // Function name expected
    if((*tokens)->type == TOKEN_IDENTIFIER) {
        strcpy(variable, (*tokens)->value); 
        (*tokens)++; // Consume function name

        // Expect '(' after function name
        if((*tokens)->type == TOKEN_OPEN_PARENTHESIS) {
            fprintf(w_file, "%s %s", variable, (*tokens)->value);
            (*tokens)++; // Consume '('

            // Expect expression
            if (!is_parse_expression(tokens, w_file)) {
                report_error("InvalidFunctionCall-Expected expression after '('");
                return false;
            }
            while ((*tokens)->type == TOKEN_COMMA) {
                fprintf(w_file, "%s", (*tokens)->value);
                (*tokens)++; // Consume ","
                if (!is_parse_expression(tokens, w_file)) {
                    report_error("InvalidFunctionCall-Expected expression after ','");
                    return false;
                }
            }

            // Expecting ')' after parameters passed
            if((*tokens)->type == TOKEN_CLOSED_PARENTHESIS) {
                fprintf(w_file, "%s", (*tokens)->value);
                (*tokens)++; // Consume ')'
                return true;
            } else {
                report_error("InvalidFunctionCall-Expected ')'");
                return false;
            }
        } else {
            // Means it may not be a function call so have to move the pointer back as we consumed identifier
            (*tokens)--;
            return false;
        }
    } else {
        // Means it may not be a function call
        return false;
    }
}


bool is_parse_expression(Token** tokens, FILE* w_file) {
    if (!is_parse_term(tokens, w_file)) {
        report_error("InvalidExpression-Expression has to have valid term");
        return false;
    }

    // Look for "+" or "-" operator
    while ((*tokens)->type == TOKEN_PLUS || (*tokens)->type == TOKEN_MINUS) {
        // Consume the "+" or "-" operator
        fprintf(w_file, " %s ", (*tokens)->value);
        (*tokens)++;
        // Parse the next expression after the operator
        if (!is_parse_term(tokens, w_file)) {
            report_error("InvalidExpression-Expression has to have valid term");
            return false;
        }
    }
    return true;  // Successfully parsed the expression
}

bool is_parse_term(Token** tokens, FILE* w_file) {
    
    if (!is_parse_factor(tokens, w_file)) {
        report_error("InvalidExpression-Term has to have valid factor");
        return false;
    }

    // Look for "*" or "/" operator
    while ((*tokens)->type == TOKEN_MULTIPLY || (*tokens)->type == TOKEN_DIVIDE) {
        // Consume the "*" or "/" operator
        fprintf(w_file, " %s ", (*tokens)->value);
        (*tokens)++;
        
        // Parse the next term after the operator
        if (!is_parse_factor(tokens, w_file)) {
            report_error("InvalidExpression-Term has to have valid factor");
            return false;
        }
    }

    return true;  // Successfully parsed the term
}

// TODO: CHECK LOGIC!
bool is_parse_factor(Token** tokens, FILE* w_file) {
    if(is_parse_function_call(tokens, w_file)) {
        return true; // Factor can be functioncall
    } else if(is_bracketed_expression(tokens, w_file)) {
        return true; // Factor can be bracketed expression
    } else if (
        (*tokens)->type == TOKEN_REALCONSTANT ||
        (*tokens)->type == TOKEN_IDENTIFIER
    ) {
        fprintf(w_file, "%s", (*tokens)->value);
        (*tokens)++; // Consume constant or indentifier
        return true; // Factor can be constant or indentifier
    } else {
        return false;
    }
}

bool is_bracketed_expression(Token** tokens, FILE* w_file) {
    if ((*tokens)->type == TOKEN_OPEN_PARENTHESIS) {
        (*tokens)++;
        if(is_parse_expression(tokens, w_file)){
            if ((*tokens)->type == TOKEN_CLOSED_PARENTHESIS){
                (*tokens)++;
                return true;
            } else {
                report_error("InvalidBracketedExpression-Expected ')'");
                return false;    
            }
        } else {
            report_error("InvalidBracketedExpression-Expression");
            return false;
        }
    } else {
        return false;
    }
}

bool is_parse_assignment(Token** tokens, FILE* w_file) {
    char variable[256];
    if ((*tokens)->type == TOKEN_IDENTIFIER) { 
        strcpy(variable, (*tokens)->value); 
        (*tokens)++; // Consume identifier
        if ((*tokens)->type == TOKEN_ASSIGNMENT) {
            fprintf(w_file, "double %s = ", variable);
            (*tokens)++; // Consume "<-"
            if (is_parse_expression(tokens, w_file)) {
                return true;
            } else {
                report_error("InvalidAssignment-Expected expression");
                return false;
            }
        } else {
            (*tokens)--;
            return false; // Not an assignment performed
        }
    } else {
        return false;
    }
}

void report_error(const char *message) {
    fprintf(stderr, "! %s\n", message);
}

void printToken(Token** token, char* funcName) {
    printf("%s: %d, %s\n", funcName, (*token)->type, (*token)->value);
}