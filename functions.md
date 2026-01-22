# Functions

This document details the functions that will be implemented in the interpreter. Functions have expected number of arguements of expected types. Feeding a function with incorrect arguements will produce error.

Because of Pinch's arrow directive syntax, functions can take their parameters from the left or the right or from both sides at the same time. A function application (pinch_func) for a function that takes 3 parameters can be written in any one of the following ways:

```
[a,b,c] -> FOO
[a,b] -> FOO <- c
a -> FOO <- [b,c]
FOO <- [a,b,c] 
```

Functions documented below will follow this format:
`FOO:: [<type_of_a>,<type_of_b>,<type_of_c>] -> <type_of_return>` 

## Functions on number

`ADD:: [Number, Number] -> Number`
Adds two numbers.

`SUB:: [Number, Number] -> Number`
Subtracts the second number from the first.

`MUL:: [Number, Number] -> Number`
Multiplies two numbers.

`DIV:: [Number, Number] -> Number`
Divides the first number by the second. Produces a runtime error on division by zero.

`MOD:: [Number, Number] -> Number`
Returns the remainder of the division of the first number by the second.

`POW:: [Number, Number] -> Number`
Raises the first number to the power of the second number.

`ABS:: Number -> Number`
Returns the absolute (positive) value of a number.

`SQRT:: Number -> Number`
Returns the square root of a number.

`EQ:: [Number, Number] -> Number`
Returns 1 if two numbers are equal, otherwise returns 0.

`NEQ:: [Number, Number] -> Number`
Returns 1 if two numbers are not equal, otherwise returns 0.

`GT:: [Number, Number] -> Number`
Returns 1 if the first number is greater than the second, otherwise returns 0.

`LT:: [Number, Number] -> Number`
Returns 1 if the first number is less than the second, otherwise returns 0.

`GTE:: [Number, Number] -> Number`
Greater than or equal to.

`LTE:: [Number, Number] -> Number`
Less than or equal to.

`FLOOR:: Number -> Number`
Rounds a number down to the nearest integer.

`CEIL:: Number -> Number`
Rounds a number up to the nearest integer.

`ROUND:: Number -> Number`
Rounds to the nearest integer (standard rounding).

`RAND:: [] -> Number`
Returns a random floating-point number between 0.0 and 1.0.

## Functions on text

`UPPER:: [Text] -> Text`
Converts the string to uppercase.

`LOWER:: [Text] -> Text`
Converts the string to lowercase.

`CONCAT:: [Text, Text] -> Text`
Joins two strings together.

`LEN:: [Text] -> Number`
Returns the number of characters in the string.

`SUBSTR:: [Text, Number, Number] -> Text`
Extracts a portion of the string. Parameters are [source, start_Index, length]

`CONTAINS:: [Text, Text] -> Number`
Returns 1 if the second string is found inside the first, otherwise returns 0.

`FIND:: [Text, Text] -> Number`
Finds the first index of the second string inside the first string. Returns -1 if not found.

`STR_EQ:: [Text, Text] -> Number`
Returns 1 if two strings are identical, otherwise returns 0.

## Special functions

`IF:: [Number, Type_A, Type_A] -> Type_A`
Type_A can be Number, Text or Jump. Returns the first parameter of Type_A if the number is greater than 0.5, otherwise returns the second parameter of Type_A.

`JUMP_IF:: [Number, Jump, Jump] -> []`
Perform the first Jump if the number is greater than 0.5, otherwise perform the second Jump. Note: Jump is either `n<=` or `=>m`. `n<=` denotes a backward jump of n lines relative to the current line, and `=>m` denotes a forward jump of m lines relative to the current line.

`SLEEP :: Number -> []`
Sleep for a number of seconds given by the parameter.
