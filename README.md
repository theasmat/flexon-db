# FlexonDB

[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)](https://github.com/theasmat/flexon-db)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/theasmat/flexon-db)

🚀 **FlexonDB** is a fast, lightweight, and cross-platform columnar database engine designed for simplicity and performance. Built in C with a focus on minimal dependencies and maximum compatibility.

## ✨ Features

### Core Functionality
- **🔄 Schema-based Tables** - Define structured data with strong typing
- **📊 Multiple Data Types** - Support for strings, integers, floats, and booleans
- **💾 Efficient Storage** - Compact binary file format (.fxdb)
- **🔗 JSON Integration** - Native JSON import/export capabilities
- **📈 Append Operations** - Add data to existing databases seamlessly

### Interfaces
- **💻 Command Line Interface** - Full-featured CLI for database operations
- **🖥️ Interactive Shell** - User-friendly shell with readline support
- **📱 Mobile Support** - React Native and native mobile bindings
- **🔧 C API** - Direct library integration for applications

### Export Formats
- **📋 Tabular Display** - Pretty-printed tables for human consumption
- **📄 CSV Export** - Industry-standard comma-separated values
- **🌐 JSON Export** - Modern JSON format for web applications

## 🚀 Quick Start

### Installation

```bash
# Clone the repository
git clone https://github.com/theasmat/flexon-db.git
cd flexon-db

# Build FlexonDB
make all

# Optional: Install system-wide
sudo make install
```

### Basic Usage

```bash
# Create a new database
./build/flexon create employees.fxdb --schema "name string, age int32, department string, salary float"

# Insert data
./build/flexon insert employees.fxdb --data '{"name": "Alice Johnson", "age": 30, "department": "Engineering", "salary": 75000.50}'

# Read data
./build/flexon read employees.fxdb

# Export as CSV
./build/flexon dump employees.fxdb --format csv

# Export as JSON
./build/flexon dump employees.fxdb --format json
```

### Interactive Shell

```bash
# Launch interactive shell
./build/flexon

# Shell commands
flexondb> create products.fxdb schema="id int32, name string, price float, available bool"
flexondb> use products.fxdb
flexondb> insert id=1 name="Laptop" price=999.99 available=true
flexondb> select *
flexondb> export csv
flexondb> exit
```

## 📚 Documentation

### Data Types

| Type     | Description                    | Size    | Example Values           |
|----------|--------------------------------|---------|--------------------------|
| `string` | Variable-length text           | 256B    | `"Hello World"`          |
| `int32`  | 32-bit signed integer          | 4B      | `42`, `-1000`            |
| `float`  | 32-bit floating point          | 4B      | `3.14`, `-0.5`           |
| `bool`   | Boolean true/false             | 1B      | `true`, `false`          |

### Schema Syntax

```
"field1 type1, field2 type2, field3 type3"
```

**Examples:**
```bash
# Simple user table
"name string, age int32"

# Product catalog
"id int32, name string, price float, available bool"

# Employee records
"name string, department string, salary float, active bool"
```

### Command Reference

#### CLI Commands

```bash
# Database Management
flexon create <file.fxdb> --schema "<schema>"     # Create database
flexon info <file.fxdb>                          # Show database info
flexon list                                      # List databases

# Data Operations  
flexon insert <file.fxdb> --data '<json>'        # Insert JSON data
flexon read <file.fxdb> [--limit N]             # Read data
flexon dump <file.fxdb> [--format csv|json]     # Export data

# Options
-d, --directory <path>    # Specify database directory
```

#### Shell Commands

```bash
# Database Operations
use <database>           # Switch to database
show databases          # List available databases
create <db> schema="..." # Create new database
drop <database>         # Delete database
info                    # Show current database info
schema                  # Show database schema

# Data Operations
select * [limit N]      # Query data
insert field=value ...  # Insert data interactively
count                   # Show row count
export [csv|json]       # Export data

# Utility
status                  # Show session info
history                 # Show command history
clear                   # Clear screen
help                    # Show help
exit, quit              # Exit shell
```

## 🏗️ Architecture

FlexonDB uses a columnar storage format optimized for analytical workloads:

```
┌─────────────────────────────────────────────────────────────┐
│                     .fxdb File Format                      │
├─────────────────────────────────────────────────────────────┤
│ Header (88 bytes)                                          │
│ ├─ Magic Number: "FXDB"                                    │
│ ├─ Version, Schema Info                                    │
│ └─ Data Offsets & Sizes                                    │
├─────────────────────────────────────────────────────────────┤
│ Schema Section                                              │
│ ├─ Field Definitions                                       │
│ ├─ Field Types & Sizes                                     │
│ └─ Schema String                                           │
├─────────────────────────────────────────────────────────────┤
│ Data Section                                                │
│ ├─ Row Chunks (10,000 rows each)                          │
│ ├─ Efficient Binary Encoding                               │
│ └─ Optimized for Sequential Access                         │
└─────────────────────────────────────────────────────────────┘
```

### Key Components

- **🔧 Core Engine** (`src/core/`) - Database operations and file management
- **💻 CLI Interface** (`src/cli/`) - Command-line tools and argument parsing  
- **🖥️ Shell Interface** (`src/shell/`) - Interactive shell and command processing
- **🔗 Compatibility Layer** (`src/compat/`) - Cross-platform abstractions
- **📱 Mobile Bindings** (`mobile/`) - React Native and native mobile support

## 🔧 API Reference

### C API Example

```c
#include "flexondb.h"

// Create database
schema_t* schema = parse_schema("name string, age int32");
writer_t* writer = writer_create_default("users.fxdb", schema);

// Insert data
writer_insert_json(writer, "{\"name\": \"John\", \"age\": 30}");
writer_close(writer);

// Read data
reader_t* reader = reader_open("users.fxdb");
query_result_t* result = reader_read_rows(reader, 10);
reader_print_rows(reader, result);

// Cleanup
reader_free_result(result);
reader_close(reader);
free_schema(schema);
```

### Error Handling

All FlexonDB functions return appropriate error codes:

- `0` - Success
- `-1` - General error
- `NULL` - Invalid pointer/allocation failure

Check return values and use provided error messages for debugging.

## 🛠️ Building from Source

### Prerequisites

- **GCC/Clang** - C99 compatible compiler
- **Make** - Build system
- **Readline** - Optional, for shell history (Linux/macOS)

### Build Options

```bash
# Standard build
make all

# Clean build
make clean && make all

# Individual components
make core     # Core database engine
make cli      # CLI tools only  
make shell    # Shell components
```

### Cross-Platform Support

FlexonDB builds on:
- ✅ **Linux** (tested on Ubuntu, CentOS, Alpine)
- ✅ **macOS** (Intel and Apple Silicon)
- ✅ **Windows** (MinGW, MSVC)
- ✅ **Android** (via NDK)
- ✅ **iOS** (via Xcode)

See [BUILDING.md](docs/BUILDING.md) for detailed platform-specific instructions.

## 📱 Mobile Integration

### React Native

```javascript
import FlexonDB from './react-native/FlexonDBModule';

// Create database
await FlexonDB.createDatabase('myapp.fxdb', 'name string, score int32');

// Insert data
await FlexonDB.insertData('myapp.fxdb', '{"name": "Player1", "score": 1000}');

// Read data
const data = await FlexonDB.readData('myapp.fxdb');
console.log(JSON.parse(data));
```

### Native iOS (Objective-C)

```objc
#import "FlexonDBBridge.h"

// Create and use database
[FlexonDBBridge createDatabase:@"app.fxdb" schema:@"name string, age int32"];
[FlexonDBBridge insertData:@"app.fxdb" json:@"{\"name\":\"User\",\"age\":25}"];
NSString* data = [FlexonDBBridge readData:@"app.fxdb"];
```

### Native Android (Java)

```java
import com.flexondb.FlexonDB;

// Create and use database
FlexonDB.createDatabase("app.fxdb", "name string, age int32");
FlexonDB.insertData("app.fxdb", "{\"name\":\"User\",\"age\":25}");
String data = FlexonDB.readData("app.fxdb");
```

## ⚡ Performance

FlexonDB is optimized for:

- **🚀 Fast Inserts** - Batch operations with chunked storage
- **📊 Efficient Reads** - Sequential access patterns
- **💾 Small Footprint** - Minimal memory usage
- **🔧 Low Overhead** - Direct binary format

### Benchmarks

| Operation | Throughput | Notes |
|-----------|------------|-------|
| Insert | ~50K rows/sec | JSON parsing included |
| Read | ~100K rows/sec | Sequential access |
| File Size | ~265 bytes/row | 4-field example schema |

## 🤝 Contributing

We welcome contributions! See [CONTRIBUTING.md](docs/CONTRIBUTING.md) for guidelines.

### Development Setup

```bash
git clone https://github.com/theasmat/flexon-db.git
cd flexon-db
make all
make test
```

### Testing

```bash
# Run all tests
make test

# Individual test suites
make test-schema   # Schema parsing tests
make test-writer   # Writer functionality tests  
make test-reader   # Reader functionality tests
```

## 📄 License

FlexonDB is released under the MIT License. See [LICENSE](LICENSE) for details.

## 🙏 Acknowledgments

- Built with love for the database community
- Inspired by modern columnar databases
- Designed for simplicity and performance

---

**FlexonDB** - Fast, Simple, Cross-Platform Database Engine

For questions, issues, or contributions, visit our [GitHub repository](https://github.com/theasmat/flexon-db) or check out the [documentation](docs/).