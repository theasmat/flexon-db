# Common Utilities
set(COMMON_SOURCES
    error.c
    utils.c
    io_utils.c
    logo.c
    welcome.c
)

add_library(flexondb_common STATIC ${COMMON_SOURCES})

# Include directories
target_include_directories(flexondb_common 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/common
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/platform
)

# Link with dependencies - platform should be linked first
target_link_libraries(flexondb_common flexondb_platform)

# Set target properties
set_target_properties(flexondb_common PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)

# Export common library for other modules
set(FLEXONDB_COMMON_LIBRARY flexondb_common PARENT_SCOPE)