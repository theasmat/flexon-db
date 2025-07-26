#ifndef SHELL_H
#define SHELL_H

#include "schema.h"
#include "reader.h"
#include "writer.h"
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

// Maximum command line length
#define MAX_COMMAND_LEN 1024
#define MAX_DATABASE_NAME_LEN 256
#define MAX_PATH_LEN 512

// ANSI Color codes for enhanced output
#define COLOR_RESET     "\033[0m"
#define COLOR_BOLD      "\033[1m"
#define COLOR_DIM       "\033[2m"

// Foreground colors
#define COLOR_BLACK     "\033[30m"
#define COLOR_RED       "\033[31m"
#define COLOR_GREEN     "\033[32m"
#define COLOR_YELLOW    "\033[33m"
#define COLOR_BLUE      "\033[34m"
#define COLOR_MAGENTA   "\033[35m"
#define COLOR_CYAN      "\033[36m"
#define COLOR_WHITE     "\033[37m"

// Bright colors
#define COLOR_BR_BLACK  "\033[90m"
#define COLOR_BR_RED    "\033[91m"
#define COLOR_BR_GREEN  "\033[92m"
#define COLOR_BR_YELLOW "\033[93m"
#define COLOR_BR_BLUE   "\033[94m"
#define COLOR_BR_MAGENTA "\033[95m"
#define COLOR_BR_CYAN   "\033[96m"
#define COLOR_BR_WHITE  "\033[97m"

// Background colors
#define COLOR_BG_BLACK  "\033[40m"
#define COLOR_BG_RED    "\033[41m"
#define COLOR_BG_GREEN  "\033[42m"
#define COLOR_BG_YELLOW "\033[43m"
#define COLOR_BG_BLUE   "\033[44m"
#define COLOR_BG_MAGENTA "\033[45m"
#define COLOR_BG_CYAN   "\033[46m"
#define COLOR_BG_WHITE  "\033[47m"

// Semantic color aliases for shell components
#define COLOR_SUCCESS   COLOR_BR_GREEN
#define COLOR_ERROR     COLOR_BR_RED
#define COLOR_WARNING   COLOR_BR_YELLOW
#define COLOR_INFO      COLOR_BR_CYAN
#define COLOR_PROMPT    COLOR_BR_BLUE COLOR_BOLD
#define COLOR_EMPHASIS  COLOR_BR_WHITE COLOR_BOLD
#define COLOR_MUTED     COLOR_BR_BLACK

// Shell session information
typedef struct {
    char user[64];              // Current user
    char working_dir[MAX_PATH_LEN]; // Working directory
    char current_db[MAX_DATABASE_NAME_LEN]; // Active database
    time_t session_start;       // Session start time
    uint32_t commands_executed; // Commands executed in session
} shell_session_t;

// Command types for the interactive shell
typedef enum {
    CMD_USE,
    CMD_SHOW_DATABASES,
    CMD_CREATE,
    CMD_DROP,
    CMD_SELECT,
    CMD_COUNT,
    CMD_INSERT,
    CMD_EXPORT,
    CMD_INFO,
    CMD_SCHEMA,
    CMD_STATUS,
    CMD_HELP,
    CMD_CLEAR,
    CMD_HISTORY,
    CMD_EXIT,
    CMD_QUIT,
    CMD_UNKNOWN
} shell_command_t;

// Parsed command structure
typedef struct {
    shell_command_t type;
    char* args[16];             // Command arguments
    int arg_count;
    char* raw_line;             // Original command line
} parsed_command_t;

// Function declarations for shell.c

/**
 * Initialize and run the interactive shell
 */
int run_interactive_shell(const char* directory);

/**
 * Initialize shell session
 */
shell_session_t* init_session(const char* directory);

/**
 * Print welcome screen with logo and session info
 */
void print_welcome_screen(const shell_session_t* session);

/**
 * Print shell prompt based on current state
 */
void print_prompt(const shell_session_t* session);

/**
 * Execute a shell command
 */
int execute_shell_command(shell_session_t* session, const parsed_command_t* cmd);

/**
 * Print goodbye message with session statistics
 */
void print_goodbye(const shell_session_t* session);

/**
 * Free session resources
 */
void free_session(shell_session_t* session);

// Function declarations for parser.c

/**
 * Parse command line into structured command
 */
parsed_command_t* parse_command(const char* line);

/**
 * Free parsed command resources
 */
void free_parsed_command(parsed_command_t* cmd);

/**
 * Get command type from string
 */
shell_command_t get_command_type(const char* cmd_str);

// Function declarations for formatter.c

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

// Function declarations for session.c

/**
 * Get list of .fxdb files in directory
 */
int get_database_list(const char* directory, char databases[][MAX_DATABASE_NAME_LEN], int max_count);

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
 * Measure command execution time
 */
typedef struct {
    struct timespec start;
    struct timespec end;
} timing_info_t;

/**
 * Start timing measurement
 */
void start_timing(timing_info_t* timing);

/**
 * End timing measurement and return elapsed milliseconds
 */
double end_timing(timing_info_t* timing);

#endif // SHELL_H