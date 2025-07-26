#include "../../include/error.h"
#include <stdio.h>
#include <string.h>

/* ============================================================================
 * Error Handling Implementation
 * ============================================================================ */

void error_init(error_context_t* ctx) {
    if (!ctx) return;
    CLEAR_ERROR(ctx);
}

const char* error_category_to_string(error_category_t category) {
    switch (category) {
        case ERROR_NONE:              return "No Error";
        case ERROR_FILE_IO:           return "File I/O Error";
        case ERROR_MEMORY:            return "Memory Error";
        case ERROR_INVALID_SCHEMA:    return "Invalid Schema";
        case ERROR_INVALID_DATA:      return "Invalid Data";
        case ERROR_DATABASE_CORRUPT:  return "Database Corrupt";
        case ERROR_PERMISSION:        return "Permission Denied";
        case ERROR_DISK_SPACE:        return "Disk Space";
        case ERROR_NETWORK:           return "Network Error";
        case ERROR_TIMEOUT:           return "Timeout";
        case ERROR_UNKNOWN:           return "Unknown Error";
        default:                      return "Undefined Error";
    }
}

void error_print(const error_context_t* ctx) {
    if (!ctx || !HAS_ERROR(ctx)) return;
    error_fprint(stderr, ctx);
}

void error_fprint(FILE* fp, const error_context_t* ctx) {
    if (!fp || !ctx || !HAS_ERROR(ctx)) return;
    
    fprintf(fp, "Error: %s\n", error_category_to_string(ctx->category));
    if (strlen(ctx->message) > 0) {
        fprintf(fp, "Message: %s\n", ctx->message);
    }
    if (strlen(ctx->context) > 0) {
        fprintf(fp, "Context: %s\n", ctx->context);
    }
    if (ctx->code != 0) {
        fprintf(fp, "Code: %d\n", ctx->code);
    }
    
#if ENABLE_DEBUG
    if (strlen(ctx->file) > 0) {
        fprintf(fp, "Location: %s:%d in %s()\n", ctx->file, ctx->line, ctx->function);
    }
#endif
}

void error_format(const error_context_t* ctx, char* buffer, size_t buffer_size) {
    if (!ctx || !buffer || buffer_size == 0) return;
    
    if (!HAS_ERROR(ctx)) {
        snprintf(buffer, buffer_size, "No error");
        return;
    }
    
    if (strlen(ctx->message) > 0) {
        snprintf(buffer, buffer_size, "%s: %s", 
                error_category_to_string(ctx->category), ctx->message);
    } else {
        snprintf(buffer, buffer_size, "%s", error_category_to_string(ctx->category));
    }
}

error_category_t status_to_error_category(status_code_t status) {
    switch (status) {
        case STATUS_OK:                 return ERROR_NONE;
        case STATUS_ERROR:              return ERROR_UNKNOWN;
        case STATUS_NOT_FOUND:          return ERROR_FILE_IO;
        case STATUS_INVALID_INPUT:      return ERROR_INVALID_DATA;
        case STATUS_PERMISSION_DENIED:  return ERROR_PERMISSION;
        case STATUS_DISK_FULL:          return ERROR_DISK_SPACE;
        default:                        return ERROR_UNKNOWN;
    }
}

status_code_t error_category_to_status(error_category_t category) {
    switch (category) {
        case ERROR_NONE:              return STATUS_OK;
        case ERROR_FILE_IO:           return STATUS_NOT_FOUND;
        case ERROR_MEMORY:            return STATUS_ERROR;
        case ERROR_INVALID_SCHEMA:    return STATUS_INVALID_INPUT;
        case ERROR_INVALID_DATA:      return STATUS_INVALID_INPUT;
        case ERROR_DATABASE_CORRUPT:  return STATUS_ERROR;
        case ERROR_PERMISSION:        return STATUS_PERMISSION_DENIED;
        case ERROR_DISK_SPACE:        return STATUS_DISK_FULL;
        case ERROR_NETWORK:           return STATUS_ERROR;
        case ERROR_TIMEOUT:           return STATUS_ERROR;
        case ERROR_UNKNOWN:           return STATUS_ERROR;
        default:                      return STATUS_ERROR;
    }
}

int error_is_recoverable(const error_context_t* ctx) {
    if (!ctx || !HAS_ERROR(ctx)) return 1;
    
    switch (ctx->category) {
        case ERROR_MEMORY:
        case ERROR_DATABASE_CORRUPT:
        case ERROR_DISK_SPACE:
            return 0; // Not recoverable
        case ERROR_FILE_IO:
        case ERROR_INVALID_SCHEMA:
        case ERROR_INVALID_DATA:
        case ERROR_PERMISSION:
        case ERROR_NETWORK:
        case ERROR_TIMEOUT:
            return 1; // Potentially recoverable
        default:
            return 0;
    }
}