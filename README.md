# 🚀 FlexonDB - Modern Cross-Platform Database Engine

[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)](https://github.com/theasmat/flexon-db)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/theasmat/flexon-db)

**FlexonDB** is a high-performance, lightweight columnar database engine with modern cross-platform development tools, comprehensive testing, and 20+ data types. Built with CMake-first development workflow and enhanced developer experience across Linux, macOS, Windows, Android, and iOS.

## ✨ Enhanced Features

### 🎯 Modern Cross-Platform Development
- **CMake-Based Build System** - Professional cross-platform builds with presets
- **Multi-Platform Support** - Linux, macOS, Windows, Android, iOS native builds
- **Cross-Compilation Ready** - Android NDK and iOS toolchain support
- **Platform Abstractions** - Clean terminal/readline integration per platform
- **Automated Build Scripts** - One-command builds for all platforms

### 🔧 Core Database Engine
- **Schema-based Tables** - Structured data with strong typing
- **Optimized Storage** - Compact binary format with size optimization
- **JSON Integration** - Native import/export capabilities
- **Enhanced Data Types** - 20+ types with smart defaults and aliases
- **Cross-Platform CLI** - Unified command interface across all platforms

### 📊 Advanced Data Types
- **String Types**: `string16`, `string32`, `string64`, `string128`, `string256`, `string512`, `text`
- **Integer Types**: `int8`, `int16`, `int32`, `int64`, `uint8`, `uint16`, `uint32`, `uint64`
- **Float Types**: `float32`, `float64`, `decimal` with aliases (`num`, `bignum`, `double`)
- **Special Types**: `bool`, `timestamp`, `date`, `uuid`, `json`, `blob`

## 🚀 Quick Start

### Platform Requirements

| Platform | Requirements | Terminal Support |
|----------|-------------|------------------|
| **Linux** | GCC/Clang, CMake 3.10+ | GNU Readline |
| **macOS** | Xcode CLI Tools, CMake 3.10+ | libedit (built-in) |
| **Windows** | MSVC/MinGW, CMake 3.10+ | Linenoise/fallback |
| **Android** | Android NDK r21+, CMake 3.10+ | Linenoise |
| **iOS** | Xcode 12+, iOS 12.0+ | Linenoise |

### One-Command Build (All Platforms)

```bash
# Clone and build for your platform
git clone https://github.com/theasmat/flexon-db.git
cd flexon-db

# Build for current platform
./scripts/build_all.sh

# Build for multiple platforms
./scripts/build_all.sh linux macos windows

# Build with cross-compilation (requires toolchains)
./scripts/build_all.sh android ios

# Clean build for all available platforms
./scripts/build_all.sh --clean all
```

### Platform-Specific Builds

#### Linux/macOS/Windows
```bash
# Modern CMake workflow
mkdir -p build/linux
cd build/linux
cmake -DCMAKE_BUILD_TYPE=Release ../..
cmake --build . --parallel

# Output: dist/linux/bin/flexon
```

#### Android (Cross-Compilation)
```bash
# Requires Android NDK
export ANDROID_NDK=/path/to/android-ndk

mkdir -p build/android
cd build/android
cmake -DCMAKE_TOOLCHAIN_FILE=../../toolchains/android.cmake \
      -DCMAKE_BUILD_TYPE=Release ../..
cmake --build . --parallel

# Output: dist/android/bin/flexon (ARM64)
```

#### iOS (Cross-Compilation, macOS only)
```bash
# Requires Xcode
mkdir -p build/ios
cd build/ios
cmake -DCMAKE_TOOLCHAIN_FILE=../../toolchains/ios.cmake \
      -DCMAKE_BUILD_TYPE=Release ../..
cmake --build . --parallel

# Output: dist/ios/bin/flexon (ARM64)
```

### CMake Presets (Advanced)

```bash
# Debug build with sanitizers
cmake --preset dev-debug
cmake --build build/dev-debug

# Release build optimized
cmake --preset dev-release
cmake --build build/dev-release

# Cross-platform auto-detection
cmake -DBUILD_PLATFORM=auto -S . -B build/auto
```

## 📦 Installation

### Automated Installation

#### Linux/macOS
```bash
# System-wide installation (requires sudo)
./scripts/install.sh

# User installation (no sudo required)
./scripts/install.sh -d $HOME/bin

# Install with libraries and headers
./scripts/install.sh -l
```

#### Windows
```powershell
# System-wide installation (requires Administrator)
.\scripts\install.ps1

# User installation (no admin required)
.\scripts\install.ps1 -UserInstall

# Install with libraries
.\scripts\install.ps1 -LibraryInstall
```

### Manual Installation

```bash
# Copy binaries from dist/{platform}/bin/
cp dist/linux/bin/flexon /usr/local/bin/
cp dist/macos/bin/flexon /usr/local/bin/   # macOS
cp dist/windows/bin/flexon.exe C:\Program Files\FlexonDB\  # Windows
```

## 📝 Cross-Platform Usage

### Database Creation with Modern Types

```bash
# Optimized schema with precise types
flexon create users.fxdb --schema "id int32, name string64, email string128"

# Compact schema for IoT/mobile (71% size reduction)
flexon create sensors.fxdb --schema "id int16, temp float32, humidity int8, timestamp timestamp"

# High-precision financial data
flexon create ledger.fxdb --schema "amount decimal, currency string16, created timestamp"

# Using type aliases for convenience
flexon create analytics.fxdb --schema "value bignum, count int, metadata json"
```

### Data Operations (Cross-Platform)

```bash
# Insert data with JSON format
flexon insert users.fxdb --data '{"id": 1, "name": "Alice", "email": "alice@example.com"}'

# Read data with limits
flexon read users.fxdb --limit 10

# Export in multiple formats
flexon dump users.fxdb --format csv
flexon dump users.fxdb --format json
flexon dump users.fxdb --format table
```

### Interactive Shell (Platform-Adaptive)

```bash
# Start interactive shell with platform-specific terminal support
flexon-shell    # Uses GNU readline (Linux), libedit (macOS), or linenoise (Windows/mobile)

# Enhanced help system
flexondb> help                    # Show all commands
flexondb> help create             # Detailed command help
flexondb> types                   # Show all 20+ data types
```

## 🏗️ Development & Building

### Build System Architecture

```
flexon-db/
├── CMakeLists.txt                 # Root build configuration
├── CMakePresets.json             # Development presets
├── src/
│   ├── core/CMakeLists.txt       # Core database engine
│   ├── shell/CMakeLists.txt      # Interactive shell
│   ├── cli/CMakeLists.txt        # Command-line interface
│   ├── common/CMakeLists.txt     # Shared utilities
│   ├── platform/CMakeLists.txt  # Platform abstractions
│   └── compat/CMakeLists.txt     # Compatibility layer
├── toolchains/
│   ├── android.cmake             # Android NDK toolchain
│   └── ios.cmake                 # iOS Xcode toolchain
└── scripts/
    ├── build_all.sh              # Cross-platform build automation
    ├── install.sh                # Linux/macOS installation
    └── install.ps1               # Windows installation
```

### Platform Detection & Abstraction

The build system automatically detects platforms and configures appropriate:
- **Terminal Libraries**: GNU readline (Linux), libedit (macOS), linenoise (Windows/mobile)
- **ANSI Color Support**: Full ANSI colors on Unix, Windows 10+ ANSI, fallback stubs
- **File System APIs**: POSIX APIs on Unix, Win32 APIs on Windows
- **Cross-Compilation**: Android NDK and iOS toolchain integration

### Build Options

```bash
# Development features
cmake -DDEV_MODE=ON              # Enable debug symbols, sanitizers, compile_commands.json
cmake -DENABLE_TESTING=ON        # Build test suite
cmake -DBUILD_BENCHMARKS=ON      # Build performance benchmarks

# Platform options
cmake -DFLEXON_FORCE_COLORS=ON   # Force color output
cmake -DFLEXON_BUILD_SHELL=OFF   # Disable shell (mobile builds)

# Cross-compilation
cmake -DCMAKE_TOOLCHAIN_FILE=toolchains/android.cmake  # Android
cmake -DCMAKE_TOOLCHAIN_FILE=toolchains/ios.cmake      # iOS
```

## 🧪 Testing & Validation

### Comprehensive Test Suite

```bash
# Run all tests
cmake --build build --target test-all

# Individual test categories
cmake --build build --target test-schema     # Schema validation
cmake --build build --target test-writer     # Write operations
cmake --build build --target test-reader     # Read operations

# Cross-platform validation
./scripts/build_all.sh --clean all           # Build all platforms
```

### Platform Validation

| Platform | Status | Features |
|----------|--------|----------|
| **Linux** | ✅ Fully Tested | GNU readline, ANSI colors, full feature set |
| **macOS** | ✅ Fully Tested | libedit, ANSI colors, full feature set |
| **Windows** | ✅ Ready to Test | Linenoise, Windows 10+ ANSI, full feature set |
| **Android** | ✅ Ready to Test | Linenoise, mobile optimizations |
| **iOS** | ✅ Ready to Test | Linenoise, mobile optimizations |

## 📊 Performance & Optimization

### Size-Optimized Schemas

```bash
# Before: Generic types (265 bytes per row)
create old.fxdb --schema "id int32, name string, score float, active bool"

# After: Precise types (75 bytes per row - 71% reduction)
create new.fxdb --schema "id int16, name string64, score float64, active bool"
```

### Platform-Specific Optimizations

- **Mobile Builds**: Size-optimized (`-Os`), reduced feature set, linenoise integration
- **Desktop Builds**: Performance-optimized (`-O2`), full terminal integration
- **Cross-Platform**: Consistent API, platform-adaptive implementations

## 🤝 Contributing

FlexonDB uses modern CMake development practices:

1. **Use cross-platform builds**: `./scripts/build_all.sh host`
2. **Run comprehensive tests**: `cmake --build build --target test-all`
3. **Follow modular architecture**: Each module has its own CMakeLists.txt
4. **Test on multiple platforms**: Use toolchains for cross-compilation
5. **Maintain compatibility**: Ensure builds work on all supported platforms

## 🌍 Cross-Platform Deployment

### Distribution Structure

```
dist/
├── linux/          # Linux x86_64 binaries
├── macos/          # macOS universal binaries
├── windows/        # Windows x64 binaries
├── android/        # Android ARM64 binaries
└── ios/            # iOS ARM64 binaries
```

### Integration Examples

```bash
# Container deployment (Linux)
FROM alpine:latest
COPY dist/linux/bin/flexon /usr/local/bin/
RUN chmod +x /usr/local/bin/flexon

# macOS app bundle
cp dist/macos/bin/flexon MyApp.app/Contents/MacOS/

# Windows installer
copy dist\windows\bin\flexon.exe "%ProgramFiles%\MyApp\"

# Android APK integration
cp dist/android/bin/flexon app/src/main/jniLibs/arm64-v8a/

# iOS framework integration
cp dist/ios/bin/flexon MyApp.framework/
```

## 📄 License

MIT License - see [LICENSE](LICENSE) for details.

---

**FlexonDB** - Modern cross-platform database engine with enhanced developer experience 🚀