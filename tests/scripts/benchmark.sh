#!/bin/bash
set -e

# FlexonDB Hyperfine Benchmark Script
# Uses hyperfine for accurate performance measurements with JSON output

BINARY="../../build/dist/linux/bin/flexon"
OUTPUT_DIR="../output"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

echo "🚀 FlexonDB Performance Benchmarking"
echo "===================================="
echo "Binary: $BINARY"
echo "Output: $OUTPUT_DIR"
echo "Timestamp: $TIMESTAMP"
echo ""

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Check if hyperfine is available, install if needed
if ! command -v hyperfine &> /dev/null; then
    echo "⚠️  Hyperfine not found. Install with:"
    echo "   cargo install hyperfine"
    echo "   # or on Ubuntu: sudo apt install hyperfine"
    echo ""
    echo "🔄 Falling back to manual benchmarks..."
    
    # Fallback manual benchmarks
    echo "📊 Manual benchmark: Database creation..."
    time $BINARY create test_bench.fxdb --schema 'id int32, name string, score float' 2>&1
    
    echo ""
    echo "📈 Manual benchmark: Data insertion..."  
    for i in {1..100}; do
        $BINARY insert test_bench.fxdb --data "{\"id\": $i, \"name\": \"User$i\", \"score\": 95.5}" >/dev/null 2>&1
    done
    echo "Inserted 100 records"
    
    echo ""
    echo "📖 Manual benchmark: Data reading..."
    time $BINARY read test_bench.fxdb --limit 50 >/dev/null 2>&1
    
    rm -f test_bench.fxdb
    echo "✅ Manual benchmarks complete!"
    exit 0
fi

# Hyperfine is available, run proper benchmarks
echo "✅ Hyperfine found, running comprehensive benchmarks..."
echo ""

# Database creation benchmarks
echo "📊 Benchmarking database creation..."
hyperfine \
    --export-json "$OUTPUT_DIR/benchmark_create_$TIMESTAMP.json" \
    --warmup 3 \
    --runs 50 \
    --prepare "rm -f test_bench.fxdb" \
    "$BINARY create test_bench.fxdb --schema 'id int32, name string, score float'"

# Insertion benchmarks
echo ""
echo "📈 Benchmarking data insertion..."
hyperfine \
    --export-json "$OUTPUT_DIR/benchmark_insert_$TIMESTAMP.json" \
    --warmup 3 \
    --runs 100 \
    --setup "$BINARY create test_insert.fxdb --schema 'id int32, name string, score float'" \
    --cleanup "rm -f test_insert.fxdb" \
    "$BINARY insert test_insert.fxdb --data '{\"id\": 1, \"name\": \"test\", \"score\": 95.5}'"

# Read benchmarks (requires setup script)
echo ""
echo "📖 Benchmarking data reading..."

# Create setup script for large database
cat > /tmp/setup_large_db.sh << 'EOF'
#!/bin/bash
rm -f large_test.fxdb
../build/dist/linux/bin/flexon create large_test.fxdb --schema 'id int32, name string, score float, active bool'
for i in {1..1000}; do
    ../build/dist/linux/bin/flexon insert large_test.fxdb --data "{\"id\": $i, \"name\": \"User$i\", \"score\": 95.5, \"active\": true}" >/dev/null
done
EOF
chmod +x /tmp/setup_large_db.sh

hyperfine \
    --export-json "$OUTPUT_DIR/benchmark_read_$TIMESTAMP.json" \
    --warmup 2 \
    --runs 50 \
    --setup "/tmp/setup_large_db.sh" \
    --cleanup "rm -f large_test.fxdb" \
    "$BINARY read large_test.fxdb --limit 100"

# Export format benchmarks
echo ""
echo "📤 Benchmarking export formats..."
hyperfine \
    --export-json "$OUTPUT_DIR/benchmark_export_$TIMESTAMP.json" \
    --warmup 2 \
    --runs 30 \
    --setup "/tmp/setup_large_db.sh" \
    --cleanup "rm -f large_test.fxdb export_test.*" \
    "$BINARY dump large_test.fxdb --format csv" \
    "$BINARY dump large_test.fxdb --format json" \
    "$BINARY dump large_test.fxdb --format table"

# Cleanup
rm -f /tmp/setup_large_db.sh

# Check if Python is available for report generation
if command -v python3 &> /dev/null; then
    echo ""
    echo "📋 Generating combined report..."
    python3 scripts/generate_report.py "$OUTPUT_DIR" > "$OUTPUT_DIR/combined_benchmark_$TIMESTAMP.json"
    echo "📊 Report saved to: $OUTPUT_DIR/combined_benchmark_$TIMESTAMP.json"
else
    echo ""
    echo "⚠️  Python3 not available for report generation"
fi

echo ""
echo "✅ Benchmarking complete! Results in $OUTPUT_DIR/"
echo "📁 Files created:"
ls -la "$OUTPUT_DIR/"*"$TIMESTAMP"* 2>/dev/null || echo "   (No files with timestamp found)"