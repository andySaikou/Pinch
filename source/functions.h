// functions.h

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "interpreter.h"

Value* ADD(Value **args, int count);
Value* SUB(Value **args, int count);
Value* MUL(Value **args, int count);
Value* DIV(Value **args, int count);
Value* MOD(Value **args, int count);
Value* POW(Value **args, int count);
Value* ABS(Value **args, int count);
Value* SQRT(Value **args, int count);
Value* EQ(Value **args, int count);
Value* NEQ(Value **args, int count);
Value* GT(Value **args, int count);
Value* LT(Value **args, int count);
Value* GTE(Value **args, int count);
Value* LTE(Value **args, int count);
Value* FLOOR(Value **args, int count);
Value* CEIL(Value **args, int count);
Value* ROUND(Value **args, int count);
Value* RAND(Value **args, int count);

Value* UPPER(Value **args, int count);
Value* LOWER(Value **args, int count);
Value* CONCAT(Value **args, int count);
Value* LEN(Value **args, int count);
Value* SUBSTR(Value **args, int count);
Value* CONTAINS(Value **args, int count);
Value* FIND(Value **args, int count);
Value* STR_EQ(Value **args, int count);

#endif