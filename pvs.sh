#!/bin/bash

# PVS-Studio Static Analysis Automation Script
# This script automates the process of running PVS-Studio analysis on the SO-TP2 project

set -e  # Exit on any error

echo "=========================================="
echo "PVS-Studio Static Analysis for SO-TP2"
echo "=========================================="

# Check if PVS-Studio is installed
if ! command -v pvs-studio-analyzer &> /dev/null; then
    echo "Error: PVS-Studio analyzer not found. Please install PVS-Studio first."
    exit 1
fi

if ! command -v plog-converter &> /dev/null; then
    echo "Error: plog-converter not found. Please install PVS-Studio first."
    exit 1
fi

# Activate PVS-Studio free license
echo "Activating PVS-Studio free license..."
pvs-studio-analyzer credentials "PVS-Studio Free" "FREE-FREE-FREE-FREE"

# Clean previous analysis files
echo "Cleaning previous analysis files..."
rm -f strace_out PVS-Studio.log report.tasks
rm -f Userland/strace_out

# Clean and rebuild the project to ensure fresh compilation
echo "Cleaning project..."
make clean

echo "Starting PVS-Studio trace analysis..."
echo "Running: pvs-studio-analyzer trace -- make"
pvs-studio-analyzer trace -- make

echo "Analyzing traced compilation..."
echo "Running: pvs-studio-analyzer analyze"
pvs-studio-analyzer analyze

# Check if log file was created
if [ ! -f "PVS-Studio.log" ]; then
    echo "Error: PVS-Studio.log was not created. Analysis may have failed."
    exit 1
fi

echo "Converting analysis results to readable format..."
echo "Running: plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log"
plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log

# Check if report was created successfully
if [ -f "report.tasks" ]; then
    echo "=========================================="
    echo "Analysis completed successfully!"
    echo "Results saved to: report.tasks"
    echo "=========================================="
    
    # Show a summary of findings
    if [ -s "report.tasks" ]; then
        echo "Analysis Summary:"
        echo "Total issues found: $(wc -l < report.tasks)"
        echo ""
        echo "First 10 issues (if any):"
        head -10 report.tasks 2>/dev/null || echo "No issues found or report is empty"
    else
        echo "No issues found - report file is empty."
    fi
else
    echo "Error: Failed to generate report.tasks file"
    exit 1
fi

echo ""
echo "Files generated:"
echo "- PVS-Studio.log (raw analysis log)"
echo "- report.tasks (human-readable report)"
echo "- strace_out (trace file)"

echo ""
echo "To view the full report, run: cat report.tasks"
echo "To view specific issue types, you can filter the report.tasks file"