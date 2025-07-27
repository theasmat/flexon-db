#include "test_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Global test statistics
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

void test_init(const char* test_suite_name) {
    tests_run = 0;
    tests_passed = 0;
    tests_failed = 0;
    printf("\n=== %s ===\n\n", test_suite_name);
}

void test_assert(bool condition, const char* description) {
    tests_run++;
    if (condition) {
        tests_passed++;
        printf("✅ PASS: %s\n", description);
    } else {
        tests_failed++;
        printf("❌ FAIL: %s\n", description);
    }
}

void test_assert_equal_int(int expected, int actual, const char* description) {
    tests_run++;
    if (expected == actual) {
        tests_passed++;
        printf("✅ PASS: %s (expected: %d, actual: %d)\n", description, expected, actual);
    } else {
        tests_failed++;
        printf("❌ FAIL: %s (expected: %d, actual: %d)\n", description, expected, actual);
    }
}

void test_assert_equal_str(const char* expected, const char* actual, const char* description) {
    tests_run++;
    if (expected == NULL && actual == NULL) {
        tests_passed++;
        printf("✅ PASS: %s (both NULL)\n", description);
    } else if (expected == NULL || actual == NULL) {
        tests_failed++;
        printf("❌ FAIL: %s (one is NULL: expected='%s', actual='%s')\n", 
               description, expected ? expected : "NULL", actual ? actual : "NULL");
    } else if (strcmp(expected, actual) == 0) {
        tests_passed++;
        printf("✅ PASS: %s\n", description);
    } else {
        tests_failed++;
        printf("❌ FAIL: %s (expected: '%s', actual: '%s')\n", description, expected, actual);
    }
}

void test_assert_not_null(void* ptr, const char* description) {
    tests_run++;
    if (ptr != NULL) {
        tests_passed++;
        printf("✅ PASS: %s (not NULL)\n", description);
    } else {
        tests_failed++;
        printf("❌ FAIL: %s (is NULL)\n", description);
    }
}

int test_finalize(void) {
    printf("\n=== Test Summary ===\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    
    if (tests_failed == 0) {
        printf("🎉 ALL TESTS PASSED!\n\n");
        return 0;
    } else {
        printf("💥 %d TESTS FAILED!\n\n", tests_failed);
        return 1;
    }
}

// Timing utilities
timing_info_t* timing_start(void) {
    timing_info_t* timing = malloc(sizeof(timing_info_t));
    if (timing) {
        clock_gettime(CLOCK_MONOTONIC, &timing->start);
    }
    return timing;
}

void timing_end(timing_info_t* timing) {
    if (timing) {
        clock_gettime(CLOCK_MONOTONIC, &timing->end);
        timing->elapsed_ms = (timing->end.tv_sec - timing->start.tv_sec) * 1000.0 +
                           (timing->end.tv_nsec - timing->start.tv_nsec) / 1000000.0;
    }
}

double timing_get_ms(timing_info_t* timing) {
    return timing ? timing->elapsed_ms : 0.0;
}

void timing_free(timing_info_t* timing) {
    if (timing) {
        free(timing);
    }
}

// Test data generation
char* generate_test_string(int length) {
    char* str = malloc(length + 1);
    if (str) {
        for (int i = 0; i < length; i++) {
            str[i] = 'A' + (rand() % 26);
        }
        str[length] = '\0';
    }
    return str;
}

int generate_test_int(int min, int max) {
    return min + (rand() % (max - min + 1));
}

float generate_test_float(float min, float max) {
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

// Test database helpers
void cleanup_test_files(void) {
    system("rm -f test_*.fxdb benchmark_*.fxdb");
}