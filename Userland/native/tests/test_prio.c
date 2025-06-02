#include "../include/stdio.h"
#include "../include/tests.h"
#include "syscall.h"
#include <stdint.h>
#include <syscalls.h>

#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 10000000 // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 0 // TODO: Change as required
#define MEDIUM 1 // TODO: Change as required
#define HIGHEST 2 // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

static void endless_loop_print_wrap() {
    endless_loop_print(MINOR_WAIT);
}

void test_prio() {
    int64_t pids[TOTAL_PROCESSES];
    char *argv[] = {0};
    uint64_t i;

    for (i = 0; i < TOTAL_PROCESSES; i++)
        pids[i] = sys_exec((void *)&endless_loop_print_wrap, NULL, 0, "test", prio[i]);

    bussy_wait(WAIT);
    printf("\nCHANGING PRIORITIES...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        sys_nice(pids[i], prio[i]);

    bussy_wait(WAIT);
    printf("\nBLOCKING...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        sys_block(pids[i]);

    printf("CHANGING PRIORITIES WHILE BLOCKED...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        sys_nice(pids[i], MEDIUM);

    printf("UNBLOCKING...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        sys_unblock(pids[i]);

    bussy_wait(WAIT);
    printf("\nKILLING...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        sys_kill(pids[i]);
}