# FlexonDB Cross-Platform Compatibility

This document describes the cross-platform compatibility improvements made to FlexonDB to support Linux, macOS, Windows, Android, and iOS platforms.

## Overview

FlexonDB has been refactored with a comprehensive compatibility layer that provides:

- **Platform Detection**: Automatic detection of operating systems and compilers
- **Function Compatibility**: Shims for missing platform functions
- **Feature Detection**: Runtime detection of available features
- **Readline Compatibility**: Support for GNU readline, libedit, or stub fallback
- **Cross-Platform Build System**: Both Makefile and CMake support

## Platform Support

### ✅ Fully Supported Platforms

- **Linux** (Ubuntu, CentOS, Fedora, etc.)
  - GNU readline support
  - Full POSIX compatibility
  - All features available

- **macOS** 
  - libedit readline support (macOS default)
  - Full POSIX compatibility  
  - All features available

### 🔄 Supported with Fallbacks

- **Windows** (MinGW/MSVC)
  - No readline (uses stub implementation)
  - Custom implementations for POSIX functions
  - File path compatibility layer

- **Android NDK**
  - Android logging support
  - No readline (uses stub implementation)
  - Mobile-optimized minimal features

- **iOS** 
  - iOS logging support
  - No readline (uses stub implementation)
  - Mobile-optimized minimal features

## Architecture

### Compatibility Headers

#### `include/platform.h`
- Central platform detection and feature macros
- Unified cross-platform includes
- Platform-specific logging macros
- File path separator definitions

#### `include/compat.h`
- Function compatibility declarations
- Feature availability macros
- Readline compatibility layer
- Safe string operation wrappers

#### `src/compat/compat.c`
- Implementation of compatibility shims
- Windows-specific implementations
- Readline stub implementation for mobile platforms

### Key Features

#### Platform Detection
```c
#ifdef FLEXON_PLATFORM_WINDOWS
    // Windows-specific code
#elif defined(FLEXON_PLATFORM_MACOS)
    // macOS-specific code
#elif defined(FLEXON_PLATFORM_LINUX)
    // Linux-specific code
#elif defined(FLEXON_PLATFORM_ANDROID)
    // Android-specific code
#elif defined(FLEXON_PLATFORM_IOS)
    // iOS-specific code
#endif
```

#### Feature Detection
```c
#ifdef FLEXON_HAVE_READLINE
    // Full readline functionality
#else
    // Fallback to stub implementation
#endif

#ifdef FLEXON_HAVE_POSIX
    // POSIX-specific features
#endif
```

#### Cross-Platform Logging
```c
FLEXON_LOG("Info message");
FLEXON_LOG_ERROR("Error message");
FLEXON_LOG_DEBUG("Debug message");
```

## Build Systems

### Makefile (Traditional)
```bash
# Basic build
make all

# Clean build
make clean && make all

# Run tests
make test

# Install (Linux/macOS)
make install
```

The Makefile includes automatic readline detection:
- Tries pkg-config first
- Falls back to library detection
- Uses stub implementation if none found

### CMake (Modern Cross-Platform)
```bash
mkdir build && cd build
cmake ..
make -j4

# Windows with Visual Studio
cmake .. -G "Visual Studio 16 2019"
cmake --build .

# Cross-compile for Android
cmake .. -DANDROID=ON -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake
make
```

CMake provides:
- Automatic platform detection
- Optional dependency detection
- Cross-compilation support
- Install rules for desktop platforms

## Compatibility Features

### String Functions
- `strdup()` - Implemented for Windows
- `getline()` - Implemented for platforms without it
- Safe string operations with bounds checking

### File System
- Cross-platform path separators
- Windows file access compatibility
- Directory operations

### Time Functions  
- `clock_gettime()` implementation for Windows
- `struct timespec` definition where needed

### Memory Mapping
- POSIX `mmap()` on Unix-like systems
- Windows `CreateFileMapping()` wrapper
- Graceful fallback when unavailable

### Readline Interface
- GNU readline on Linux
- libedit on macOS
- Stub implementation for mobile/embedded

The stub implementation provides:
- Basic line input with `fgets()`
- No-op history functions
- Compatible API for shell interface

### Signal Handling
- POSIX signals on Unix-like systems
- Windows signal compatibility
- Graceful degradation where unavailable

## Migration from Legacy Code

### Before (Platform-Specific)
```c
#include <readline/readline.h>
#include <unistd.h>
#include <sys/stat.h>

char* line = readline("prompt> ");
add_history(line);
```

### After (Cross-Platform)
```c
#include "platform.h"
#include "compat.h"

// Works on all platforms
char* line = readline("prompt> ");
add_history(line);
```

## Building for Specific Platforms

### Linux
```bash
make all
# or
cmake .. && make
```

### macOS
```bash
# Uses libedit by default
make all
# or  
cmake .. && make
```

### Windows (MinGW)
```bash
# In MSYS2/MinGW environment
make all
# or
cmake .. -G "MinGW Makefiles" && make
```

### Windows (Visual Studio)
```cmd
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
```

### Android NDK
```bash
export ANDROID_NDK=/path/to/ndk
cmake .. -DANDROID=ON \
         -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
         -DANDROID_ABI=arm64-v8a \
         -DANDROID_PLATFORM=android-21
make
```

### iOS (Xcode)
```bash
cmake .. -G Xcode \
         -DCMAKE_TOOLCHAIN_FILE=ios.toolchain.cmake \
         -DIOS_PLATFORM=OS64
```

## Testing

The compatibility layer includes comprehensive tests:

```bash
# Run all tests
make test

# Individual component tests  
./build/test_schema
./build/test_writer
./build/test_reader
./build/test_config
```

Tests verify:
- Cross-platform compilation
- Core functionality on all platforms
- Fallback behavior when features are missing
- Error handling and graceful degradation

## Feature Matrix

| Feature | Linux | macOS | Windows | Android | iOS |
|---------|-------|-------|---------|---------|-----|
| Core DB | ✅ | ✅ | ✅ | ✅ | ✅ |
| CLI Tools | ✅ | ✅ | ✅ | ❌ | ❌ |
| Interactive Shell | ✅ | ✅ | ⚠️* | ❌ | ❌ |
| File I/O | ✅ | ✅ | ✅ | ✅ | ✅ |
| Memory Mapping | ✅ | ✅ | ✅ | ✅ | ✅ |
| Signal Handling | ✅ | ✅ | ⚠️* | ⚠️* | ⚠️* |
| Logging | ✅ | ✅ | ✅ | ✅** | ✅** |

- ⚠️* = Limited functionality
- ✅** = Platform-specific logging (Android Log, iOS os_log)
- ❌ = Not available/suitable for platform

## Troubleshooting

### Common Issues

1. **Readline not found**
   - Install development packages: `apt-get install libreadline-dev`
   - Or build uses stub implementation automatically

2. **CMake not finding dependencies**
   - Install pkg-config: `apt-get install pkg-config`
   - Set CMAKE_PREFIX_PATH for custom installations

3. **Windows compilation errors**
   - Use MSYS2/MinGW for Unix-like environment
   - Or use Visual Studio with CMake

4. **Android/iOS build issues**
   - Ensure NDK/Xcode toolchain is properly configured
   - Check CMake toolchain file paths

### Debug Output

Enable debug information:
```bash
# Makefile
make CFLAGS="-DFLEXON_DEBUG=1" all

# CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug -DFLEXON_DEBUG=1
```

This provides verbose platform detection and feature availability information.

## Contributing

When adding new platform-specific code:

1. Use platform detection macros from `platform.h`
2. Provide fallbacks for missing features  
3. Add compatibility shims to `compat.c` if needed
4. Test on multiple platforms
5. Update this documentation

## Future Enhancements

Planned improvements:
- WebAssembly (WASM) support
- More Windows-specific optimizations  
- Enhanced mobile platform features
- Additional readline alternatives (editline, etc.)
- Better cross-compilation documentation