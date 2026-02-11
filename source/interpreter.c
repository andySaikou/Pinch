// Logic for the Pinch language interpreter

#include "interpreter.h"
#include "functions.h"
#include <stdlib.h>
#include <stdio.h>

Value* evaluate_factor(Factor *factor, MachineState *state);
Value* evaluate_function(Pinch_Func *func, MachineState *state);

Value* value_from_none() {
    Value *v = xalloc(sizeof(Value), "Interpreter Error: Fail to allocate memory.\n");
    v->type = VALUE_NONE;
    return v;
}

Value* value_from_error() {
    Value *v = xalloc(sizeof(Value), "Interpreter Error: Fail to allocate memory.\n");
    v->type = VALUE_ERROR;
    return v;
}

Value* value_from_num(double num) {
    Value *v = xalloc(sizeof(Value), "Interpreter Error: Fail to allocate memory.\n");
    v->type = VALUE_NUM;
    v->data.num = num;
    return v;
}

Value* value_from_str(char *s) {
    Value *v = xalloc(sizeof(Value), "Interpreter Error: Fail to allocate memory.\n");
    v->type = VALUE_STR;
    // Duplicate the string so the Value owns its own memory
    v->data.str = xalloc(strlen(s) + 1, "Interpreter Error: Fail to allocate memory.\n");
    strcpy(v->data.str, s);
    return v;
}

Value* value_from_jump(int lines, jump_type type) {
    Value *v = xalloc(sizeof(Value), "Interpreter Error: Fail to allocate memory.\n");
    v->type = VALUE_JUMP;
    v->data.jump.lines = lines;
    v->data.jump.type = type;
    return v;
}

Value* copy_value(Value *v) {
    if (v == NULL) return NULL;

    switch (v->type) {
        case VALUE_NUM:
            return value_from_num(v->data.num);  
        case VALUE_STR:
            return value_from_str(v->data.str);
        case VALUE_JUMP:
            return value_from_jump(v->data.jump.lines, v->data.jump.type);
        case VALUE_NONE: 
            return value_from_none();
        case VALUE_ERROR:
            return value_from_error();
        default:
            return NULL;
    }
}

void free_value(Value *v) {
    if (v == NULL) return;

    switch (v->type) {
        case VALUE_STR:
            xfree(v->data.str);
            break;     
        case VALUE_NUM:
        case VALUE_JUMP:
        case VALUE_NONE:
        case VALUE_ERROR:
        case VALUE_ANY:
            break;
    }
    xfree(v);
}

void print_value(Value *value) {
    switch (value->type) {
        case VALUE_NUM:
            printf("%f\n", value->data.num);
            break;
        case VALUE_STR:
            printf("%s\n", value->data.str);
            break;
        case VALUE_JUMP:
            if (value->data.jump.type == JUMP_BACKWARD) {
                printf("%d <=\n", value->data.jump.lines);
            } else {
                printf("=> %d\n", value->data.jump.lines);
            }
            break;
        default:
            break;
    }
}

Value* evaluate_function(Pinch_Func *func, MachineState *state) {
    int count = func->factors->count;
    
    // Allocate a temporary array for evaluated arguments
    Value **args = xalloc(sizeof(Value*) * count, "Interpreter Error: Fail to allocate memory.\n");

    // Evaluate all arguments
    for (int i = 0; i < count; i++) {
        args[i] = evaluate_factor(func->factors->items[i], state);
        
        // If an argument fails, stop immediately.
        if (args[i]->type == VALUE_ERROR) {
            // Clean up the arguments evaluated so far
            for (int j = 0; j < i; j++) free_value(args[j]);
            xfree(args);
            return value_from_error();
        }
    }

    Value *result = NULL;
    char *name = func->name;

    // Call library function
    if (strcmp(name, "ADD") == 0) result = ADD(args, count);
    else if (strcmp(name, "SUB") == 0) result = SUB(args, count);
    else if (strcmp(name, "MUL") == 0) result = MUL(args, count);
    else if (strcmp(name, "DIV") == 0) result = DIV(args, count);
    else if (strcmp(name, "MOD") == 0) result = MOD(args, count);
    else if (strcmp(name, "POW") == 0) result = POW(args, count);
    else if (strcmp(name, "ABS") == 0) result = ABS(args, count);
    else if (strcmp(name, "SQRT") == 0) result = SQRT(args, count);
    else if (strcmp(name, "EQ") == 0) result = EQ(args, count);
    else if (strcmp(name, "NEQ") == 0) result = NEQ(args, count);
    else if (strcmp(name, "GT") == 0) result = GT(args, count);
    else if (strcmp(name, "LT") == 0) result = LT(args, count);
    else if (strcmp(name, "GTE") == 0) result = GTE(args, count);
    else if (strcmp(name, "LTE") == 0) result = LTE(args, count);
    else if (strcmp(name, "FLOOR") == 0) result = FLOOR(args, count);
    else if (strcmp(name, "CEIL") == 0) result = CEIL(args, count);
    else if (strcmp(name, "ROUND") == 0) result = ROUND(args, count);
    else if (strcmp(name, "RAND") == 0) result = RAND(args, count);

    else if (strcmp(name, "UPPER") == 0) result = UPPER(args, count);
    else if (strcmp(name, "LOWER") == 0) result = LOWER(args, count);
    else if (strcmp(name, "CONCAT") == 0) result = CONCAT(args, count);
    else if (strcmp(name, "LEN") == 0) result = LEN(args, count);
    else if (strcmp(name, "SUBSTR") == 0) result = SUBSTR(args, count);
    else if (strcmp(name, "CONTAINS") == 0) result = CONTAINS(args, count);
    else if (strcmp(name, "FIND") == 0) result = FIND(args, count);
    else if (strcmp(name, "STR_EQ") == 0) result = STR_EQ(args, count);
    
    // Special functions
    else if (strcmp(name, "IF") == 0) result = IF(args, count);
    else if (strcmp(name, "SLEEP") == 0) result = SLEEP(args, count);

    // JUMP:: Jump -> []
    else if (strcmp(name, "JUMP") == 0) {
        if (count != 1 || args[0]->type != VALUE_JUMP) {
            fprintf(stderr, "Runtime Error: JUMP expects 1 Jump argument.\n");
            result = value_from_error();
        } else {
            int lines = args[0]->data.jump.lines;
            if (args[0]->data.jump.type == JUMP_FORWARD) {
                state->program_counter += lines;
            } else {
                state->program_counter -= lines;
            }
            // Offset upcoming program_counter increment in the main loop
            state->program_counter--; 
            result = value_from_none();
        }
    }
    // JUMP_IF:: [Number, Jump, Jump] -> []
    else if (strcmp(name, "JUMP_IF") == 0) {
        if (count != 3 || args[0]->type != VALUE_NUM || args[1]->type != VALUE_JUMP) {
            fprintf(stderr, "Runtime Error: JUMP_IF expects [Number, Jump].\n");
            result = value_from_error();
        } else {
            if (args[0]->data.num >= 0.5) {
                int lines = args[1]->data.jump.lines;
                if (args[1]->data.jump.type == JUMP_FORWARD) {
                    state->program_counter += lines;
                } else {
                    state->program_counter -= lines;
                }
                // Offset upcoming program_counter increment in the main loop
                state->program_counter--;
            }
            result = value_from_none();
        }
    }
    else {
        fprintf(stderr, "Runtime Error: Unknown function '%s'\n", name);
        result = value_from_error();
    }

    // Clean up temporary argument Values
    for (int i = 0; i < count; i++) {
        free_value(args[i]);
    }
    xfree(args);
    return result;
}

Value* evaluate_factor(Factor *factor, MachineState *state) {
    switch (factor->type) {
        case FACTOR_NUM:
            return value_from_num(factor->data.num);
            
        case FACTOR_STR:
            return value_from_str(factor->data.str);
            
        case FACTOR_JUMP:
            return value_from_jump(factor->data.jump.lines, factor->data.jump.type);

        case FACTOR_VAR: {
            // Hashmap lookup
            Value *val = (Value*)hashmap_lookup(state->variables, factor->data.var);
            if (val == NULL) {
                fprintf(stderr, "Runtime Error: Undefined variable '%s'\n", factor->data.var);
                return value_from_error();
            }
            return copy_value(val); 
        }

        case FACTOR_FUNC:
            return evaluate_function(factor->data.func, state);
            
        default:
            return value_from_error();
    }
}

bool interpret_factor(Factor *factor, MachineState *state) {
    Value *result = evaluate_factor(factor, state);

    if (result->type == VALUE_ERROR) {
        free_value(result);
        return false;
    }
    print_value(result);
    free_value(result);
    return true;
}

bool interpret_variable(Pinch_Var *var_assign, MachineState *state) {
    // Evaluate factor
    Value *result = evaluate_factor(var_assign->factors->items[0], state);

    // If evaluation failed, free and return unsuccessful
    if (result->type == VALUE_ERROR) {
        free_value(result);
        return false;
    } 

    // If variable exists, delete old value first
    Value *old_value = (Value*)hashmap_lookup(state->variables, var_assign->name);
    if (old_value) {
        hashmap_delete(state->variables, var_assign->name);
        free_value(old_value);
    }
    // Insert new hashmap entry
    char *key = xalloc(strlen(var_assign->name) + 1, "Interpreter Error: Fail to allocate memory.\n");
    strcpy(key, var_assign->name);
    hashmap_insert(state->variables, key, result);
    return true;
}

bool interpret_function(Pinch_Func *function, MachineState *state) {
    Value *result = evaluate_function(function, state);

    if (result->type == VALUE_ERROR) {
        free_value(result);
        return false;
    }

    print_value(result);
    free_value(result);
    return true;
}

bool interpret_line(Statement *line, MachineState *state, bool interactive) {

    bool interpret_success;

    switch (line->type) {
        case FACTOR:
            interpret_success = interpret_factor(line->content.factor, state);
            break;
        case PINCH_VAR:
            interpret_success = interpret_variable(line->content.pinch_var, state);
            break;
        case PINCH_FUNC_S: {
            char *func_name = line->content.pinch_func->name;
            // JUMP and JUMP_IF are disabled in interactive mode
            if (interactive) {
                if (strcmp(func_name, "JUMP") == 0) {
                    fprintf(stderr, "Interpreter Constraint: JUMP cannot be used in interactive mode.\n");
                    return false;
                } else if (strcmp(func_name, "JUMP_IF") == 0) {
                    fprintf(stderr, "Interpreter Constraint: JUMP_IF cannot be used in interactive mode.\n");
                    return false;
                }
            }
            interpret_success = interpret_function(line->content.pinch_func, state);
            break;
        }
    }
    return interpret_success;
}
