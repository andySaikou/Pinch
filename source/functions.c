// In-built Library Functions

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "functions.h"
#include "util.h" 

// functions.c

// Helper to create error value and print error message
Value* create_type_error(char *func_name, char *expected, ValueType actual) {
    Value *v = value_from_error();
    fprintf(stderr, "Runtime Error [%s]: Expected %s, got type %d.\n", func_name, expected, actual);
    return v;
}

// Helper to validate parameter count and types
Value* validate_args(char *func_name, Value **args, int actual_count, int expected_count, ...) {
    // 1. Check Argument Count
    if (actual_count != expected_count) {
        fprintf(stderr, "Runtime Error [%s]: Expected %d arguments, got %d.\n", 
                func_name, expected_count, actual_count);
        return value_from_error();
    }

    // 2. Check Argument Types
    va_list expected_types;
    va_start(expected_types, expected_count);

    for (int i = 0; i < expected_count; i++) {
        // Retrieve the next expected type from arguments
        ValueType expected = va_arg(expected_types, ValueType);
        
        // If strict match fails
        if (expected != VALUE_ANY && args[i]->type != expected) {
            va_end(expected_types);
            
            // Convert enum to string for the error message
            char *expect_str;
            switch (expected) {
                case VALUE_NUM:  expect_str = "Number"; break;
                case VALUE_STR:  expect_str = "String"; break;
                case VALUE_JUMP: expect_str = "Jump"; break;
                default:         expect_str = "Unknown"; break;
            }
            
            return create_type_error(func_name, expect_str, args[i]->type);
        }
    }

    va_end(expected_types);
    return NULL;    // Successful
}

// ---------------------------------------------------------
// FUNCTIONS ON NUMBER
// ---------------------------------------------------------

// ADD :: [Number, Number] -> Number
Value* ADD(Value **args, int count) {
    Value *err = validate_args("ADD", args, count, 2, VALUE_NUM, VALUE_NUM);
    if (err) return err;
    return value_from_num(args[0]->data.num + args[1]->data.num);
}

// SUB :: [Number, Number] -> Number
Value* SUB(Value **args, int count) {
    Value *err = validate_args("SUB", args, count, 2, VALUE_NUM, VALUE_NUM);
    if (err) return err;
    return value_from_num(args[0]->data.num - args[1]->data.num);
}

// MUL :: [Number, Number] -> Number
Value* MUL(Value **args, int count) {
    Value *err = validate_args("MUL", args, count, 2, VALUE_NUM, VALUE_NUM);
    if (err) return err;
    return value_from_num(args[0]->data.num * args[1]->data.num);
}

// DIV :: [Number, Number] -> Number
Value* DIV(Value **args, int count) {
    Value *err = validate_args("DIV", args, count, 2, VALUE_NUM, VALUE_NUM);
    if (err) return err;

    if (args[1]->data.num == 0) {
        fprintf(stderr, "Runtime Error: Division by zero.\n");
        return value_from_error();
    }

    return value_from_num(args[0]->data.num / args[1]->data.num);
}

// MOD :: [Number, Number] -> Number
Value* MOD(Value **args, int count) {
    Value *err = validate_args("MOD", args, count, 2, VALUE_NUM, VALUE_NUM);
    if (err) return err;

    if (args[1]->data.num == 0) {
        fprintf(stderr, "Runtime Error [MOD]: Division by zero.\n");
        return value_from_error();
    }

    return value_from_num(fmod(args[0]->data.num, args[1]->data.num));
}

// POW :: [Number, Number] -> Number
Value* POW(Value **args, int count) {
    Value *err = validate_args("POW", args, count, 2, VALUE_NUM, VALUE_NUM);
    if (err) return err;
    return value_from_num(pow(args[0]->data.num, args[1]->data.num));
}

// ABS :: Number -> Number
Value* ABS(Value **args, int count) {
    Value *err = validate_args("ABS", args, count, 1, VALUE_NUM);
    if (err) return err;
    return value_from_num(fabs(args[0]->data.num));
}

// SQRT :: Number -> Number
Value* SQRT(Value **args, int count) {
    Value *err = validate_args("SQRT", args, count, 1, VALUE_NUM);
    if (err) return err;

    if (args[0]->data.num < 0) {
        fprintf(stderr, "Runtime Error: Square root of negative number.\n");
        return value_from_error();
    }

    return value_from_num(sqrt(args[0]->data.num));
}

// EQ :: [Number, Number] -> Number
Value* EQ(Value **args, int count) {
    Value *err = validate_args("EQ", args, count, 2, VALUE_NUM, VALUE_NUM);
    if (err) return err;
    return value_from_num((args[0]->data.num == args[1]->data.num) ? 1.0 : 0.0);
}

// NEQ :: [Number, Number] -> Number
Value* NEQ(Value **args, int count) {
    Value *err = validate_args("NEQ", args, count, 2, VALUE_NUM, VALUE_NUM);
    if (err) return err;
    return value_from_num((args[0]->data.num != args[1]->data.num) ? 1.0 : 0.0);
}

// GT :: [Number, Number] -> Number
Value* GT(Value **args, int count) {
    Value *err = validate_args("GT", args, count, 2, VALUE_NUM, VALUE_NUM);
    if (err) return err;
    return value_from_num((args[0]->data.num > args[1]->data.num) ? 1.0 : 0.0);
}

// LT :: [Number, Number] -> Number
Value* LT(Value **args, int count) {
    Value *err = validate_args("LT", args, count, 2, VALUE_NUM, VALUE_NUM);
    if (err) return err;
    return value_from_num((args[0]->data.num < args[1]->data.num) ? 1.0 : 0.0);
}

// GTE :: [Number, Number] -> Number
Value* GTE(Value **args, int count) {
    Value *err = validate_args("GTE", args, count, 2, VALUE_NUM, VALUE_NUM);
    if (err) return err;
    return value_from_num((args[0]->data.num >= args[1]->data.num) ? 1.0 : 0.0);
}

// LTE :: [Number, Number] -> Number
Value* LTE(Value **args, int count) {
    Value *err = validate_args("LTE", args, count, 2, VALUE_NUM, VALUE_NUM);
    if (err) return err;
    return value_from_num((args[0]->data.num <= args[1]->data.num) ? 1.0 : 0.0);
}

// FLOOR :: Number -> Number
Value* FLOOR(Value **args, int count) {
    Value *err = validate_args("FLOOR", args, count, 1, VALUE_NUM);
    if (err) return err;
    return value_from_num(floor(args[0]->data.num));
}

// CEIL :: Number -> Number
Value* CEIL(Value **args, int count) {
    Value *err = validate_args("CEIL", args, count, 1, VALUE_NUM);
    if (err) return err;
    return value_from_num(ceil(args[0]->data.num));
}

// ROUND :: Number -> Number
Value* ROUND(Value **args, int count) {
    Value *err = validate_args("ROUND", args, count, 1, VALUE_NUM);
    if (err) return err;
    return value_from_num(round(args[0]->data.num));
}

// RAND :: [] -> Number
Value* RAND(Value **args, int count) {
    Value *err = validate_args("RAND", args, count, 0);
    if (err) return err;
    
    // Returns double between 0.0 and 1.0
    return value_from_num((double)rand() / (double)RAND_MAX);
}

// ---------------------------------------------------------
// FUNCTIONS ON TEXT
// ---------------------------------------------------------

// UPPER :: [Text] -> Text
Value* UPPER(Value **args, int count) {
    Value *err = validate_args("UPPER", args, count, 1, VALUE_STR);
    if (err) return err;

    char *source = args[0]->data.str;
    char *upper_str = xalloc(strlen(source) + 1, "Runtime Error: Memory allocation failed for UPPER.");
    
    // Iterate and convert
    for (int i = 0; source[i]; i++) {
        upper_str[i] = toupper((unsigned char)source[i]);
    }
    upper_str[strlen(source)] = '\0';

    Value *result = value_from_str(upper_str);
    xfree(upper_str);
    return result;
}

// LOWER :: [Text] -> Text
Value* LOWER(Value **args, int count) {
    Value *err = validate_args("LOWER", args, count, 1, VALUE_STR);
    if (err) return err;

    char *source = args[0]->data.str;
    char *lower_str = xalloc(strlen(source) + 1, "Runtime Error: Memory allocation failed for LOWER.");

    for (int i = 0; source[i]; i++) {
        lower_str[i] = tolower((unsigned char)source[i]);
    }
    lower_str[strlen(source)] = '\0';

    Value *result = value_from_str(lower_str);
    xfree(lower_str);
    return result;
}

// CONCAT :: [Text, Text] -> Text
Value* CONCAT(Value **args, int count) {
    Value *err = validate_args("CONCAT", args, count, 2, VALUE_STR, VALUE_STR);
    if (err) return err;

    char *s1 = args[0]->data.str;
    char *s2 = args[1]->data.str;

    size_t new_len = strlen(s1) + strlen(s2) + 1;
    char *combined = xalloc(new_len, "Runtime Error: Memory allocation failed for CONCAT.");

    strcpy(combined, s1);
    strcat(combined, s2);

    Value *result = value_from_str(combined);
    xfree(combined);
    return result;
}

// LEN :: [Text] -> Number
Value* LEN(Value **args, int count) {
    Value *err = validate_args("LEN", args, count, 1, VALUE_STR);
    if (err) return err;

    return value_from_num((double)strlen(args[0]->data.str));
}

// SUBSTR :: [Text, Number, Number] -> Text
// Parameters: [source, start_index, length]
Value* SUBSTR(Value **args, int count) {
    Value *err = validate_args("SUBSTR", args, count, 3, VALUE_STR, VALUE_NUM, VALUE_NUM);
    if (err) return err;

    char *source = args[0]->data.str;
    int start = (int)args[1]->data.num;
    int length = (int)args[2]->data.num;
    int source_len = (int)strlen(source);

    // Bounds checking
    if (start < 0 || start >= source_len || length < 0) {
        return value_from_str("");
    }

    // Adjust length if it goes past the end
    if (start + length > source_len) {
        length = source_len - start;
    }

    char *sub = xalloc(length + 1, "Interpreter Error: Fail to allocate memory.\n");
    strncpy(sub, source + start, length);
    sub[length] = '\0';

    Value *result = value_from_str(sub);
    xfree(sub);
    return result;
}

// CONTAINS :: [Text, Text] -> Number
Value* CONTAINS(Value **args, int count) {
    Value *err = validate_args("CONTAINS", args, count, 2, VALUE_STR, VALUE_STR);
    if (err) return err;

    char *haystack = args[0]->data.str;
    char *needle = args[1]->data.str;

    // strstr returns pointer to found string or NULL
    if (strstr(haystack, needle) != NULL) {
        return value_from_num(1.0);
    } else {
        return value_from_num(0.0);
    }
}

// FIND :: [Text, Text] -> Number
// Finds index of second string in first. Returns -1 if not found.
Value* FIND(Value **args, int count) {
    Value *err = validate_args("FIND", args, count, 2, VALUE_STR, VALUE_STR);
    if (err) return err;

    char *haystack = args[0]->data.str;
    char *needle = args[1]->data.str;

    char *found_ptr = strstr(haystack, needle);

    if (found_ptr != NULL) {
        // Calculate index using pointer arithmetic
        int index = (int)(found_ptr - haystack);
        return value_from_num((double)index);
    } else {
        return value_from_num(-1.0);
    }
}

// STR_EQ :: [Text, Text] -> Number
Value* STR_EQ(Value **args, int count) {
    Value *err = validate_args("STR_EQ", args, count, 2, VALUE_STR, VALUE_STR);
    if (err) return err;

    if (strcmp(args[0]->data.str, args[1]->data.str) == 0) {
        return value_from_num(1.0);
    } else {
        return value_from_num(0.0);
    }
}

// ---------------------------------------------------------
// SPECIAL FUNCTIONS
// ---------------------------------------------------------

// IF:: [Number, Type_A, Type_A] -> Type_A
Value* IF(Value **args, int count) {
    Value *err = validate_args("IF", args, count, 3, VALUE_NUM, VALUE_ANY, VALUE_ANY);
    if (err) return err;

    if (args[0]->data.num >= 0.5) {
        return copy_value(args[1]);
    } else {
        return copy_value(args[2]);
    }
}

// SLEEP :: Number -> []
Value* SLEEP(Value **args, int count) {
    Value *err = validate_args("SLEEP", args, count, 1, VALUE_NUM);
    if (err) return err;

    unsigned int seconds = (unsigned int)args[0]->data.num;
    sleep(seconds);
    return value_from_none();
}
