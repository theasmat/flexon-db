#include "../../include/common/command_processor.h"
#include "../../include/core/data_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Command registry with unified handlers
const flexon_command_t flexon_commands[] = {
    {
        "create",
        "Create a new FlexonDB database with schema",
        "create <file.fxdb> --schema \"field1 type1, field2 type2, ...\"",
        "create users.fxdb --schema \"id int32, name string64, email string128\"",
        cmd_create,
        shell_cmd_create
    },
    {
        "insert", 
        "Insert a row into existing database",
        "insert <file.fxdb> --data '{\"field1\": \"value1\", \"field2\": value2}'",
        "insert users.fxdb --data '{\"id\": 1, \"name\": \"Alice\", \"email\": \"alice@example.com\"}'",
        cmd_insert,
        shell_cmd_insert
    },
    {
        "read",
        "Read and display rows from database", 
        "read <file.fxdb> [--limit N]",
        "read users.fxdb --limit 10",
        cmd_read,
        shell_cmd_read
    },
    {
        "info",
        "Show database information and schema",
        "info <file.fxdb>",
        "info users.fxdb",
        cmd_info,
        shell_cmd_info
    },
    {
        "dump",
        "Export all data in specified format",
        "dump <file.fxdb> [--format csv|json|table]",
        "dump users.fxdb --format csv",
        cmd_dump,
        shell_cmd_dump
    },
    {
        "list",
        "List all .fxdb files in directory",
        "list [--directory path]",
        "list --directory /path/to/databases",
        cmd_list,
        shell_cmd_list
    },
    {
        "help",
        "Show help information",
        "help [command]",
        "help create",
        cmd_help,
        shell_cmd_help
    },
    {
        "types",
        "Show all available data types",
        "types",
        "types",
        cmd_types,
        shell_cmd_types
    },
    {NULL, NULL, NULL, NULL, NULL, NULL}  // Terminator
};

// Enhanced help system with table format
void flexon_show_help_table(const char* filter) {
    printf("\n📚 FlexonDB Commands\n");
    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("%-12s %-30s %s\n", "COMMAND", "USAGE", "DESCRIPTION");
    printf("───────────────────────────────────────────────────────────────────\n");
    
    for (int i = 0; flexon_commands[i].name; i++) {
        if (filter && strstr(flexon_commands[i].name, filter) == NULL) {
            continue;
        }
        
        // Truncate long usage strings for table formatting
        char usage_truncated[31];
        strncpy(usage_truncated, flexon_commands[i].usage, 30);
        usage_truncated[30] = '\0';
        if (strlen(flexon_commands[i].usage) > 30) {
            strcpy(usage_truncated + 27, "...");
        }
        
        printf("%-12s %-30s %s\n", 
               flexon_commands[i].name,
               usage_truncated,
               flexon_commands[i].description);
    }
    
    printf("───────────────────────────────────────────────────────────────────\n");
    printf("\nType 'help <command>' for detailed help on any command\n");
    printf("Type 'types' to see all available data types\n\n");
}

// Show detailed help for a specific command
void flexon_show_command_help(const char* command) {
    for (int i = 0; flexon_commands[i].name; i++) {
        if (strcmp(flexon_commands[i].name, command) == 0) {
            printf("\n📖 Command: %s\n", flexon_commands[i].name);
            printf("═══════════════════════════════════════════════════════════════════\n");
            printf("Description: %s\n\n", flexon_commands[i].description);
            printf("Usage: %s\n\n", flexon_commands[i].usage);
            printf("Example: %s\n\n", flexon_commands[i].examples);
            return;
        }
    }
    printf("❌ Unknown command: %s\n", command);
    printf("Type 'help' to see all available commands.\n\n");
}

// Show comprehensive data types help
void flexon_show_data_types(void) {
    printf("\n📊 FlexonDB Data Types\n");
    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("%-15s %-10s %-8s %s\n", "TYPE", "ALIAS", "SIZE", "DESCRIPTION");
    printf("───────────────────────────────────────────────────────────────────\n");
    
    // String types
    printf("%-15s %-10s %-8s %s\n", "string", "", "256B", "Default string (256 chars)");
    printf("%-15s %-10s %-8s %s\n", "string16", "", "16B", "Short string (16 chars)");
    printf("%-15s %-10s %-8s %s\n", "string32", "", "32B", "Medium string (32 chars)");
    printf("%-15s %-10s %-8s %s\n", "string64", "", "64B", "Long string (64 chars)");
    printf("%-15s %-10s %-8s %s\n", "string128", "", "128B", "Extra long string (128 chars)");
    printf("%-15s %-10s %-8s %s\n", "string256", "", "256B", "Default string (256 chars)");
    printf("%-15s %-10s %-8s %s\n", "string512", "", "512B", "Very long string (512 chars)");
    printf("%-15s %-10s %-8s %s\n", "text", "", "VAR", "Variable length text");
    printf("\n");
    
    // Integer types
    printf("%-15s %-10s %-8s %s\n", "int", "int32", "4B", "Default integer (32-bit)");
    printf("%-15s %-10s %-8s %s\n", "int8", "", "1B", "Tiny integer (8-bit)");
    printf("%-15s %-10s %-8s %s\n", "int16", "", "2B", "Short integer (16-bit)");
    printf("%-15s %-10s %-8s %s\n", "int32", "", "4B", "Standard integer (32-bit)");
    printf("%-15s %-10s %-8s %s\n", "int64", "", "8B", "Long integer (64-bit)");
    printf("%-15s %-10s %-8s %s\n", "uint8", "", "1B", "Unsigned tiny integer");
    printf("%-15s %-10s %-8s %s\n", "uint16", "", "2B", "Unsigned short integer");
    printf("%-15s %-10s %-8s %s\n", "uint32", "", "4B", "Unsigned standard integer");
    printf("%-15s %-10s %-8s %s\n", "uint64", "", "8B", "Unsigned long integer");
    printf("\n");
    
    // Floating point types
    printf("%-15s %-10s %-8s %s\n", "float", "num", "4B", "Default float (32-bit)");
    printf("%-15s %-10s %-8s %s\n", "float32", "", "4B", "Single precision float");
    printf("%-15s %-10s %-8s %s\n", "float64", "double", "8B", "Double precision float");
    printf("%-15s %-10s %-8s %s\n", "decimal", "", "16B", "High precision decimal");
    printf("%-15s %-10s %-8s %s\n", "bignum", "", "8B", "Alias for float64");
    printf("\n");
    
    // Special types
    printf("%-15s %-10s %-8s %s\n", "bool", "", "1B", "Boolean true/false");
    printf("%-15s %-10s %-8s %s\n", "timestamp", "", "8B", "Unix timestamp");
    printf("%-15s %-10s %-8s %s\n", "date", "", "4B", "Date only");
    printf("%-15s %-10s %-8s %s\n", "uuid", "", "36B", "UUID string");
    printf("%-15s %-10s %-8s %s\n", "json", "", "VAR", "JSON object");
    printf("%-15s %-10s %-8s %s\n", "blob", "", "VAR", "Binary data");
    
    printf("───────────────────────────────────────────────────────────────────\n");
    printf("\n💡 Smart Defaults:\n");
    printf("   • 'string' → string256 (256 chars)\n");
    printf("   • 'int' → int32 (32-bit integer)\n");
    printf("   • 'float' → float32 (32-bit float)\n");
    printf("   • 'double' → float64 (64-bit float)\n");
    printf("   • 'num' → float32 (alias for float)\n");
    printf("   • 'bignum' → float64 (alias for double)\n");
    printf("\n📝 Examples:\n");
    printf("   create users.fxdb schema=\"id int, name string64, email string128\"\n");
    printf("   create data.fxdb schema=\"value bignum, created timestamp\"\n");
    printf("   create compact.fxdb schema=\"flag bool, code string16, score float\"\n\n");
}

// Show comprehensive examples
void flexon_show_examples(void) {
    printf("\n📋 FlexonDB Usage Examples\n");
    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("\n🏗️ Creating Databases:\n");
    printf("   flexon create users.fxdb --schema \"id int32, name string64, email string128\"\n");
    printf("   flexon create products.fxdb --schema \"id int, name string, price float64\"\n");
    printf("   flexon create compact.fxdb --schema \"flag bool, code string16\"\n");
    printf("\n📝 Inserting Data:\n");
    printf("   flexon insert users.fxdb --data '{\"id\": 1, \"name\": \"Alice\", \"email\": \"alice@example.com\"}'\n");
    printf("   flexon insert products.fxdb --data '{\"id\": 1, \"name\": \"Widget\", \"price\": 19.99}'\n");
    printf("\n📖 Reading Data:\n");
    printf("   flexon read users.fxdb\n");
    printf("   flexon read users.fxdb --limit 10\n");
    printf("   flexon info users.fxdb\n");
    printf("\n📤 Exporting Data:\n");
    printf("   flexon dump users.fxdb --format csv\n");
    printf("   flexon dump users.fxdb --format json\n");
    printf("   flexon dump users.fxdb --format table\n");
    printf("\n📁 Managing Files:\n");
    printf("   flexon list\n");
    printf("   flexon list --directory /path/to/databases\n\n");
}

// Find command index in registry
int find_command_index(const char* command_name) {
    for (int i = 0; flexon_commands[i].name; i++) {
        if (strcmp(flexon_commands[i].name, command_name) == 0) {
            return i;
        }
    }
    return -1;
}

// Basic command handlers (placeholders for now)
int cmd_help(int argc, char* argv[]) {
    if (argc > 1) {
        flexon_show_command_help(argv[1]);
    } else {
        flexon_show_help_table(NULL);
    }
    return 0;
}

int cmd_types(int argc, char* argv[]) {
    (void)argc; (void)argv;  // Suppress unused parameter warnings
    flexon_show_data_types();
    return 0;
}

int shell_cmd_help(const char* command) {
    // Parse command to extract argument
    if (strstr(command, "help ") == command) {
        const char* arg = command + 5;
        while (*arg == ' ') arg++;  // Skip whitespace
        if (*arg) {
            flexon_show_command_help(arg);
        } else {
            flexon_show_help_table(NULL);
        }
    } else {
        flexon_show_help_table(NULL);
    }
    return 0;
}

int shell_cmd_types(const char* command) {
    (void)command;  // Suppress unused parameter warning
    flexon_show_data_types();
    return 0;
}

// Placeholder implementations for other commands
int cmd_create(int argc, char* argv[]) { 
    printf("CLI create command: argc=%d\n", argc);
    return 0; 
}
int cmd_insert(int argc, char* argv[]) { 
    printf("CLI insert command: argc=%d\n", argc);
    return 0; 
}
int cmd_read(int argc, char* argv[]) { 
    printf("CLI read command: argc=%d\n", argc);
    return 0; 
}
int cmd_info(int argc, char* argv[]) { 
    printf("CLI info command: argc=%d\n", argc);
    return 0; 
}
int cmd_dump(int argc, char* argv[]) { 
    printf("CLI dump command: argc=%d\n", argc);
    return 0; 
}
int cmd_list(int argc, char* argv[]) { 
    printf("CLI list command: argc=%d\n", argc);
    return 0; 
}

int shell_cmd_create(const char* command) { 
    printf("Shell create command: %s\n", command);
    return 0; 
}
int shell_cmd_insert(const char* command) { 
    printf("Shell insert command: %s\n", command);
    return 0; 
}
int shell_cmd_read(const char* command) { 
    printf("Shell read command: %s\n", command);
    return 0; 
}
int shell_cmd_info(const char* command) { 
    printf("Shell info command: %s\n", command);
    return 0; 
}
int shell_cmd_dump(const char* command) { 
    printf("Shell dump command: %s\n", command);
    return 0; 
}
int shell_cmd_list(const char* command) { 
    printf("Shell list command: %s\n", command);
    return 0; 
}