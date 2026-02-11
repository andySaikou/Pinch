// Main function for the pinch interpreter

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "util.h"
#include "list.h"

#define MAX_LINE_LENGTH 1024

// DJB2 Hash Function for Strings
int hash_string(const void *key) {
    unsigned long hash = 5381;
    int c;
    const char *str = (const char *)key;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    
    // Bitwise AND ensures the hash is a positive integer
    return (int)(hash & 0x7FFFFFFF); 
}

struct hashmap *create_var_hashmap() {
    // 16 initial buckets, 5 elements per bucket initial capacity
    return hashmap_new(16, 5, hash_string);
}

void free_state(MachineState *state) {
    if (state == NULL) return;

    // Free Statements
    if (state->statements != NULL) {
        for (int i = 0; i < state->stmt_count; i++) {
            free_statement(state->statements[i]);
        }
        xfree(state->statements);
    }

    // Free the Variables Hashmap
    if (state->variables != NULL) {
        struct hashmap *map = state->variables;
        
        // Iterate over the master list of buckets
        for (int i = 0; i < map->bucket_size; i++) {
            struct list *bucket = list_get(map->buckets, i);
            
            if (bucket != NULL) {
                // Iterate over every key-value pair inside this bucket
                for (int j = 0; j < bucket->length; j++) {
                    struct pair *kv = list_get(bucket, j);
                    if (kv != NULL) {
                        xfree(kv->k);
                        free_value((Value*)kv->v);
                        xfree(kv);
                    }
                }
                free_list(bucket);
            }
        }
        
        free_list(map->buckets); 
        xfree(map);
    }
    xfree(state);
}

// ---------------------------------------------------------
// Interactive REPL Mode
// ---------------------------------------------------------
void run_repl() {
    printf("PINCH INTERACTIVE MODE\n");
    printf("Type 'exit' or 'quit' to close.\n");

    char line[MAX_LINE_LENGTH];
    
    // Create MachineState
    MachineState *state = xalloc(sizeof(MachineState), "Alloc MachineState");
    state->program_counter = 0;
    state->statements = NULL;
    state->stmt_count = 0;
    state->variables = create_var_hashmap();
    
    while (true) {
        printf(">> ");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break; // Handle EOF (Ctrl+D)
        }

        // Remove trailing newline
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "exit") == 0 || strcmp(line, "quit") == 0) {
            break;
        }

        // Skip empty lines
        if (strlen(line) == 0) continue; 

        // Parse a single line
        parse_statement_result res = parse_statement(line);
        
        if (res.success) {
            interpret_line(res.stmt, state, true);
            free_statement(res.stmt);
        } else {
            fprintf(stderr, "Syntax Error.\n");
        }
    }
    free_state(state);
}

// ---------------------------------------------------------
// File Execution Mode
// ---------------------------------------------------------
void run_file(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filepath);
        exit(EXIT_FAILURE);
    }

    // Find file size
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read entire file into memory
    char *source_code = xalloc(fsize + 1, "Interpreter Error: Fail to allocate memory for source code.\n");
    fread(source_code, 1, fsize, file);
    fclose(file);
    source_code[fsize] = 0; // Null-terminate
    
    // Create and initialize MachineState
    MachineState *state = xalloc(sizeof(MachineState), "Interpreter Error: Fail to allocate memory for MachineState.\n");
    state->program_counter = 0;
    state->stmt_count = 0;
    state->variables = create_var_hashmap();
    
    int capacity = 64; // Initial array capacity
    state->statements = xalloc(capacity * sizeof(Statement*), "Interpreter Error: Fail to allocate memory for statements.\n");

    bool syntax_error = false;
    int physical_line = 1;
    char *current_pos = source_code;

    // Traverse the file string line by line
    while (*current_pos != '\0') {
        char *newline = strchr(current_pos, '\n');
        int len = newline ? (newline - current_pos) : (int)strlen(current_pos);
        
        char *line_buf = xalloc(len + 2, "Interpreter Error: Fail to allocate memory for a line of code.\n");
        strncpy(line_buf, current_pos, len);
        line_buf[len] = '\n';
        line_buf[len + 1] = '\0';

        // Check if the line is just empty whitespace
        bool is_empty = true;
        for (int i = 0; i < len; i++) {
            if (line_buf[i] != ' ' && line_buf[i] != '\t' && line_buf[i] != '\r') {
                is_empty = false;
                break;
            }
        }

        if (!is_empty) {
            parse_statement_result res = parse_statement(line_buf);
            
            if (res.success) {
                // Resize the dynamic array if hit capacity
                if (state->stmt_count >= capacity) {
                    capacity *= 2;
                    Statement **new_stmts = xalloc(capacity * sizeof(Statement*), "Resize stmts");
                    for (int i = 0; i < state->stmt_count; i++) {
                        new_stmts[i] = state->statements[i];
                    }
                    xfree(state->statements);
                    state->statements = new_stmts;
                }
                state->statements[state->stmt_count++] = res.stmt;
            } else {
                fprintf(stderr, "Syntax Error on line %d.\n", physical_line);
                syntax_error = true;
                xfree(line_buf);
                break; // Stop parsing on first error
            }
        }
        
        xfree(line_buf);
        
        if (!newline) break; 
        current_pos = newline + 1;
        physical_line++;
    }
    
    xfree(source_code);

    if (syntax_error) {
        free_state(state);
        fprintf(stderr, "Compilation failed due to syntax error.\n");
        exit(EXIT_FAILURE);
    }

    // Execution
    while (state->program_counter >= 0 && state->program_counter < state->stmt_count) {
        Statement *current_stmt = state->statements[state->program_counter];
        
        // Interpret a single line (Interactive = false)
        bool success = interpret_line(current_stmt, state, false);
        
        if (!success) {
            fprintf(stderr, "Execution halted at statement %d.\n", state->program_counter + 1);
            break; 
        }
        state->program_counter++;
    }
    free_state(state);
}

// ---------------------------------------------------------
// Main Entry
// ---------------------------------------------------------
int main(int argc, char **argv) {
    if (argc == 1) {
        run_repl();
    } else if (argc == 2) {
        run_file(argv[1]);
    } else {
        fprintf(stderr, "Too many arguments provided.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
