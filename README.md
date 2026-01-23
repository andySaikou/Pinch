# Pinch

Pinch is a simple language based solely on dataflow that is represented by arrows. This repository will be an interpreter written in C that interpretes Pinch.

## Example programs

Calculate the hypotenuse of a right triangle:
```
a <- 3
b <- 4

(a -> MUL <- a) -> sq_a
(b -> MUL <- b) -> sq_b
(sq_a -> ADD <- sq_b) -> sum
(sum -> SQRT) -> result

result  #output: 5
```

Generate the first 10 numbers of the Fibonacci sequence:
```
0 -> a
1 -> b
10 -> count

a   #output: 0, 1, 1, 2...
(a -> ADD <- b) -> next

b -> a
next -> b
(count -> SUB <- 1) -> count

(count -> GT <- 0) -> check
[check, 6<=, =>1] -> JUMP_IF
```

Take a input string, check for a specific keyword, and output a status message:
```
"Pinch Language" -> raw_input
raw_input -> UPPER -> upper_input

"LANGUAGE" -> keyword

(upper_input -> CONTAINS <- keyword) -> is_found
(is_found -> IF <- ["Keyword Detected", "Keyword Not Found"]) -> status_msg

(status_msg -> CONCAT <- " in ") -> prefix
(prefix -> CONCAT <- upper_input) -> final_report

final_report    #output: "Keyword Detected in PINCH LANGUAGE"
```

## Syntax

Using the BNF syntax:

- `(x-y)` stands for ‘range’, meaning any value from `x` to `y` inclusive;
- `(x)?` stands for ‘optional’, meaning that `x` can occur zero or one times;
- `(x)+` stands for ‘repeatable’, meaning that `x` can occur one or more times;
- `(x)*` stands for ‘optional and repeatable’, meaning that `x` can occur zero or more times.

```
<small_letter> ::= ('a'-'z')
<big_letter> ::= ('A'-'Z')
<char> ::= any ASCII char excluding '"'
<digit> ::= ('0'-'9')
<num_literal> ::= ('-')? <digit>+ ('.' <digit>+)?
<str_literal> ::= <char>* 

<left_double_arrow> ::= '<='
<right_doule_arrow> ::= '=>'
<jump_literal> ::= <digit>+ <left_double_arrow> | <right_doule_arrow> <digit>+

<var_name> ::= <small_letter> (<small_letter> | '_')*
<func_name> ::= <big_letter> (<big_letter> | '_')*

<factor> ::= <num_literal> | '"' <str_literal> '"' | <jump_literal> | <var-name> | '('<pinch_func>')'
<factors> ::= '[' (<factor>',')* <factor> ']' | <factor>

<left_arrow> ::= '<-'
<right_arrow> ::= '->'
<left_pinch> ::= <factors> <right_arrow>
<right_pinch> ::= <left_arrow> <factors>

<pinch_func> ::= <left_pinch>? <func_name> <right_pinch>?
<pinch_var> ::= <left_pinch> <var_name> | <var_name> <right_pinch> | <var_name>

<statement> ::= ( <pinch_var> | <pinch_func> | <factor> ) '\eol'
<program> ::= <statement>* '\eof'
```


## Semantics
- Pinch has three data types: Number, Text and the special Jump type. When operating with numbers, they behave like double with some degree of rounding applied to the output.
- Pre-defined functions have expected number of arguements of expected types. Feeding a function with incorrect arguements will produce error. Refer to [functions](functions.md).
- Some pre-defined functions return nothing. Assigning such a function result to a variable will produce error. Refer to [functions](functions.md).
- Although it is syntactically correct to direct more than one factor at a variable at a time (using the `[a,b,...]` closure defined in `factors`), this does not comply with the semantics and will cause error.

## Interpreter constraints
- Any `var_name`, `func_name`, `num_literal` or `jump_literal` can be at most 64 characters long.
- Any `str_literal` can be at most 1024 characters long.
