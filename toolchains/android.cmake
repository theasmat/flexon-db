# Android NDK Toolchain for FlexonDB
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=toolchains/android.cmake -S . -B build/android

set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_SYSTEM_VERSION 21)  # Android API Level 21 (Android 5.0)
set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)  # 64-bit ARM
set(CMAKE_ANDROID_STL_TYPE c++_static)

# Set Android NDK path - can be overridden via environment variable
if(DEFINED ENV{ANDROID_NDK})
    set(CMAKE_ANDROID_NDK $ENV{ANDROID_NDK})
elseif(DEFINED ENV{ANDROID_NDK_ROOT})
    set(CMAKE_ANDROID_NDK $ENV{ANDROID_NDK_ROOT})
else()
    # Common NDK installation paths
    if(EXISTS "/opt/android-ndk")
        set(CMAKE_ANDROID_NDK "/opt/android-ndk")
    elseif(EXISTS "$ENV{HOME}/Android/Sdk/ndk-bundle")
        set(CMAKE_ANDROID_NDK "$ENV{HOME}/Android/Sdk/ndk-bundle")
    elseif(EXISTS "/usr/local/android-ndk")
        set(CMAKE_ANDROID_NDK "/usr/local/android-ndk")
    else()
        message(FATAL_ERROR "Android NDK not found. Please set ANDROID_NDK environment variable or install NDK.")
    endif()
endif()

# Platform-specific settings for FlexonDB
set(BUILD_PLATFORM "android")
set(FLEXON_MOBILE_BUILD ON)

# Android-specific definitions
add_definitions(-DFLEXON_PLATFORM_ANDROID=1)
add_definitions(-DFLEXON_PLATFORM_MOBILE=1)
add_definitions(-DFLEXON_PLATFORM_LINUX=0)

# Disable features not available on Android
add_definitions(-DFLEXON_HAVE_GNU_READLINE=0)
add_definitions(-DFLEXON_HAVE_LIBEDIT_READLINE=0)

# Optional: Enable linenoise for basic line editing
# add_definitions(-DFLEXON_HAS_LINENOISE=1)

# Android-specific compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# Optimize for mobile
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -DNDEBUG")

# Android logging support
find_library(ANDROID_LOG_LIB log)

message(STATUS "Android NDK: ${CMAKE_ANDROID_NDK}")
message(STATUS "Android API Level: ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Android ABI: ${CMAKE_ANDROID_ARCH_ABI}")
message(STATUS "FlexonDB Android build configured")