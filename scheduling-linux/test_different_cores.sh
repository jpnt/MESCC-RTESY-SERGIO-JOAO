#!/bin/bash
# Test CPU affinity - parent and child on DIFFERENT cores
echo "Testing parent and child on different cores (CPU 2 and 3)..."
set -x
time -p ./cpu_affinity 2 3 100000000
