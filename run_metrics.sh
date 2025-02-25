#!/bin/bash
NUM_RUNS=1000

# Create a temporary file for each run's output.
tempfile=$(mktemp)

# Initialize accumulators.
total_load=0
total_throughput=0
total_turnaround_cpu=0
total_turnaround_io=0
total_turnaround_daemon=0

# Run the executable NUM_RUNS times.
for ((i=1; i<=NUM_RUNS; i++)); do
    ./a.out > "$tempfile"
    
    # Extract values using awk; adjust the patterns to match your output.
    load=$(awk -F": " '/Average System Load:/ {print $2}' "$tempfile")
    throughput=$(awk -F": " '/Throughput \(tasks finished per tick\):/ {print $2}' "$tempfile")
    turnaround_cpu=$(awk -F": " '/Average Turnaround Time for CPU-Intensive Tasks:/ {print $2}' "$tempfile")
    turnaround_io=$(awk -F": " '/Average Turnaround Time for I\/O-Intensive Tasks:/ {print $2}' "$tempfile")
    turnaround_daemon=$(awk -F": " '/Average Turnaround Time for Daemon Tasks:/ {print $2}' "$tempfile")
    
    # If any value is missing, print a warning and default to 0.
    if [[ -z "$load" ]]; then
        echo "Run $i: missing Average System Load. Defaulting to 0."
        load=0
    fi
    if [[ -z "$throughput" ]]; then
        echo "Run $i: missing Throughput. Defaulting to 0."
        throughput=0
    fi
    if [[ -z "$turnaround_cpu" ]]; then
        echo "Run $i: missing CPU-Intensive turnaround. Defaulting to 0."
        turnaround_cpu=0
    fi
    if [[ -z "$turnaround_io" ]]; then
        echo "Run $i: missing I/O-Intensive turnaround. Defaulting to 0."
        turnaround_io=0
    fi
    if [[ -z "$turnaround_daemon" ]]; then
        echo "Run $i: missing Daemon turnaround. Defaulting to 0."
        turnaround_daemon=0
    fi

    # Accumulate values using awk for floating-point arithmetic.
    total_load=$(awk -v tot="$total_load" -v val="$load" 'BEGIN {printf "%.6f", tot+val}')
    total_throughput=$(awk -v tot="$total_throughput" -v val="$throughput" 'BEGIN {printf "%.6f", tot+val}')
    total_turnaround_cpu=$(awk -v tot="$total_turnaround_cpu" -v val="$turnaround_cpu" 'BEGIN {printf "%.6f", tot+val}')
    total_turnaround_io=$(awk -v tot="$total_turnaround_io" -v val="$turnaround_io" 'BEGIN {printf "%.6f", tot+val}')
    total_turnaround_daemon=$(awk -v tot="$total_turnaround_daemon" -v val="$turnaround_daemon" 'BEGIN {printf "%.6f", tot+val}')
done

rm "$tempfile"

# Compute averages.
avg_load=$(awk -v tot="$total_load" -v runs="$NUM_RUNS" 'BEGIN {printf "%.6f", tot/runs}')
avg_throughput=$(awk -v tot="$total_throughput" -v runs="$NUM_RUNS" 'BEGIN {printf "%.6f", tot/runs}')
avg_turnaround_cpu=$(awk -v tot="$total_turnaround_cpu" -v runs="$NUM_RUNS" 'BEGIN {printf "%.6f", tot/runs}')
avg_turnaround_io=$(awk -v tot="$total_turnaround_io" -v runs="$NUM_RUNS" 'BEGIN {printf "%.6f", tot/runs}')
avg_turnaround_daemon=$(awk -v tot="$total_turnaround_daemon" -v runs="$NUM_RUNS" 'BEGIN {printf "%.6f", tot/runs}')

echo "Results over $NUM_RUNS runs:"
echo "Average System Load: $avg_load"
echo "Throughput (tasks finished per tick): $avg_throughput"
echo "Average Turnaround Time for CPU-Intensive Tasks: $avg_turnaround_cpu"
echo "Average Turnaround Time for I/O-Intensive Tasks: $avg_turnaround_io"
echo "Average Turnaround Time for Daemon Tasks: $avg_turnaround_daemon"

