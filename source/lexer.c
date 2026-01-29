// Logic for basic token consumption

#include "lexer.h"
#include "util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Append char c to the end of a null-terminated string s
void append_char(char *s, char c) {
    // Move pointer to the end
    while (*s++)
        ;
    // Append the new character
    *(s - 1) = c;
    // Add null terminator to mark new end
    *s = '\0';
}

// Repeatedly skip whitespace characters and characters following a '#' comment
char *skip_whitespace(char *input) {
    while (true) {
        if (*input == ' ' || *input == '\t') {
            input++;
        } else if (*input == '#') {
            while (*input != '\n' && *input != '\0') {
                input++;
            }
        } else {
            break;
        }
    }
    return input;
}

// consume_token :: Attempt to consume a character of the expected type
consume_token_result consume_token(token_type expected_type,
                                          char *input) {
    // Input is empty
    if (input[0] == '\0') {
        return (consume_token_result){false, '\0', input};
    }

    bool token_found = false;

    switch (expected_type) {
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
    case TOKEN_MINUS:
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
    case TOKEN_LESS_THAN:
        if (input[0] == '<') {
            token_found = true;
        }
        break;
    case TOKEN_GREATER_THAN:
        if (input[0] == '>') {
            token_found = true;
        }
        break;
    case TOKEN_EQUAL:
        if (input[0] == '=') {
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
        return (consume_token_result){true, input[0], &input[1]};
    } else {
        return (consume_token_result){false, '\0', input};
    }
}

// consume_var_name :: Attempt to consume a valid var_name
// <var_name> ::= <small_letter> (<small_letter> | '_')*
consume_name_result consume_var_name(char *input) {

    char *current_input = skip_whitespace(input);
    
    // Allocate 64 char wide buffer for var_name and initialise
    char *var_name = xalloc((NAME_BUFFER_LENGTH + 1) * sizeof(char),
                            "Interpreter Error: Fail to allocate memory while "
                            "parsing variable name.\n");
    var_name[0] = '\0';

    // Step 1: try to consume a small letter, return on fail
    consume_token_result result =
        consume_token(TOKEN_SMALL_LETTER, current_input);
    if (!result.success) {
        // Step 1 not successful, return with consume fail
        xfree(var_name);
        return (consume_name_result){false, NULL, input};
    }

    // Add the first consumed token to var_name buffer
    append_char(var_name, result.consumed_token);
    int var_name_length = 1;

    // Step 2: iteratively try to consume small letter or underscore
    current_input = result.next_input;
    bool reached_end = false;

    while (!reached_end) {
        result = consume_token(TOKEN_SMALL_LETTER_OR_UNDERSCORE, current_input);

        if (result.success) {
            // If var_name exceeds buffer size, exit program
            if (var_name_length >= NAME_BUFFER_LENGTH) {
                fprintf(stderr,
                        "Interpreter Constraint: Variable name exceeds maximum "
                        "length of %d characters.\n",
                        NAME_BUFFER_LENGTH);
                xfree(var_name);
                exit(EXIT_FAILURE);
            }

            // If successful, add to buffer
            append_char(var_name, result.consumed_token);
            current_input = result.next_input;
            var_name_length++;
        } else {
            reached_end = true;
        }
    }

    return (consume_name_result){true, var_name, current_input};
}

// consume_func_name :: Attempt to consume a valid func_name
// <func_name> ::= <big_letter> (<big_letter> | '_')*
consume_name_result consume_func_name(char *input) {

    char *current_input = skip_whitespace(input);

    // Allocate 64 char wide buffer for func_name and initialise
    char *func_name = xalloc((NAME_BUFFER_LENGTH + 1) * sizeof(char),
                             "Interpreter Error: Fail to allocate memory while "
                             "parsing function name.\n");
    func_name[0] = '\0';

    // Step 1: try to consume a big letter, return on fail
    consume_token_result result = consume_token(TOKEN_BIG_LETTER, current_input);
    if (!result.success) {
        // Step 1 not successful, return with consume fail
        xfree(func_name);
        return (consume_name_result){false, NULL, input};
    }

    // Add the first consumed token to func_name buffer
    append_char(func_name, result.consumed_token);
    int func_name_length = 1;

    // Step 2: iteratively try to consume big letter or underscore
    current_input = result.next_input;
    bool reached_end = false;

    while (!reached_end) {
        result = consume_token(TOKEN_BIG_LETTER_OR_UNDERSCORE, current_input);
        if (result.success) {
            // If func_name exceeds buffer size, exit program
            if (func_name_length >= NAME_BUFFER_LENGTH) {
                fprintf(stderr,
                        "Interpreter Constraint: Function name exceeds maximum "
                        "length of %d characters.\n",
                        NAME_BUFFER_LENGTH);
                xfree(func_name);
                exit(EXIT_FAILURE);
            }

            // If successful, add to buffer
            append_char(func_name, result.consumed_token);
            current_input = result.next_input;
            func_name_length++;
        } else {
            reached_end = true;
        }
    }

    return (consume_name_result){true, func_name, current_input};
}

// consume_str_literal :: Attempt to consume a str_literal including the
// quotation marks
// '"' <str_literal> '"'
// <str_literal> ::= <char>*
// <char> ::= any ASCII char excluding '"'
consume_name_result consume_str_literal(char *input) {

    char *current_input = skip_whitespace(input);

    // Allocate 64 char wide buffer for string buffer and initialise
    char *str_buffer =
        xalloc((STRING_BUFFER_LENGTH + 1) * sizeof(char),
               "Interpreter Error: Fail to allocate memory while "
               "parsing function name.\n");
    str_buffer[0] = '\0';

    int str_length = 0;

    // Step 1: try to consume a quotation mark, return on fail
    consume_token_result result =
        consume_token(TOKEN_QUOTATION, current_input);
    if (result.success) {
        // If successful, advance input (no need to add quoatation to buffer)
        current_input = result.next_input;
    } else {
        // If unsuccessful, return str_literal consume unsuccessful
        xfree(str_buffer);
        return (consume_name_result){false, NULL, input};
    }

    // Step 2: iteratively try to consume ACSII char
    bool reached_end = false;

    while (!reached_end) {
        result = consume_token(TOKEN_ASCII_CHAR, current_input);
        if (result.success) {
            // If str_literal exceeds buffer size, exit program
            if (str_length >= STRING_BUFFER_LENGTH) {
                fprintf(stderr,
                        "Interpreter Constraint: String literal exceeds maximum "
                        "length of %d characters.\n",
                        STRING_BUFFER_LENGTH);
                xfree(str_buffer);
                exit(EXIT_FAILURE);
            }

            // If successful, add to buffer
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
        fprintf(stderr,
                "Syntax Error: Unclosed quotation for string literal %s.\n",
                str_buffer);
        xfree(str_buffer);
        exit(EXIT_FAILURE);
    }

    return (consume_name_result){true, str_buffer, current_input};
}

// consume_num_literal :: Attempt to consume a num_literal
// <num_literal> ::= ('-')? <digit>+ ('.' <digit>+)?
consume_num_result consume_num_literal(char *input) {

    char *current_input = skip_whitespace(input);
    
    // Allocate 64 char wide buffer for num_literal input and initialise
    char *num_buffer =
        xalloc((NAME_BUFFER_LENGTH + 1) * sizeof(char),
               "Interpreter Error: Fail to allocate memory while "
               "parsing number literal.\n");
    num_buffer[0] = '\0';

    int num_length = 0;

    // Step 1: try to consume a minus sign (optional)
    consume_token_result result =
        consume_token(TOKEN_MINUS, current_input);
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
        xfree(num_buffer);
        return (consume_num_result){false, 0, input};
    }

    // Step 3: iteratively try to consume digit or decimal point
    bool saw_decimal_point = false;
    bool reached_end = false;

    while (!reached_end) {
        if (saw_decimal_point) {
            // If there is already a decimal point, try to consume digit only
            result = consume_token(TOKEN_DIGIT, current_input);
        } else {
            // If no decimal point yet, try to consume digit or decimal point
            result = consume_token(TOKEN_DIGIT_OR_DECIMAL_POINT, current_input);
        }

        if (result.success) {
            // If num_buffer length exceeds buffer size, exit program
            if (num_length >= NAME_BUFFER_LENGTH) {
                fprintf(stderr,
                        "Interpreter Constraint: Number literal exceeds maximum "
                        "length of %d characters.\n",
                        NAME_BUFFER_LENGTH);
                xfree(num_buffer);
                exit(EXIT_FAILURE);
            }

            // If successful, add to buffer
            append_char(num_buffer, result.consumed_token);
            current_input = result.next_input;
            num_length++;
            saw_decimal_point |=
                result.consumed_token == '.'; // Update decimal point encounter
        } else {
            reached_end = true;
        }
    }

    // Convert the string to a double and free the buffer
    double consumed_num = atof(num_buffer);
    xfree(num_buffer);
    return (consume_num_result){true, consumed_num, current_input};
}

// consume_left_arrow :: Attempt to consume a left_arrow
// <left_arrow> ::= '<-'
consume_arrow_result consume_left_arrow(char *input) {

    char *current_input = skip_whitespace(input);

    bool consume_success = false;

    // Step 1: Try to consume '<'
    consume_token_result result = consume_token(TOKEN_LESS_THAN, current_input);

    if (result.success) {
        // Step 2: Try to consume '-', if previously successful
        result = consume_token(TOKEN_MINUS, result.next_input);
        consume_success = result.success;
    }

    if (consume_success) {
        return (consume_arrow_result){true, result.next_input};
    } else {
        return (consume_arrow_result){false, input};
    }
}

// consume_right_arrow :: Attempt to consume a right_arrow
// <right_arrow> ::= '->'
consume_arrow_result consume_right_arrow(char *input) {

    char *current_input = skip_whitespace(input);

    bool consume_success = false;

    // Step 1: Try to consume '-'
    consume_token_result result = consume_token(TOKEN_MINUS, current_input);

    if (result.success) {
        // Step 2: Try to consume '>', if previously successful
        result = consume_token(TOKEN_GREATER_THAN, result.next_input);
        consume_success = result.success;
    }

    if (consume_success) {
        return (consume_arrow_result){true, result.next_input};
    } else {
        return (consume_arrow_result){false, input};
    }
}

// consume_left_double_arrow :: Attempt to consume a left_double_arrow
// <left_double_arrow> ::= '<='
consume_arrow_result consume_left_double_arrow(char *input) {

    char *current_input = skip_whitespace(input);

    bool consume_success = false;

    // Step 1: Try to consume '<'
    consume_token_result result = consume_token(TOKEN_LESS_THAN, current_input);

    if (result.success) {
        // Step 2: Try to consume '=', if previously successful
        result = consume_token(TOKEN_EQUAL, result.next_input);
        consume_success = result.success;
    }

    if (consume_success) {
        return (consume_arrow_result){true, result.next_input};
    } else {
        return (consume_arrow_result){false, input};
    }
}

// consume_right_doule_arrow :: Attempt to consume a right_doule_arrow
// <right_doule_arrow> ::= '=>'
consume_arrow_result consume_right_doule_arrow(char *input) {

    char *current_input = skip_whitespace(input);

    bool consume_success = false;

    // Step 1: Try to consume '='
    consume_token_result result = consume_token(TOKEN_EQUAL, current_input);

    if (result.success) {
        // Step 2: Try to consume '>', if previously successful
        result = consume_token(TOKEN_GREATER_THAN, result.next_input);
        consume_success = result.success;
    }

    if (consume_success) {
        return (consume_arrow_result){true, result.next_input};
    } else {
        return (consume_arrow_result){false, input};
    }
}

// consume_jump_literal :: Attempt to consume a list of digits
// <digit>+
consume_int_result consume_integer(char *input) {

    char *current_input = skip_whitespace(input);

    char *num_buffer =
        xalloc((NAME_BUFFER_LENGTH + 1) * sizeof(char),
               "Interpreter Error: Fail to allocate memory while "
               "parsing number literal.\n");
    num_buffer[0] = '\0';

    int num_length = 0;

    bool reached_end = false;
    consume_token_result result;

    // Iteratively consume digits
    while(!reached_end) {
        result = consume_token(TOKEN_DIGIT, current_input);

        if (result.success) {
            if (num_length >= NAME_BUFFER_LENGTH) {
                fprintf(stderr,
                        "Interpreter Constraint: Number literal exceeds maximum "
                        "length of %d characters.\n",
                        NAME_BUFFER_LENGTH);
                xfree(num_buffer);
                exit(EXIT_FAILURE);
            }

            // If successful, add to buffer
            append_char(num_buffer, result.consumed_token);
            current_input = result.next_input;
            num_length++;
        } else {
            // If unsuccessful, set reached_end flag
            reached_end = true;
        }
    }

    if (num_length > 0) {
        int consumed_num = atoi(num_buffer);
        xfree(num_buffer);
        return (consume_int_result){true, consumed_num, current_input};
    } else {
        xfree(num_buffer);
        return (consume_int_result){false, 0, input};
    }
}

// consume_jump_literal :: Attempt to consume a jump_literal
// <left_double_arrow> ::= '<='
// <right_doule_arrow> ::= '=>'
// <jump_literal> ::= <digit>+ <left_double_arrow> | <right_doule_arrow> <digit>+
consume_jump_result consume_jump_literal(char *input) {

    char *current_input = skip_whitespace(input);
    
    jump_type type = JUMP_NOT_FOUND;
    bool consume_success = false;

    consume_arrow_result arrow_result;
    consume_int_result int_result;

    // Step 1: Try to consume <right_doule_arrow> '=>'
    arrow_result = consume_right_doule_arrow(current_input);

    // If successful, this is forward jump, try to consume digits
    if (arrow_result.success) {
        type = JUMP_FORWARD;
        current_input = skip_whitespace(arrow_result.next_input);
        int_result = consume_integer(current_input);

        if (int_result.success) {
            consume_success = true;
            current_input = int_result.next_input;
        }
    }

    // If unsuccessful, this is backward jump, consume digits then '<='
    else {
        type = JUMP_BACKWARD;
        int_result = consume_integer(current_input);

        if (int_result.success) {
            current_input = skip_whitespace(int_result.next_input);
            arrow_result = consume_left_double_arrow(current_input);

            if (arrow_result.success) {
                consume_success = true;
                current_input = arrow_result.next_input;
            }
        }
    }

    if (consume_success) {
        return (consume_jump_result){true, type, int_result.number, current_input};
    } else {
        return (consume_jump_result){false, JUMP_NOT_FOUND, 0, input}; 
    }
}
