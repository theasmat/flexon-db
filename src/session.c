#define _GNU_SOURCE
#include "../include/shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>

/**
 * Get current user name
 */
void get_current_user(char* buffer, size_t buffer_size) {
    struct passwd *pw = getpwuid(getuid());
    if (pw && pw->pw_name) {
        strncpy(buffer, pw->pw_name, buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
    } else {
        strncpy(buffer, "unknown", buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
    }
}

/**
 * Get list of .fxdb files in directory
 */
int get_database_list(const char* directory, char databases[][MAX_DATABASE_NAME_LEN], int max_count) {
    const char* search_dir = directory ? directory : ".";
    
    DIR *dir = opendir(search_dir);
    if (!dir) {
        return -1;
    }
    
    struct dirent *entry;
    int count = 0;
    
    while ((entry = readdir(dir)) != NULL && count < max_count) {
        if (entry->d_type == DT_REG) { // Regular file
            char *ext = strrchr(entry->d_name, '.');
            if (ext && strcmp(ext, ".fxdb") == 0) {
                strncpy(databases[count], entry->d_name, MAX_DATABASE_NAME_LEN - 1);
                databases[count][MAX_DATABASE_NAME_LEN - 1] = '\0';
                count++;
            }
        }
    }
    
    closedir(dir);
    return count;
}

/**
 * Check if database exists
 */
bool database_exists(const char* directory, const char* database_name) {
    char* full_path = get_database_path(directory, database_name);
    if (!full_path) {
        return false;
    }
    
    struct stat st;
    bool exists = (stat(full_path, &st) == 0 && S_ISREG(st.st_mode));
    free(full_path);
    return exists;
}

/**
 * Get database file path
 */
char* get_database_path(const char* directory, const char* database_name) {
    if (!database_name) {
        return NULL;
    }
    
    // If no directory specified, use current directory
    if (!directory || strlen(directory) == 0) {
        return strdup(database_name);
    }
    
    // Build full path
    size_t dir_len = strlen(directory);
    size_t name_len = strlen(database_name);
    size_t total_len = dir_len + name_len + 2; // +2 for '/' and '\0'
    
    char* full_path = malloc(total_len);
    if (!full_path) {
        return NULL;
    }
    
    snprintf(full_path, total_len, "%s/%s", directory, database_name);
    return full_path;
}

/**
 * Start timing measurement
 */
void start_timing(timing_info_t* timing) {
    clock_gettime(CLOCK_MONOTONIC, &timing->start);
}

/**
 * End timing measurement and return elapsed milliseconds
 */
double end_timing(timing_info_t* timing) {
    clock_gettime(CLOCK_MONOTONIC, &timing->end);
    
    double start_ms = timing->start.tv_sec * 1000.0 + timing->start.tv_nsec / 1000000.0;
    double end_ms = timing->end.tv_sec * 1000.0 + timing->end.tv_nsec / 1000000.0;
    
    return end_ms - start_ms;
}