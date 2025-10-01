#!/bin/sh
# Test SCHED_DEADLINE scheduler
# runtime=10ms, deadline=30ms, period=30ms (in nanoseconds)
echo "Changing to SCHED_DEADLINE..."
sudo ./sched_change deadline 10000000 30000000 30000000
