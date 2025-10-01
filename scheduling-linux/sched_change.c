/* Change process scheduler
 *
 * Usage:
 *   sudo ./sched_change <policy> <priority>
 *   sudo ./sched_change deadline <runtime> <deadline> <period>
 *
 * Examples:
 *   sudo ./sched_change fifo 15
 *   sudo ./sched_change rr 30
 *   sudo ./sched_change deadline 10000000 30000000 30000000
 *
 * Policies: fifo, rr, deadline
 * Priority range: 1-99 (1=lowest, 99=highest)
 * Deadline times in nanoseconds
 */

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/types.h>

/* SCHED_DEADLINE support via sched_setattr syscall */
#ifndef SCHED_DEADLINE
#define SCHED_DEADLINE 6
#endif

struct sched_attr {
    __u32 size;
    __u32 sched_policy;
    __u64 sched_flags;
    __s32 sched_nice;
    __u32 sched_priority;
    __u64 sched_runtime;
    __u64 sched_deadline;
    __u64 sched_period;
};

int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags) {
    return syscall(__NR_sched_setattr, pid, attr, flags);
}

const char* policy_name(int policy) {
    switch(policy) {
        case SCHED_OTHER: return "SCHED_OTHER";
        case SCHED_FIFO:  return "SCHED_FIFO";
        case SCHED_RR:    return "SCHED_RR";
        case SCHED_DEADLINE: return "SCHED_DEADLINE";
        default: return "UNKNOWN";
    }
}

void show_scheduler() {
    pid_t pid = getpid();
    int policy = sched_getscheduler(pid);
    struct sched_param param;
    sched_getparam(pid, &param);

    printf("PID %d: %s priority %d\n", pid, policy_name(policy), param.sched_priority);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s <policy> <priority>\n", argv[0]);
        fprintf(stderr, "  %s deadline <runtime> <deadline> <period>\n", argv[0]);
        fprintf(stderr, "Policies: fifo, rr, deadline\n");
        return 1;
    }

    printf("Before: ");
    show_scheduler();

    /* SCHED_DEADLINE uses different API */
    if (strcmp(argv[1], "deadline") == 0) {
        if (argc != 5) {
            fprintf(stderr, "deadline needs: <runtime> <deadline> <period> (nanoseconds)\n");
            return 1;
        }

        struct sched_attr attr = {0};
        attr.size = sizeof(attr);
        attr.sched_policy = SCHED_DEADLINE;
        attr.sched_runtime = atoll(argv[2]);
        attr.sched_deadline = atoll(argv[3]);
        attr.sched_period = atoll(argv[4]);

        if (sched_setattr(0, &attr, 0) == -1) {
            perror("sched_setattr");
            fprintf(stderr, "Need root: sudo %s deadline ...\n", argv[0]);
            return 1;
        }

        printf("After:  ");
        show_scheduler();
        printf("\nRunning. Press Ctrl+C to exit.\n");

        while(1) {
            sleep(1);
        }
        return 0;
    }

    /* SCHED_FIFO and SCHED_RR */
    int policy;
    if (strcmp(argv[1], "fifo") == 0) {
        policy = SCHED_FIFO;
    } else if (strcmp(argv[1], "rr") == 0) {
        policy = SCHED_RR;
    } else {
        fprintf(stderr, "Unknown policy '%s'. Use: fifo, rr, deadline\n", argv[1]);
        return 1;
    }

    int priority = atoi(argv[2]);

    /* Validate priority range */
    int min = sched_get_priority_min(policy);
    int max = sched_get_priority_max(policy);

    if (priority < min || priority > max) {
        fprintf(stderr, "Priority must be between %d and %d\n", min, max);
        return 1;
    }

    /* Change scheduler */
    struct sched_param param;
    param.sched_priority = priority;

    if (sched_setscheduler(0, policy, &param) == -1) {
        perror("sched_setscheduler");
        fprintf(stderr, "Need root: sudo %s %s %d\n", argv[0], argv[1], priority);
        return 1;
    }

    printf("After:  ");
    show_scheduler();
    printf("\nRunning. Press Ctrl+C to exit.\n");

    while(1) {
        sleep(1);
    }

    return 0;
}
