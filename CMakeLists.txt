cmake_minimum_required(VERSION 3.10)
project(FlexonDB VERSION 1.0.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Auto-detect platform if not specified
if(NOT DEFINED BUILD_PLATFORM)
    if(APPLE)
        if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
            set(BUILD_PLATFORM "ios")
        else()
            set(BUILD_PLATFORM "macos")
        endif()
    elseif(WIN32)
        set(BUILD_PLATFORM "windows")
    elseif(ANDROID)
        set(BUILD_PLATFORM "android")
    elseif(UNIX)
        set(BUILD_PLATFORM "linux")
    else()
        set(BUILD_PLATFORM "unknown")
    endif()
endif()

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
        set(FLEXON_PLATFORM_MOBILE ON)
    else()
        set(FLEXON_PLATFORM_MACOS ON)
    endif()
elseif(ANDROID)
    set(FLEXON_PLATFORM_ANDROID ON)
    set(FLEXON_PLATFORM_MOBILE ON)
elseif(UNIX)
    set(FLEXON_PLATFORM_LINUX ON)
endif()

# Set default values for platform flags
if(NOT DEFINED FLEXON_PLATFORM_WINDOWS)
    set(FLEXON_PLATFORM_WINDOWS OFF)
endif()
if(NOT DEFINED FLEXON_PLATFORM_APPLE)
    set(FLEXON_PLATFORM_APPLE OFF)
endif()
if(NOT DEFINED FLEXON_PLATFORM_MACOS)
    set(FLEXON_PLATFORM_MACOS OFF)
endif()
if(NOT DEFINED FLEXON_PLATFORM_IOS)
    set(FLEXON_PLATFORM_IOS OFF)
endif()
if(NOT DEFINED FLEXON_PLATFORM_ANDROID)
    set(FLEXON_PLATFORM_ANDROID OFF)
endif()
if(NOT DEFINED FLEXON_PLATFORM_LINUX)
    set(FLEXON_PLATFORM_LINUX OFF)
endif()
if(NOT DEFINED FLEXON_PLATFORM_MOBILE)
    set(FLEXON_PLATFORM_MOBILE OFF)
endif()

# Set build output directories based on platform
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../dist/${BUILD_PLATFORM}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../dist/${BUILD_PLATFORM}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../dist/${BUILD_PLATFORM}/lib)

# Create output directories
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

# Installation directories
set(INSTALL_BIN_DIR ${CMAKE_BINARY_DIR}/../dist/${BUILD_PLATFORM}/bin)
set(INSTALL_LIB_DIR ${CMAKE_BINARY_DIR}/../dist/${BUILD_PLATFORM}/lib)
set(INSTALL_INCLUDE_DIR ${CMAKE_BINARY_DIR}/../dist/${BUILD_PLATFORM}/include)

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
include_directories(include/platform)
include_directories(include/common)

# Add source subdirectories in dependency order
add_subdirectory(src/platform)
add_subdirectory(src/compat)
add_subdirectory(src/common)
add_subdirectory(src/core)
add_subdirectory(src/shell)
add_subdirectory(src/cli)

# Additional targets for backward compatibility
# Create unified core library including all dependencies
add_library(flexondb_unified INTERFACE)
target_link_libraries(flexondb_unified 
    INTERFACE
    flexondb_core
    flexondb_shell
    flexondb_common
    flexondb_platform
    flexondb_compat
)

# Legacy unified API (if exists)
if(EXISTS "${CMAKE_SOURCE_DIR}/core/flexondb.c")
    add_library(flexondb_legacy STATIC core/flexondb.c)
    target_link_libraries(flexondb_legacy flexondb_unified)
    target_include_directories(flexondb_legacy PUBLIC include core)
endif()

# Alternative CLI and shell executables (if they exist)
if(NOT FLEXON_PLATFORM_MOBILE)
    if(EXISTS "${CMAKE_SOURCE_DIR}/cli/main.c")
        add_executable(flexon-cli cli/main.c)
        if(TARGET flexondb_legacy)
            target_link_libraries(flexon-cli flexondb_legacy flexondb_unified)
        else()
            target_link_libraries(flexon-cli flexondb_unified)
        endif()
        target_include_directories(flexon-cli PRIVATE include core)
    endif()
    
    if(EXISTS "${CMAKE_SOURCE_DIR}/cli/shell.c")
        add_executable(flexon-shell cli/shell.c)
        if(TARGET flexondb_legacy)
            target_link_libraries(flexon-shell flexondb_legacy flexondb_unified)
        else()
            target_link_libraries(flexon-shell flexondb_unified)
        endif()
        target_include_directories(flexon-shell PRIVATE include core)
    endif()
endif()

# Test executables
if(EXISTS "${CMAKE_SOURCE_DIR}/examples")
    add_executable(test_schema examples/test_schema.c)
    target_link_libraries(test_schema flexondb_unified)
    target_include_directories(test_schema PRIVATE include)
    
    add_executable(test_writer examples/test_writer.c)
    target_link_libraries(test_writer flexondb_unified)
    target_include_directories(test_writer PRIVATE include)
    
    add_executable(test_reader examples/test_reader.c)
    target_link_libraries(test_reader flexondb_unified)
    target_include_directories(test_reader PRIVATE include)
endif()

# Configuration test
if(EXISTS "${CMAKE_SOURCE_DIR}/test_config.c")
    add_executable(test_config test_config.c)
    target_link_libraries(test_config flexondb_unified)
    target_include_directories(test_config PRIVATE include)
endif()

# Install rules (desktop only)
if(NOT FLEXON_PLATFORM_MOBILE)
    # Install main executable
    if(TARGET flexon)
        install(TARGETS flexon DESTINATION ${INSTALL_BIN_DIR})
    endif()
    
    # Install additional executables if they exist
    if(TARGET flexon-cli)
        install(TARGETS flexon-cli DESTINATION ${INSTALL_BIN_DIR})
    endif()
    if(TARGET flexon-shell)
        install(TARGETS flexon-shell DESTINATION ${INSTALL_BIN_DIR})
    endif()
    
    # Install libraries
    install(TARGETS flexondb_unified flexondb_core flexondb_platform flexondb_common
            DESTINATION ${INSTALL_LIB_DIR})
    
    # Install headers
    install(DIRECTORY include/ DESTINATION ${INSTALL_INCLUDE_DIR}
            FILES_MATCHING PATTERN "*.h")
endif()

# Print configuration summary
message(STATUS "FlexonDB Configuration Summary:")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME} (${BUILD_PLATFORM})")
message(STATUS "  Compiler: ${CMAKE_C_COMPILER_ID}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Mobile platform: ${FLEXON_PLATFORM_MOBILE}")
message(STATUS "  Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# Platform-specific readline information
if(FLEXON_PLATFORM_MACOS)
    message(STATUS "  Terminal library: libedit (macOS default)")
elseif(FLEXON_PLATFORM_LINUX)
    message(STATUS "  Terminal library: GNU readline (if available)")
elseif(FLEXON_PLATFORM_WINDOWS)
    message(STATUS "  Terminal library: linenoise or fallback")
elseif(FLEXON_PLATFORM_MOBILE)
    message(STATUS "  Terminal library: linenoise or custom")
endif()

# Build options
option(FLEXON_BUILD_SHELL "Build interactive shell" ON)
option(FLEXON_BUILD_TESTS "Build test executables" ON)
option(FLEXON_FORCE_COLORS "Force color output on all platforms" OFF)

if(FLEXON_FORCE_COLORS)
    add_definitions(-DFLEXON_FORCE_COLORS=1)
endif()

# Version information
add_definitions(
    -DFLEXON_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    -DFLEXON_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    -DFLEXON_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

# Add custom target for cleaning database files
add_custom_target(clean-db
    COMMAND ${CMAKE_COMMAND} -E remove -f *.fxdb
    COMMAND ${CMAKE_COMMAND} -E remove_directory demo_databases
    COMMENT "Cleaning database files"
)

# Add custom target for running tests
if(NOT FLEXON_PLATFORM_MOBILE AND FLEXON_BUILD_TESTS)
    add_custom_target(run-tests
        DEPENDS test_schema test_writer test_reader
        COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_schema
        COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_writer  
        COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_reader
        COMMENT "Running FlexonDB tests"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

# Platform-specific post-build steps
if(FLEXON_PLATFORM_WINDOWS)
    # Windows-specific post-build steps
    message(STATUS "  Note: Windows ANSI color support requires Windows 10+")
elseif(FLEXON_PLATFORM_MOBILE)
    # Mobile-specific configuration
    message(STATUS "  Note: Mobile platforms use simplified terminal interface")
endif()