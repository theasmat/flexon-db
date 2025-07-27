# CLI Interface
set(CLI_SOURCES
    main.c
)

# CLI executable (desktop only)
if(NOT FLEXON_PLATFORM_MOBILE)
    add_executable(flexon ${CLI_SOURCES})
    
    # Include directories
    target_include_directories(flexon 
        PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/include/platform
            ${CMAKE_SOURCE_DIR}/include/common
    )
    
    # Link with all dependencies
    target_link_libraries(flexon 
        flexondb_shell
        flexondb_core
        flexondb_common
        flexondb_platform
    )
    
    # Set target properties
    set_target_properties(flexon PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED ON
    )
    
    # Export CLI executable
    set(FLEXONDB_CLI_EXECUTABLE flexon PARENT_SCOPE)
endif()

# Create library version of CLI for linking
add_library(flexondb_cli STATIC ${CLI_SOURCES})

target_include_directories(flexondb_cli 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/platform
        ${CMAKE_SOURCE_DIR}/include/common
)

target_link_libraries(flexondb_cli 
    flexondb_shell
    flexondb_core
    flexondb_common
    flexondb_platform
)

set_target_properties(flexondb_cli PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)