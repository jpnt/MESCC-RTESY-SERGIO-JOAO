#!/bin/sh
# Test SCHED_FIFO scheduler with priority 15
echo "Changing to SCHED_FIFO with priority 15..."
sudo ./sched_change fifo 15
