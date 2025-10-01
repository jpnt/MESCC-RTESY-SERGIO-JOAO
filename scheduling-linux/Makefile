CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGETS = sched_change cpu_affinity

all: $(TARGETS)

sched_change: sched_change.c
	$(CC) $(CFLAGS) -o $@ $<

cpu_affinity: cpu_affinity.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
