#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdbool.h>
#include <time.h>

// Test assertion macros and functions
void test_init(const char* test_suite_name);
void test_assert(bool condition, const char* description);
void test_assert_equal_int(int expected, int actual, const char* description);
void test_assert_equal_str(const char* expected, const char* actual, const char* description);
void test_assert_not_null(void* ptr, const char* description);
int test_finalize(void);

// Timing utilities for performance tests
typedef struct {
    struct timespec start;
    struct timespec end;
    double elapsed_ms;
} timing_info_t;

timing_info_t* timing_start(void);
void timing_end(timing_info_t* timing);
double timing_get_ms(timing_info_t* timing);
void timing_free(timing_info_t* timing);

// Test data generation
char* generate_test_string(int length);
int generate_test_int(int min, int max);
float generate_test_float(float min, float max);

// Test database helpers
void cleanup_test_files(void);

#endif // TEST_UTILS_H