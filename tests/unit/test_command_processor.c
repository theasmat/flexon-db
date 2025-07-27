#include "../test_utils.h"
#include "../../include/common/command_processor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    test_init("Unified Command Processor Tests");
    
    // Test 1: Command registry validation
    printf("Test 1: Command registry validation\n");
    int command_count = 0;
    for (int i = 0; flexon_commands[i].name; i++) {
        command_count++;
        test_assert_not_null(flexon_commands[i].name, "Command name should not be NULL");
        test_assert_not_null(flexon_commands[i].description, "Command description should not be NULL");
        test_assert_not_null(flexon_commands[i].usage, "Command usage should not be NULL");
        test_assert_not_null(flexon_commands[i].examples, "Command examples should not be NULL");
        test_assert_not_null(flexon_commands[i].cli_handler, "CLI handler should not be NULL");
        test_assert_not_null(flexon_commands[i].shell_handler, "Shell handler should not be NULL");
    }
    test_assert(command_count >= 8, "Should have at least 8 commands registered");
    
    // Test 2: Command lookup
    printf("\nTest 2: Command lookup functionality\n");
    test_assert_equal_int(0, find_command_index("create"), "create command index");
    test_assert_equal_int(1, find_command_index("insert"), "insert command index");
    test_assert_equal_int(6, find_command_index("help"), "help command index");
    test_assert_equal_int(7, find_command_index("types"), "types command index");
    test_assert_equal_int(-1, find_command_index("nonexistent"), "nonexistent command should return -1");
    
    // Test 3: Help system (basic validation)
    printf("\nTest 3: Help system basic validation\n");
    printf("Testing help table display...\n");
    flexon_show_help_table(NULL);
    printf("✅ Help table displayed successfully\n");
    
    printf("Testing specific command help...\n");
    flexon_show_command_help("create");
    printf("✅ Command help displayed successfully\n");
    
    printf("Testing data types help...\n");
    flexon_show_data_types();
    printf("✅ Data types help displayed successfully\n");
    
    // Test 4: Command handlers (basic invocation)
    printf("\nTest 4: Command handlers basic invocation\n");
    char* help_args[] = {"help", "create"};
    int help_result = cmd_help(2, help_args);
    test_assert_equal_int(0, help_result, "help command should return 0");
    
    char* types_args[] = {"types"};
    int types_result = cmd_types(1, types_args);
    test_assert_equal_int(0, types_result, "types command should return 0");
    
    // Test 5: Shell command handlers
    printf("\nTest 5: Shell command handlers\n");
    int shell_help_result = shell_cmd_help("help create");
    test_assert_equal_int(0, shell_help_result, "shell help command should return 0");
    
    int shell_types_result = shell_cmd_types("types");
    test_assert_equal_int(0, shell_types_result, "shell types command should return 0");
    
    // Test 6: Verify all commands have consistent naming
    printf("\nTest 6: Command naming consistency\n");
    const char* expected_commands[] = {"create", "insert", "read", "info", "dump", "list", "help", "types"};
    int expected_count = sizeof(expected_commands) / sizeof(expected_commands[0]);
    
    for (int i = 0; i < expected_count; i++) {
        int index = find_command_index(expected_commands[i]);
        test_assert(index >= 0, expected_commands[i]);
        if (index >= 0) {
            test_assert_equal_str(expected_commands[i], flexon_commands[index].name, 
                                  "Command name should match expected");
        }
    }
    
    printf("\n");
    return test_finalize();
}