// Definitions for syntax structures

struct num_literal {
    int value;
};

struct str_literal {
    char *value;
};

struct var_name {
    char *name;
};

struct func_name {
    char *name;
};

enum factor_type {
    NUM_LITERAL,
    STR_LITERAL,
    VAR_NAME,
    PINCH_FUNC
};

struct factor {
    enum factor_type type;
    union {
        int num_literal;
        char *str_literal;
        struct var_name var_name;
        struct func_name pinch_func;
    } value;
};

struct factors {
    struct factor *factors;
};

struct pinch_func {
    char *name;
    struct factors args;
};

struct pinch_var {
    char *name;
    struct factor values;
};

enum statement_type {
    PINCH_VAR,
    PINCH_FUNC_S,  // Avoid name collision with factor_type PINCH_FUNC
    FACTOR
};

struct statement {
    enum statement_type type;
    union {
        struct pinch_var pinch_var;
        struct pinch_func pinch_func;
        struct factor factor;
    } content;
};
