# FlexonDB Mobile Integration

This document describes the mobile and React Native integration for FlexonDB, providing cross-platform database functionality for Android, iOS, and desktop environments.

## Components Overview

### 1. Core Unified API (`core/flexondb.{h,c}`)

The unified API provides a simplified interface to the FlexonDB functionality:

- `createDatabase(path, schema)` - Create a new database with specified schema
- `insertData(path, json)` - Insert JSON data into existing database
- `readData(path)` - Read data from database (returns allocated string)
- `deleteDatabase(path)` - Delete database file
- `updateDatabase(path, json)` - Update database with new JSON data
- `csvToFlexonDB(csvPath, dbPath)` - Convert CSV file to FlexonDB format

Cross-platform logging is implemented using conditional compilation:
- Android: Uses `__android_log_print`
- iOS: Uses `os_log`
- Desktop: Uses `printf`

### 2. Mobile Bridges

#### Android JNI Bridge (`mobile/android/jni/flexondb_jni.c`)

Provides JNI wrappers for all core functions with proper Java/C string handling:
- `Java_com_flexondb_FlexonDB_createDatabase`
- `Java_com_flexondb_FlexonDB_insertData`
- `Java_com_flexondb_FlexonDB_readData`
- `Java_com_flexondb_FlexonDB_deleteDatabase`
- `Java_com_flexondb_FlexonDB_updateDatabase`
- `Java_com_flexondb_FlexonDB_csvToFlexonDB`

#### iOS Objective-C Bridge (`mobile/ios/FlexonDBBridge.{h,m}`)

Provides Objective-C class methods that directly call core C functions:
- All methods use UTF8String conversion for NSString/char* interoperability
- Proper memory management with automatic cleanup

### 3. Enhanced CLI Tools

#### Command Line Interface (`cli/main.c`)

New CLI tool with colorful ANSI output supporting:
- `create <path> <schema>` - Create database
- `update <path> <json>` - Update database
- `csv2fxdb <csvPath> <dbPath>` - Convert CSV to FlexonDB
- `read <path>` - Read database
- `delete <path>` - Delete database

#### Interactive Shell (`cli/shell.c`)

Interactive shell interface with:
- Colorful prompt and output
- Command history support
- Real-time command processing
- Error handling with colored messages

### 4. React Native Module (`react-native/FlexonDBModule.js`)

JavaScript module providing Promise-based APIs:
- Automatic path correction using `react-native-fs`
- Platform-specific document directory handling
- All core functions exposed as async methods

### 5. Build System (`Makefile.new`)

Enhanced Makefile with targets for:
- `cli-tools` - Build new CLI tools
- `test-cli` - Test CLI functionality
- `test-shell` - Test interactive shell
- `demo-cli` - Run CLI demonstration

## Testing

### Current Status

✅ **Working Components:**
- Core API layer with cross-platform compatibility
- Database creation and schema parsing
- CSV to FlexonDB conversion (structure creation)
- CLI tools with colorful output
- Interactive shell interface
- Mobile bridge implementations
- React Native module structure

⚠️ **Limitations:**
- `writer_insert_json` is not implemented in the underlying FlexonDB
- `writer_open` for appending to existing databases is not implemented
- JSON insertion functionality is stubbed but demonstrates the API

### Build and Test

```bash
# Build new CLI tools
make -f Makefile.new cli-tools

# Test basic functionality
./build/flexondb-cli create test.fxdb "name string, age int32"
./build/flexondb-cli read test.fxdb

# Test CSV conversion
echo "name,age,city\nAlice,30,New York\nBob,25,Seattle" > test.csv
./build/flexondb-cli csv2fxdb test.csv people.fxdb

# Test interactive shell
./build/flexondb-shell
```

## Architecture Benefits

1. **Modular Design**: Core API separates platform-specific concerns
2. **Cross-Platform**: Single codebase works on Android, iOS, and desktop
3. **Consistent Interface**: Same API across all platforms
4. **Memory Safety**: Proper cleanup and error handling
5. **Extensible**: Easy to add new functions to all platforms

## Future Enhancements

1. Complete `writer_insert_json` implementation
2. Implement `writer_open` for database appending
3. Add query functionality to the unified API
4. Enhanced error reporting with error codes
5. Performance optimizations for mobile platforms
6. Unit test suite for all components

## Usage Examples

### CLI Usage
```bash
# Create database
./build/flexondb-cli create employees.fxdb "name string, department string, salary float"

# Convert CSV
./build/flexondb-cli csv2fxdb employees.csv employees.fxdb

# Read data
./build/flexondb-cli read employees.fxdb
```

### React Native Usage
```javascript
import FlexonDB from './react-native/FlexonDBModule';

// Create database
await FlexonDB.createDatabase('mydb.fxdb', 'name string, age int32');

// Insert data
await FlexonDB.insertData('mydb.fxdb', '{"name": "John", "age": 30}');

// Read data
const result = await FlexonDB.readData('mydb.fxdb');
```

This implementation provides a solid foundation for mobile FlexonDB integration while maintaining compatibility with the existing desktop functionality.