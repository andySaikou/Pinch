// interpreter.h

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.h"
#include "hashmap.h"

typedef enum {
    VALUE_NUM,
    VALUE_STR,
    VALUE_JUMP,
    VALUE_NONE,
    VALUE_ERROR,
    VALUE_ANY
} ValueType;

typedef struct {
    ValueType type;
    union {
        double num;
        char *str;
        struct {
            jump_type type;
            int lines;
        } jump;
    } data;
} Value;

typedef struct {
    int program_counter;
    Statement **statements;
    int stmt_count;
    struct hashmap *variables;
} MachineState;


Value* value_from_none();
Value* value_from_error();
Value* value_from_num(double num);
Value* value_from_str(char *s);
Value* value_from_jump(int lines, jump_type type);
Value* copy_value(Value *v);
void free_value(Value *v);

bool interpret_line(Statement *line, MachineState *state, bool interactive);

#endif
