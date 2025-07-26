#!/bin/bash

echo "🚀 FlexonDB Advanced CLI Test Script"
echo "====================================="

# Clean up previous tests
rm -rf test_databases
rm -f *.fxdb

echo ""
echo "Building FlexonDB..."
make clean && make all

if [ $? -ne 0 ]; then
    echo "❌ Build failed!"
    exit 1
fi

echo ""
echo "✅ Build successful!"

echo ""
echo "=== Testing CLI Tool with Directory Support ==="

# Test 1: Create database in current directory
echo ""
echo "1️⃣  Creating database in current directory..."
./build/flexon create employees.fxdb --schema "name string, age int32, salary float, active bool"

# Test 2: Create database in specific directory
echo ""
echo "2️⃣  Creating database in specific directory..."
./build/flexon create products.fxdb --schema "id int32, name string, price float" -d test_databases

# Test 3: List files in current directory
echo ""
echo "3️⃣  Listing databases in current directory..."
./build/flexon list

# Test 4: List files in specific directory
echo ""
echo "4️⃣  Listing databases in test_databases directory..."
./build/flexon list -d test_databases

# Test 5: Create test data
echo ""
echo "5️⃣  Creating test data..."
./build/test_writer

# Test 6: Info from current directory
echo ""
echo "6️⃣  Database info (current directory)..."
./build/flexon info test.fxdb

# Test 7: Info from specific directory
echo ""
echo "7️⃣  Database info (specific directory)..."
./build/flexon info products.fxdb -d test_databases

# Test 8: Read from current directory
echo ""
echo "8️⃣  Reading data (current directory)..."
./build/flexon read test.fxdb --limit 2

# Test 9: Read from specific directory with limit
echo ""
echo "9️⃣  Reading data (specific directory)..."
./build/flexon info products.fxdb -d test_databases

# Test 10: Error handling - non-existent directory
echo ""
echo "🔟 Testing error handling..."
./build/flexon list -d /non/existent/directory

echo ""
echo "=== All Tests Complete! ==="
echo ""
echo "Files created:"
ls -la *.fxdb 2>/dev/null || echo "No .fxdb files in current directory"
echo ""
echo "Directory contents:"
ls -la test_databases/ 2>/dev/null || echo "test_databases directory not found"

echo "🎉 Test script finished!"