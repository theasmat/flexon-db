#ifndef FLEXONDB_H
#define FLEXONDB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Initialize the database using a file path and schema.
int createDatabase(const char* path, const char* schema);

// Insert JSON data into the database.
int insertData(const char* path, const char* json);

// Read data from the database.
char* readData(const char* path);

// Delete the database.
int deleteDatabase(const char* path);

// Update the database record with new JSON, return result status (0 for success).
int updateDatabase(const char* path, const char* json);

// Convert CSV file to FlexonDB format, returning status code.
int csvToFlexonDB(const char* csvPath, const char* dbPath);

#ifdef __cplusplus
}
#endif

#endif /* FLEXONDB_H */