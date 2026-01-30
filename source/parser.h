// parser.h

#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef struct Pinch_Func Pinch_Func;
typedef struct Factors Factors;
typedef struct Factor Factor;
typedef struct Pinch_Var Pinch_Var;
typedef struct Statement Statement;

// <Factor> ::= <num_literal> | '"' <str_literal> '"' | 
//              <jump_literal> | <var-name> | '('<pinch_func>')'
typedef enum {
    FACTOR_NUM,
    FACTOR_STR,
    FACTOR_JUMP,
    FACTOR_VAR,
    FACTOR_FUNC
} factor_type;

enum statement_type {
    PINCH_VAR,
    PINCH_FUNC_S,  // Avoid name collision with factor_type PINCH_FUNC
    FACTOR
};

struct Factor {
    factor_type type;
    union {
        double num;
        char *str;
    
        struct { 
            jump_type type; 
            int lines; 
        } jump;

        char *var;
        Pinch_Func *func;
    } data;
};

struct Factors {
    Factor **items;
    int count;
    int capacity;
};

struct Pinch_Func {
    char *name;
    Factors *factors;
};

struct Pinch_Var {
    char *name;
    Factors *factors;
};

struct Statement {
    enum statement_type type;
    union {
        Pinch_Var *pinch_var;
        Pinch_Func *pinch_func;
        Factor *factor;
    } content;
};

typedef struct {
    bool success;
    Factor *factor;
    char *next_input;
} parse_factor_result;

typedef struct {
    bool success;
    Factors *factors;
    char *next_input;
} parse_factors_result;

typedef struct {
    bool success;
    Pinch_Func *func;
    char *next_input;
} parse_pinch_func_result;

typedef struct {
    bool success;
    Pinch_Var *var;
    char *next_input;
} parse_pinch_var_result;

typedef struct {
    bool success;
    Statement *stmt;
    char *next_input;
} parse_statement_result;

void free_statement(Statement *stmt);
parse_statement_result parse_statement(char *input);

#endif
