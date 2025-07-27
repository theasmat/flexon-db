#!/usr/bin/env python3
"""
FlexonDB Benchmark Report Generator
Combines hyperfine JSON outputs into a comprehensive performance report
"""

import json
import sys
import os
import glob
from datetime import datetime
from pathlib import Path

def load_json_file(filepath):
    """Load and parse a JSON file, return None on error"""
    try:
        with open(filepath, 'r') as f:
            return json.load(f)
    except (FileNotFoundError, json.JSONDecodeError) as e:
        print(f"Warning: Could not load {filepath}: {e}", file=sys.stderr)
        return None

def extract_hyperfine_stats(data):
    """Extract key statistics from hyperfine JSON output"""
    if not data or 'results' not in data:
        return None
    
    results = data['results'][0]  # Take first result
    return {
        'command': results.get('command', 'unknown'),
        'mean_ms': results.get('mean', 0) * 1000,  # Convert to ms
        'stddev_ms': results.get('stddev', 0) * 1000,
        'min_ms': results.get('min', 0) * 1000,
        'max_ms': results.get('max', 0) * 1000,
        'runs': len(results.get('times', [])),
        'parameters': results.get('parameters', {})
    }

def generate_combined_report(output_dir):
    """Generate a combined benchmark report from all JSON files in output_dir"""
    output_path = Path(output_dir)
    if not output_path.exists():
        print(f"Error: Output directory {output_dir} does not exist", file=sys.stderr)
        return None
    
    # Find all benchmark JSON files
    json_files = glob.glob(str(output_path / "benchmark_*.json"))
    
    if not json_files:
        print(f"Warning: No benchmark JSON files found in {output_dir}", file=sys.stderr)
        return None
    
    # Combine all results
    combined_report = {
        'metadata': {
            'generated_at': datetime.now().isoformat(),
            'output_directory': str(output_path),
            'total_files_processed': len(json_files),
            'flexondb_version': '1.0.0'
        },
        'benchmarks': {}
    }
    
    for json_file in sorted(json_files):
        filename = Path(json_file).name
        benchmark_type = filename.split('_')[1]  # extract 'create', 'insert', etc.
        
        data = load_json_file(json_file)
        if data:
            stats = extract_hyperfine_stats(data)
            if stats:
                combined_report['benchmarks'][benchmark_type] = {
                    'source_file': filename,
                    'statistics': stats,
                    'performance_category': categorize_performance(stats['mean_ms'])
                }
    
    # Add summary statistics
    if combined_report['benchmarks']:
        combined_report['summary'] = generate_summary(combined_report['benchmarks'])
    
    return combined_report

def categorize_performance(mean_ms):
    """Categorize performance based on mean execution time"""
    if mean_ms < 1:
        return 'excellent'
    elif mean_ms < 10:
        return 'good'
    elif mean_ms < 100:
        return 'acceptable'
    elif mean_ms < 1000:
        return 'slow'
    else:
        return 'very_slow'

def generate_summary(benchmarks):
    """Generate summary statistics across all benchmarks"""
    if not benchmarks:
        return {}
    
    all_means = [b['statistics']['mean_ms'] for b in benchmarks.values()]
    all_runs = [b['statistics']['runs'] for b in benchmarks.values()]
    
    return {
        'total_benchmarks': len(benchmarks),
        'total_test_runs': sum(all_runs),
        'fastest_operation': min(all_means),
        'slowest_operation': max(all_means),
        'average_time_ms': sum(all_means) / len(all_means),
        'performance_distribution': {
            'excellent': len([m for m in all_means if m < 1]),
            'good': len([m for m in all_means if 1 <= m < 10]),
            'acceptable': len([m for m in all_means if 10 <= m < 100]),
            'slow': len([m for m in all_means if 100 <= m < 1000]),
            'very_slow': len([m for m in all_means if m >= 1000])
        }
    }

def main():
    if len(sys.argv) != 2:
        print("Usage: python3 generate_report.py <output_directory>", file=sys.stderr)
        sys.exit(1)
    
    output_dir = sys.argv[1]
    report = generate_combined_report(output_dir)
    
    if report:
        print(json.dumps(report, indent=2))
    else:
        print("Error: Could not generate report", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()