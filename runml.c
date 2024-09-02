#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_IDENTIFIER_LEN 12
#define MAX_LINE_LEN 256

void remove_comment(char* line) {
    char* comment = strchr(line, '#');
    if (comment != NULL) {
        *comment = '\0';
    }
}

void process_line(char* line) {
    remove_comment(line);

    // Further processing like tokenizing and parsing
}

int main(int argc, char* argv[]) {
    char line[MAX_LINE_LEN];
    FILE* file = fopen("program.ml", "r");
    
    if (file == NULL) {
        printf("Could not open file.\n");
        return 1;
    }
    
    while (fgets(line, sizeof(line), file)) {
        process_line(line);
    }
    
    fclose(file);
    return 0;
}
