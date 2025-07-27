#include "../include/platform.h"
#include "../include/compat.h"
#include "../core/flexondb.h"

// ANSI color codes for colorful CLI output.
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_RESET "\x1b[0m"

int main(int argc, char *argv[]) {
    printf(COLOR_GREEN "Welcome to FlexonDB CLI Interface\n" COLOR_RESET);
    
    if (argc < 2) {
        printf(COLOR_YELLOW "Usage: %s <command> [options]\n" COLOR_RESET, argv[0]);
        printf("Commands:\n");
        printf("  create <path> <schema>     - Create a new database\n");
        printf("  update <path> <json>       - Update database with JSON data\n");
        printf("  csv2fxdb <csvPath> <dbPath> - Convert CSV to FlexonDB\n");
        printf("  read <path>                - Read data from database\n");
        printf("  delete <path>              - Delete database\n");
        return 1;
    }
    
    // A very simple command parser.
    if (strcmp(argv[1], "create") == 0 && argc == 4) {
        int result = createDatabase(argv[2], argv[3]);
        if(result == 0)
            printf(COLOR_GREEN "Database created at %s\n" COLOR_RESET, argv[2]);
        else
            printf(COLOR_RED "Failed to create database at %s\n" COLOR_RESET, argv[2]);
    } else if (strcmp(argv[1], "update") == 0 && argc == 4) {
        int result = updateDatabase(argv[2], argv[3]);
        if(result == 0)
            printf(COLOR_GREEN "Database at %s updated successfully\n" COLOR_RESET, argv[2]);
        else
            printf(COLOR_RED "Failed to update database at %s\n" COLOR_RESET, argv[2]);
    } else if (strcmp(argv[1], "csv2fxdb") == 0 && argc == 4) {
        int result = csvToFlexonDB(argv[2], argv[3]);
        if(result == 0)
            printf(COLOR_GREEN "CSV file %s converted to FlexonDB at %s successfully\n" COLOR_RESET, argv[2], argv[3]);
        else
            printf(COLOR_RED "Failed to convert CSV file %s\n" COLOR_RESET, argv[2]);
    } else if (strcmp(argv[1], "read") == 0 && argc == 3) {
        char* result = readData(argv[2]);
        if(result) {
            printf(COLOR_GREEN "Read result: %s\n" COLOR_RESET, result);
            free(result);
        } else {
            printf(COLOR_RED "Failed to read from database %s\n" COLOR_RESET, argv[2]);
        }
    } else if (strcmp(argv[1], "delete") == 0 && argc == 3) {
        int result = deleteDatabase(argv[2]);
        if(result == 0)
            printf(COLOR_GREEN "Database at %s deleted successfully\n" COLOR_RESET, argv[2]);
        else
            printf(COLOR_RED "Failed to delete database at %s\n" COLOR_RESET, argv[2]);
    } else {
        printf(COLOR_YELLOW "Unknown command or wrong arguments.\n" COLOR_RESET);
        printf("Usage: %s <command> [options]\n", argv[0]);
    }
    return 0;
}