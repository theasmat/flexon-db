#ifndef FLEXON_DEFINITION_H
#define FLEXON_DEFINITION_H

#define FXDB_EXT ".fxdb"
#define FXDB_MAGIC "FXDB01"
#define FXDB_VERSION 1

#define MAX_COLUMNS 64
#define MAX_FIELD_NAME 32

typedef enum
{
    FIELD_INT,
    FIELD_FLOAT,
    FIELD_STRING,
    FIELD_BOOL
} field_type_t;

#endif // FLEXON_DEFINITION_H