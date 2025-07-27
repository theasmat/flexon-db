# iOS Toolchain for FlexonDB
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=toolchains/ios.cmake -S . -B build/ios

set(CMAKE_SYSTEM_NAME iOS)
set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")  # iOS 12.0 minimum
set(CMAKE_OSX_ARCHITECTURES "arm64")     # 64-bit ARM for modern devices

# iOS SDK settings
set(CMAKE_OSX_SYSROOT iphoneos)

# Platform-specific settings for FlexonDB
set(BUILD_PLATFORM "ios")
set(FLEXON_MOBILE_BUILD ON)

# iOS-specific definitions
add_definitions(-DFLEXON_PLATFORM_IOS=1)
add_definitions(-DFLEXON_PLATFORM_APPLE=1)
add_definitions(-DFLEXON_PLATFORM_MOBILE=1)
add_definitions(-DFLEXON_PLATFORM_MACOS=0)
add_definitions(-DFLEXON_PLATFORM_LINUX=0)

# Disable features not typically available on iOS
add_definitions(-DFLEXON_HAVE_GNU_READLINE=0)
add_definitions(-DFLEXON_HAVE_LIBEDIT_READLINE=0)

# Optional: Enable linenoise for basic line editing
# add_definitions(-DFLEXON_HAS_LINENOISE=1)

# iOS-specific compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fembed-bitcode")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fembed-bitcode")

# Optimize for mobile
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -DNDEBUG")

# Code signing (for actual device deployment)
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO")

# iOS simulator support (optional)
option(IOS_SIMULATOR "Build for iOS Simulator" OFF)
if(IOS_SIMULATOR)
    set(CMAKE_OSX_SYSROOT iphonesimulator)
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
    message(STATUS "Building for iOS Simulator (x86_64)")
else()
    message(STATUS "Building for iOS Device (arm64)")
endif()

# Framework linking
find_library(FOUNDATION_FRAMEWORK Foundation)
find_library(UIKIT_FRAMEWORK UIKit)

message(STATUS "iOS Deployment Target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")
message(STATUS "iOS Architecture: ${CMAKE_OSX_ARCHITECTURES}")
message(STATUS "iOS SDK: ${CMAKE_OSX_SYSROOT}")
message(STATUS "FlexonDB iOS build configured")