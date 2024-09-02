#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LINE_LEN 256

void remove_comment(char* line);
void process_line(char* line);

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



// Removing comments
void remove_comment(char* line) {
    char* comment = strchr(line, '#');
    if (comment != NULL) {
        *comment = '\0';
    }
}

// Function which is used to process lines in the overall program
void process_line(char* line) {
    remove_comment(line);
    is_parsed_program(line);
}

bool is_parsed_program(char *line) {
    return is_parsed_program_item(line);
}

bool is_parsed_program_item(char *line) {
    return parsed_statement(line) || parsed_function(line)
}

bool parsed_statement(char *line) {
    return true;
}

bool parsed_function(char *line) {
    return true;
}
