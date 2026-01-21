// Logic for basic token consumption

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

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
    case TOKEN_ASCII_CHAR:
        if (input[0] != '"') {
            token_found = true;
        }
        break;
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
    case TOKEN_MINUS_SIGN:
        if (input[0] == '-') {
            token_found = true;
        }
        break;
    case TOKEN_DIGIT_OR_DECIMAL_POINT:
        if (input[0] == '.') {
            token_found = true;
        } // intended fall through
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
struct consume_name_result consume_var_name(char *input) {

    // Allocate 50 char wide buffer for var_name and initialise
    char *var_name = malloc((STRING_BUFFER_LENGTH + 1) * sizeof(char));
    if (var_name == NULL) {
        // If malloc fails, exit program
        fprintf(stderr, "Interpreter Error: Fail to allocate memory while parsing variable name.\n");
        exit(EXIT_FAILURE);
    }
    var_name[0] = '\0';

    // Step 1: try to consume a small letter, return on fail
    struct consume_token_result result = consume_token(TOKEN_SMALL_LETTER, input);
    if (!result.success) {
        // Step 1 not successful, return with consume fail
        free(var_name);
        return (struct consume_name_result){false, NULL, input};
    }

    // Add the first consumed token to var_name buffer
    append_char(var_name, result.consumed_token);
    int var_name_length = 1;
    
    // Step 2: iteratively try to consume small letter or underscore
    char *current_input = result.next_input;
    bool reached_end = false;

    while (!reached_end) {
        // If var_name exceeds buffer size, exit program
        if (var_name_length >= STRING_BUFFER_LENGTH) { 
            fprintf(stderr, "Interpreter Constraint: Variable name exceeds maximum length of %d characters.\n", STRING_BUFFER_LENGTH);
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

    return (struct consume_name_result){true, var_name, current_input};
}

// consume_func_name :: Attempt to consume a valid func_name
// <func_name> ::= <big_letter> (<big_letter> | '_')*
struct consume_name_result consume_func_name(char *input) {

    // Allocate 50 char wide buffer for func_name and initialise
    char *func_name = malloc((STRING_BUFFER_LENGTH + 1) * sizeof(char));
    if (func_name == NULL) {
        // If malloc fails, exit program
        fprintf(stderr, "Interpreter Error: Fail to allocate memory while parsing function name.\n");
        exit(EXIT_FAILURE);
    }
    func_name[0] = '\0';

    // Step 1: try to consume a big letter, return on fail
    struct consume_token_result result = consume_token(TOKEN_BIG_LETTER, input);
    if (!result.success) {
        // Step 1 not successful, return with consume fail
        free(func_name);
        return (struct consume_name_result){false, NULL, input};
    }

    // Add the first consumed token to func_name buffer
    append_char(func_name, result.consumed_token);
    int func_name_length = 1;
    
    // Step 2: iteratively try to consume big letter or underscore
    char *current_input = result.next_input;
    bool reached_end = false;

    while (!reached_end) {
        // If func_name exceeds buffer size, exit program
        if (func_name_length >= STRING_BUFFER_LENGTH) { 
            fprintf(stderr, "Interpreter Constraint: Function name exceeds maximum length of %d characters.\n", STRING_BUFFER_LENGTH);
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

    return (struct consume_name_result){true, func_name, current_input};
}

// consume_str_literal :: Attempt to consume a str_literal including the quotation marks
// '"' <str_literal> '"'
// <str_literal> ::= <char>*
// <char> ::= any ASCII char excluding '"'
struct consume_name_result consume_str_literal(char *input) {

    // Allocate 50 char wide buffer for string buffer and initialise
    char *str_buffer = malloc((STRING_BUFFER_LENGTH + 1) * sizeof(char));
    if (str_buffer == NULL) {
        // If malloc fails, exit program
        fprintf(stderr, "Interpreter Error: Fail to allocate memory while parsing function name.\n");
        exit(EXIT_FAILURE);
    }
    str_buffer[0] = '\0';

    char *current_input = input;
    int str_length = 0;

    // Step 1: try to consume a quotation mark, return on fail
    struct consume_token_result result = consume_token(TOKEN_QUOTATION, current_input);
    if (result.success) {
        // If successful, advance input (no need to add quoatation to buffer)
        current_input = result.next_input;
    } else { 
        // If unsuccessful, return str_literal consume unsuccessful
        free(str_buffer);
        return (struct consume_name_result){false, NULL, input};
    }
    
    // Step 2: iteratively try to consume ACSII char
    bool reached_end = false;

    while (!reached_end) {
        // If str_literal exceeds buffer size, exit program
        if (str_length >= STRING_BUFFER_LENGTH) { 
            fprintf(stderr, "Interpreter Constraint: String literal exceeds maximum length of %d characters.\n", STRING_BUFFER_LENGTH);
            free(str_buffer); 
            exit(EXIT_FAILURE);
        }

        result = consume_token(TOKEN_ASCII_CHAR, current_input);
        if (result.success) {
            append_char(str_buffer, result.consumed_token);
            current_input = result.next_input;
            str_length++;
        } else {
            reached_end = true;
        }
    }

    // Step 3: try to consume a quotation mark, return on fail
    result = consume_token(TOKEN_QUOTATION, current_input);
    if (result.success) {
        // If successful, advance input (no need to add quoatation to buffer)
        current_input = result.next_input;
    } else { 
        // If unsuccessful, exit program and produce error
        fprintf(stderr, "Syntax Error: Unclosed quotation for string literal %s.\n", str_buffer);
        free(str_buffer);
        exit(EXIT_FAILURE);
    }

    return (struct consume_name_result){true, str_buffer, current_input};
}

// consume_num_literal :: Attempt to consume a num_literal
// <num_literal> ::= ('-')? <digit>+ ('.' <digit>+)?
struct consume_num_result consume_num_literal(char *input) {

    // Allocate 50 char wide buffer for num_literal input and initialise
    char *num_buffer = malloc((STRING_BUFFER_LENGTH + 1) * sizeof(char));
    if (num_buffer == NULL) {
        // If malloc fails, exit program
        fprintf(stderr, "Interpreter Error: Fail to allocate memory while parsing number literal.\n");
        exit(EXIT_FAILURE);
    }
    num_buffer[0] = '\0';

    char *current_input = input;
    int num_length = 0;

    // Step 1: try to consume a minus sign (optional)
    struct consume_token_result result = consume_token(TOKEN_MINUS_SIGN, current_input);
    if (result.success) {
        // If successful, add to buffer
        append_char(num_buffer, result.consumed_token);
        current_input = result.next_input;
        num_length++;
    }

    // Step 2: try to consume a digit, return on fail
    result = consume_token(TOKEN_DIGIT, current_input);
    if (result.success) {
        // If successful, add to buffer
        append_char(num_buffer, result.consumed_token);
        current_input = result.next_input;
        num_length++;
    } else {
        // If unsuccessful, free buffer and return unsuccessful consume
        free(num_buffer);
        return (struct consume_num_result){false, 0, input};
    }
    
    // Step 3: iteratively try to consume digit or decimal point
    bool saw_decimal_point = false;
    bool reached_end = false;

    while (!reached_end) {
        // If num_buffer length exceeds buffer size, exit program
        if (num_length >= STRING_BUFFER_LENGTH) { 
            fprintf(stderr, "Interpreter Constraint: Number literal exceeds maximum length of %d characters.\n", STRING_BUFFER_LENGTH);
            free(num_buffer); 
            exit(EXIT_FAILURE);
        }

        if (saw_decimal_point) {
            // If there is already a decimal point, try to consume digit only
            result = consume_token(TOKEN_DIGIT, current_input);
        } else {
            // If no decimal point yet, try to consume digit or decimal point
            result = consume_token(TOKEN_DIGIT_OR_DECIMAL_POINT, current_input);
        }

        if (result.success) {
            append_char(num_buffer, result.consumed_token);
            current_input = result.next_input;
            num_length++;            
            saw_decimal_point |= result.consumed_token == '.'; // Update decimal point encounter
        } else {
            reached_end = true;
        }
    }

    // Convert the string to a double and free the buffer
    double consumed_num = atof(num_buffer);
    free(num_buffer);
    return (struct consume_num_result){true, consumed_num, current_input};
}
