// shell.c

#include "../../include/platform.h"
#include "../../include/compat.h"
#include "../../include/shell.h"
#include "../../include/welcome.h"

// External logo from main.c

extern const char *logo;

// Global flag for signal handling
static volatile int interrupt_received = 0;

/**
 * Signal handler for SIGINT (Ctrl+C)
 */
#ifdef FLEXON_HAVE_SIGNALS
static void sigint_handler(int sig)
{
    (void)sig; // Unused parameter
    interrupt_received = 1;

    printf(COLOR_WARNING "\n🛑 Caught Ctrl+C! Use " COLOR_EMPHASIS "'quit'" COLOR_WARNING ", " COLOR_EMPHASIS "'exit'" COLOR_WARNING ", or " COLOR_EMPHASIS "'q'" COLOR_WARNING " to exit gracefully." COLOR_RESET "\n");

    rl_on_new_line();
    rl_replace_line("", 0); 
    rl_redisplay();
}

/**
 * Setup signal handlers
 */
static void setup_signal_handlers(void)
{
#ifdef FLEXON_HAVE_POSIX
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);
#elif defined(FLEXON_PLATFORM_WINDOWS)
    signal(SIGINT, sigint_handler);
#endif
}
#else
/* No signal handling available */
static void setup_signal_handlers(void) {
    /* No-op on platforms without signal support */
}
#endif

/**
 * Custom readline prompt generator
 */
static char* generate_prompt(const shell_session_t *session)
{
    static char prompt[256];
    
    if (strlen(session->current_db) > 0)
    {
        // Extract database name without .fxdb extension for cleaner display
        char db_name[MAX_DATABASE_NAME_LEN];
        strncpy(db_name, session->current_db, sizeof(db_name) - 1);
        db_name[sizeof(db_name) - 1] = '\0';

        char *ext = strstr(db_name, ".fxdb");
        if (ext)
            *ext = '\0';

        snprintf(prompt, sizeof(prompt), COLOR_PROMPT "flexondb:" COLOR_EMPHASIS "%s" COLOR_PROMPT "> " COLOR_RESET, db_name);
    }
    else
    {
        snprintf(prompt, sizeof(prompt), COLOR_PROMPT "flexondb> " COLOR_RESET);
    }
    
    return prompt;
}

/**
 * Initialize shell session
 */
shell_session_t *init_session(const char *directory)
{
    shell_session_t *session = malloc(sizeof(shell_session_t));
    if (!session)
        return NULL;

    // Get current user
    get_current_user(session->user, sizeof(session->user));

    // Set working directory
    if (directory)
    {
        strncpy(session->working_dir, directory, sizeof(session->working_dir) - 1);
    }
    else
    {
        if (getcwd(session->working_dir, sizeof(session->working_dir)) == NULL)
        {
            strcpy(session->working_dir, ".");
        }
    }
    session->working_dir[sizeof(session->working_dir) - 1] = '\0';

    // Initialize session state
    session->current_db[0] = '\0';
    session->session_start = time(NULL);
    session->commands_executed = 0;

    return session;
}

/**
 * Print welcome screen with logo and session info
 */
void print_welcome_screen(const shell_session_t *session)
{

    print_welcome_message();
    printf("\n");
    // Session information
    char time_buffer[64];
    format_timestamp(session->session_start, time_buffer, sizeof(time_buffer));

    printf("👤 User: %s\n", session->user);
    printf("📅 Session started: %s\n", time_buffer);
    printf("📁 Working directory: %s\n", session->working_dir);

    // Show available databases
    printf("\n📋 Available databases:\n");
    char databases[50][MAX_DATABASE_NAME_LEN];
    int db_count = get_database_list(session->working_dir, databases, 50);

    if (db_count > 0)
    {
        for (int i = 0; i < db_count; i++)
        {
            struct stat st;
            char *full_path = get_database_path(session->working_dir, databases[i]);
            if (full_path && stat(full_path, &st) == 0)
            {
                char size_str[32];
                format_file_size(st.st_size, size_str, sizeof(size_str));
                printf("  📄 %s (%s)\n", databases[i], size_str);
                free(full_path);
            }
            else
            {
                printf("  📄 %s\n", databases[i]);
                if (full_path)
                    free(full_path);
            }
        }
    }
    else
    {
        printf("  No databases found. Use 'create <name> schema=\"...\"' to create one.\n");
    }

    printf("\n💡 Type 'help' for available commands or 'exit' to quit.\n\n");
}

/**
 * Print goodbye message with session statistics
 */
void print_goodbye(const shell_session_t *session)
{
    printf("\n");
    printf(COLOR_SUCCESS "👋 Goodbye from FlexonDB!" COLOR_RESET "\n");
    printf(COLOR_MUTED "═══════════════════════════" COLOR_RESET "\n\n");

    // Calculate session duration
    time_t end_time = time(NULL);
    double duration = difftime(end_time, session->session_start);

    printf("📊 Session Summary:\n");
    printf("   Commands executed: %u\n", session->commands_executed);
    printf("   Session duration: %.0f seconds\n", duration);

    if (strlen(session->current_db) > 0)
    {
        printf("   Last database: %s\n", session->current_db);
    }

    printf("\n✨ Thank you for using FlexonDB!\n");
}

/**
 * Free session resources
 */
void free_session(shell_session_t *session)
{
    if (session)
    {
        free(session);
    }
}

// Forward declaration for shell commands
static int cmd_shell_help(shell_session_t *session, const parsed_command_t *cmd);
static int cmd_shell_status(shell_session_t *session, const parsed_command_t *cmd);
static int cmd_shell_show_databases(shell_session_t *session, const parsed_command_t *cmd);
static int cmd_shell_use(shell_session_t *session, const parsed_command_t *cmd);
static int cmd_shell_info(shell_session_t *session, const parsed_command_t *cmd);
static int cmd_shell_schema(shell_session_t *session, const parsed_command_t *cmd);
static int cmd_shell_clear(shell_session_t *session, const parsed_command_t *cmd);
static int cmd_shell_create(shell_session_t *session, const parsed_command_t *cmd);
static int cmd_shell_count(shell_session_t *session, const parsed_command_t *cmd);

/**
 * Execute a shell command
 */
int execute_shell_command(shell_session_t *session, const parsed_command_t *cmd)
{
    if (!session || !cmd)
        return -1;

    timing_info_t timing;
    start_timing(&timing);

    int result = 0;

    switch (cmd->type)
    {
    case CMD_HELP:
        result = cmd_shell_help(session, cmd);
        break;

    case CMD_STATUS:
        result = cmd_shell_status(session, cmd);
        break;

    case CMD_SHOW_DATABASES:
        result = cmd_shell_show_databases(session, cmd);
        break;

    case CMD_USE:
        result = cmd_shell_use(session, cmd);
        break;

    case CMD_INFO:
        result = cmd_shell_info(session, cmd);
        break;

    case CMD_SCHEMA:
        result = cmd_shell_schema(session, cmd);
        break;

    case CMD_CLEAR:
        result = cmd_shell_clear(session, cmd);
        break;

    case CMD_CREATE:
        result = cmd_shell_create(session, cmd);
        break;

    case CMD_COUNT:
        result = cmd_shell_count(session, cmd);
        break;

    case CMD_EXIT:
    case CMD_QUIT:
        return 1; // Signal to exit

    case CMD_UNKNOWN:
    default:
        printf(COLOR_ERROR "❌ Unknown command: " COLOR_RESET "%s\n", cmd->args[0] ? cmd->args[0] : "");
        printf(COLOR_INFO "💡 Type 'help' for available commands." COLOR_RESET "\n");
        result = -1;
        break;
    }

    if (result == 0)
    {
        session->commands_executed++;

        // Show timing for successful commands
        double elapsed = end_timing(&timing);
        if (elapsed > 10.0)
        { // Only show timing for commands that take > 10ms
            printf(COLOR_MUTED "⏱️  Command completed in %.1f ms" COLOR_RESET "\n", elapsed);
        }
    }

    return result;
}

/**
 * Help command implementation
 */
static int cmd_shell_help(shell_session_t *session, const parsed_command_t *cmd)
{
    (void)session; // Unused parameter
    (void)cmd;     // Unused parameter

    printf("📚 FlexonDB Interactive Shell Commands\n");
    printf("═══════════════════════════════════════\n\n");

    const char *headers[] = {"Command", "Description"};
    const char *data[][2] = {
        {"use <database>", "Switch to a database"},
        {"show databases", "List all available databases"},
        {"create <db> schema=\"...\"", "Create a new database"},
        {"drop <database>", "Delete a database (not implemented)"},
        {"select * [limit N]", "Read rows from current database (not implemented)"},
        {"count", "Show row count for current database"},
        {"insert field=value ...", "Insert a row (not implemented)"},
        {"export [csv|json]", "Export data (not implemented)"},
        {"info", "Show current database information"},
        {"schema", "Show current database schema"},
        {"status", "Show session information"},
        {"clear", "Clear the screen"},
        {"history", "Show command history (not implemented)"},
        {"help", "Show this help message"},
        {"exit, quit", "Exit the shell"}};

    int column_widths[] = {25, 50};
    print_table_header(headers, 2, column_widths);

    for (int i = 0; i < 15; i++)
    {
        print_table_row(data[i], 2, column_widths);
    }

    print_table_footer(2, column_widths);

    printf("\n💡 Commands are case-sensitive. Use 'status' to see current session info.\n");
    return 0;
}

/**
 * Status command implementation
 */
static int cmd_shell_status(shell_session_t *session, const parsed_command_t *cmd)
{
    (void)cmd; // Unused parameter

    printf("📊 Session Status\n");
    printf("═════════════════\n\n");

    const char *headers[] = {"Property", "Value"};
    int column_widths[] = {20, 40};

    print_table_header(headers, 2, column_widths);

    // User
    const char *row1[] = {"User", session->user};
    print_table_row(row1, 2, column_widths);

    // Working directory
    const char *row2[] = {"Working Directory", session->working_dir};
    print_table_row(row2, 2, column_widths);

    // Current database
    const char *current_db = strlen(session->current_db) > 0 ? session->current_db : "None";
    const char *row3[] = {"Current Database", current_db};
    print_table_row(row3, 2, column_widths);

    // Session start time
    char time_buffer[64];
    format_timestamp(session->session_start, time_buffer, sizeof(time_buffer));
    const char *row4[] = {"Session Started", time_buffer};
    print_table_row(row4, 2, column_widths);

    // Commands executed
    char cmd_count[32];
    snprintf(cmd_count, sizeof(cmd_count), "%u", session->commands_executed);
    const char *row5[] = {"Commands Executed", cmd_count};
    print_table_row(row5, 2, column_widths);

    // Session duration
    time_t current_time = time(NULL);
    double duration = difftime(current_time, session->session_start);
    char duration_str[32];
    snprintf(duration_str, sizeof(duration_str), "%.0f seconds", duration);
    const char *row6[] = {"Session Duration", duration_str};
    print_table_row(row6, 2, column_widths);

    print_table_footer(2, column_widths);

    return 0;
}

/**
 * Show databases command implementation
 */
static int cmd_shell_show_databases(shell_session_t *session, const parsed_command_t *cmd)
{
    (void)cmd; // Unused parameter

    printf("📋 Available Databases\n");
    printf("══════════════════════\n\n");

    char databases[50][MAX_DATABASE_NAME_LEN];
    int db_count = get_database_list(session->working_dir, databases, 50);

    if (db_count < 0)
    {
        printf("❌ Failed to list databases in directory: %s\n", session->working_dir);
        return -1;
    }

    if (db_count == 0)
    {
        printf("No databases found in: %s\n", session->working_dir);
        printf("💡 Use 'create <name> schema=\"...\"' to create a database.\n");
        return 0;
    }

    const char *headers[] = {"Database", "Size", "Status"};
    int column_widths[] = {30, 15, 15};

    print_table_header(headers, 3, column_widths);

    for (int i = 0; i < db_count; i++)
    {
        char *full_path = get_database_path(session->working_dir, databases[i]);
        char size_str[32] = "Unknown";
        char status_str[16] = "Unknown";

        if (full_path)
        {
            struct stat st;
            if (stat(full_path, &st) == 0)
            {
                format_file_size(st.st_size, size_str, sizeof(size_str));
                strcpy(status_str, "Ready");
            }
            else
            {
                strcpy(status_str, "Error");
            }
            free(full_path);
        }

        const char *row[] = {databases[i], size_str, status_str};
        print_table_row(row, 3, column_widths);
    }

    print_table_footer(3, column_widths);

    printf("\n📊 Total: %d database(s)\n", db_count);
    return 0;
}

/**
 * Use command implementation
 */
static int cmd_shell_use(shell_session_t *session, const parsed_command_t *cmd)
{
    if (cmd->arg_count < 2)
    {
        printf("❌ Usage: use <database>\n");
        printf("💡 Use 'show databases' to see available databases.\n");
        return -1;
    }

    const char *db_name = cmd->args[1];

    // Check if database exists
    if (!database_exists(session->working_dir, db_name))
    {
        printf("❌ Database not found: %s\n", db_name);
        printf("💡 Use 'show databases' to see available databases.\n");
        return -1;
    }

    // Set current database
    strncpy(session->current_db, db_name, sizeof(session->current_db) - 1);
    session->current_db[sizeof(session->current_db) - 1] = '\0';

    printf("✅ Connected to: %s\n", db_name);
    return 0;
}

/**
 * Info command implementation
 */
static int cmd_shell_info(shell_session_t *session, const parsed_command_t *cmd)
{
    (void)cmd; // Unused parameter

    if (strlen(session->current_db) == 0)
    {
        printf("❌ No database selected. Use 'use <database>' first.\n");
        return -1;
    }

    char *full_path = get_database_path(session->working_dir, session->current_db);
    if (!full_path)
    {
        printf("❌ Failed to get database path\n");
        return -1;
    }

    reader_t *reader = reader_open(full_path);
    if (!reader)
    {
        printf("❌ Failed to open database: %s\n", session->current_db);
        free(full_path);
        return -1;
    }

    printf("📊 Database Information\n");
    printf("═══════════════════════\n\n");

    uint32_t total_rows, total_chunks;
    reader_get_stats(reader, &total_rows, &total_chunks);

    const char *headers[] = {"Property", "Value"};
    int column_widths[] = {20, 40};

    print_table_header(headers, 2, column_widths);

    // Database name
    const char *row1[] = {"Database", session->current_db};
    print_table_row(row1, 2, column_widths);

    // File size
    struct stat st;
    char size_str[32];
    if (stat(full_path, &st) == 0)
    {
        format_file_size(st.st_size, size_str, sizeof(size_str));
    }
    else
    {
        strcpy(size_str, "Unknown");
    }
    const char *row2[] = {"Size", size_str};
    print_table_row(row2, 2, column_widths);

    // Total rows
    char rows_str[32];
    snprintf(rows_str, sizeof(rows_str), "%u", total_rows);
    const char *row3[] = {"Total Rows", rows_str};
    print_table_row(row3, 2, column_widths);

    // Total chunks
    char chunks_str[32];
    snprintf(chunks_str, sizeof(chunks_str), "%u", total_chunks);
    const char *row4[] = {"Total Chunks", chunks_str};
    print_table_row(row4, 2, column_widths);

    // Chunk size
    char chunk_size_str[32];
    snprintf(chunk_size_str, sizeof(chunk_size_str), "%u rows", reader->header.chunk_size);
    const char *row5[] = {"Chunk Size", chunk_size_str};
    print_table_row(row5, 2, column_widths);

    // Schema version
    char version_str[32];
    snprintf(version_str, sizeof(version_str), "%u", reader->header.version);
    const char *row6[] = {"Schema Version", version_str};
    print_table_row(row6, 2, column_widths);

    // Field count
    char fields_str[32];
    snprintf(fields_str, sizeof(fields_str), "%u", reader->schema->field_count);
    const char *row7[] = {"Fields", fields_str};
    print_table_row(row7, 2, column_widths);

    // Row size
    char row_size_str[32];
    snprintf(row_size_str, sizeof(row_size_str), "%u bytes", reader->schema->row_size);
    const char *row8[] = {"Row Size", row_size_str};
    print_table_row(row8, 2, column_widths);

    // Status
    const char *row9[] = {"Status", "Ready"};
    print_table_row(row9, 2, column_widths);

    print_table_footer(2, column_widths);

    reader_close(reader);
    free(full_path);
    return 0;
}

/**
 * Schema command implementation
 */
static int cmd_shell_schema(shell_session_t *session, const parsed_command_t *cmd)
{
    (void)cmd; // Unused parameter

    if (strlen(session->current_db) == 0)
    {
        printf("❌ No database selected. Use 'use <database>' first.\n");
        return -1;
    }

    char *full_path = get_database_path(session->working_dir, session->current_db);
    if (!full_path)
    {
        printf("❌ Failed to get database path\n");
        return -1;
    }

    reader_t *reader = reader_open(full_path);
    if (!reader)
    {
        printf("❌ Failed to open database: %s\n", session->current_db);
        free(full_path);
        return -1;
    }

    printf("📋 Database Schema: %s\n", session->current_db);
    printf("═══════════════════════════════\n\n");

    print_schema(reader->schema);

    reader_close(reader);
    free(full_path);
    return 0;
}

/**
 * Clear command implementation
 */
static int cmd_shell_clear(shell_session_t *session, const parsed_command_t *cmd)
{
    (void)session; // Unused parameter
    (void)cmd;     // Unused parameter

    // Clear screen using ANSI escape codes
    printf("\033[2J\033[H");
    return 0;
}

/**
 * Create command implementation
 */
static int cmd_shell_create(shell_session_t *session, const parsed_command_t *cmd)
{
    if (cmd->arg_count < 2)
    {
        printf(COLOR_ERROR "❌ Usage:" COLOR_RESET " create <database> --schema \"field1 type1, field2 type2, ...\" [-d directory]\n");
        printf(COLOR_ERROR "❌    or:" COLOR_RESET " create <database> schema=\"field1 type1, field2 type2, ...\" [-d directory]\n");
        printf(COLOR_INFO "💡 Example:" COLOR_RESET " create products.fxdb --schema \"id int32, name string, price float\"\n");
        printf(COLOR_INFO "💡 Example:" COLOR_RESET " create products.fxdb schema=\"id int32, name string, price float\" -d ~/databases\n");
        return -1;
    }

    const char *db_name = cmd->args[1];
    const char *schema_str = NULL;
    const char *directory = NULL;
    
    // Parse arguments looking for schema and directory options
    for (int i = 2; i < cmd->arg_count; i++)
    {
        const char *arg = cmd->args[i];
        
        // Handle --schema format (CLI style)
        if (strcmp(arg, "--schema") == 0 && i + 1 < cmd->arg_count)
        {
            schema_str = cmd->args[i + 1];
            i++; // Skip next argument as it's the schema value
        }
        // Handle -d or -p directory options
        else if ((strcmp(arg, "-d") == 0 || strcmp(arg, "-p") == 0) && i + 1 < cmd->arg_count)
        {
            directory = cmd->args[i + 1];
            i++; // Skip next argument as it's the directory value
        }
        // Handle schema= format (current shell style)
        else if (strncmp(arg, "schema=", 7) == 0)
        {
            schema_str = arg + 7; // Skip "schema="
        }
        // Handle --directory= format
        else if (strncmp(arg, "--directory=", 12) == 0)
        {
            directory = arg + 12; // Skip "--directory="
        }
    }

    if (!schema_str)
    {
        printf(COLOR_ERROR "❌ Schema not specified. Use one of these formats:" COLOR_RESET "\n");
        printf("   create <database> --schema \"field1 type1, field2 type2, ...\"\n");
        printf("   create <database> schema=\"field1 type1, field2 type2, ...\"\n");
        printf(COLOR_INFO "💡 Example:" COLOR_RESET " create products.fxdb --schema \"id int32, name string, price float\"\n");
        return -1;
    }

    // Remove quotes from schema if present (for schema= format)
    char schema_buffer[1024];
    if (schema_str[0] == '"' || schema_str[0] == '\'')
    {
        strncpy(schema_buffer, schema_str + 1, sizeof(schema_buffer) - 1);
        schema_buffer[sizeof(schema_buffer) - 1] = '\0';

        // Remove trailing quote
        size_t len = strlen(schema_buffer);
        if (len > 0 && (schema_buffer[len - 1] == '"' || schema_buffer[len - 1] == '\''))
        {
            schema_buffer[len - 1] = '\0';
        }
        schema_str = schema_buffer;
    }
    
    // Use directory if specified, otherwise use session working directory
    const char *working_dir = directory ? directory : session->working_dir;

    // Check if database already exists
    if (database_exists(working_dir, db_name))
    {
        printf("❌ Database already exists: %s\n", db_name);
        printf("💡 Use 'drop %s' first to remove it (when implemented).\n", db_name);
        return -1;
    }

    // Get full path
    char *full_path = get_database_path(working_dir, db_name);
    if (!full_path)
    {
        printf("❌ Failed to build database path\n");
        return -1;
    }

    printf(COLOR_INFO "🛠️  Creating database: " COLOR_EMPHASIS "%s" COLOR_RESET "\n", db_name);
    if (directory)
    {
        printf(COLOR_INFO "📁 Directory: " COLOR_RESET "%s\n", directory);
    }
    printf(COLOR_INFO "📋 Schema: " COLOR_RESET "%s\n\n", schema_str);

    schema_t *schema = parse_schema(schema_str);
    if (!schema)
    {
        printf(COLOR_ERROR "❌ Failed to parse schema" COLOR_RESET "\n");
        printf(COLOR_INFO "💡 Check your schema format: \"field1 type1, field2 type2, ...\"" COLOR_RESET "\n");
        printf(COLOR_INFO "💡 Valid types: int32, float, string, bool" COLOR_RESET "\n");
        free(full_path);
        return -1;
    }

    printf(COLOR_SUCCESS "✅ Parsed schema:" COLOR_RESET "\n");
    print_schema(schema);
    printf("\n");

    writer_t *writer = writer_create_default(full_path, schema);
    if (!writer)
    {
        printf(COLOR_ERROR "❌ Failed to create database file" COLOR_RESET "\n");
        free_schema(schema);
        free(full_path);
        return -1;
    }

    if (writer_close(writer) != 0)
    {
        printf(COLOR_ERROR "❌ Failed to finalize database file" COLOR_RESET "\n");
        writer_free(writer);
        free_schema(schema);
        free(full_path);
        return -1;
    }

    writer_free(writer);
    free_schema(schema);

    printf(COLOR_SUCCESS "🎉 Database created successfully: " COLOR_EMPHASIS "%s" COLOR_RESET "\n", db_name);

    // Show file info
    struct stat st;
    if (stat(full_path, &st) == 0)
    {
        char size_str[32];
        format_file_size(st.st_size, size_str, sizeof(size_str));
        printf(COLOR_INFO "📊 File size: " COLOR_RESET "%s\n", size_str);
    }

    free(full_path);
    return 0;
}

/**
 * Count command implementation
 */
static int cmd_shell_count(shell_session_t *session, const parsed_command_t *cmd)
{
    (void)cmd; // Unused parameter

    if (strlen(session->current_db) == 0)
    {
        printf("❌ No database selected. Use 'use <database>' first.\n");
        return -1;
    }

    char *full_path = get_database_path(session->working_dir, session->current_db);
    if (!full_path)
    {
        printf("❌ Failed to get database path\n");
        return -1;
    }

    reader_t *reader = reader_open(full_path);
    if (!reader)
    {
        printf("❌ Failed to open database: %s\n", session->current_db);
        free(full_path);
        return -1;
    }

    uint32_t total_rows = reader_get_row_count(reader);

    printf("📊 Row Count: %s\n", session->current_db);
    printf("═════════════════════════════\n\n");

    const char *headers[] = {"Property", "Value"};
    int column_widths[] = {15, 20};

    print_table_header(headers, 2, column_widths);

    // Total rows
    char rows_str[32];
    snprintf(rows_str, sizeof(rows_str), "%u", total_rows);
    const char *row1[] = {"Total Rows", rows_str};
    print_table_row(row1, 2, column_widths);

    // Database name
    const char *row2[] = {"Database", session->current_db};
    print_table_row(row2, 2, column_widths);

    print_table_footer(2, column_widths);

    if (total_rows == 0)
    {
        printf("\n💡 Database is empty. Use 'insert' command to add data (when implemented).\n");
    }

    reader_close(reader);
    free(full_path);
    return 0;
}

/**
 * Initialize and run the interactive shell
 */
int run_interactive_shell(const char *directory)
{
    shell_session_t *session = init_session(directory);
    if (!session)
    {
        printf(COLOR_ERROR "❌ Failed to initialize shell session" COLOR_RESET "\n");
        return 1;
    }

    // Setup signal handlers for better Ctrl+C handling
    setup_signal_handlers();

    // Initialize readline
    rl_readline_name = "flexondb";
    using_history();
    
    // Load history from file
    char *home = getenv("HOME");
    char history_file[512];
    if (home)
    {
        snprintf(history_file, sizeof(history_file), "%s/.flexondb_history", home);
        read_history(history_file);
    }

    print_welcome_screen(session);

    char *line;
    while (1)
    {
        // Reset interrupt flag
        interrupt_received = 0;
        
        // Get line from readline with custom prompt
        char *prompt = generate_prompt(session);
        line = readline(prompt);

        // Check for EOF (Ctrl+D)
        if (!line)
        {
            printf("\n");
            break;
        }

        // Handle Ctrl+C during command input
        if (interrupt_received)
        {
            interrupt_received = 0;
            free(line);
            continue;
        }

        // Skip empty lines
        if (strlen(line) == 0)
        {
            free(line);
            continue;
        }

        // Add non-empty line to history
        add_history(line);

        // Parse and execute command
        parsed_command_t *cmd = parse_command(line);
        if (cmd)
        {
            int result = execute_shell_command(session, cmd);
            free_parsed_command(cmd);

            if (result == 1)
            {
                // Exit signal
                free(line);
                break;
            }
        }
        
        free(line);
    }

    // Save history
    if (home)
    {
        write_history(history_file);
        // Keep only last 1000 commands
        history_truncate_file(history_file, 1000);
    }

    print_goodbye(session);
    free_session(session);

    return 0;
}