#define _GNU_SOURCE
// Enable BSD extensions on macOS and other BSD systems
#ifndef _WIN32
    #define _DEFAULT_SOURCE
    #define _BSD_SOURCE
#endif
#include "../../include/schema.h"
#include "../../include/writer.h"
#include "../../include/reader.h"
#include "../../include/shell.h"
#include "../../include/welcome.h"
#include "../../include/io_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

// Cross-platform directory scanning
#ifdef _WIN32
    #include <windows.h>
    #define PATH_SEPARATOR '\\'
    #define PATH_SEPARATOR_STR "\\"
#else
    #include <dirent.h>
    #define PATH_SEPARATOR '/'
    #define PATH_SEPARATOR_STR "/"
    
    // Fallback definitions for BSD dirent constants if not available
    #ifndef DT_UNKNOWN
        #define DT_UNKNOWN 0
    #endif
    #ifndef DT_REG
        #define DT_REG 8
    #endif
#endif

// Print usage information
void print_usage(const char *program_name)
{
    printf("Usage: %s <command> [options]\n\n", program_name);
    printf("Commands:\n");
    printf("  create <file.fxdb> --schema \"field1 type1, field2 type2, ...\" [-d directory] [-p path]\n");
    printf("         Create a new FlexonDB file with specified schema\n\n");
    printf("  insert <file.fxdb> --data '{\"field1\": \"value1\", \"field2\": value2}' [-d directory] [-p path]\n");
    printf("         Insert a row into existing database (JSON format)\n\n");
    printf("  read   <file.fxdb> [--limit N] [-d directory] [-p path]\n");
    printf("         Read and display rows from database\n\n");
    printf("  info   <file.fxdb> [-d directory] [-p path]\n");
    printf("         Show database information and schema\n\n");
    printf("  dump   <file.fxdb> [--format csv|json|table] [-d directory] [-p path]\n");
    printf("         Export all data in specified format (default: table)\n\n");
    printf("  list   [-d directory] [-p path]\n");
    printf("         List all .fxdb files in directory\n\n");
    printf("Options:\n");
    printf("  -d, --directory <path>  Specify directory for database files\n");
    printf("  -p, --path <path>       Specify path for database files (same as -d)\n");
    printf("                         (default: current working directory)\n\n");
    printf("Data Types:\n");
    printf("  int32   - 32-bit signed integer\n");
    printf("  float   - 32-bit floating point\n");
    printf("  string  - Variable length string (max 256 chars)\n");
    printf("  bool    - Boolean (true/false)\n\n");
    printf("Examples:\n");
    printf("  %s create people.fxdb --schema \"name string, age int32, salary float\"\n", program_name);
    printf("  %s create people.fxdb --schema \"name string, age int32\" -d /path/to/db\n", program_name);
    printf("  %s insert people.fxdb --data '{\"name\": \"Alice\", \"age\": 30}' -d /path/to/db\n", program_name);
    printf("  %s read people.fxdb --limit 10\n", program_name);
    printf("  %s dump people.fxdb --format csv\n", program_name);
    printf("  %s dump people.fxdb --format json -d /home/user/databases\n", program_name);
    printf("  %s info people.fxdb -d /home/user/databases\n", program_name);
    printf("  %s list -d /home/user/databases\n", program_name);
    printf("\n");
}

// Helper function to check if directory exists
int directory_exists(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

// Helper function to create directory if it doesn't exist
int create_directory(const char *path)
{
    if (directory_exists(path))
    {
        return 0; // Already exists
    }

    if (mkdir(path, 0755) == 0)
    {
        printf("📁 Created directory: %s\n", path);
        return 0;
    }

    fprintf(stderr, "❌ Failed to create directory '%s': %s\n", path, strerror(errno));
    return -1;
}

// Helper function to check if file exists
int file_exists(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

// Cross-platform path joining function
char* join_path(const char* dir, const char* filename) {
    if (!filename) return NULL;
    if (!dir || strlen(dir) == 0) return strdup(filename);
    
    size_t dir_len = strlen(dir);
    size_t file_len = strlen(filename);
    
    // Check if directory already ends with separator
    bool need_separator = (dir_len > 0 && dir[dir_len - 1] != PATH_SEPARATOR);
    size_t total_len = dir_len + file_len + (need_separator ? 1 : 0) + 1; // +1 for '\0'
    
    char* result = malloc(total_len);
    if (!result) return NULL;
    
    strcpy(result, dir);
    if (need_separator) {
        strcat(result, PATH_SEPARATOR_STR);
    }
    strcat(result, filename);
    
    return result;
}

// Helper function to build full file path with .fxdb extension normalization
char *build_file_path(const char *directory, const char *filename)
{
    if (!filename)
        return NULL;

    // First normalize the filename to ensure .fxdb extension
    char* normalized_filename = fxdb_normalize_filename(filename);
    if (!normalized_filename)
        return NULL;

    // If no directory specified, use current directory
    if (!directory || strlen(directory) == 0)
    {
        return normalized_filename; // Already allocated by fxdb_normalize_filename
    }

    // Build full path using cross-platform path joining
    char *full_path = join_path(directory, normalized_filename);
    free(normalized_filename);
    return full_path;
}

// Cross-platform directory listing function
int list_fxdb_files(const char* directory, char*** file_list, int* file_count) {
    *file_list = NULL;
    *file_count = 0;
    
#ifdef _WIN32
    // Windows implementation
    char search_pattern[MAX_PATH];
    snprintf(search_pattern, sizeof(search_pattern), "%s\\*.fxdb", directory);
    
    WIN32_FIND_DATA find_data;
    HANDLE find_handle = FindFirstFile(search_pattern, &find_data);
    
    if (find_handle == INVALID_HANDLE_VALUE) {
        return 0; // No files found, not an error
    }
    
    // Count files first
    int count = 0;
    do {
        if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            count++;
        }
    } while (FindNextFile(find_handle, &find_data));
    FindClose(find_handle);
    
    if (count == 0) {
        return 0;
    }
    
    // Allocate file list
    *file_list = malloc(count * sizeof(char*));
    if (!*file_list) {
        return -1;
    }
    
    // Collect files
    find_handle = FindFirstFile(search_pattern, &find_data);
    int index = 0;
    do {
        if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            (*file_list)[index] = strdup(find_data.cFileName);
            if (!(*file_list)[index]) {
                // Cleanup on error
                for (int i = 0; i < index; i++) {
                    free((*file_list)[i]);
                }
                free(*file_list);
                *file_list = NULL;
                FindClose(find_handle);
                return -1;
            }
            index++;
        }
    } while (FindNextFile(find_handle, &find_data) && index < count);
    FindClose(find_handle);
    
    *file_count = count;
    return 0;
    
#else
    // Unix/Linux implementation
    DIR* dir = opendir(directory);
    if (!dir) {
        return -1;
    }
    
    // Count .fxdb files first
    int count = 0;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG || entry->d_type == DT_UNKNOWN) {
            size_t name_len = strlen(entry->d_name);
            if (name_len > 5 && strcmp(entry->d_name + name_len - 5, ".fxdb") == 0) {
                count++;
            }
        }
    }
    rewinddir(dir);
    
    if (count == 0) {
        closedir(dir);
        return 0;
    }
    
    // Allocate file list
    *file_list = malloc(count * sizeof(char*));
    if (!*file_list) {
        closedir(dir);
        return -1;
    }
    
    // Collect files
    int index = 0;
    while ((entry = readdir(dir)) != NULL && index < count) {
        if (entry->d_type == DT_REG || entry->d_type == DT_UNKNOWN) {
            size_t name_len = strlen(entry->d_name);
            if (name_len > 5 && strcmp(entry->d_name + name_len - 5, ".fxdb") == 0) {
                (*file_list)[index] = strdup(entry->d_name);
                if (!(*file_list)[index]) {
                    // Cleanup on error
                    for (int i = 0; i < index; i++) {
                        free((*file_list)[i]);
                    }
                    free(*file_list);
                    *file_list = NULL;
                    closedir(dir);
                    return -1;
                }
                index++;
            }
        }
    }
    closedir(dir);
    
    *file_count = count;
    return 0;
#endif
}

// List command - show all .fxdb files in directory (cross-platform)
int cmd_list(const char *directory)
{
    const char *search_dir = directory ? directory : ".";
    printf("📂 FlexonDB files in directory: %s\n\n", search_dir);

    char** file_list = NULL;
    int file_count = 0;
    int result = list_fxdb_files(search_dir, &file_list, &file_count);
    
    if (result != 0) {
        printf("❌ Failed to list files in directory\n");
        return 1;
    }
    
    if (file_count == 0) {
        printf("  No .fxdb files found.\n");
        printf("  💡 Use 'flexon create <filename> --schema \"...\"' to create a database.\n");
        return 0;
    }
    
    // Display files with size information
    for (int i = 0; i < file_count; i++) {
        printf("  📄 %s", file_list[i]);
        
        // Try to get file size
        char* full_path = join_path(search_dir, file_list[i]);
        if (full_path) {
            struct stat st;
            if (stat(full_path, &st) == 0) {
                printf(" (%lld bytes)", (long long)st.st_size);
            }
            free(full_path);
        }
        printf("\n");
        
        free(file_list[i]);
    }
    
    free(file_list);
    printf("\n📊 Found %d database file(s).\n", file_count);
    return 0;
}

// Create command with directory support and enhanced file handling
int cmd_create(const char *filename, const char *schema_str, const char *directory)
{
    // Build full file path (includes normalization to .fxdb)
    char *full_path = build_file_path(directory, filename);
    if (!full_path)
    {
        printf("❌ Failed to build file path\n");
        return 1;
    }

    // Check if file already exists
    if (fxdb_database_exists(full_path))
    {
        printf("❌ Database already exists: %s\n", full_path);
        printf("💡 Use a different filename or delete the existing database first.\n");
        free(full_path);
        return 1;
    }

    // Create directory if specified and doesn't exist
    if (directory && create_directory(directory) != 0)
    {
        free(full_path);
        return 1;
    }

    printf("🛠️  Creating database: %s\n", full_path);
    printf("📋 Schema: %s\n\n", schema_str);

    schema_t *schema = parse_schema(schema_str);
    if (!schema)
    {
        printf("❌ Failed to parse schema\n");
        free(full_path);
        return 1;
    }

    printf("✅ Parsed schema:\n");
    print_schema(schema);
    printf("\n");

    // Use enhanced database creation
    int result = fxdb_database_create(full_path, schema, NULL);
    if (result != 0)
    {
        printf("❌ Failed to create database file\n");
        free_schema(schema);
        free(full_path);
        return 1;
    }

    free_schema(schema);

    printf("🎉 Database created successfully: %s\n", full_path);

    // Show file info
    struct stat st;
    if (stat(full_path, &st) == 0)
    {
        printf("📊 File size: %lld bytes\n", (long long)st.st_size);
    }

    free(full_path);
    return 0;
}

// Info command with directory support and enhanced error handling
int cmd_info(const char *filename, const char *directory)
{
    char *full_path = build_file_path(directory, filename);
    if (!full_path)
    {
        printf("❌ Failed to build file path\n");
        return 1;
    }

    // Check if database exists first
    if (!fxdb_database_exists(full_path))
    {
        printf("❌ Database does not exist: %s\n", full_path);
        printf("💡 Use 'flexon list' to see available databases or 'flexon create' to create a new one.\n");
        free(full_path);
        return 1;
    }

    reader_t *reader = reader_open(full_path);
    if (!reader)
    {
        printf("❌ Failed to open database: %s\n", full_path);
        printf("💡 The file may be corrupted or in an unsupported format.\n");
        free(full_path);
        return 1;
    }

    printf("📊 Database Information: %s\n\n", full_path);

    uint32_t total_rows, total_chunks;
    reader_get_stats(reader, &total_rows, &total_chunks);

    printf("File Statistics:\n");
    printf("  📁 Full path: %s\n", full_path);
    printf("  📈 File format version: %u\n", reader->header.version);
    printf("  📊 Total rows: %u\n", total_rows);
    printf("  📦 Total chunks: %u\n", total_chunks);
    printf("  🔧 Chunk size: %u rows\n", reader->header.chunk_size);
    printf("  💾 Schema size: %u bytes\n", reader->header.schema_size);
    printf("  💾 Data size: %u bytes\n", reader->header.data_size);

    // Show file size
    struct stat st;
    if (stat(full_path, &st) == 0)
    {
        printf("  📏 Total file size: %lld bytes\n", (long long)st.st_size);
    }

    printf("\n");

    printf("Schema:\n");
    print_schema(reader->schema);

    reader_close(reader);
    free(full_path);
    return 0;
}

// Read command with directory support
int cmd_read(const char *filename, uint32_t limit, const char *directory)
{
    char *full_path = build_file_path(directory, filename);
    if (!full_path)
    {
        printf("❌ Failed to build file path\n");
        return 1;
    }

    reader_t *reader = reader_open(full_path);
    if (!reader)
    {
        printf("❌ Failed to open database: %s\n", full_path);
        free(full_path);
        return 1;
    }

    printf("📖 Reading from database: %s\n\n", full_path);

    if (limit == 0)
    {
        limit = reader_get_row_count(reader);
    }

    query_result_t *result = reader_read_rows(reader, limit);
    if (!result)
    {
        printf("❌ Failed to read rows\n");
        reader_close(reader);
        free(full_path);
        return 1;
    }

    reader_print_rows(reader, result);

    reader_free_result(result);
    reader_close(reader);
    free(full_path);
    return 0;
}

// Insert command implementation
int cmd_insert(const char *filename, const char *json_data, const char *directory)
{
    char *full_path = build_file_path(directory, filename);
    if (!full_path)
    {
        printf("❌ Failed to build file path\n");
        return 1;
    }

    // Check if database exists
    if (!file_exists(full_path))
    {
        printf("❌ Database file does not exist: %s\n", full_path);
        printf("💡 Use 'create' command to create a new database first\n");
        free(full_path);
        return 1;
    }

    // Open database for appending
    writer_t *writer = writer_open(full_path);
    if (!writer)
    {
        printf("❌ Failed to open database for insertion: %s\n", full_path);
        free(full_path);
        return 1;
    }

    printf("📝 Inserting data into: %s\n", full_path);
    printf("🔍 Data: %s\n", json_data);

    // Insert JSON data
    if (writer_insert_json(writer, json_data) == 0)
    {
        printf("✅ Data inserted successfully\n");
    }
    else
    {
        printf("❌ Failed to insert data\n");
        writer_close(writer);
        writer_free(writer);
        free(full_path);
        return 1;
    }

    // Close writer
    writer_close(writer);
    writer_free(writer);
    free(full_path);
    return 0;
}

// Dump command implementation
int cmd_dump(const char *filename, const char *format, const char *directory)
{
    char *full_path = build_file_path(directory, filename);
    if (!full_path)
    {
        printf("❌ Failed to build file path\n");
        return 1;
    }

    reader_t *reader = reader_open(full_path);
    if (!reader)
    {
        printf("❌ Failed to open database: %s\n", full_path);
        free(full_path);
        return 1;
    }

    printf("📤 Dumping data from: %s\n", full_path);
    
    uint32_t total_rows = reader_get_row_count(reader);
    if (total_rows == 0)
    {
        printf("📄 Database is empty\n");
        reader_close(reader);
        free(full_path);
        return 0;
    }

    printf("📊 Format: %s | Total rows: %u\n\n", format ? format : "table", total_rows);

    query_result_t *result = reader_read_rows(reader, total_rows);
    if (!result)
    {
        printf("❌ Failed to read data\n");
        reader_close(reader);
        free(full_path);
        return 1;
    }

    // Output in requested format
    if (format && strcmp(format, "csv") == 0)
    {
        // CSV output
        const schema_t *schema = reader->schema;
        
        // Print CSV header
        for (uint32_t i = 0; i < schema->field_count; i++)
        {
            printf("%s", schema->fields[i].name);
            if (i < schema->field_count - 1) printf(",");
        }
        printf("\n");
        
        // Print CSV data rows
        for (uint32_t r = 0; r < result->row_count; r++)
        {
            const row_data_t *row = &result->rows[r];
            for (uint32_t f = 0; f < row->field_count; f++)
            {
                const field_value_t *value = &row->values[f];
                
                switch (schema->fields[f].type)
                {
                    case TYPE_STRING:
                        printf("\"%s\"", value->value.string_val ? value->value.string_val : "");
                        break;
                    case TYPE_INT32:
                        printf("%d", value->value.int32_val);
                        break;
                    case TYPE_FLOAT:
                        printf("%.2f", value->value.float_val);
                        break;
                    case TYPE_BOOL:
                        printf("%s", value->value.bool_val ? "true" : "false");
                        break;
                    default:
                        printf("null");
                        break;
                }
                if (f < row->field_count - 1) printf(",");
            }
            printf("\n");
        }
    }
    else if (format && strcmp(format, "json") == 0)
    {
        // JSON output
        const schema_t *schema = reader->schema;
        
        printf("[\n");
        for (uint32_t r = 0; r < result->row_count; r++)
        {
            const row_data_t *row = &result->rows[r];
            printf("  {");
            
            for (uint32_t f = 0; f < row->field_count; f++)
            {
                const field_value_t *value = &row->values[f];
                printf("\"%s\": ", schema->fields[f].name);
                
                switch (schema->fields[f].type)
                {
                    case TYPE_STRING:
                        printf("\"%s\"", value->value.string_val ? value->value.string_val : "");
                        break;
                    case TYPE_INT32:
                        printf("%d", value->value.int32_val);
                        break;
                    case TYPE_FLOAT:
                        printf("%.2f", value->value.float_val);
                        break;
                    case TYPE_BOOL:
                        printf("%s", value->value.bool_val ? "true" : "false");
                        break;
                    default:
                        printf("null");
                        break;
                }
                if (f < row->field_count - 1) printf(", ");
            }
            
            printf("}");
            if (r < result->row_count - 1) printf(",");
            printf("\n");
        }
        printf("]\n");
    }
    else
    {
        // Default table output
        reader_print_rows(reader, result);
    }

    reader_free_result(result);
    reader_close(reader);
    free(full_path);
    return 0;
}

// Parse command line arguments with directory support
int main(int argc, char *argv[])
{
    // Check if no arguments provided - launch interactive shell
    if (argc == 1)
    {
        return run_interactive_shell(NULL);
    }

    // Check if only directory/path flag provided - launch interactive shell with directory
    if (argc == 3 && (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--directory") == 0 ||
                      strcmp(argv[1], "-p") == 0 || strcmp(argv[1], "--path") == 0))
    {
        return run_interactive_shell(argv[2]);
    }

    // Original CLI mode - print logo and handle commands
    print_welcome_message();
    printf("\n");
    if (argc < 2)
    {
        print_usage(argv[0]);
        return 1;
    }

    const char *command = argv[1];
    const char *directory = NULL;

    // Parse directory/path option from any position
    for (int i = 2; i < argc; i++)
    {
        if ((strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--directory") == 0 ||
             strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--path") == 0) && i + 1 < argc)
        {
            directory = argv[i + 1];
            break;
        }
    }

    if (strcmp(command, "create") == 0)
    {
        if (argc < 5 || strcmp(argv[3], "--schema") != 0)
        {
            printf("❌ Usage: %s create <file.fxdb> --schema \"field1 type1, field2 type2\" [-d directory] [-p path]\n", argv[0]);
            return 1;
        }
        return cmd_create(argv[2], argv[4], directory);
    }
    else if (strcmp(command, "info") == 0)
    {
        if (argc < 3)
        {
            printf("❌ Usage: %s info <file.fxdb> [-d directory] [-p path]\n", argv[0]);
            return 1;
        }
        return cmd_info(argv[2], directory);
    }
    else if (strcmp(command, "read") == 0)
    {
        if (argc < 3)
        {
            printf("❌ Usage: %s read <file.fxdb> [--limit N] [-d directory] [-p path]\n", argv[0]);
            return 1;
        }

        uint32_t limit = 0;
        for (int i = 3; i < argc; i++)
        {
            if (strcmp(argv[i], "--limit") == 0 && i + 1 < argc)
            {
                limit = atoi(argv[i + 1]);
                break;
            }
        }

        return cmd_read(argv[2], limit, directory);
    }
    else if (strcmp(command, "list") == 0)
    {
        return cmd_list(directory);
    }
    else if (strcmp(command, "insert") == 0)
    {
        if (argc < 5 || strcmp(argv[3], "--data") != 0)
        {
            printf("❌ Insert command requires: insert <file.fxdb> --data '<json>'\n");
            printf("💡 Example: %s insert people.fxdb --data '{\"name\": \"Alice\", \"age\": 30}'\n", argv[0]);
            return 1;
        }
        return cmd_insert(argv[2], argv[4], directory);
    }
    else if (strcmp(command, "dump") == 0)
    {
        if (argc < 3)
        {
            printf("❌ Dump command requires: dump <file.fxdb> [--format csv|json|table]\n");
            return 1;
        }
        
        // Check for format option
        const char* format = "table"; // default
        for (int i = 3; i < argc; i++)
        {
            if (strcmp(argv[i], "--format") == 0 && i + 1 < argc)
            {
                format = argv[i + 1];
                break;
            }
        }
        
        return cmd_dump(argv[2], format, directory);
    }
    else
    {
        printf("❌ Unknown command: %s\n\n", command);
        print_usage(argv[0]);
        return 1;
    }
}