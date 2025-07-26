#define _GNU_SOURCE
#include "../include/shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * Format file size in human-readable format
 */
void format_file_size(uint64_t bytes, char* buffer, size_t buffer_size) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    double size = (double)bytes;
    int unit_index = 0;
    
    while (size >= 1024.0 && unit_index < 4) {
        size /= 1024.0;
        unit_index++;
    }
    
    if (unit_index == 0) {
        snprintf(buffer, buffer_size, "%llu %s", (unsigned long long)bytes, units[unit_index]);
    } else {
        snprintf(buffer, buffer_size, "%.1f %s", size, units[unit_index]);
    }
}

/**
 * Format timestamp in human-readable format
 */
void format_timestamp(time_t timestamp, char* buffer, size_t buffer_size) {
    struct tm* tm_info = localtime(&timestamp);
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", tm_info);
}

/**
 * Print table header with borders
 */
void print_table_header(const char* headers[], int column_count, int column_widths[]) {
    // Top border
    printf("┌");
    for (int i = 0; i < column_count; i++) {
        for (int j = 0; j < column_widths[i] + 2; j++) {
            printf("─");
        }
        if (i < column_count - 1) {
            printf("┬");
        }
    }
    printf("┐\n");
    
    // Header row
    printf("│");
    for (int i = 0; i < column_count; i++) {
        printf(" %-*s │", column_widths[i], headers[i]);
    }
    printf("\n");
    
    // Separator
    printf("├");
    for (int i = 0; i < column_count; i++) {
        for (int j = 0; j < column_widths[i] + 2; j++) {
            printf("─");
        }
        if (i < column_count - 1) {
            printf("┼");
        }
    }
    printf("┤\n");
}

/**
 * Print table row with borders
 */
void print_table_row(const char* values[], int column_count, int column_widths[]) {
    printf("│");
    for (int i = 0; i < column_count; i++) {
        printf(" %-*s │", column_widths[i], values[i] ? values[i] : "");
    }
    printf("\n");
}

/**
 * Print table footer with borders
 */
void print_table_footer(int column_count, int column_widths[]) {
    printf("└");
    for (int i = 0; i < column_count; i++) {
        for (int j = 0; j < column_widths[i] + 2; j++) {
            printf("─");
        }
        if (i < column_count - 1) {
            printf("┴");
        }
    }
    printf("┘\n");
}

/**
 * Calculate optimal column widths for table
 */
void calculate_column_widths(const char* headers[], const char* data[][16], 
                           int row_count, int column_count, int column_widths[]) {
    // Initialize with header widths
    for (int i = 0; i < column_count; i++) {
        column_widths[i] = strlen(headers[i]);
    }
    
    // Check data rows for maximum width
    for (int row = 0; row < row_count; row++) {
        for (int col = 0; col < column_count; col++) {
            if (data[row][col]) {
                int len = strlen(data[row][col]);
                if (len > column_widths[col]) {
                    column_widths[col] = len;
                }
            }
        }
    }
    
    // Set reasonable maximum widths to prevent overly wide tables
    for (int i = 0; i < column_count; i++) {
        if (column_widths[i] > 50) {
            column_widths[i] = 50;
        }
        if (column_widths[i] < 8) {
            column_widths[i] = 8;
        }
    }
}