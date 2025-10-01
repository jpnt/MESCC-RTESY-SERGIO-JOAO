#!/bin/bash
# Test CPU affinity - parent and child on SAME core
echo "Testing parent and child on same core (CPU 1)..."
set -x
time -p ./cpu_affinity 1 1 100000000
