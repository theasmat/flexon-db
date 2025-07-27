cmake_minimum_required(VERSION 3.10)
project(FlexonDB VERSION 1.0.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Platform detection and configuration
if(WIN32)
    set(FLEXON_PLATFORM_WINDOWS ON)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(FLEXON_PLATFORM_WIN64 ON)
    else()
        set(FLEXON_PLATFORM_WIN32 ON)
    endif()
elseif(APPLE)
    set(FLEXON_PLATFORM_APPLE ON)
    if(IOS)
        set(FLEXON_PLATFORM_IOS ON)
    else()
        set(FLEXON_PLATFORM_MACOS ON)
    endif()
elseif(ANDROID)
    set(FLEXON_PLATFORM_ANDROID ON)
elseif(UNIX)
    set(FLEXON_PLATFORM_LINUX ON)
endif()

# Compiler flags
if(MSVC)
    add_compile_options(/W4)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -g)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O2)
    endif()
endif()

# Include directories
include_directories(include)

# Source files
set(COMMON_SOURCES
    src/common/error.c
    src/common/utils.c
    src/common/io_utils.c
    src/common/logo.c
    src/common/welcome.c
)

set(CORE_SOURCES
    src/core/schema.c
    src/core/writer.c
    src/core/reader.c
)

set(SHELL_SOURCES
    src/shell/session.c
    src/shell/formatter.c
    src/shell/parser.c
    src/shell/shell.c
)

set(COMPAT_SOURCES
    src/compat/compat.c
)

set(UNIFIED_API_SOURCES
    core/flexondb.c
)

# Platform-specific source detection
set(ALL_SOURCES 
    ${COMMON_SOURCES} 
    ${CORE_SOURCES} 
    ${COMPAT_SOURCES}
    ${UNIFIED_API_SOURCES}
)

# Find optional dependencies
find_package(PkgConfig QUIET)

# Readline detection
set(READLINE_FOUND FALSE)
set(READLINE_LIBRARIES "")

if(NOT FLEXON_PLATFORM_ANDROID AND NOT FLEXON_PLATFORM_IOS)
    # Try to find readline on desktop platforms
    if(FLEXON_PLATFORM_MACOS)
        # macOS uses libedit by default
        find_library(EDIT_LIBRARY NAMES edit)
        if(EDIT_LIBRARY)
            set(READLINE_FOUND TRUE)
            set(READLINE_LIBRARIES ${EDIT_LIBRARY})
            add_definitions(-DFLEXON_HAVE_LIBEDIT_READLINE)
        endif()
    else()
        # Linux/Unix - try GNU readline first
        find_library(READLINE_LIBRARY NAMES readline)
        find_library(HISTORY_LIBRARY NAMES history)
        if(READLINE_LIBRARY)
            set(READLINE_FOUND TRUE)
            set(READLINE_LIBRARIES ${READLINE_LIBRARY})
            if(HISTORY_LIBRARY)
                list(APPEND READLINE_LIBRARIES ${HISTORY_LIBRARY})
            endif()
            add_definitions(-DFLEXON_HAVE_GNU_READLINE)
        endif()
    endif()
endif()

if(NOT READLINE_FOUND)
    message(STATUS "Readline not found - using stub implementation")
    add_definitions(-DFLEXON_NO_READLINE)
endif()

# Platform-specific libraries
set(PLATFORM_LIBRARIES "")

if(FLEXON_PLATFORM_WINDOWS)
    list(APPEND PLATFORM_LIBRARIES kernel32 user32)
elseif(FLEXON_PLATFORM_ANDROID)
    list(APPEND PLATFORM_LIBRARIES log)
endif()

# Create static library for core functionality
add_library(flexondb_core STATIC ${ALL_SOURCES})
target_link_libraries(flexondb_core ${PLATFORM_LIBRARIES})

# CLI executable (desktop only)
if(NOT FLEXON_PLATFORM_MOBILE)
    add_executable(flexon src/cli/main.c ${SHELL_SOURCES})
    target_link_libraries(flexon flexondb_core ${READLINE_LIBRARIES} ${PLATFORM_LIBRARIES})
    
    # Alternative CLI executable using new CLI implementation
    if(EXISTS "${CMAKE_SOURCE_DIR}/cli/main.c")
        add_executable(flexon-cli cli/main.c)
        target_link_libraries(flexon-cli flexondb_core ${PLATFORM_LIBRARIES})
    endif()
    
    # Shell executable
    if(EXISTS "${CMAKE_SOURCE_DIR}/cli/shell.c")
        add_executable(flexon-shell cli/shell.c)
        target_link_libraries(flexon-shell flexondb_core ${PLATFORM_LIBRARIES})
    endif()
endif()

# Test executables
if(EXISTS "${CMAKE_SOURCE_DIR}/examples")
    add_executable(test_schema examples/test_schema.c)
    target_link_libraries(test_schema flexondb_core ${PLATFORM_LIBRARIES})
    
    add_executable(test_writer examples/test_writer.c)
    target_link_libraries(test_writer flexondb_core ${PLATFORM_LIBRARIES})
    
    add_executable(test_reader examples/test_reader.c)
    target_link_libraries(test_reader flexondb_core ${PLATFORM_LIBRARIES})
endif()

# Configuration test
if(EXISTS "${CMAKE_SOURCE_DIR}/test_config.c")
    add_executable(test_config test_config.c)
    target_link_libraries(test_config flexondb_core ${PLATFORM_LIBRARIES})
endif()

# Install rules (desktop only)
if(NOT FLEXON_PLATFORM_MOBILE)
    install(TARGETS flexon DESTINATION bin)
    install(FILES include/flexondb.h DESTINATION include)
    install(TARGETS flexondb_core DESTINATION lib)
endif()

# Print configuration summary
message(STATUS "FlexonDB Configuration Summary:")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Compiler: ${CMAKE_C_COMPILER_ID}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Readline support: ${READLINE_FOUND}")
if(READLINE_FOUND)
    message(STATUS "  Readline libraries: ${READLINE_LIBRARIES}")
endif()
message(STATUS "  Platform libraries: ${PLATFORM_LIBRARIES}")

# Option to disable shell when readline is not available
option(FLEXON_BUILD_SHELL "Build interactive shell (requires readline)" ${READLINE_FOUND})

if(NOT FLEXON_BUILD_SHELL)
    message(STATUS "  Interactive shell: DISABLED (no readline)")
else()
    message(STATUS "  Interactive shell: ENABLED")
endif()

# Additional compiler definitions based on detected features
target_compile_definitions(flexondb_core PRIVATE
    FLEXON_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    FLEXON_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    FLEXON_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

# Mobile platform specific configurations
if(FLEXON_PLATFORM_ANDROID)
    target_compile_definitions(flexondb_core PRIVATE FLEXON_PLATFORM_ANDROID)
elseif(FLEXON_PLATFORM_IOS)
    target_compile_definitions(flexondb_core PRIVATE FLEXON_PLATFORM_IOS)
endif()

# Add custom target for cleaning database files
add_custom_target(clean-db
    COMMAND ${CMAKE_COMMAND} -E remove -f *.fxdb
    COMMAND ${CMAKE_COMMAND} -E remove_directory demo_databases
    COMMENT "Cleaning database files"
)

# Add custom target for running tests
if(NOT FLEXON_PLATFORM_MOBILE)
    add_custom_target(run-tests
        DEPENDS test_schema test_writer test_reader
        COMMAND ./test_schema
        COMMAND ./test_writer  
        COMMAND ./test_reader
        COMMENT "Running FlexonDB tests"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()