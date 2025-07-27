# Building FlexonDB

This document provides detailed instructions for building FlexonDB on different platforms.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Quick Build](#quick-build)
- [Platform-Specific Instructions](#platform-specific-instructions)
- [Build Options](#build-options)
- [Troubleshooting](#troubleshooting)

## Prerequisites

### All Platforms
- **C Compiler**: GCC 4.9+, Clang 3.5+, or MSVC 2015+
- **Make**: GNU Make 3.81+ or compatible build system
- **Standard C Library**: C99 compatible

### Optional Dependencies
- **Readline**: For enhanced shell experience with command history
  - Linux: `libreadline-dev` or `readline-devel`
  - macOS: Usually available (libedit)
  - Windows: Not required

## Quick Build

For most systems, a simple build process works:

```bash
# Clone repository
git clone https://github.com/theasmat/flexon-db.git
cd flexon-db

# Build everything
make all

# Run tests to verify
make test

# Optional: Install system-wide
sudo make install
```

## Platform-Specific Instructions

### Linux (Ubuntu/Debian)

#### Install Dependencies
```bash
# Required packages
sudo apt update
sudo apt install build-essential

# Optional: Readline for enhanced shell
sudo apt install libreadline-dev
```

#### Build
```bash
make clean
make all
```

#### Verify Installation
```bash
./build/flexon --help
./build/test_schema
```

### Linux (CentOS/RHEL/Fedora)

#### Install Dependencies
```bash
# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install readline-devel

# Fedora
sudo dnf groupinstall "Development Tools"
sudo dnf install readline-devel
```

#### Build
```bash
make clean
make all
```

### macOS

#### Install Dependencies
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Optional: Install Homebrew and readline
brew install readline
```

#### Build
```bash
# Standard build (uses libedit by default)
make clean
make all

# With Homebrew readline
export LDFLAGS="-L/opt/homebrew/lib"
export CPPFLAGS="-I/opt/homebrew/include"
make clean
make all
```

### Windows (MinGW)

#### Install MinGW-w64
1. Download and install [MinGW-w64](https://www.mingw-w64.org/downloads/)
2. Add MinGW bin directory to PATH

#### Build
```bash
# In MinGW terminal
make clean
make all CC=gcc
```

### Windows (MSVC)

#### Prerequisites
- Visual Studio 2015 or later
- Windows SDK

#### Build with CMake
```bash
# Create build directory
mkdir build
cd build

# Generate Visual Studio project
cmake .. -G "Visual Studio 16 2019" -A x64

# Build
cmake --build . --config Release
```

### Windows (WSL)

Use the Linux Ubuntu instructions within WSL:

```bash
sudo apt update
sudo apt install build-essential libreadline-dev
make all
```

## Cross-Platform Build with CMake

FlexonDB includes CMake support for cross-platform builds:

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build .

# Install (optional)
cmake --install .
```

### CMake Options

```bash
# Disable shell features
cmake .. -DFLEXON_BUILD_SHELL=OFF

# Specify custom readline path
cmake .. -DREADLINE_ROOT=/usr/local

# Build for specific architecture
cmake .. -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"  # macOS Universal
```

## Mobile Platform Builds

### Android (NDK)

#### Prerequisites
- Android NDK r21+
- CMake 3.10+

#### Build
```bash
# Set NDK path
export ANDROID_NDK=/path/to/android-ndk

# Configure for Android
cmake .. -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
         -DANDROID_ABI=arm64-v8a \
         -DANDROID_PLATFORM=android-21

# Build
cmake --build .
```

### iOS

#### Prerequisites
- Xcode 10+
- iOS SDK 12.0+

#### Build
```bash
# Configure for iOS
cmake .. -G Xcode \
         -DCMAKE_SYSTEM_NAME=iOS \
         -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0

# Build
cmake --build . --config Release
```

## Build Options

### Make Targets

```bash
# Core targets
make all          # Build everything
make core         # Core database engine only
make cli          # CLI tools only
make shell        # Shell components only

# Test targets  
make test         # Run all tests
make test-schema  # Schema parsing tests
make test-writer  # Writer functionality tests
make test-reader  # Reader functionality tests

# Utility targets
make clean        # Clean build artifacts
make install      # Install system-wide
make help         # Show available targets
```

### Compiler Flags

Default flags can be overridden:

```bash
# Debug build
make CFLAGS="-Wall -Wextra -std=c99 -g -O0 -DDEBUG"

# Release build with optimizations
make CFLAGS="-Wall -Wextra -std=c99 -O3 -DNDEBUG"

# Custom compiler
make CC=clang

# Cross-compilation
make CC=arm-linux-gnueabihf-gcc
```

### Build Configurations

#### Minimal Build (No Readline)
```bash
make CFLAGS="-DFLEXON_NO_READLINE" all
```

#### Debug Build
```bash
make CFLAGS="-g -O0 -DDEBUG -DENABLE_DEBUG=1" all
```

#### Static Linking
```bash
make LDFLAGS="-static" all
```

## Build Verification

### Smoke Tests

After building, run these tests to verify functionality:

```bash
# 1. Test basic functionality
./build/test_schema
./build/test_writer
./build/test_reader

# 2. Test CLI
./build/flexon create test.fxdb --schema "name string, age int32"
./build/flexon insert test.fxdb --data '{"name": "Test", "age": 25}'
./build/flexon read test.fxdb

# 3. Test shell (if readline available)
echo -e "create test2.fxdb schema=\"name string\"\nuse test2.fxdb\ninsert name=\"Shell Test\"\nselect *\nexit" | ./build/flexon
```

### Performance Test

```bash
# Create test with larger dataset
time ./build/flexon create large.fxdb --schema "id int32, name string, value float"

# Insert multiple records (if shell supports it)
for i in {1..1000}; do
    ./build/flexon insert large.fxdb --data "{\"id\": $i, \"name\": \"Record $i\", \"value\": $((RANDOM % 1000)).0}"
done

# Read performance
time ./build/flexon read large.fxdb > /dev/null
```

## Troubleshooting

### Common Issues

#### "readline not found"
```bash
# Solution 1: Install readline development package
sudo apt install libreadline-dev    # Ubuntu/Debian
sudo yum install readline-devel      # CentOS/RHEL

# Solution 2: Build without readline
make CFLAGS="-DFLEXON_NO_READLINE" all
```

#### "command not found: make"
```bash
# Install build tools
sudo apt install build-essential    # Ubuntu/Debian
sudo yum groupinstall "Development Tools"  # CentOS/RHEL
xcode-select --install              # macOS
```

#### "undefined reference" errors
```bash
# Check for missing libraries
ldd ./build/flexon

# Ensure proper linking order
make clean && make all
```

#### Windows-specific issues
```bash
# Ensure MinGW is in PATH
echo $PATH | grep mingw

# Use Windows-style paths if needed
make INCLUDES="-IC:/path/to/headers"
```

### Build Logs

For debugging build issues, enable verbose output:

```bash
# Verbose make
make V=1 all

# CMake verbose
cmake --build . --verbose

# Save build log
make all 2>&1 | tee build.log
```

### Platform-Specific Notes

#### Linux
- Some distributions require `readline-dev` vs `libreadline-dev`
- Older systems may need `gcc-4.9` or newer
- Static builds may require `glibc-static`

#### macOS
- Uses `libedit` by default (readline-compatible)
- Universal binaries supported via CMake
- May need Xcode command line tools

#### Windows
- MinGW recommended over Cygwin
- MSVC builds require CMake
- Path separators and line endings may cause issues

## Advanced Build Configuration

### Custom Installation Prefix

```bash
# Install to custom location
make PREFIX=/opt/flexondb install

# With CMake
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/flexondb
cmake --install .
```

### Development Build

```bash
# Enable all warnings and debugging
make CFLAGS="-Wall -Wextra -Wpedantic -g -O0 -DDEBUG -fsanitize=address" \
     LDFLAGS="-fsanitize=address" \
     all
```

### Cross-Compilation

```bash
# ARM Linux
make CC=arm-linux-gnueabihf-gcc \
     AR=arm-linux-gnueabihf-ar \
     STRIP=arm-linux-gnueabihf-strip \
     all

# MIPS
make CC=mips-linux-gnu-gcc all
```

---

For additional help or platform-specific issues, please check our [Issues](https://github.com/theasmat/flexon-db/issues) page or create a new issue.