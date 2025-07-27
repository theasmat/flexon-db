#include "platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if FLEXON_PLATFORM_WINDOWS
    #include <windows.h>
    #include <direct.h>
    #include <io.h>
    #define access _access
    #define mkdir _mkdir
    #define rmdir _rmdir
    #define getcwd _getcwd
    #define chdir _chdir
    #define F_OK 0
    #define W_OK 2
    #define R_OK 4
#else
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <dirent.h>
    #include <libgen.h>
#endif

int flexon_file_exists(const char* path) {
    if (!path) {
        return -1;
    }
    return access(path, F_OK) == 0 ? 1 : 0;
}

int flexon_is_directory(const char* path) {
    if (!path) {
        return -1;
    }

#if FLEXON_PLATFORM_WINDOWS
    DWORD attrs = GetFileAttributesA(path);
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        return -1;
    }
    return (attrs & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;
#else
    struct stat st;
    if (stat(path, &st) != 0) {
        return -1;
    }
    return S_ISDIR(st.st_mode) ? 1 : 0;
#endif
}

int flexon_is_regular_file(const char* path) {
    if (!path) {
        return -1;
    }

#if FLEXON_PLATFORM_WINDOWS
    DWORD attrs = GetFileAttributesA(path);
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        return -1;
    }
    return (attrs & FILE_ATTRIBUTE_DIRECTORY) ? 0 : 1;
#else
    struct stat st;
    if (stat(path, &st) != 0) {
        return -1;
    }
    return S_ISREG(st.st_mode) ? 1 : 0;
#endif
}

int flexon_get_file_info(const char* path, flexon_file_info_t* info) {
    if (!path || !info) {
        return -1;
    }

    memset(info, 0, sizeof(*info));

#if FLEXON_PLATFORM_WINDOWS
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &fad)) {
        return -1;
    }

    info->is_directory = (fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;
    info->is_regular_file = !info->is_directory;
    info->is_readable = access(path, R_OK) == 0 ? 1 : 0;
    info->is_writable = access(path, W_OK) == 0 ? 1 : 0;
    
    /* Convert FILETIME to Unix timestamp */
    ULARGE_INTEGER ull;
    ull.LowPart = fad.ftLastWriteTime.dwLowDateTime;
    ull.HighPart = fad.ftLastWriteTime.dwHighDateTime;
    info->modified_time = (long long)(ull.QuadPart / 10000000ULL - 11644473600ULL);
    
    /* File size */
    ull.LowPart = fad.nFileSizeLow;
    ull.HighPart = fad.nFileSizeHigh;
    info->size = (long long)ull.QuadPart;
#else
    struct stat st;
    if (stat(path, &st) != 0) {
        return -1;
    }

    info->is_directory = S_ISDIR(st.st_mode) ? 1 : 0;
    info->is_regular_file = S_ISREG(st.st_mode) ? 1 : 0;
    info->is_readable = access(path, R_OK) == 0 ? 1 : 0;
    info->is_writable = access(path, W_OK) == 0 ? 1 : 0;
    info->size = (long long)st.st_size;
    info->modified_time = (long long)st.st_mtime;
#endif

    return 0;
}

int flexon_create_directory(const char* path) {
    if (!path) {
        return -1;
    }

#if FLEXON_PLATFORM_WINDOWS
    return CreateDirectoryA(path, NULL) ? 0 : -1;
#else
    return mkdir(path, 0755) == 0 ? 0 : -1;
#endif
}

int flexon_create_directories(const char* path) {
    if (!path) {
        return -1;
    }

    /* Check if it already exists */
    if (flexon_file_exists(path)) {
        return flexon_is_directory(path) ? 0 : -1;
    }

    /* Copy path to work with */
    char* path_copy = strdup(path);
    if (!path_copy) {
        return -1;
    }

    /* Find last separator */
    char* last_sep = strrchr(path_copy, FLEXON_PATH_SEPARATOR);
    if (last_sep) {
        *last_sep = '\0';
        
        /* Recursively create parent directories */
        if (flexon_create_directories(path_copy) != 0) {
            free(path_copy);
            return -1;
        }
    }

    free(path_copy);
    return flexon_create_directory(path);
}

int flexon_remove_file(const char* path) {
    if (!path) {
        return -1;
    }

#if FLEXON_PLATFORM_WINDOWS
    return DeleteFileA(path) ? 0 : -1;
#else
    return unlink(path) == 0 ? 0 : -1;
#endif
}

int flexon_remove_directory(const char* path) {
    if (!path) {
        return -1;
    }

#if FLEXON_PLATFORM_WINDOWS
    return RemoveDirectoryA(path) ? 0 : -1;
#else
    return rmdir(path) == 0 ? 0 : -1;
#endif
}

int flexon_get_current_directory(char* buffer, size_t size) {
    if (!buffer || size == 0) {
        return -1;
    }

#if FLEXON_PLATFORM_WINDOWS
    DWORD len = GetCurrentDirectoryA((DWORD)size, buffer);
    return (len > 0 && len < size) ? 0 : -1;
#else
    return getcwd(buffer, size) ? 0 : -1;
#endif
}

int flexon_set_current_directory(const char* path) {
    if (!path) {
        return -1;
    }

#if FLEXON_PLATFORM_WINDOWS
    return SetCurrentDirectoryA(path) ? 0 : -1;
#else
    return chdir(path) == 0 ? 0 : -1;
#endif
}

int flexon_path_join(char* result, size_t result_size, const char* path1, const char* path2) {
    if (!result || !path1 || !path2 || result_size == 0) {
        return -1;
    }

    size_t len1 = strlen(path1);
    size_t len2 = strlen(path2);
    int need_separator = 0;

    /* Check if we need to add a separator */
    if (len1 > 0 && path1[len1 - 1] != FLEXON_PATH_SEPARATOR) {
        need_separator = 1;
    }

    /* Skip leading separator in path2 */
    if (len2 > 0 && path2[0] == FLEXON_PATH_SEPARATOR) {
        path2++;
        len2--;
    }

    /* Check if result will fit */
    size_t total_len = len1 + len2 + (need_separator ? 1 : 0);
    if (total_len >= result_size) {
        return -1;
    }

    /* Build the path */
    strcpy(result, path1);
    if (need_separator) {
        strcat(result, FLEXON_PATH_SEPARATOR_STR);
    }
    strcat(result, path2);

    return 0;
}

int flexon_path_dirname(char* result, size_t result_size, const char* path) {
    if (!result || !path || result_size == 0) {
        return -1;
    }

    char* path_copy = strdup(path);
    if (!path_copy) {
        return -1;
    }

#if FLEXON_PLATFORM_WINDOWS
    char* last_sep = strrchr(path_copy, FLEXON_PATH_SEPARATOR);
    if (last_sep) {
        *last_sep = '\0';
    } else {
        strcpy(path_copy, ".");
    }
#else
    char* dir = dirname(path_copy);
    if (strlen(dir) >= result_size) {
        free(path_copy);
        return -1;
    }
    strcpy(result, dir);
    free(path_copy);
    return 0;
#endif

    if (strlen(path_copy) >= result_size) {
        free(path_copy);
        return -1;
    }

    strcpy(result, path_copy);
    free(path_copy);
    return 0;
}

int flexon_path_basename(char* result, size_t result_size, const char* path) {
    if (!result || !path || result_size == 0) {
        return -1;
    }

    const char* last_sep = strrchr(path, FLEXON_PATH_SEPARATOR);
    const char* base = last_sep ? last_sep + 1 : path;

    if (strlen(base) >= result_size) {
        return -1;
    }

    strcpy(result, base);
    return 0;
}

const char* flexon_path_extension(const char* path) {
    if (!path) {
        return "";
    }

    const char* last_dot = strrchr(path, '.');
    const char* last_sep = strrchr(path, FLEXON_PATH_SEPARATOR);

    /* Make sure the dot is after the last separator */
    if (last_dot && (!last_sep || last_dot > last_sep)) {
        return last_dot;
    }

    return "";
}

int flexon_path_normalize(char* result, size_t result_size, const char* path) {
    /* Simple implementation - just copy for now */
    /* TODO: Implement proper . and .. resolution */
    if (!result || !path || result_size == 0) {
        return -1;
    }

    if (strlen(path) >= result_size) {
        return -1;
    }

    strcpy(result, path);
    return 0;
}

int flexon_path_absolute(char* result, size_t result_size, const char* path) {
    if (!result || !path || result_size == 0) {
        return -1;
    }

    /* If already absolute, just copy */
#if FLEXON_PLATFORM_WINDOWS
    if (strlen(path) >= 2 && path[1] == ':') {
        return flexon_path_normalize(result, result_size, path);
    }
#else
    if (path[0] == FLEXON_PATH_SEPARATOR) {
        return flexon_path_normalize(result, result_size, path);
    }
#endif

    /* Get current directory and join */
    char cwd[FLEXON_MAX_PATH];
    if (flexon_get_current_directory(cwd, sizeof(cwd)) != 0) {
        return -1;
    }

    return flexon_path_join(result, result_size, cwd, path);
}

flexon_dir_entry_t* flexon_list_directory(const char* path) {
    if (!path) {
        return NULL;
    }

    flexon_dir_entry_t* head = NULL;
    flexon_dir_entry_t* tail = NULL;

#if FLEXON_PLATFORM_WINDOWS
    char search_pattern[FLEXON_MAX_PATH];
    if (flexon_path_join(search_pattern, sizeof(search_pattern), path, "*") != 0) {
        return NULL;
    }

    WIN32_FIND_DATAA find_data;
    HANDLE hFind = FindFirstFileA(search_pattern, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    do {
        /* Skip . and .. */
        if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0) {
            continue;
        }

        flexon_dir_entry_t* entry = malloc(sizeof(flexon_dir_entry_t));
        if (!entry) {
            break;
        }

        strncpy(entry->name, find_data.cFileName, sizeof(entry->name) - 1);
        entry->name[sizeof(entry->name) - 1] = '\0';
        entry->is_directory = (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;
        entry->next = NULL;

        if (!head) {
            head = tail = entry;
        } else {
            tail->next = entry;
            tail = entry;
        }
    } while (FindNextFileA(hFind, &find_data));

    FindClose(hFind);
#else
    DIR* dir = opendir(path);
    if (!dir) {
        return NULL;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        /* Skip . and .. */
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        flexon_dir_entry_t* dir_entry = malloc(sizeof(flexon_dir_entry_t));
        if (!dir_entry) {
            break;
        }

        strncpy(dir_entry->name, entry->d_name, sizeof(dir_entry->name) - 1);
        dir_entry->name[sizeof(dir_entry->name) - 1] = '\0';
        
        /* Build full path to check if it's a directory */
        char full_path[FLEXON_MAX_PATH];
        if (flexon_path_join(full_path, sizeof(full_path), path, entry->d_name) == 0) {
            dir_entry->is_directory = flexon_is_directory(full_path) == 1 ? 1 : 0;
        } else {
            dir_entry->is_directory = 0;
        }
        
        dir_entry->next = NULL;

        if (!head) {
            head = tail = dir_entry;
        } else {
            tail->next = dir_entry;
            tail = dir_entry;
        }
    }

    closedir(dir);
#endif

    return head;
}

void flexon_free_dir_list(flexon_dir_entry_t* list) {
    while (list) {
        flexon_dir_entry_t* next = list->next;
        free(list);
        list = next;
    }
}