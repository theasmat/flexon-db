# FlexonDB Test Data
# This directory contains test datasets for benchmarking and validation

## Files

- `small_dataset.json` - 5 records for basic testing
- `medium_dataset.json` - 100 records for moderate testing  
- `large_dataset.json` - 10,000 records for performance testing
- `stress_test.json` - 100,000 records for stress testing

## Schema

All datasets use the schema:
```
id int32, name string, age int32, salary float, active bool
```

## Usage

These files can be used with the FlexonDB import functionality:
```bash
flexon import test.fxdb --data tests/data/small_dataset.json
```