// Logic for parsing basic elements

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

// Append char c to the end of a null-terminated string s
void append_char(char *s, char c) {
  	// Move pointer to the end
    while (*s++);
    // Append the new character
    *(s - 1) = c;
    // Add null terminator to mark new end
    *s = '\0';
}

// consume_token :: Attempt to consume a character of the expected type 
struct consume_token_result consume_token(enum TokenType expected_type, char *input) {
    // Input is empty
    if (input[0] == '\0') {
        return (struct consume_token_result){false, '\0', input};
    }

    bool token_found = false;

    switch (expected_type)
    {
    case TOKEN_SMALL_LETTER_OR_UNDERSCORE:
        if (input[0] == '_') {
            token_found = true;
        } // intended fall through
    case TOKEN_SMALL_LETTER:
        if (input[0] >= 'a' && input[0] <= 'z') {
            token_found = true;
        }
        break;
    case TOKEN_BIG_LETTER_OR_UNDERSCORE:
        if (input[0] == '_') {
            token_found = true;
        } // intended fall through
    case TOKEN_BIG_LETTER:
        if (input[0] >= 'A' && input[0] <= 'Z') {
            token_found = true;
        }
        break;
    case TOKEN_DIGIT:
        if (input[0] >= '0' && input[0] <= '9') {
            token_found = true;
        }
        break;
    case TOKEN_QUOTATION:
        if (input[0] == '"') {
            token_found = true;
        }
        break;
    case TOKEN_COMMA:
        if (input[0] == ',') {
            token_found = true;
        }
        break;
    case TOKEN_OPEN_PAREN:
        if (input[0] == '(') {
            token_found = true;
        }
        break;
    case TOKEN_CLOSE_PAREN:
        if (input[0] == ')') {
            token_found = true;
        }
        break;
    case TOKEN_OPEN_SQUARE_BRACKET:
        if (input[0] == '[') {
            token_found = true;
        }
        break;
    case TOKEN_CLOSE_SQUARE_BRACKET:
        if (input[0] == ']') {
            token_found = true;
        }
        break;
    default:
        break;
    }

    if (token_found) {
        return (struct consume_token_result){true, input[0], &input[1]};
    } else {
        return (struct consume_token_result){false, '\0', input};
    }
}

// consume_var_name :: Attempt to consume a valid var_name
// <var_name> ::= <small_letter> (<small_letter> | '_')*
struct consume_result consume_var_name(char *input) {

    // Allocate 50 char wide buffer for var_name and initialise
    char *var_name = malloc((MAX_VAR_NAME_LENGTH + 1) * sizeof(char));
    if (var_name == NULL) {
        // If malloc fails, exit program
        fprintf(stderr, "Interpreter Error: Fail to allocate memory while parsing variable name.\n");
        exit(EXIT_FAILURE);
    }
    var_name[0] = '\0';

    // Step 1: try to consume a small letter 
    struct consume_token_result result = consume_token(TOKEN_SMALL_LETTER, input);
    if (!result.success) {
        // Step 1 not successful, return with consume fail
        free(var_name);
        return (struct consume_result){false, NULL, input};
    }

    // Add the first consumed token to var_name buffer
    append_char(var_name, result.consumed_token);
    int var_name_length = 1;
    
    // Step 2: iteratively try to consume small letter or underscore
    char *current_input = result.next_input;
    bool reached_end = false;

    while (!reached_end) {
        // If var_name exceeds buffer size, exit program
        if (var_name_length >= MAX_VAR_NAME_LENGTH) { 
            fprintf(stderr, "Interpreter Constraint: Variable name exceeds maximum length of %d characters.\n", MAX_VAR_NAME_LENGTH);
            free(var_name); 
            exit(EXIT_FAILURE);
        }

        result = consume_token(TOKEN_SMALL_LETTER_OR_UNDERSCORE, current_input);
        if (result.success) {
            append_char(var_name, result.consumed_token);
            current_input = result.next_input;
            var_name_length++;
        } else {
            reached_end = true;
        }
    }

    return (struct consume_result){true, var_name, current_input};
}

// consume_func_name :: Attempt to consume a valid func_name
// <func_name> ::= <big_letter> (<big_letter> | '_')*
struct consume_result consume_func_name(char *input) {

    // Allocate 50 char wide buffer for func_name and initialise
    char *func_name = malloc((MAX_VAR_NAME_LENGTH + 1) * sizeof(char));
    if (func_name == NULL) {
        // If malloc fails, exit program
        fprintf(stderr, "Interpreter Error: Fail to allocate memory while parsing function name.\n");
        exit(EXIT_FAILURE);
    }
    func_name[0] = '\0';

    // Step 1: try to consume a big letter 
    struct consume_token_result result = consume_token(TOKEN_BIG_LETTER, input);
    if (!result.success) {
        // Step 1 not successful, return with consume fail
        free(func_name);
        return (struct consume_result){false, NULL, input};
    }

    // Add the first consumed token to func_name buffer
    append_char(func_name, result.consumed_token);
    int func_name_length = 1;
    
    // Step 2: iteratively try to consume big letter or underscore
    char *current_input = result.next_input;
    bool reached_end = false;

    while (!reached_end) {
        // If func_name exceeds buffer size, exit program
        if (func_name_length >= MAX_FUNC_NAME_LENGTH) { 
            fprintf(stderr, "Interpreter Constraint: Function name exceeds maximum length of %d characters.\n", MAX_FUNC_NAME_LENGTH);
            free(func_name); 
            exit(EXIT_FAILURE);
        }

        result = consume_token(TOKEN_BIG_LETTER_OR_UNDERSCORE, current_input);
        if (result.success) {
            append_char(func_name, result.consumed_token);
            current_input = result.next_input;
            func_name_length++;
        } else {
            reached_end = true;
        }
    }

    return (struct consume_result){true, func_name, current_input};
}
