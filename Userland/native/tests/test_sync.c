// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/definitions.h"
#include "../include/stdio.h"
#include "../include/tests.h"
#include "syscall.h"
#include <stdint.h>
#include <syscalls.h>

#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 2

int global; // shared memory

void slow_inc(int *p, int inc) {
    int aux = *p;
    sys_yield(); // This makes the race condition highly probable
    aux += inc;
    *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
    uint64_t n;
    int sem;
    int8_t inc;
    int8_t use_sem;

    if (argc != 3)
        return -1;

    if ((n = satoi(argv[0])) <= 0)
        return -1;
    if ((inc = satoi(argv[1])) == 0)
        return -1;
    if ((use_sem = satoi(argv[2])) < 0)
        return -1;

    printf("Starting process with PID: %d\n", sys_get_pid());
    if (use_sem) {
        sem = sys_sem_open((int64_t)SEM_ID, 1);
        if (sem == -1) {
            printf("Error opening semaphore\n");
            return -1;
        }
    }

    uint64_t i;
    for (i = 0; i < n; i++) {
        if (use_sem) {

            sys_sem_wait(sem);
        }
        slow_inc(&global, inc);
        if (use_sem) {
            sys_sem_post(sem);
        }
    }

    if (use_sem)
        sys_sem_close(sem);


    printf("Process with PID: %d finished. Current global value: %d\n", sys_get_pid(), global);
    return 0;
}

uint64_t
test_sync(uint64_t argc, char *argv[]) {
    uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

    if (argc != 2)
        return -1;

    char *argv_dec[] = {argv[0], "-1", argv[1], NULL};
    char *argv_inc[] = {argv[0], "1", argv[1], NULL};

    global = 0;

    uint64_t i;
    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        pids[i] = sys_exec((void *)&my_process_inc, argv_dec, 3, "my_process_dec", low);
        pids[i + TOTAL_PAIR_PROCESSES] = sys_exec((void *)&my_process_inc, argv_inc, 3, "my_process_inc", low);
    }

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        sys_waitpid(pids[i]);
        sys_waitpid(pids[i + TOTAL_PAIR_PROCESSES]);
    }

    printf("Final value: ");

    printf("%d\n", (int)global);

    return 0;
}
