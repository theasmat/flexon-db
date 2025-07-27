#ifndef FLEXON_SESSION_H
#define FLEXON_SESSION_H

/* ============================================================================
 * FlexonDB Session Management
 * ============================================================================
 * This file contains utilities for managing shell sessions
 */

#include "config.h"
#include "types.h"
#include <stdbool.h>
#include <time.h>

/* ============================================================================
 * Function Declarations
 * ============================================================================ */

/**
 * Get list of .fxdb files in directory
 */
int get_database_list(const char* directory, char databases[][MAX_COMMAND_LENGTH], int max_count);

/**
 * Check if database exists
 */
bool database_exists(const char* directory, const char* database_name);

/**
 * Get database file path
 */
char* get_database_path(const char* directory, const char* database_name);

/**
 * Get current user name
 */
void get_current_user(char* buffer, size_t buffer_size);

/**
 * Start timing measurement
 */
void start_timing(timing_info_enhanced_t* timing);

/**
 * End timing measurement and return elapsed milliseconds
 */
double end_timing(timing_info_enhanced_t* timing);

#endif // FLEXON_SESSION_H