#include "../test_utils.h"
#include "../../include/common/command_processor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    test_init("CLI/Shell Parity Tests");
    
    // Test 1: Verify all commands have both CLI and shell handlers
    printf("Test 1: Command handler parity validation\n");
    int command_count = 0;
    for (int i = 0; flexon_commands[i].name; i++) {
        command_count++;
        
        // Verify both handlers exist
        test_assert_not_null(flexon_commands[i].cli_handler, 
                           flexon_commands[i].name);
        test_assert_not_null(flexon_commands[i].shell_handler, 
                           flexon_commands[i].name);
        
        // Verify command metadata is complete
        test_assert_not_null(flexon_commands[i].description, 
                           flexon_commands[i].name);
        test_assert_not_null(flexon_commands[i].usage, 
                           flexon_commands[i].name);
        test_assert_not_null(flexon_commands[i].examples, 
                           flexon_commands[i].name);
    }
    
    printf("Total commands verified: %d\n", command_count);
    test_assert(command_count >= 8, "Should have all expected commands");
    
    // Test 2: Help system consistency
    printf("\nTest 2: Help system consistency\n");
    
    // Test help command through both interfaces
    char* help_args[] = {"help"};
    int cli_help_result = cmd_help(1, help_args);
    test_assert_equal_int(0, cli_help_result, "CLI help should succeed");
    
    int shell_help_result = shell_cmd_help("help");
    test_assert_equal_int(0, shell_help_result, "Shell help should succeed");
    
    // Test types command through both interfaces  
    char* types_args[] = {"types"};
    int cli_types_result = cmd_types(1, types_args);
    test_assert_equal_int(0, cli_types_result, "CLI types should succeed");
    
    int shell_types_result = shell_cmd_types("types");
    test_assert_equal_int(0, shell_types_result, "Shell types should succeed");
    
    // Test 3: Command registry completeness
    printf("\nTest 3: Command registry completeness\n");
    
    const char* core_commands[] = {"create", "insert", "read", "info", "dump", "list"};
    const char* utility_commands[] = {"help", "types"};
    int core_count = sizeof(core_commands) / sizeof(core_commands[0]);
    int util_count = sizeof(utility_commands) / sizeof(utility_commands[0]);
    
    // Verify all core commands exist
    for (int i = 0; i < core_count; i++) {
        int index = find_command_index(core_commands[i]);
        test_assert(index >= 0, core_commands[i]);
    }
    
    // Verify all utility commands exist
    for (int i = 0; i < util_count; i++) {
        int index = find_command_index(utility_commands[i]);
        test_assert(index >= 0, utility_commands[i]);
    }
    
    // Test 4: Interface consistency validation
    printf("\nTest 4: Interface consistency validation\n");
    
    // All commands should provide usage and examples
    for (int i = 0; flexon_commands[i].name; i++) {
        const flexon_command_t* cmd = &flexon_commands[i];
        
        test_assert(strlen(cmd->usage) > 0, "Usage should not be empty");
        test_assert(strlen(cmd->examples) > 0, "Examples should not be empty");
        test_assert(strlen(cmd->description) > 0, "Description should not be empty");
        
        // Usage should contain the command name
        test_assert(strstr(cmd->usage, cmd->name) != NULL, 
                   "Usage should contain command name");
        
        // Examples should contain the command name
        test_assert(strstr(cmd->examples, cmd->name) != NULL, 
                   "Examples should contain command name");
    }
    
    printf("\n");
    return test_finalize();
}