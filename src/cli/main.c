#define _GNU_SOURCE
#include "../../include/schema.h"
#include "../../include/writer.h"
#include "../../include/reader.h"
#include "../../include/shell.h"
#include "../../include/welcome.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

// Print usage information
void print_usage(const char *program_name)
{
    printf("Usage: %s <command> [options]\n\n", program_name);
    printf("Commands:\n");
    printf("  create <file.fxdb> --schema \"field1 type1, field2 type2, ...\" [-d directory]\n");
    printf("         Create a new FlexonDB file with specified schema\n\n");
    printf("  insert <file.fxdb> --data '{\"field1\": \"value1\", \"field2\": value2}' [-d directory]\n");
    printf("         Insert a row into existing database (JSON format)\n\n");
    printf("  read   <file.fxdb> [--limit N] [-d directory]\n");
    printf("         Read and display rows from database\n\n");
    printf("  info   <file.fxdb> [-d directory]\n");
    printf("         Show database information and schema\n\n");
    printf("  dump   <file.fxdb> [-d directory]\n");
    printf("         Export all data in readable format\n\n");
    printf("  list   [-d directory]\n");
    printf("         List all .fxdb files in directory\n\n");
    printf("Options:\n");
    printf("  -d, --directory <path>  Specify directory for database files\n");
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

// Helper function to build full file path
char *build_file_path(const char *directory, const char *filename)
{
    if (!filename)
        return NULL;

    // If no directory specified, use current directory
    if (!directory || strlen(directory) == 0)
    {
        return strdup(filename);
    }

    // Build full path
    size_t dir_len = strlen(directory);
    size_t file_len = strlen(filename);
    size_t total_len = dir_len + file_len + 2; // +2 for '/' and '\0'

    char *full_path = malloc(total_len);
    if (!full_path)
        return NULL;

    snprintf(full_path, total_len, "%s/%s", directory, filename);
    return full_path;
}

// List command - show all .fxdb files in directory
int cmd_list(const char *directory)
{
    const char *search_dir = directory ? directory : ".";

    printf("📂 FlexonDB files in directory: %s\n\n", search_dir);

    // Simple implementation using system command
    char command[512];
    snprintf(command, sizeof(command), "find \"%s\" -maxdepth 1 -name \"*.fxdb\" -type f 2>/dev/null", search_dir);

    FILE *fp = popen(command, "r");
    if (!fp)
    {
        printf("❌ Failed to list files in directory\n");
        return 1;
    }

    char line[512];
    int count = 0;

    while (fgets(line, sizeof(line), fp))
    {
        // Remove newline
        line[strcspn(line, "\n")] = 0;

        // Extract just the filename
        char *filename = strrchr(line, '/');
        if (filename)
        {
            filename++; // Skip the '/'
        }
        else
        {
            filename = line;
        }

        printf("  📄 %s", filename);

        // Try to get file size
        struct stat st;
        if (stat(line, &st) == 0)
        {
            printf(" (%lld bytes)", (long long)st.st_size);
        }
        printf("\n");
        count++;
    }

    pclose(fp);

    if (count == 0)
    {
        printf("  No .fxdb files found.\n");
        printf("  💡 Use 'flexon create <filename> --schema \"...\"' to create a database.\n");
    }
    else
    {
        printf("\n📊 Found %d database file(s).\n", count);
    }

    return 0;
}

// Create command with directory support
int cmd_create(const char *filename, const char *schema_str, const char *directory)
{
    // Build full file path
    char *full_path = build_file_path(directory, filename);
    if (!full_path)
    {
        printf("❌ Failed to build file path\n");
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

    writer_t *writer = writer_create_default(full_path, schema);
    if (!writer)
    {
        printf("❌ Failed to create database file\n");
        free_schema(schema);
        free(full_path);
        return 1;
    }

    if (writer_close(writer) != 0)
    {
        printf("❌ Failed to finalize database file\n");
        writer_free(writer);
        free_schema(schema);
        free(full_path);
        return 1;
    }

    writer_free(writer);
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

// Info command with directory support
int cmd_info(const char *filename, const char *directory)
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

// Parse command line arguments with directory support
int main(int argc, char *argv[])
{
    // Check if no arguments provided - launch interactive shell
    if (argc == 1)
    {
        return run_interactive_shell(NULL);
    }

    // Check if only directory flag provided - launch interactive shell with directory
    if (argc == 3 && (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--directory") == 0))
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

    // Parse directory option from any position
    for (int i = 2; i < argc; i++)
    {
        if ((strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--directory") == 0) && i + 1 < argc)
        {
            directory = argv[i + 1];
            break;
        }
    }

    if (strcmp(command, "create") == 0)
    {
        if (argc < 5 || strcmp(argv[3], "--schema") != 0)
        {
            printf("❌ Usage: %s create <file.fxdb> --schema \"field1 type1, field2 type2\" [-d directory]\n", argv[0]);
            return 1;
        }
        return cmd_create(argv[2], argv[4], directory);
    }
    else if (strcmp(command, "info") == 0)
    {
        if (argc < 3)
        {
            printf("❌ Usage: %s info <file.fxdb> [-d directory]\n", argv[0]);
            return 1;
        }
        return cmd_info(argv[2], directory);
    }
    else if (strcmp(command, "read") == 0)
    {
        if (argc < 3)
        {
            printf("❌ Usage: %s read <file.fxdb> [--limit N] [-d directory]\n", argv[0]);
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
        printf("❌ Insert command not yet implemented\n");
        printf("💡 Use the test programs to add sample data for now\n");
        return 1;
    }
    else if (strcmp(command, "dump") == 0)
    {
        printf("❌ Dump command not yet implemented\n");
        printf("💡 Use 'read' command to view data for now\n");
        return 1;
    }
    else
    {
        printf("❌ Unknown command: %s\n\n", command);
        print_usage(argv[0]);
        return 1;
    }
}