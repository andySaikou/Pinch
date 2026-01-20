# Pinch

Pinch is a simple language based solely on dataflow that is represented by arrows. This repository will be an interpreter written in C that interpretes Pinch.

## Example program

The following program calculates the hypotenuse of a right triangle.
```
a <- 3
b <- 4

(a -> MUL <- a) -> sq_a
(b -> MUL <- b) -> sq_b
(sq_a -> ADD <- sq_b) -> sum
(sum -> SQRT) -> result

result  #output: 5
```

## Syntax

Using the BNF syntax:

* `(x-y)` stands for ‘range’, meaning any value from `x` to `y` inclusive;
* `(x)?` stands for ‘optional’, meaning that `x` can occur zero or one times;
* `(x)+` stands for ‘repeatable’, meaning that `x` can occur one or more times;
* `(x)*` stands for ‘optional and repeatable’, meaning that `x` can occur zero or more times.

```
<small_letter> ::= ('a'-'z')
<big_letter> ::= ('A'-'Z')
<str_literal> ::= ( <ASCII char excluding '"'> )+
<digit> ::= ('0'-'9')
<num_literal> ::= <digit>+

<var_name> ::= <small_letter> (<small_letter> | '_')*
<func_name> ::= <big_letter> (<big_letter> | '_')*

<factor> ::= <num_literal> | '"' <str_literal> '"' | <var-name> | '('<pinch_func>')'
<factors> ::= '[' (<factor>',')* <factor> ']' | <factor>

<from_left_arrow> ::= '->' | '~>'
<from_right_arrow> ::= '<-' | '<~'
<left_pinch> ::= <factors> <from_left_arrow>
<right_pinch> ::= <from_right_arrow> <factors>

<pinch_func> ::= <left_pinch>? <func_name> <right_pinch>?
<pinch_var> ::= <left_pinch> <var_name> | <var_name> <right_pinch>

<statement> ::= ( <pinch_var> | <pinch_func> | <factor> ) '\eol'
<program> ::= <statement>* '\eof'
```


## Semantics
- Pinch has two data types: number and text. When operating with numbers, they behave like double with some degree of rounding applied to the output.
- Pre-defined functions have expected number of arguements of some expected types. Feeding a function with incorrect arguements will produce error.
- Although it is syntactically correct to direct more than one factor at a variable at a time (using the '[a,b,...]' closure defined in factors), this does not comply with the semantics and will cause error.
