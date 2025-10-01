#!/bin/sh
# Test SCHED_RR scheduler with priority 30
echo "Changing to SCHED_RR with priority 30..."
sudo ./sched_change rr 30
