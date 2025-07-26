#define _GNU_SOURCE
#include "../../include/shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Trim whitespace from string
 */
static char* trim_whitespace(char* str) {
    if (!str) return NULL;
    
    // Trim leading space
    while(isspace((unsigned char)*str)) str++;
    
    if(*str == 0) return str;
    
    // Trim trailing space
    char* end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    
    return str;
}

/**
 * Get command type from string
 */
shell_command_t get_command_type(const char* cmd_str) {
    if (!cmd_str) return CMD_UNKNOWN;
    
    if (strcmp(cmd_str, "use") == 0) return CMD_USE;
    if (strcmp(cmd_str, "show") == 0) return CMD_SHOW_DATABASES;
    if (strcmp(cmd_str, "create") == 0) return CMD_CREATE;
    if (strcmp(cmd_str, "drop") == 0) return CMD_DROP;
    if (strcmp(cmd_str, "select") == 0) return CMD_SELECT;
    if (strcmp(cmd_str, "count") == 0) return CMD_COUNT;
    if (strcmp(cmd_str, "insert") == 0) return CMD_INSERT;
    if (strcmp(cmd_str, "export") == 0) return CMD_EXPORT;
    if (strcmp(cmd_str, "info") == 0) return CMD_INFO;
    if (strcmp(cmd_str, "schema") == 0) return CMD_SCHEMA;
    if (strcmp(cmd_str, "status") == 0) return CMD_STATUS;
    if (strcmp(cmd_str, "help") == 0) return CMD_HELP;
    if (strcmp(cmd_str, "clear") == 0) return CMD_CLEAR;
    if (strcmp(cmd_str, "history") == 0) return CMD_HISTORY;
    if (strcmp(cmd_str, "exit") == 0) return CMD_EXIT;
    if (strcmp(cmd_str, "quit") == 0) return CMD_QUIT;
    
    return CMD_UNKNOWN;
}

/**
 * Parse command line into structured command
 */
parsed_command_t* parse_command(const char* line) {
    if (!line) return NULL;
    
    parsed_command_t* cmd = malloc(sizeof(parsed_command_t));
    if (!cmd) return NULL;
    
    // Initialize command structure
    cmd->type = CMD_UNKNOWN;
    cmd->arg_count = 0;
    cmd->raw_line = strdup(line);
    
    // Initialize args array
    for (int i = 0; i < 16; i++) {
        cmd->args[i] = NULL;
    }
    
    // Create a working copy of the line
    char* line_copy = strdup(line);
    if (!line_copy) {
        free_parsed_command(cmd);
        return NULL;
    }
    
    // Trim whitespace
    char* trimmed = trim_whitespace(line_copy);
    
    // Skip empty lines
    if (strlen(trimmed) == 0) {
        free(line_copy);
        free_parsed_command(cmd);
        return NULL;
    }
    
    // Parse arguments, handling quoted strings
    char* pos = trimmed;
    int token_count = 0;
    
    while (*pos && token_count < 16) {
        // Skip whitespace
        while (*pos && isspace(*pos)) pos++;
        if (!*pos) break;
        
        char* token_start = pos;
        
        // Handle quoted strings
        if (*pos == '"' || *pos == '\'') {
            char quote = *pos++;
            token_start = pos; // Start after opening quote
            
            // Find closing quote
            while (*pos && *pos != quote) pos++;
            
            if (*pos == quote) {
                *pos = '\0'; // Null terminate the string
                pos++; // Move past closing quote
            }
        } else {
            // Regular token - find end
            while (*pos && !isspace(*pos)) pos++;
            if (*pos) {
                *pos = '\0';
                pos++;
            }
        }
        
        cmd->args[token_count] = strdup(token_start);
        token_count++;
    }
    
    cmd->arg_count = token_count;
    
    // Determine command type from first argument
    if (cmd->arg_count > 0) {
        cmd->type = get_command_type(cmd->args[0]);
        
        // Special handling for "show databases"
        if (cmd->type == CMD_SHOW_DATABASES && cmd->arg_count > 1 && 
            strcmp(cmd->args[1], "databases") == 0) {
            cmd->type = CMD_SHOW_DATABASES;
        } else if (cmd->type == CMD_SHOW_DATABASES) {
            // Just "show" without "databases" - invalid
            cmd->type = CMD_UNKNOWN;
        }
    }
    
    free(line_copy);
    return cmd;
}

/**
 * Free parsed command resources
 */
void free_parsed_command(parsed_command_t* cmd) {
    if (!cmd) return;
    
    // Free arguments
    for (int i = 0; i < cmd->arg_count && i < 16; i++) {
        free(cmd->args[i]);
    }
    
    // Free raw line
    free(cmd->raw_line);
    
    // Free command structure
    free(cmd);
}