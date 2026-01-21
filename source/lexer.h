// lexer.h

#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>

#define STRING_BUFFER_LENGTH 50


enum TokenType {
    TOKEN_ASCII_CHAR,
    TOKEN_SMALL_LETTER,
    TOKEN_SMALL_LETTER_OR_UNDERSCORE,
    TOKEN_BIG_LETTER,
    TOKEN_BIG_LETTER_OR_UNDERSCORE,
    TOKEN_MINUS_SIGN,
    TOKEN_DIGIT,
    TOKEN_DIGIT_OR_DECIMAL_POINT,
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

struct consume_name_result {
    bool success;
    char *consumed;
    char *next_input;
};

struct consume_num_result {
    bool success;
    double number;
    char *next_input;
};

struct consume_token_result consume_token(enum TokenType expected_type, char *input);
struct consume_name_result consume_var_name(char *input);
struct consume_name_result consume_func_name(char *input);
struct consume_name_result consume_str_literal(char *input);
struct consume_num_result consume_num_literal(char *input);

#endif
