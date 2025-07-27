# Platform Abstraction Layer
set(PLATFORM_SOURCES
    terminal.c
    filesystem.c
    colors.c
)

add_library(flexondb_platform STATIC ${PLATFORM_SOURCES})

# Include directories
target_include_directories(flexondb_platform 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/platform
)

# Platform-specific libraries and definitions
if(FLEXON_PLATFORM_WINDOWS)
    target_link_libraries(flexondb_platform kernel32 user32)
    target_compile_definitions(flexondb_platform PRIVATE FLEXON_PLATFORM_WINDOWS=1)
elseif(FLEXON_PLATFORM_APPLE)
    if(FLEXON_PLATFORM_MACOS)
        target_compile_definitions(flexondb_platform PRIVATE FLEXON_PLATFORM_MACOS=1)
        # Try to find and link libedit on macOS
        find_library(EDIT_LIBRARY NAMES edit)
        if(EDIT_LIBRARY)
            target_link_libraries(flexondb_platform ${EDIT_LIBRARY})
            target_compile_definitions(flexondb_platform PRIVATE FLEXON_HAVE_LIBEDIT_READLINE=1)
        endif()
    elseif(FLEXON_PLATFORM_IOS)
        target_compile_definitions(flexondb_platform PRIVATE FLEXON_PLATFORM_IOS=1 FLEXON_PLATFORM_MOBILE=1)
    endif()
elseif(FLEXON_PLATFORM_ANDROID)
    target_link_libraries(flexondb_platform log)
    target_compile_definitions(flexondb_platform PRIVATE FLEXON_PLATFORM_ANDROID=1 FLEXON_PLATFORM_MOBILE=1)
elseif(FLEXON_PLATFORM_LINUX)
    target_compile_definitions(flexondb_platform PRIVATE FLEXON_PLATFORM_LINUX=1)
    # Try to find and link GNU readline on Linux
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(READLINE readline)
        if(READLINE_FOUND)
            target_link_libraries(flexondb_platform ${READLINE_LIBRARIES})
            target_compile_definitions(flexondb_platform PRIVATE FLEXON_HAVE_GNU_READLINE=1)
        endif()
    endif()
endif()

# Set target properties
set_target_properties(flexondb_platform PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)

# Export platform definitions for other modules
set(FLEXON_PLATFORM_DEFINITIONS
    FLEXON_PLATFORM_WINDOWS=${FLEXON_PLATFORM_WINDOWS}
    FLEXON_PLATFORM_APPLE=${FLEXON_PLATFORM_APPLE}
    FLEXON_PLATFORM_MACOS=${FLEXON_PLATFORM_MACOS}
    FLEXON_PLATFORM_IOS=${FLEXON_PLATFORM_IOS}
    FLEXON_PLATFORM_ANDROID=${FLEXON_PLATFORM_ANDROID}
    FLEXON_PLATFORM_LINUX=${FLEXON_PLATFORM_LINUX}
    FLEXON_PLATFORM_MOBILE=${FLEXON_PLATFORM_MOBILE}
    PARENT_SCOPE
)