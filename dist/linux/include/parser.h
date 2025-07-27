#ifndef FLEXON_PARSER_H
#define FLEXON_PARSER_H

/* ============================================================================
 * FlexonDB Command Parser
 * ============================================================================ * This file contains utilities for parsing shell commands
 */

#include "config.h"
#include "types.h"

/* ============================================================================
 * Function Declarations
 * ============================================================================ */

/**
 * Parse command line into structured command
 */
parsed_command_enhanced_t* parse_command(const char* line);

/**
 * Free parsed command resources
 */
void free_parsed_command(parsed_command_enhanced_t* cmd);

/**
 * Get command type from string
 */
shell_command_type_t get_command_type(const char* cmd_str);

#endif // FLEXON_PARSER_H