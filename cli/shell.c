#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../core/flexondb.h"

#define COLOR_BLUE "\x1b[34m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_RED "\x1b[31m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_RESET "\x1b[0m"

void prompt() {
    printf(COLOR_BLUE "FlexonDB> " COLOR_RESET);
}

int main() {
    char command[256];
    printf(COLOR_GREEN "Welcome to FlexonDB Shell\n" COLOR_RESET);
    printf("Available commands:\n");
    printf("  create <path> <schema>       - Create a new database\n");
    printf("  update <path> <json>         - Update database with JSON data\n");
    printf("  csv2fxdb <csvPath> <dbPath>  - Convert CSV to FlexonDB\n");
    printf("  read <path>                  - Read data from database\n");
    printf("  delete <path>                - Delete database\n");
    printf("  exit                         - Exit shell\n\n");
    
    while(1) {
        prompt();
        if(fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }
        // remove trailing newline
        command[strcspn(command, "\n")] = 0;

        if(strcmp(command, "exit") == 0) {
            printf(COLOR_GREEN "Goodbye!\n" COLOR_RESET);
            break;
        } else if(strncmp(command, "create", 6) == 0) {
            // dummy parsing example: create <path> <schema>
            char path[128], schema[128];
            if(sscanf(command, "create %127s %127[^\n]", path, schema) == 2) {
                int result = createDatabase(path, schema);
                if(result == 0)
                    printf(COLOR_GREEN "Database created at %s\n" COLOR_RESET, path);
                else
                    printf(COLOR_RED "Failed to create database at %s\n" COLOR_RESET, path);
            } else {
                printf(COLOR_RED "Invalid create command. Usage: create <path> <schema>\n" COLOR_RESET);
            }
        } else if(strncmp(command, "update", 6) == 0) {
            char path[128], json[128];
            if(sscanf(command, "update %127s %127[^\n]", path, json) == 2) {
                int result = updateDatabase(path, json);
                if(result == 0)
                    printf(COLOR_GREEN "Database at %s updated successfully\n" COLOR_RESET, path);
                else
                    printf(COLOR_RED "Failed to update database at %s\n" COLOR_RESET, path);
            } else {
                printf(COLOR_RED "Invalid update command. Usage: update <path> <json>\n" COLOR_RESET);
            }
        } else if(strncmp(command, "csv2fxdb", 8) == 0) {
            char csvPath[128], dbPath[128];
            if(sscanf(command, "csv2fxdb %127s %127s", csvPath, dbPath) == 2) {
                int result = csvToFlexonDB(csvPath, dbPath);
                if(result == 0)
                    printf(COLOR_GREEN "CSV file %s converted to FlexonDB at %s successfully\n" COLOR_RESET, csvPath, dbPath);
                else
                    printf(COLOR_RED "Failed to convert CSV file %s\n" COLOR_RESET, csvPath);
            } else {
                printf(COLOR_RED "Invalid csv2fxdb command. Usage: csv2fxdb <csvPath> <dbPath>\n" COLOR_RESET);
            }
        } else if(strncmp(command, "read", 4) == 0) {
            char path[128];
            if(sscanf(command, "read %127s", path) == 1) {
                char* result = readData(path);
                if(result) {
                    printf(COLOR_GREEN "Read result: %s\n" COLOR_RESET, result);
                    free(result);
                } else {
                    printf(COLOR_RED "Failed to read from database %s\n" COLOR_RESET, path);
                }
            } else {
                printf(COLOR_RED "Invalid read command. Usage: read <path>\n" COLOR_RESET);
            }
        } else if(strncmp(command, "delete", 6) == 0) {
            char path[128];
            if(sscanf(command, "delete %127s", path) == 1) {
                int result = deleteDatabase(path);
                if(result == 0)
                    printf(COLOR_GREEN "Database at %s deleted successfully\n" COLOR_RESET, path);
                else
                    printf(COLOR_RED "Failed to delete database at %s\n" COLOR_RESET, path);
            } else {
                printf(COLOR_RED "Invalid delete command. Usage: delete <path>\n" COLOR_RESET);
            }
        } else if(strlen(command) > 0) {
            printf(COLOR_YELLOW "Unknown command: %s\n" COLOR_RESET, command);
        }
    }
    return 0;
}