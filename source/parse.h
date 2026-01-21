// parse.h

#ifndef PARSE_H
#define PARSE_H

#include <stdbool.h>

#define MAX_VAR_NAME_LENGTH 50
#define MAX_FUNC_NAME_LENGTH 50


enum TokenType {
    TOKEN_SMALL_LETTER,
    TOKEN_SMALL_LETTER_OR_UNDERSCORE,
    TOKEN_BIG_LETTER,
    TOKEN_BIG_LETTER_OR_UNDERSCORE,
    TOKEN_DIGIT,
    TOKEN_QUOTATION,
    TOKEN_COMMA,
    TOKEN_OPEN_PAREN,
    TOKEN_CLOSE_PAREN,
    TOKEN_OPEN_SQUARE_BRACKET,
    TOKEN_CLOSE_SQUARE_BRACKET
};

struct consume_token_result {
    bool success;
    char consumed_token;
    char *next_input;
};

struct consume_result {
    bool success;
    char *consumed;
    char *next_input;
};

struct consume_token_result consume_token(enum TokenType expected_type, char *input);
struct consume_result consume_var_name(char *input);
struct consume_result consume_func_name(char *input);

#endif
