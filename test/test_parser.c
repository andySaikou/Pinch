#include "test_harness.h"
#include "parser.h"
#include "syntax_printer.h"

// Simple factors
bool test_factors() {

    ASSERT_PARSE_RESULT("x\n", "(FACTOR x)");
    ASSERT_PARSE_RESULT("24\n", "(FACTOR 24.00)");
    ASSERT_PARSE_RESULT("-3.14\n", "(FACTOR -3.14)");
    ASSERT_PARSE_RESULT("\"Welcome to Pinch\"\n", "(FACTOR \"Welcome to Pinch\")");
    ASSERT_PARSE_RESULT("=>256\n", "(FACTOR (=> 256))");
    return true;
}

// Simple variable assignments
bool test_variable_assignments() {

    ASSERT_PARSE_RESULT("10 -> a\n", "(ASSIGN a [10.00])");
    ASSERT_PARSE_RESULT("b <- 20\n", "(ASSIGN b [20.00])");
    ASSERT_PARSE_RESULT("\"hello\" -> msg\n", "(ASSIGN msg [\"hello\"])");
    ASSERT_PARSE_RESULT("a_jump <- 10<=", "(ASSIGN a_jump [(10 <=)])");
    ASSERT_PARSE_FAIL("[a, b] -> c");
    ASSERT_PARSE_FAIL("a-> c <- b");
    ASSERT_PARSE_FAIL("c <- [a, b]");
    return true;
}

// Simple function calls
bool test_function_calls() {

    ASSERT_PARSE_RESULT("[1, 2] -> ADD\n", "(CALL ADD [1.00, 2.00])");
    ASSERT_PARSE_RESULT("[1, 2, 3] -> FOO\n", "(CALL FOO [1.00, 2.00, 3.00])");
    ASSERT_PARSE_RESULT("[1, 2] -> FOO <- 3\n", "(CALL FOO [1.00, 2.00, 3.00])");
    ASSERT_PARSE_RESULT("[1] -> FOO <- [2, 3]\n", "(CALL FOO [1.00, 2.00, 3.00])");
    ASSERT_PARSE_RESULT("FOO <- [1, 2, 3]\n", "(CALL FOO [1.00, 2.00, 3.00])");
    ASSERT_PARSE_RESULT("CONTAINS <- [\"Hello world!\", \"!\"]\n", "(CALL CONTAINS [\"Hello world!\", \"!\"])");
    ASSERT_PARSE_RESULT("RAND\n", "(CALL RAND [])");
    return true;
}

// Jump instructions
bool test_jump_functions() {

    ASSERT_PARSE_RESULT("5<= -> JUMP\n", "(CALL JUMP [(5 <=)])");
    ASSERT_PARSE_RESULT("still_in_loop -> JUMP_IF <- [18<=, =>1]\n", "(CALL JUMP_IF [still_in_loop, (18 <=), (=> 1)])");
    return true;
}

// Nesting expressions
bool test_nested_expressions() {

    ASSERT_PARSE_RESULT("ABS <- (ADD <- [-27, 6])\n", "(CALL ABS [(ADD [-27.00, 6.00])])");
    ASSERT_PARSE_RESULT("(16 -> SQRT) -> x\n", "(ASSIGN x [(SQRT [16.00])])");
    ASSERT_PARSE_RESULT("[1, 2, 3, x, y, z] -> LONG_FUNC <- [(ADD <- [p, q]), \"parameter\"]\n", 
    "(CALL LONG_FUNC [1.00, 2.00, 3.00, x, y, z, (ADD [p, q]), \"parameter\"])");
    return true;
}

int main() {
    RUN_TEST(test_factors);
    RUN_TEST(test_variable_assignments);
    RUN_TEST(test_function_calls);
    RUN_TEST(test_jump_functions);
    RUN_TEST(test_nested_expressions);
    PRINT_STATS();
    return tests_failed == 0 ? 0 : 1;
}
