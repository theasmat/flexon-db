#ifndef COMMON_COMMAND_PROCESSOR_H
#define COMMON_COMMAND_PROCESSOR_H

/**
 * FlexonDB Unified Command Processing System
 * 
 * This provides a unified interface for both CLI and shell command processing,
 * ensuring complete parity between the two modes of operation.
 */

// Unified command structure
typedef struct {
    const char* name;
    const char* description;
    const char* usage;
    const char* examples;
    int (*cli_handler)(int argc, char* argv[]);
    int (*shell_handler)(const char* command);
} flexon_command_t;

// Command processing functions
int flexon_process_cli_command(int argc, char* argv[]);
int flexon_process_shell_command(const char* command);

// Help system functions
void flexon_show_help_table(const char* filter);
void flexon_show_command_help(const char* command);
void flexon_show_data_types(void);
void flexon_show_examples(void);

// Command registry (defined in command_processor.c)
extern const flexon_command_t flexon_commands[];

// Individual command handlers (can be shared between CLI and shell)
int cmd_create(int argc, char* argv[]);
int cmd_insert(int argc, char* argv[]);
int cmd_read(int argc, char* argv[]);
int cmd_info(int argc, char* argv[]);
int cmd_dump(int argc, char* argv[]);
int cmd_list(int argc, char* argv[]);
int cmd_help(int argc, char* argv[]);
int cmd_types(int argc, char* argv[]);

// Shell-specific handlers
int shell_cmd_create(const char* command);
int shell_cmd_insert(const char* command);
int shell_cmd_read(const char* command);
int shell_cmd_info(const char* command);
int shell_cmd_dump(const char* command);
int shell_cmd_list(const char* command);
int shell_cmd_help(const char* command);
int shell_cmd_types(const char* command);

// Utility functions
char** parse_command_args(const char* command, int* argc);
void free_command_args(char** args, int argc);
int find_command_index(const char* command_name);

#endif // COMMON_COMMAND_PROCESSOR_H