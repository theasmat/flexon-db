# 🚀 FlexonDB - Modern Cross-Platform Database Engine

[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)](https://github.com/theasmat/flexon-db)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/theasmat/flexon-db)

**FlexonDB** is a high-performance, lightweight columnar database engine with modern development tools, comprehensive testing, and 20+ data types. Built with CMake-first development workflow and enhanced developer experience.

## ✨ Enhanced Features

### 🎯 Modern Development Experience
- **CMake-First Workflow** - Professional build system with presets
- **Comprehensive Testing** - Unit tests, integration tests, and benchmarks
- **Enhanced Data Types** - 20+ types with smart defaults and aliases
- **Unified CLI/Shell** - Perfect command parity between interfaces
- **Auto-Suggestions** - Tab completion and context-aware help

### 🔧 Core Database Engine
- **Schema-based Tables** - Structured data with strong typing
- **Optimized Storage** - Compact binary format with size optimization
- **JSON Integration** - Native import/export capabilities
- **Cross-Platform** - Linux, macOS, Windows, iOS, Android support

### 📊 Advanced Data Types
- **String Types**: `string16`, `string32`, `string64`, `string128`, `string256`, `string512`, `text`
- **Integer Types**: `int8`, `int16`, `int32`, `int64`, `uint8`, `uint16`, `uint32`, `uint64`
- **Float Types**: `float32`, `float64`, `decimal` with aliases (`num`, `bignum`, `double`)
- **Special Types**: `bool`, `timestamp`, `date`, `uuid`, `json`, `blob`

## 🚀 Quick Start

### Modern CMake Workflow

```bash
# Clone the repository
git clone https://github.com/theasmat/flexon-db.git
cd flexon-db

# Modern CMake development build
cmake --preset dev-debug
cmake --build build/dev-debug

# Run comprehensive tests
cmake --build build/dev-debug --target test-all

# Legacy shortcuts (optional)
make -f Makefile.dev test
make -f Makefile.dev help-dev
```

### Available CMake Presets

```bash
cmake --preset dev-debug         # Debug build with symbols and sanitizers
cmake --preset dev-release       # Optimized development build
cmake --preset dev-test          # Test configuration
cmake --preset dev-profile       # Profiling build with debug info
```

### Cross-Platform Builds

```bash
cmake -DBUILD_PLATFORM=auto -S . -B build/auto      # Auto-detect platform
cmake -DBUILD_PLATFORM=linux -S . -B build/linux   # Linux build
cmake -DBUILD_PLATFORM=macos -S . -B build/macos   # macOS build
cmake -DBUILD_PLATFORM=windows -S . -B build/win   # Windows build
```

## 📝 Enhanced Usage Examples

### Database Creation with Modern Types

```bash
# Optimized schema with precise types
./flexon create users.fxdb --schema "id int32, name string64, email string128"

# Compact schema for memory efficiency  
./flexon create sessions.fxdb --schema "id int16, token string32, active bool"

# High-precision financial data
./flexon create transactions.fxdb --schema "amount decimal, created timestamp"

# Using type aliases
./flexon create analytics.fxdb --schema "value bignum, count int, metadata json"
```

### Data Operations

```bash
# Insert data with JSON format
./flexon insert users.fxdb --data '{"id": 1, "name": "Alice", "email": "alice@example.com"}'

# Read data with limits
./flexon read users.fxdb --limit 10

# Export in multiple formats
./flexon dump users.fxdb --format csv
./flexon dump users.fxdb --format json
./flexon dump users.fxdb --format table
```

### Interactive Shell with Enhanced Help

```bash
# Start interactive shell
./flexon-shell

# Enhanced help system
flexondb> help                    # Show all commands in table format
flexondb> help create            # Detailed command help
flexondb> types                  # Show all 20+ data types with examples
```

## 📊 Data Types Reference

| Type Category | Types | Smart Defaults | Examples |
|---------------|-------|----------------|----------|
| **Strings** | `string16`-`string512`, `text` | `string` → `string256` | Short codes, names, descriptions |
| **Integers** | `int8`-`int64`, `uint8`-`uint64` | `int` → `int32` | IDs, counts, flags |
| **Floats** | `float32`, `float64`, `decimal` | `float` → `float32` | Prices, measurements |
| **Special** | `bool`, `timestamp`, `date`, `uuid`, `json`, `blob` | Type-specific | Flags, dates, metadata |

### Type Aliases for Convenience

```
string → string256    int → int32       float → float32
num → float32        double → float64   bignum → float64
```

## 🧪 Comprehensive Testing

### Running Tests

```bash
# All tests via CMake
cmake --build build/dev-debug --target test-all

# Individual test suites
make -f Makefile.dev test-schema        # Schema tests
make -f Makefile.dev test-writer        # Writer tests  
make -f Makefile.dev test-reader        # Reader tests

# Performance benchmarks
make -f Makefile.dev test-benchmarks    # Run benchmarks
tests/scripts/benchmark.sh              # Hyperfine benchmarks
```

### Test Categories

- **Unit Tests** - Schema, writer, reader, data types (100+ test cases)
- **Integration Tests** - CLI/shell parity, cross-platform compatibility
- **Benchmarks** - Insert, read, query, format performance
- **Performance** - Hyperfine-based benchmarks with JSON reporting

## 🏗️ Development Workflow

### Development Mode Features

```bash
# Enable development mode with enhanced features
cmake --preset dev-debug

# Features enabled in DEV_MODE:
# - Compile commands export for IDEs
# - Debug symbols and sanitizers
# - Comprehensive testing
# - Benchmark suite
```

### Legacy Makefile Support

```bash
# Quick test commands (wraps CMake)
make -f Makefile.dev test           # Run all tests
make -f Makefile.dev clean          # Clean build artifacts
make -f Makefile.dev help-dev       # Show development commands
```

## 📁 Project Structure

```
flexon-db/
├── CMakePresets.json              # Modern CMake presets
├── CMakeLists.txt                 # Enhanced build configuration
├── Makefile.dev                   # Legacy wrapper for tests
├── include/
│   ├── core/data_types.h         # Extended type system
│   └── common/command_processor.h # Unified CLI/Shell interface
├── src/
│   ├── core/
│   │   ├── data_types.c          # 20+ data type implementation
│   │   └── schema.c              # Enhanced schema parser
│   └── common/command_processor.c # Unified command handling
└── tests/
    ├── unit/                     # Unit tests (schema, types, commands)
    ├── integration/              # Integration tests (CLI/shell parity)
    ├── benchmarks/               # Performance benchmarks
    └── scripts/                  # Automation (hyperfine, reporting)
```

## 🎯 Advanced Examples

### Size-Optimized Schemas

```bash
# Before: 265 bytes per row
create old.fxdb --schema "id int32, name string, score float, active bool"

# After: 75 bytes per row (71% reduction)
create new.fxdb --schema "id int16, name string64, score float64, active bool"
```

### Type-Specific Use Cases

```bash
# IoT sensor data (compact)
create sensors.fxdb --schema "id int16, temp float32, humidity int8, timestamp timestamp"

# User profiles (readable)
create profiles.fxdb --schema "uid uuid, name string64, bio text, joined date"

# Financial records (precise)
create ledger.fxdb --schema "amount decimal, currency string16, created timestamp"
```

## 📚 Command Reference

| Command | Purpose | Example |
|---------|---------|---------|
| `create` | Create database with schema | `create users.fxdb --schema "id int, name string64"` |
| `insert` | Add data to database | `insert users.fxdb --data '{"id": 1, "name": "Alice"}'` |
| `read` | Display data | `read users.fxdb --limit 10` |
| `info` | Show database information | `info users.fxdb` |
| `dump` | Export data | `dump users.fxdb --format csv` |
| `list` | List database files | `list --directory /path/to/databases` |
| `help` | Show help | `help create` |
| `types` | Show data types | `types` |

## 🚀 Performance Features

- **Optimized Row Sizes** - Smart type selection reduces storage by 60-70%
- **Benchmark Suite** - Hyperfine integration with JSON reporting
- **Memory Efficiency** - Precise type sizing minimizes memory usage
- **Cross-Platform** - Optimized builds for each target platform

## 🤝 Contributing

FlexonDB uses modern CMake development practices:

1. Use `cmake --preset dev-debug` for development
2. Run `make -f Makefile.dev test` before committing
3. Follow the existing code style and testing patterns
4. Add tests for new features

## 📄 License

MIT License - see [LICENSE](LICENSE) for details.

---

**FlexonDB** - Modern database engine with enhanced developer experience 🚀