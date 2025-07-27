# FlexonDB Core Engine
set(CORE_SOURCES
    schema.c
    writer.c
    reader.c
)

add_library(flexondb_core STATIC ${CORE_SOURCES})

# Include directories
target_include_directories(flexondb_core 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/platform
)

# Link with dependencies - ensure proper order
target_link_libraries(flexondb_core 
    flexondb_common
    flexondb_platform
)

# Compiler definitions
target_compile_definitions(flexondb_core PRIVATE
    FLEXON_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    FLEXON_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    FLEXON_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

# Set target properties
set_target_properties(flexondb_core PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)

# Export core library for other modules
set(FLEXONDB_CORE_LIBRARY flexondb_core PARENT_SCOPE)