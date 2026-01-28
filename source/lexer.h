// lexer.h

#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>

#define STRING_BUFFER_LENGTH 1024
#define NAME_BUFFER_LENGTH 64


typedef enum {
    TOKEN_ASCII_CHAR,
    TOKEN_SMALL_LETTER,
    TOKEN_SMALL_LETTER_OR_UNDERSCORE,
    TOKEN_BIG_LETTER,
    TOKEN_BIG_LETTER_OR_UNDERSCORE,
    TOKEN_MINUS,
    TOKEN_DIGIT,
    TOKEN_DIGIT_OR_DECIMAL_POINT,
    TOKEN_QUOTATION,
    TOKEN_COMMA,
    TOKEN_LESS_THAN,
    TOKEN_GREATER_THAN,
    TOKEN_EQUAL,
    TOKEN_OPEN_PAREN,
    TOKEN_CLOSE_PAREN,
    TOKEN_OPEN_SQUARE_BRACKET,
    TOKEN_CLOSE_SQUARE_BRACKET
} token_type;

typedef struct {
    bool success;
    char consumed_token;
    char *next_input;
} consume_token_result;

typedef struct {
    bool success;
    char *consumed;
    char *next_input;
} consume_name_result;

typedef struct {
    bool success;
    double number;
    char *next_input;
} consume_num_result;

typedef struct {
    bool success;
    char *next_input;
} consume_arrow_result;

typedef struct {
    bool success;
    int number;
    char *next_input;
} consume_int_result;

typedef enum {
    JUMP_FORWARD,
    JUMP_BACKWARD,
    JUMP_NOT_FOUND
} jump_type;

typedef struct {
    bool success;
    jump_type type;
    int lines;
    char *next_input;
} consume_jump_result;

char *skip_whitespace(char *input);
consume_token_result consume_token(token_type expected_type, char *input);
consume_name_result consume_var_name(char *input);
consume_name_result consume_func_name(char *input);
consume_name_result consume_str_literal(char *input);
consume_num_result consume_num_literal(char *input);
consume_arrow_result consume_left_arrow(char *input);
consume_arrow_result consume_right_arrow(char *input);
consume_int_result consume_integer(char *input);
consume_jump_result consume_jump_literal(char *input);

#endif
