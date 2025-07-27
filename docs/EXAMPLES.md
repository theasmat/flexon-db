# FlexonDB Examples

This document provides practical examples of using FlexonDB for various use cases.

## Table of Contents

- [Basic Usage](#basic-usage)
- [Data Management](#data-management)
- [CLI Examples](#cli-examples)
- [Interactive Shell Examples](#interactive-shell-examples)
- [Programming Examples](#programming-examples)
- [Real-World Use Cases](#real-world-use-cases)

## Basic Usage

### Creating Your First Database

```bash
# Create an employee database
./build/flexon create employees.fxdb --schema "name string, department string, age int32, salary float"

# Verify creation
./build/flexon info employees.fxdb
```

Output:
```
🛠️  Creating database: employees.fxdb
📋 Schema: name string, department string, age int32, salary float

✅ Parsed schema:
Schema (4 fields, 265 bytes per row):
┌─────────────────────────────────┬──────────┬───────────┐
│ Field Name                      │ Type     │ Size (B)  │
├─────────────────────────────────┼──────────┼───────────┤
│ name                            │ string   │ 256       │
│ department                      │ string   │ 256       │
│ age                             │ int32    │ 4         │
│ salary                          │ float    │ 4         │
└─────────────────────────────────┴──────────┴───────────┘

🎉 Database created successfully: employees.fxdb
```

### Adding Data

```bash
# Add single employee
./build/flexon insert employees.fxdb --data '{"name": "Alice Johnson", "department": "Engineering", "age": 30, "salary": 75000.50}'

# Add multiple employees
./build/flexon insert employees.fxdb --data '{"name": "Bob Smith", "department": "Marketing", "age": 25, "salary": 55000.00}'
./build/flexon insert employees.fxdb --data '{"name": "Carol Davis", "department": "Engineering", "age": 35, "salary": 85000.25}'
./build/flexon insert employees.fxdb --data '{"name": "David Wilson", "department": "Sales", "age": 28, "salary": 62000.00}'
```

### Reading Data

```bash
# Read all data (table format)
./build/flexon read employees.fxdb

# Read with limit
./build/flexon read employees.fxdb --limit 2

# Export as CSV
./build/flexon dump employees.fxdb --format csv

# Export as JSON
./build/flexon dump employees.fxdb --format json
```

## Data Management

### Working with Different Data Types

```bash
# Product catalog with all data types
./build/flexon create products.fxdb --schema "id int32, name string, price float, available bool"

# Insert products
./build/flexon insert products.fxdb --data '{"id": 1, "name": "Laptop", "price": 999.99, "available": true}'
./build/flexon insert products.fxdb --data '{"id": 2, "name": "Mouse", "price": 25.50, "available": false}'
./build/flexon insert products.fxdb --data '{"id": 3, "name": "Keyboard", "price": 75.00, "available": true}'
```

### Appending to Existing Database

```bash
# Create initial database
./build/flexon create users.fxdb --schema "username string, email string, age int32"
./build/flexon insert users.fxdb --data '{"username": "alice", "email": "alice@example.com", "age": 30}'

# Later, add more users (database automatically opened for appending)
./build/flexon insert users.fxdb --data '{"username": "bob", "email": "bob@example.com", "age": 25}'
./build/flexon insert users.fxdb --data '{"username": "carol", "email": "carol@example.com", "age": 35}'

# Read all users
./build/flexon read users.fxdb
```

### Database Information

```bash
# Show database schema and info
./build/flexon info employees.fxdb

# List all databases in current directory
./build/flexon list

# List databases in specific directory
./build/flexon list -d /path/to/databases
```

## CLI Examples

### Employee Management System

```bash
# Setup
./build/flexon create company.fxdb --schema "employee_id int32, name string, department string, hire_date string, salary float"

# Add employees
./build/flexon insert company.fxdb --data '{"employee_id": 1001, "name": "Alice Johnson", "department": "Engineering", "hire_date": "2023-01-15", "salary": 75000.0}'
./build/flexon insert company.fxdb --data '{"employee_id": 1002, "name": "Bob Smith", "department": "Marketing", "hire_date": "2023-02-01", "salary": 55000.0}'
./build/flexon insert company.fxdb --data '{"employee_id": 1003, "name": "Carol Davis", "department": "Engineering", "hire_date": "2023-01-20", "salary": 80000.0}'

# Generate reports
echo "=== All Employees ==="
./build/flexon read company.fxdb

echo "=== CSV Export for Payroll ==="
./build/flexon dump company.fxdb --format csv > payroll.csv

echo "=== JSON Export for API ==="
./build/flexon dump company.fxdb --format json > employees.json
```

### Inventory Management

```bash
# Create inventory database
./build/flexon create inventory.fxdb --schema "sku string, product_name string, quantity int32, unit_price float, in_stock bool"

# Add inventory items
./build/flexon insert inventory.fxdb --data '{"sku": "LAP001", "product_name": "Gaming Laptop", "quantity": 15, "unit_price": 1299.99, "in_stock": true}'
./build/flexon insert inventory.fxdb --data '{"sku": "MOU001", "product_name": "Wireless Mouse", "quantity": 50, "unit_price": 29.99, "in_stock": true}'
./build/flexon insert inventory.fxdb --data '{"sku": "KEY001", "product_name": "Mechanical Keyboard", "quantity": 0, "unit_price": 149.99, "in_stock": false}'

# Check inventory
./build/flexon read inventory.fxdb
```

### Log Analysis

```bash
# Create log database
./build/flexon create access_logs.fxdb --schema "timestamp string, ip_address string, status_code int32, response_time float"

# Add log entries
./build/flexon insert access_logs.fxdb --data '{"timestamp": "2023-12-01 10:30:15", "ip_address": "192.168.1.100", "status_code": 200, "response_time": 0.45}'
./build/flexon insert access_logs.fxdb --data '{"timestamp": "2023-12-01 10:30:16", "ip_address": "192.168.1.101", "status_code": 404, "response_time": 0.12}'
./build/flexon insert access_logs.fxdb --data '{"timestamp": "2023-12-01 10:30:17", "ip_address": "192.168.1.100", "status_code": 200, "response_time": 0.67}'

# Export logs for analysis
./build/flexon dump access_logs.fxdb --format csv > access_analysis.csv
```

## Interactive Shell Examples

### Database Exploration

```bash
# Start interactive shell
./build/flexon

# Commands in shell:
flexondb> show databases
flexondb> create test.fxdb schema="name string, score int32"
flexondb> use test.fxdb
flexondb:test> insert name="Player1" score=1000
flexondb:test> insert name="Player2" score=1500
flexondb:test> select *
flexondb:test> count
flexondb:test> export csv
flexondb:test> exit
```

### Data Entry Session

```bash
# Interactive data entry
./build/flexon

flexondb> create survey.fxdb schema="respondent_id int32, name string, age int32, satisfaction float"
flexondb> use survey.fxdb

# Add survey responses
flexondb:survey> insert respondent_id=1 name="John Doe" age=25 satisfaction=4.5
flexondb:survey> insert respondent_id=2 name="Jane Smith" age=30 satisfaction=3.8
flexondb:survey> insert respondent_id=3 name="Bob Johnson" age=45 satisfaction=4.2

# Review data
flexondb:survey> select *
flexondb:survey> export json
flexondb:survey> exit
```

### Database Management

```bash
./build/flexon

# Create multiple databases
flexondb> create customers.fxdb schema="id int32, name string, email string"
flexondb> create orders.fxdb schema="order_id int32, customer_id int32, amount float"
flexondb> create products.fxdb schema="product_id int32, name string, price float"

# Show all databases
flexondb> show databases

# Work with customers
flexondb> use customers.fxdb
flexondb:customers> insert id=1 name="Alice" email="alice@example.com"
flexondb:customers> insert id=2 name="Bob" email="bob@example.com"
flexondb:customers> select *

# Switch to orders
flexondb:customers> use orders.fxdb
flexondb:orders> insert order_id=101 customer_id=1 amount=299.99
flexondb:orders> insert order_id=102 customer_id=2 amount=149.50
flexondb:orders> select *

# Clean up
flexondb:orders> drop products.fxdb
flexondb:orders> exit
```

## Programming Examples

### C API Usage

```c
// complete_example.c
#include "flexondb.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Create customer database
    schema_t* schema = parse_schema("customer_id int32, name string, email string, active bool");
    if (!schema) {
        fprintf(stderr, "Failed to parse schema\n");
        return 1;
    }
    
    // Create database
    writer_t* writer = writer_create_default("customers.fxdb", schema);
    if (!writer) {
        fprintf(stderr, "Failed to create database\n");
        free_schema(schema);
        return 1;
    }
    
    // Insert customers
    const char* customers[] = {
        "{\"customer_id\": 1, \"name\": \"Alice Johnson\", \"email\": \"alice@example.com\", \"active\": true}",
        "{\"customer_id\": 2, \"name\": \"Bob Smith\", \"email\": \"bob@example.com\", \"active\": true}",
        "{\"customer_id\": 3, \"name\": \"Carol Davis\", \"email\": \"carol@example.com\", \"active\": false}"
    };
    
    for (int i = 0; i < 3; i++) {
        if (writer_insert_json(writer, customers[i]) != 0) {
            fprintf(stderr, "Failed to insert customer %d\n", i + 1);
        } else {
            printf("✓ Inserted customer %d\n", i + 1);
        }
    }
    
    // Close writer
    writer_close(writer);
    writer_free(writer);
    
    // Read data back
    reader_t* reader = reader_open("customers.fxdb");
    if (reader) {
        printf("\nCustomer Database (%u rows):\n", reader_get_row_count(reader));
        
        query_result_t* result = reader_read_rows(reader, 0);
        if (result) {
            reader_print_rows(reader, result);
            reader_free_result(result);
        }
        
        reader_close(reader);
    }
    
    free_schema(schema);
    return 0;
}
```

Compile and run:
```bash
gcc -Iinclude complete_example.c build/schema.o build/writer.o build/reader.o build/error.o build/utils.o build/io_utils.o build/logo.o build/welcome.o build/compat.o -o complete_example
./complete_example
```

### Batch Data Processing

```c
// batch_processor.c
#include "flexondb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int process_csv_to_flexondb(const char* csv_file, const char* db_file, const char* schema_str) {
    FILE* csv = fopen(csv_file, "r");
    if (!csv) {
        fprintf(stderr, "Cannot open CSV file: %s\n", csv_file);
        return -1;
    }
    
    schema_t* schema = parse_schema(schema_str);
    if (!schema) {
        fprintf(stderr, "Invalid schema: %s\n", schema_str);
        fclose(csv);
        return -1;
    }
    
    writer_t* writer = writer_create_default(db_file, schema);
    if (!writer) {
        fprintf(stderr, "Cannot create database: %s\n", db_file);
        free_schema(schema);
        fclose(csv);
        return -1;
    }
    
    char line[1024];
    int row_count = 0;
    
    // Skip CSV header
    fgets(line, sizeof(line), csv);
    
    // Process each line
    while (fgets(line, sizeof(line), csv)) {
        // Convert CSV line to JSON (simplified)
        char json[1024];
        char name[256], dept[256];
        int age;
        float salary;
        
        if (sscanf(line, "%255[^,],%255[^,],%d,%f", name, dept, &age, &salary) == 4) {
            snprintf(json, sizeof(json), 
                "{\"name\": \"%s\", \"department\": \"%s\", \"age\": %d, \"salary\": %.2f}",
                name, dept, age, salary);
            
            if (writer_insert_json(writer, json) == 0) {
                row_count++;
                if (row_count % 100 == 0) {
                    printf("Processed %d rows...\n", row_count);
                }
            }
        }
    }
    
    printf("✓ Processed %d rows from %s to %s\n", row_count, csv_file, db_file);
    
    writer_close(writer);
    writer_free(writer);
    free_schema(schema);
    fclose(csv);
    
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <csv_file> <db_file> <schema>\n", argv[0]);
        printf("Example: %s data.csv employees.fxdb \"name string, department string, age int32, salary float\"\n", argv[0]);
        return 1;
    }
    
    return process_csv_to_flexondb(argv[1], argv[2], argv[3]);
}
```

## Real-World Use Cases

### E-commerce Order Processing

```bash
# Create orders database
./build/flexon create orders.fxdb --schema "order_id int32, customer_email string, product_name string, quantity int32, total_amount float, order_date string"

# Process daily orders
./build/flexon insert orders.fxdb --data '{"order_id": 10001, "customer_email": "alice@example.com", "product_name": "Wireless Headphones", "quantity": 1, "total_amount": 129.99, "order_date": "2023-12-01"}'
./build/flexon insert orders.fxdb --data '{"order_id": 10002, "customer_email": "bob@example.com", "product_name": "Smartphone Case", "quantity": 2, "total_amount": 39.98, "order_date": "2023-12-01"}'

# Generate daily report
echo "=== Daily Sales Report ==="
./build/flexon dump orders.fxdb --format csv > daily_sales.csv
cat daily_sales.csv

# Monthly aggregation (export for spreadsheet analysis)
./build/flexon dump orders.fxdb --format json > monthly_orders.json
```

### IoT Sensor Data Collection

```bash
# Create sensor data database
./build/flexon create sensor_data.fxdb --schema "sensor_id string, timestamp string, temperature float, humidity float, battery_level int32"

# Collect sensor readings
./build/flexon insert sensor_data.fxdb --data '{"sensor_id": "TEMP001", "timestamp": "2023-12-01T10:00:00Z", "temperature": 22.5, "humidity": 45.2, "battery_level": 85}'
./build/flexon insert sensor_data.fxdb --data '{"sensor_id": "TEMP002", "timestamp": "2023-12-01T10:00:00Z", "temperature": 21.8, "humidity": 48.1, "battery_level": 92}'
./build/flexon insert sensor_data.fxdb --data '{"sensor_id": "TEMP001", "timestamp": "2023-12-01T10:05:00Z", "temperature": 22.7, "humidity": 44.8, "battery_level": 84}'

# Export for analysis
./build/flexon dump sensor_data.fxdb --format json > sensor_readings.json
```

### Log Aggregation

```bash
# Application logs
./build/flexon create app_logs.fxdb --schema "timestamp string, level string, component string, message string, user_id int32"

# Add log entries
./build/flexon insert app_logs.fxdb --data '{"timestamp": "2023-12-01T10:30:15", "level": "INFO", "component": "auth", "message": "User login successful", "user_id": 1001}'
./build/flexon insert app_logs.fxdb --data '{"timestamp": "2023-12-01T10:30:20", "level": "ERROR", "component": "payment", "message": "Payment processing failed", "user_id": 1002}'
./build/flexon insert app_logs.fxdb --data '{"timestamp": "2023-12-01T10:30:25", "level": "WARN", "component": "auth", "message": "Multiple login attempts", "user_id": 1003}'

# Export for log analysis tools
./build/flexon dump app_logs.fxdb --format json > application_logs.json
```

### Financial Transaction Records

```bash
# Transaction database
./build/flexon create transactions.fxdb --schema "transaction_id string, account_id int32, amount float, transaction_type string, timestamp string, approved bool"

# Record transactions
./build/flexon insert transactions.fxdb --data '{"transaction_id": "TXN001", "account_id": 12345, "amount": 250.00, "transaction_type": "credit", "timestamp": "2023-12-01T09:15:00Z", "approved": true}'
./build/flexon insert transactions.fxdb --data '{"transaction_id": "TXN002", "account_id": 12345, "amount": -50.00, "transaction_type": "debit", "timestamp": "2023-12-01T09:20:00Z", "approved": true}'
./build/flexon insert transactions.fxdb --data '{"transaction_id": "TXN003", "account_id": 67890, "amount": 1000.00, "transaction_type": "credit", "timestamp": "2023-12-01T09:25:00Z", "approved": false}'

# Generate transaction report
echo "=== Transaction Report ==="
./build/flexon read transactions.fxdb

# Export for accounting system
./build/flexon dump transactions.fxdb --format csv > transaction_export.csv
```

## Performance Tips

### Batch Operations

For large datasets, batch your insertions:

```bash
# Instead of many individual inserts, prepare data and insert in batches
# (This example shows the concept - actual batching would be done programmatically)

# Create large dataset
./build/flexon create large_dataset.fxdb --schema "id int32, data string, value float"

# Better: Insert multiple records in a loop or script
for i in {1..1000}; do
    ./build/flexon insert large_dataset.fxdb --data "{\"id\": $i, \"data\": \"Record $i\", \"value\": $((RANDOM % 1000)).5}"
done
```

### Memory Considerations

```bash
# For very large databases, read with limits
./build/flexon read huge_database.fxdb --limit 1000

# Export in chunks for processing
./build/flexon dump huge_database.fxdb --format csv > chunk1.csv
```

---

For more advanced examples and use cases, see the [C API documentation](API.md) and explore the `examples/` directory in the repository.