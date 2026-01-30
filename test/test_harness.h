// test_harness.h

#ifndef TEST_HARNESS_H
#define TEST_HARNESS_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define ASSERT_TRUE(cond) do { \
    if (!(cond)) { \
        printf("\033[31m[FAIL]\033[0m %s:%d: Assertion failed: %s\n", __FILE__, __LINE__, #cond); \
        return false; \
    } \
} while(0)

#define ASSERT_PARSE_RESULT(code_input, expected_str) do { \
    parse_statement_result res = parse_statement(code_input); \
    if (!res.success) { \
        printf("\n\033[31m[FAIL]\033[0m Parse failed for: %s\n", code_input); \
        return false; \
    } \
    char *actual_str = statement_to_string(res.stmt); \
    if (strcmp(actual_str, expected_str) != 0) { \
        printf("\n\033[31m[FAIL]\033[0m Syntax mismatch!\n"); \
        printf("   Input:    %s", code_input); \
        printf("   Expected: %s\n", expected_str); \
        printf("   Actual:   %s\n", actual_str); \
        free_statement(res.stmt); \
        return false; \
    } \
    free_statement(res.stmt); \
} while(0)

#define ASSERT_PARSE_FAIL(code_input) do { \
    parse_statement_result res = parse_statement(code_input); \
    if (res.success) { \
        printf("\n\033[31m[FAIL]\033[0m Parse succeeded but should fail for: %s\n", code_input); \
        free_statement(res.stmt); \
        return false; \
    } \
} while(0)

#define RUN_TEST(test_func) do { \
    tests_run++; \
    printf("Running %s...", #test_func); \
    if (test_func()) { \
        printf("\033[32m[PASS]\033[0m\n"); \
        tests_passed++; \
    } else { \
        tests_failed++; \
    } \
} while(0)

#define PRINT_STATS() do { \
    printf("\n=== TEST SUMMARY ===\n"); \
    printf("Run: %d | \033[32mPassed: %d\033[0m | \033[31mFailed: %d\033[0m\n", tests_run, tests_passed, tests_failed); \
    if (tests_failed > 0) printf("\033[31mSOME TESTS FAILED!\033[0m\n"); \
    else printf("\033[32mALL TESTS PASSED!\033[0m\n"); \
} while(0)

#endif
