// Logic for language syntax parser

#include "lexer.h"
#include "parser.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>

// Helper to allocate memory for a factor
Factor* create_factor(factor_type type) {
    Factor *f = xalloc(sizeof(Factor), 
                        "Interpreter Error: Fail to allocate memory while parsing factor\n");
    f->type = type;
    return f;
}


// Forward declarations
void free_pinch_func(Pinch_Func *pinch_func);
parse_pinch_func_result parse_pinch_func(char *input);

// Helper to free up a Factor struct
void free_factor(Factor *f) {
    if (f == NULL) return;

    switch (f->type) {
        case FACTOR_STR:
            xfree(f->data.str);
            break;
        case FACTOR_VAR:
            xfree(f->data.var);
            break;
        case FACTOR_FUNC:
            free_pinch_func(f->data.func);
            break;
        case FACTOR_NUM:
        case FACTOR_JUMP:
            break;
    }
    xfree(f);
}

// Helper to free up a Factors struct
void free_factors(Factors *factors){
    if (factors != NULL) {
        if (factors->items != NULL) {
            for(int i=0; i<factors->count; i++) {
                xfree(factors -> items[i]);
            }
            xfree(factors->items);
        }
        xfree(factors);
    }
}

// Helper to free up a Pinch_Func struct
void free_pinch_func(Pinch_Func *pinch_func) {
    if (pinch_func != NULL) {
        xfree(pinch_func->name);
        free_factors(pinch_func->factors);
    }
}

// Helper to free up a Pinch_Var struct
void free_pinch_var(Pinch_Var *pinch_var) {
    if (pinch_var != NULL) {
        xfree(pinch_var->name);
        free_factors(pinch_var->factors);
    }
}

// Helper to free up a Statement struct
void free_statement(Statement *stmt) {
    if (stmt != NULL) {
        if (stmt->type == PINCH_VAR) {
            free_pinch_var(stmt->content.pinch_var);
        } else if (stmt->type == PINCH_FUNC_S) {
            free_pinch_func(stmt->content.pinch_func);
        } else {
            free_factor(stmt->content.factor);
        }
        xfree(stmt);
    }
}

// <factor> ::= <num_literal> | '"' <str_literal> '"' | <jump_literal> | <var-name> | '('<pinch_func>')'
parse_factor_result parse_factor(char *input) {
    char *current_input = skip_whitespace(input);

    // Case 1: Nested function application '(' <pinch_func> ')'
    consume_token_result paren_result = consume_token(TOKEN_OPEN_PAREN, current_input);
    if (paren_result.success) {

        parse_pinch_func_result func_result = parse_pinch_func(paren_result.next_input);

        // If parse pinch_func unsuccessful, return parse unsuccessful
        if (!func_result.success) {
            return (parse_factor_result){false, NULL, input};
        }

        // Try to consume closing ')'
        current_input = skip_whitespace(func_result.next_input);
        consume_token_result paren_result = consume_token(TOKEN_CLOSE_PAREN, current_input);

        // If cannot consume ')', return parse unsuccessful
        if (!paren_result.success) {
            free_factors(func_result.func->factors);
            xfree(func_result.func->name);
            xfree(func_result.func);
            return (parse_factor_result){false, NULL, input};
        }

        Factor *factor = create_factor(FACTOR_FUNC);
        factor->data.func = func_result.func;
        return (parse_factor_result){true, factor, paren_result.next_input};
    }

    // Case 2: Jump Literal
    consume_jump_result jump_res = consume_jump_literal(current_input);
    if (jump_res.success) {
        Factor *factor = create_factor(FACTOR_JUMP);
        factor->data.jump.type = jump_res.type;
        factor->data.jump.lines = jump_res.lines;
        return (parse_factor_result){true, factor, jump_res.next_input};
    }

    // Case 3: Number Literal
    consume_num_result num_result = consume_num_literal(current_input);
    if (num_result.success) {
        Factor *factor = create_factor(FACTOR_NUM);
        factor->data.num = num_result.number;
        return (parse_factor_result){true, factor, num_result.next_input};
    }

    // Case 4: String Literal
    consume_name_result str_result = consume_str_literal(current_input);
    if (str_result.success) {
        Factor *factor = create_factor(FACTOR_STR);
        factor->data.str = str_result.name;
        return (parse_factor_result){true, factor, str_result.next_input};
    }

    // Case 5: Variable Name
    consume_name_result var_res = consume_var_name(current_input);
    if (var_res.success) {
        Factor *factor = create_factor(FACTOR_VAR);
        factor->data.var = var_res.name;
        return (parse_factor_result){true, factor, var_res.next_input};
    }

    // No Match Found
    return (parse_factor_result){false, NULL, input};
}

// <factors> ::= '[' (<factor>',')* <factor> ']' | <factor>
parse_factors_result parse_factors(char *input) {

    char *current_input = skip_whitespace(input);

    Factors *factors = xalloc(sizeof(Factors), "Interpreter Error: Fail to allocate memory while parsing factors.\n");
    // Allocate 5 times size of Factor to the list of Factor in factors, sufficient for all in-built functions
    factors -> items = xalloc(5 * sizeof(Factor*), "Interpreter Error: Fail to allocate memory while parsing factors.\n");
    factors -> count = 0;
    factors -> capacity = 5;

    consume_token_result paren_result = consume_token(TOKEN_OPEN_SQUARE_BRACKET, current_input);

    // Case 1: '[' (<factor>',')* <factor> ']'
    if (paren_result.success) {
        current_input = paren_result.next_input;

        while (true) {
            current_input = skip_whitespace(current_input);
            parse_factor_result factor_result = parse_factor(current_input);

            if (factor_result.success) {
                current_input = factor_result.next_input;

                // If factors' list is full, resize list
                if (factors->count >= factors->capacity) {
                    factors -> items = realloc(factors->items, 2 * factors->capacity * sizeof(Factor*));
                    if (factors->items == NULL) {
                        fprintf(stderr, "Interpreter Error: Fail to allocate memory while parsing factors.\n");
                        exit(EXIT_FAILURE);
                    }
                    factors -> capacity *= 2;
                }

                // Append to the factor items list
                factors -> items[factors->count] = factor_result.factor;
                factors -> count ++;
                
                // Try to consume ',' separator if 
                current_input = skip_whitespace(current_input);
                consume_token_result comma_result = consume_token(TOKEN_COMMA, current_input);

                if (comma_result.success) {
                    current_input = comma_result.next_input;
                } else {
                    break;
                }
                current_input = skip_whitespace(current_input);

            } 
            // Cannot parse current input as a factor, return parse unsuccessful
            else {
                free_factors(factors);
                return (parse_factors_result){false, NULL, input};
            }
        }

        // Try to consume the closing ']'
        current_input = skip_whitespace(current_input);
        paren_result = consume_token(TOKEN_CLOSE_SQUARE_BRACKET, current_input);

        if (paren_result.success) {
            return (parse_factors_result){true, factors, paren_result.next_input};
        } else {
            // No closing ']' found, return parse unsuccessful
            free_factors(factors);
            return (parse_factors_result){false, NULL, input};
        }
    }

    // Case 2: <factor>
    else {
        parse_factor_result factor_result = parse_factor(current_input);
        if (factor_result.success) {
            factors -> items[0] = factor_result.factor;
            factors -> count = 1;
            return (parse_factors_result){true, factors, factor_result.next_input};
        } else {
            free_factors(factors);
            return (parse_factors_result){false, NULL, input};
        }
    }
}

// <left_pinchs> ::= <factors> <right_arrow>
parse_factors_result parse_left_pinch(char *input) {
    
    // Try to parse factors
    parse_factors_result factors_result = parse_factors(input);

    if (factors_result.success) {
        // If successful, try to consume right arrow
        char *current_input = skip_whitespace(factors_result.next_input);   
        consume_arrow_result arrow_result = consume_right_arrow(current_input);

        if (arrow_result.success) {
            return (parse_factors_result){true, factors_result.factors, arrow_result.next_input};
        } else {
            free_factors(factors_result.factors);
            return (parse_factors_result){false, NULL, input};
        }
    } 
    
    else {
        return (parse_factors_result){false, NULL, input};
    }
}

// <right_pinchs> ::= <left_arrow> <factors>
parse_factors_result parse_right_pinch(char *input) {
    
    // Try to consume left arrow
    char *current_input = skip_whitespace(input);   
    consume_arrow_result arrow_result = consume_left_arrow(current_input);

    if (arrow_result.success) {
        // If successful, try to parse factors
        parse_factors_result factors_result = parse_factors(arrow_result.next_input);

        if (factors_result.success) {
            return (parse_factors_result){true, factors_result.factors, factors_result.next_input};
        } else {
            return (parse_factors_result){false, NULL, input};
        }
    }

    else {
        return (parse_factors_result){false, NULL, input};
    }
}

// <pinch_func> ::= <left_pinchs>? <func_name> <right_pinchs>?
parse_pinch_func_result parse_pinch_func(char *input) {

    // Try to parse left pinch
    parse_factors_result left_pinch_result = parse_left_pinch(input);
    // Try to consume func_name
    consume_name_result func_name_result = consume_func_name(left_pinch_result.next_input);

    if (func_name_result.success) {
        // Try to parse right pinch
        parse_factors_result right_pinch_result = parse_right_pinch(func_name_result.next_input);
        Factors *final_factors = NULL;

        // Case 1: Both left and right pinch exist
        if (left_pinch_result.success && right_pinch_result.success) {
            final_factors = left_pinch_result.factors;
            Factors *r_factors = right_pinch_result.factors;

            // expand left pinch capacity to fit right pinch
            final_factors->capacity += r_factors->count; 
            final_factors->items = realloc(final_factors->items, final_factors->capacity * sizeof(Factor*));
            
            if (final_factors->items == NULL) {
                fprintf(stderr, "Interpreter Error: Fail to allocate memory while parsing function.\n");
                exit(EXIT_FAILURE);
            }

            // copy pointers from right pinch to left pinch
            for (int i = 0; i < r_factors->count; i++) {
                final_factors->items[final_factors->count] = r_factors->items[i];
                final_factors->count++;
            }

            xfree(r_factors->items);
            xfree(r_factors);
        }
        // Case 2: only left pinch exists
        else if (left_pinch_result.success) {
            final_factors = left_pinch_result.factors;
        }
        // Case 3: only right pinch exists
        else if (right_pinch_result.success) {
            final_factors = right_pinch_result.factors;
        }
        // Case 4: no function argument
        else {
            final_factors = xalloc(sizeof(Factors), "Interpreter Error: Fail to allocate memory while parsing function.\n");
            final_factors->items = NULL;
            final_factors->count = 0;
            final_factors->capacity = 0;
        }

        // Create Pinch_Func struct for return
        Pinch_Func *pinch_func = xalloc(sizeof(Pinch_Func), "Interpreter Error: Fail to allocate memory while parsing function.\n");
        pinch_func->name = func_name_result.name;
        pinch_func->factors = final_factors;

        char *final_input = right_pinch_result.success ? right_pinch_result.next_input : func_name_result.next_input;
        return (parse_pinch_func_result){true, pinch_func, final_input};
    }

    // If consume func_name unsuccessful, return parse unsuccessful
    else {
        free_factors(left_pinch_result.factors);
        return (parse_pinch_func_result){false, NULL, input};
    }
}

// <pinch_var> ::= <left_pinch> <var_name> | <var_name> <right_pinch> | <var_name>
// <left_pinch> ::= <factor> <right_arrow>
// <right_pinch> ::= <left_arrow> <factor>
// pinch_var strictly allows only one parameter
parse_pinch_var_result parse_pinch_var(char *input) {
    
    // Try to parse left pinch
    parse_factors_result left_pinch_result = parse_left_pinch(input);
    consume_name_result var_name_result = consume_var_name(left_pinch_result.next_input);

    if (var_name_result.success) {
        // If successfully parse both left pinch and var name
        if (left_pinch_result.success) {
            // Enforce only one parameter
            if (left_pinch_result.factors->count == 1) {
                Pinch_Var *pinch_var = xalloc(sizeof(Pinch_Var), "Interpreter Error: Fail to allocate memory while parsing variable assignment.\n");
                pinch_var -> name = var_name_result.name;
                pinch_var -> factors = left_pinch_result.factors;
                return (parse_pinch_var_result){true, pinch_var, var_name_result.next_input};
            } else {
                free_factors(left_pinch_result.factors);
                return (parse_pinch_var_result){false, NULL, input};
            }
        }
        // If var name successfully, and left pinch not parsed
        else {
            parse_factors_result right_pinch_result = parse_right_pinch(var_name_result.next_input);

            if (right_pinch_result.success) {
                // Enforce only one parameter
                if (right_pinch_result.factors->count == 1) {
                    Pinch_Var *pinch_var = xalloc(sizeof(Pinch_Var), "Interpreter Error: Fail to allocate memory while parsing variable assignment.\n");
                    pinch_var -> name = var_name_result.name;
                    pinch_var -> factors = right_pinch_result.factors;
                    return (parse_pinch_var_result){true, pinch_var, right_pinch_result.next_input};
                } else {
                    free_factors(right_pinch_result.factors);
                    return (parse_pinch_var_result){false, NULL, input};
                }
            } else {
                xfree(var_name_result.name);
                return (parse_pinch_var_result){false, NULL, input};
            }
        }
    }

    // If consume var_name unsuccessful, return parse unsuccessful
    else {
        free_factors(left_pinch_result.factors);
        return (parse_pinch_var_result){false, NULL, input};
    }
}

// <statement> ::= ( <pinch_var> | <pinch_func> | <factor> ) '\eol'
parse_statement_result parse_statement(char *input) {
    char *current_input = skip_whitespace(input);
    Statement *stmt = xalloc(sizeof(Statement), "Interpreter Error: Fail to allocate memory while parsing statement.\n");
    
    // We need a temporary pointer to track where the logic ends
    // so we can check for the newline character afterwards.
    char *after_logic_input = NULL;

    // Case 1: Variable assignment
    parse_pinch_var_result var_res = parse_pinch_var(current_input);
    if (var_res.success) {
        stmt->type = PINCH_VAR;
        stmt->content.pinch_var = var_res.var;
        after_logic_input = var_res.next_input;
    }

    else {
        // Case 2: Function call
        parse_pinch_func_result func_res = parse_pinch_func(current_input);
        if (func_res.success) {
            stmt->type = PINCH_FUNC_S;
            stmt->content.pinch_func = func_res.func;
            after_logic_input = func_res.next_input;
        }

        else {
            // Case 3: Factor
            parse_factor_result factor_res = parse_factor(current_input);
            if (factor_res.success) {
                stmt->type = FACTOR;
                stmt->content.factor = factor_res.factor;
                after_logic_input = factor_res.next_input;
            } 
            else {
                // Case 4: No match found
                xfree(stmt);
                return (parse_statement_result){false, NULL, input};
            }
        }
    }

    while (*after_logic_input == ' ' || *after_logic_input == '\t') {
        after_logic_input++;
    }

    // Comments are valid at the end of a line
    if (*after_logic_input == '#') {
        while (*after_logic_input != '\n' && *after_logic_input != '\0') {
            after_logic_input++;
        }
    }

    if (*after_logic_input == '\n') {
        // End of line, move to next line
        return (parse_statement_result){true, stmt, after_logic_input + 1};
    } 
    else if (*after_logic_input == '\0') {
        // End of file
        return (parse_statement_result){true, stmt, after_logic_input};
    } 
    else {
        xfree(stmt);
        return (parse_statement_result){false, NULL, input};
    }
}
