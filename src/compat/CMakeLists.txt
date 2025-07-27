# Compatibility Layer
set(COMPAT_SOURCES
    compat.c
)

add_library(flexondb_compat STATIC ${COMPAT_SOURCES})

# Include directories
target_include_directories(flexondb_compat 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/platform
)

# Link with platform layer
target_link_libraries(flexondb_compat flexondb_platform)

# Set target properties
set_target_properties(flexondb_compat PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)

# Export compat library for other modules
set(FLEXONDB_COMPAT_LIBRARY flexondb_compat PARENT_SCOPE)