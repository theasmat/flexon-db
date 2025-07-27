#ifndef FLEXON_FORMATTER_H
#define FLEXON_FORMATTER_H

/* ============================================================================
 * FlexonDB Table Formatting Utilities
 * ============================================================================
 * This file contains utilities for formatting tabular output
 */

#include "config.h"
#include <stdint.h>
#include <time.h>

/* ============================================================================
 * Function Declarations
 * ============================================================================ */

/**
 * Format file size in human-readable format
 */
void format_file_size(uint64_t bytes, char* buffer, size_t buffer_size);

/**
 * Format timestamp in human-readable format
 */
void format_timestamp(time_t timestamp, char* buffer, size_t buffer_size);

/**
 * Print table header with borders
 */
void print_table_header(const char* headers[], int column_count, int column_widths[]);

/**
 * Print table row with borders
 */
void print_table_row(const char* values[], int column_count, int column_widths[]);

/**
 * Print table footer with borders
 */
void print_table_footer(int column_count, int column_widths[]);

/**
 * Calculate optimal column widths for table
 */
void calculate_column_widths(const char* headers[], const char* data[][16], 
                           int row_count, int column_count, int column_widths[]);

#endif // FLEXON_FORMATTER_H