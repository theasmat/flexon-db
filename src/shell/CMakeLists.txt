# Interactive Shell
set(SHELL_SOURCES
    session.c
    formatter.c
    parser.c
    shell.c
)

add_library(flexondb_shell STATIC ${SHELL_SOURCES})

# Include directories
target_include_directories(flexondb_shell 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/platform
        ${CMAKE_SOURCE_DIR}/include/common
)

# Link with dependencies
target_link_libraries(flexondb_shell 
    flexondb_core
    flexondb_common
    flexondb_platform
)

# Set target properties
set_target_properties(flexondb_shell PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)

# Export shell library for other modules
set(FLEXONDB_SHELL_LIBRARY flexondb_shell PARENT_SCOPE)