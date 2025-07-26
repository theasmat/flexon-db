#include "../include/config.h"
#include "../include/types.h"
#include "../include/error.h"
#include "../include/utils.h"
#include <stdio.h>

int main() {
    printf("=== FlexonDB Centralized Configuration Demo ===\n\n");
    
    // Test configuration constants
    printf("📁 File Extensions:\n");
    printf("   Database: %s\n", FXDB_EXT);
    printf("   Temporary: %s\n", TEMP_EXT);
    printf("   Backup: %s\n", BACKUP_EXT);
    printf("\n");
    
    printf("📊 Database Limits:\n");
    printf("   Max Columns: %d\n", MAX_COLUMNS);
    printf("   Max Field Name Length: %d\n", MAX_FIELD_NAME_LENGTH);
    printf("   Max String Length: %d\n", MAX_STRING_LENGTH);
    printf("   Max Chunk Size: %d\n", MAX_CHUNK_SIZE);
    printf("\n");
    
    printf("🔧 Compile-time Switches:\n");
    printf("   Logging: %s\n", ENABLE_LOGGING ? "Enabled" : "Disabled");
    printf("   Debug: %s\n", ENABLE_DEBUG ? "Enabled" : "Disabled");
    printf("   Color Output: %s\n", ENABLE_COLOR_OUTPUT ? "Enabled" : "Disabled");
    printf("\n");
    
    // Test field type utilities
    printf("🔤 Field Type System:\n");
    printf("   int32 -> %s\n", utils_field_type_to_string(TYPE_INT32));
    printf("   float -> %s\n", utils_field_type_to_string(TYPE_FLOAT));
    printf("   string -> %s\n", utils_field_type_to_string(TYPE_STRING));
    printf("   bool -> %s\n", utils_field_type_to_string(TYPE_BOOL));
    printf("\n");
    
    // Test error handling
    printf("⚠️  Error Handling System:\n");
    error_context_t error;
    error_init(&error);
    
    SET_ERROR(&error, ERROR_INVALID_DATA, 100, "Test error message");
    printf("   Category: %s\n", error_category_to_string(error.category));
    printf("   Message: %s\n", error.message);
    printf("   Recoverable: %s\n", error_is_recoverable(&error) ? "Yes" : "No");
    printf("\n");
    
    // Test utilities
    printf("🛠️  Utility Functions:\n");
    char buffer[256];
    utils_format_file_size(1024*1024*1024, buffer, sizeof(buffer));
    printf("   File size (1GB): %s\n", buffer);
    
    uint64_t timestamp = utils_get_timestamp();
    utils_format_timestamp(timestamp, buffer, sizeof(buffer));
    printf("   Current time: %s\n", buffer);
    
    utils_get_current_user(buffer, sizeof(buffer));
    printf("   Current user: %s\n", buffer);
    printf("\n");
    
    // Test validation
    printf("✅ Validation Functions:\n");
    printf("   'user_name' is valid field name: %s\n", 
           utils_validate_field_name("user_name") ? "Yes" : "No");
    printf("   'invalid-name' is valid field name: %s\n", 
           utils_validate_field_name("invalid-name") ? "Yes" : "No");
    printf("   'mydatabase.fxdb' is valid database name: %s\n", 
           utils_validate_database_name("mydatabase.fxdb") ? "Yes" : "No");
    printf("\n");
    
    printf("✨ FlexonDB centralized configuration system is working!\n");
    return 0;
}