# FlexonDB Cross-Platform Build System

FlexonDB has been refactored to use a modern, modular CMake build system with comprehensive cross-platform support. This document describes the new build system architecture and usage.

## 🏗️ Build System Architecture

### Directory Structure

```
flexon-db/
├── CMakeLists.txt              # Root CMake configuration
├── include/                    # Header files
│   ├── common/                 # Common utility headers
│   └── platform/               # Platform abstraction headers
├── src/                        # Source code
│   ├── platform/               # Platform abstraction implementation
│   ├── core/                   # Database engine
│   ├── shell/                  # Interactive shell
│   ├── cli/                    # Command-line interface
│   ├── common/                 # Common utilities
│   └── compat/                 # Compatibility layer
├── build/                      # Build directories (git-ignored)
│   ├── linux/
│   ├── macos/
│   ├── windows/
│   ├── android/
│   └── ios/
├── dist/                       # Distribution binaries (git-ignored)
│   ├── linux/
│   ├── macos/
│   ├── windows/
│   ├── android/
│   └── ios/
├── scripts/                    # Build and installation scripts
│   ├── build_all.sh           # Multi-platform build automation
│   ├── install.sh             # Linux/macOS installation
│   └── install.ps1            # Windows installation
└── toolchains/                # Cross-compilation toolchains
    ├── android.cmake          # Android NDK toolchain
    └── ios.cmake              # iOS toolchain
```

## 🚀 Quick Start

### Building for Your Platform

The easiest way to build FlexonDB is using the build automation script:

```bash
# Build for current platform
./scripts/build_all.sh

# Build with parallel jobs
./scripts/build_all.sh -j 4

# Clean build
./scripts/build_all.sh --clean
```

### Manual CMake Build

```bash
# Create build directory
mkdir -p build/linux
cd build/linux

# Configure and build
cmake ../..
make -j$(nproc)

# The binaries will be in ../dist/linux/bin/
```

### Installation

```bash
# Linux/macOS
./scripts/install.sh

# Windows (PowerShell as Administrator)
.\scripts\install.ps1
```

## 🛠️ Platform Support

### Supported Platforms

| Platform | Status | Build Command | Requirements |
|----------|--------|---------------|--------------|
| Linux    | ✅ Full | `./scripts/build_all.sh linux` | GCC/Clang |
| macOS    | ✅ Full | `./scripts/build_all.sh macos` | Xcode Command Line Tools |
| Windows  | ✅ Planned | `./scripts/build_all.sh windows` | MinGW or Visual Studio |
| Android  | ✅ Ready | `./scripts/build_all.sh android` | Android NDK |
| iOS      | ✅ Ready | `./scripts/build_all.sh ios` | Xcode (macOS only) |

### Platform Features

| Feature | Linux | macOS | Windows | Android | iOS |
|---------|-------|-------|---------|---------|-----|
| Core DB | ✅ | ✅ | ✅ | ✅ | ✅ |
| CLI     | ✅ | ✅ | ✅ | ❌ | ❌ |
| Shell   | ✅ | ✅ | ⚠️ | ❌ | ❌ |
| Readline| GNU | libedit | Fallback | linenoise | linenoise |
| Colors  | ✅ | ✅ | ⚠️ | ✅ | ✅ |

## 📦 Cross-Compilation

### Android

1. Install Android NDK
2. Set environment variable:
   ```bash
   export ANDROID_NDK=/path/to/android-ndk
   ```
3. Build:
   ```bash
   ./scripts/build_all.sh android
   ```

The Android toolchain targets:
- API Level 21 (Android 5.0+)
- arm64-v8a architecture
- Static C++ standard library

### iOS

1. Install Xcode on macOS
2. Build:
   ```bash
   ./scripts/build_all.sh ios
   ```

The iOS toolchain targets:
- iOS 12.0+
- arm64 architecture
- Embedded bitcode enabled

### Custom Toolchains

You can create custom toolchain files in the `toolchains/` directory:

```cmake
# toolchains/custom.cmake
set(CMAKE_SYSTEM_NAME YourSystem)
# ... your configuration
```

Then use it:
```bash
cmake -DCMAKE_TOOLCHAIN_FILE=toolchains/custom.cmake -S . -B build/custom
```

## 🔧 Build System Details

### CMake Structure

The build system is modular with each source directory having its own `CMakeLists.txt`:

- **Root**: Platform detection, configuration, output directories
- **src/platform/**: Platform abstraction layer
- **src/core/**: Database engine core
- **src/shell/**: Interactive shell with readline abstraction
- **src/cli/**: Command-line interface
- **src/common/**: Shared utilities
- **src/compat/**: Compatibility layer for older systems

### Platform Abstraction

The platform abstraction layer provides:

- **Terminal**: Unified readline/libedit/linenoise interface
- **Filesystem**: Cross-platform file operations
- **Colors**: ANSI color support with Windows compatibility
- **Platform Detection**: Compile-time platform identification

### Build Types

```bash
# Debug build
./scripts/build_all.sh --type Debug

# Release build (default)
./scripts/build_all.sh --type Release

# Release with debug info
./scripts/build_all.sh --type RelWithDebInfo
```

## 📋 Build Options

### CMake Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `BUILD_PLATFORM` | Target platform | Auto-detected |
| `FLEXON_BUILD_SHELL` | Build interactive shell | ON |
| `FLEXON_BUILD_TESTS` | Build test executables | ON |
| `FLEXON_FORCE_COLORS` | Force color output | OFF |

### Example Configuration

```bash
cmake -DBUILD_PLATFORM=linux \
      -DFLEXON_FORCE_COLORS=ON \
      -DCMAKE_BUILD_TYPE=Release \
      -S . -B build/linux
```

## 🧪 Testing

### Running Tests

```bash
# Build and run tests
cd build/linux
make run-tests

# Or run individual tests
./test_schema
./test_writer
./test_reader
```

### Platform-Specific Testing

Each platform build includes its own test suite:

```bash
# Test Linux build
build/dist/linux/bin/test_schema

# Test Android build (on device/emulator)
adb push build/dist/android/bin/test_schema /data/local/tmp/
adb shell /data/local/tmp/test_schema
```

## 🎯 Installation Options

### System-Wide Installation

```bash
# Linux/macOS (requires sudo)
./scripts/install.sh

# Windows (requires admin)
.\scripts\install.ps1
```

### User Installation

```bash
# Linux/macOS
./scripts/install.sh -d $HOME/bin

# Windows
.\scripts\install.ps1 -UserInstall
```

### Development Installation

```bash
# Install libraries and headers too
./scripts/install.sh -l

# Custom directories
./scripts/install.sh -d /opt/flexondb --lib-dir /opt/flexondb/lib
```

## 🔍 Troubleshooting

### Common Issues

**CMake not found**
```bash
# Ubuntu/Debian
sudo apt install cmake

# macOS
brew install cmake

# Windows
# Download from https://cmake.org/
```

**Android NDK not found**
```bash
export ANDROID_NDK=/path/to/android-ndk
# Or set ANDROID_NDK_ROOT
```

**iOS build on non-macOS**
```
iOS builds are only supported on macOS with Xcode installed.
```

**Readline not found on Linux**
```bash
# Ubuntu/Debian
sudo apt install libreadline-dev

# CentOS/RHEL
sudo yum install readline-devel
```

### Build Cleanup

```bash
# Clean all builds
rm -rf build/ dist/

# Clean specific platform
rm -rf build/linux dist/linux

# CMake clean
./scripts/build_all.sh --clean
```

## 📝 Migration Guide

### From Makefile to CMake

The old Makefile is still available but deprecated. Key differences:

| Makefile | CMake |
|----------|-------|
| `make all` | `cmake --build build/linux` |
| `make clean` | `rm -rf build/linux` |
| `make install` | `./scripts/install.sh` |
| `make test` | `make run-tests` (in build dir) |

### Legacy Support

The original Makefile remains functional for transition:

```bash
# Old way (still works)
make all

# New way (recommended)
./scripts/build_all.sh
```

## 🚀 Advanced Usage

### Parallel Builds

```bash
# Use all CPU cores
./scripts/build_all.sh -j $(nproc)

# Specific number of jobs
./scripts/build_all.sh -j 4
```

### Multiple Platforms

```bash
# Build for multiple platforms
./scripts/build_all.sh linux macos android

# Build for all platforms
./scripts/build_all.sh all
```

### Custom Build Scripts

You can extend the build system by creating custom scripts:

```bash
#!/bin/bash
# scripts/build_custom.sh

# Your custom build logic
./scripts/build_all.sh linux
./scripts/install.sh -d /custom/path
```

## 📚 Further Reading

- [CMake Documentation](https://cmake.org/documentation/)
- [Android NDK Documentation](https://developer.android.com/ndk)
- [iOS Development Guide](https://developer.apple.com/ios/)
- [Cross-Compilation with CMake](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html)

---

*This build system represents a complete modernization of FlexonDB's build infrastructure, enabling cross-platform development and easy distribution.*