#ifndef FLEXON_ERROR_H
#define FLEXON_ERROR_H

/* ============================================================================
 * FlexonDB Error Handling System
 * ============================================================================
 * This file provides centralized error handling utilities.
 */

#include "config.h"
#include "types.h"
#include <stdio.h>

/* ============================================================================
 * Error Handling Macros
 * ============================================================================ */

/**
 * Set error with context information
 */
#define SET_ERROR(ctx, cat, code_val, msg, ...) \
    do { \
        if (ctx) { \
            (ctx)->category = (cat); \
            (ctx)->code = (code_val); \
            snprintf((ctx)->message, sizeof((ctx)->message), (msg), ##__VA_ARGS__); \
            snprintf((ctx)->file, sizeof((ctx)->file), "%s", __FILE__); \
            (ctx)->line = __LINE__; \
            snprintf((ctx)->function, sizeof((ctx)->function), "%s", __func__); \
            (ctx)->context[0] = '\0'; \
        } \
    } while (0)

/**
 * Set error with additional context
 */
#define SET_ERROR_CTX(ctx, cat, code_val, msg, context_info, ...) \
    do { \
        SET_ERROR(ctx, cat, code_val, msg, ##__VA_ARGS__); \
        if (ctx) { \
            snprintf((ctx)->context, sizeof((ctx)->context), "%s", (context_info)); \
        } \
    } while (0)

/**
 * Clear error context
 */
#define CLEAR_ERROR(ctx) \
    do { \
        if (ctx) { \
            (ctx)->category = ERROR_NONE; \
            (ctx)->code = 0; \
            (ctx)->message[0] = '\0'; \
            (ctx)->file[0] = '\0'; \
            (ctx)->line = 0; \
            (ctx)->function[0] = '\0'; \
            (ctx)->context[0] = '\0'; \
        } \
    } while (0)

/**
 * Check if error is set
 */
#define HAS_ERROR(ctx) ((ctx) && (ctx)->category != ERROR_NONE)

/* ============================================================================
 * Function Declarations
 * ============================================================================ */

/**
 * Initialize error context
 */
void error_init(error_context_t* ctx);

/**
 * Get string representation of error category
 */
const char* error_category_to_string(error_category_t category);

/**
 * Print error information to stderr
 */
void error_print(const error_context_t* ctx);

/**
 * Print error information to file
 */
void error_fprint(FILE* fp, const error_context_t* ctx);

/**
 * Format error as string
 */
void error_format(const error_context_t* ctx, char* buffer, size_t buffer_size);

/**
 * Convert status code to error category
 */
error_category_t status_to_error_category(status_code_t status);

/**
 * Convert error category to status code
 */
status_code_t error_category_to_status(error_category_t category);

/**
 * Check if error is recoverable
 */
int error_is_recoverable(const error_context_t* ctx);

#endif // FLEXON_ERROR_H