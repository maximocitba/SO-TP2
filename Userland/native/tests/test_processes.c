#include "../include/stdio.h"
#include "../include/tests.h"
#include "syscall.h"
#include <stdint.h>
#include <syscalls.h>
#include "../include/definitions.h"

enum State { RUNNING,
    BLOCKED,
    KILLED };

typedef struct P_rq {
    int32_t pid;
    enum State state;
} p_rq;

static void endless_loop_wrap() {
    printf("[endless_loop_wrap] process with pid: %d started\n", sys_get_pid());
    endless_loop();
}

int64_t test_processes(uint64_t argc, char *argv[]) {
    uint8_t rq;
    int alive = 0;
    uint8_t action;
    uint64_t max_processes;
    //char *argv_aux[] = {0};

    if (argc <= 0)
        return -1;

    max_processes = argc;

    p_rq p_rqs[max_processes];

    int wh = 1;
    while (wh--) {
        // Create max_processes processes
        for (rq = 0; rq < max_processes; rq++) {
            p_rqs[rq].pid = sys_exec((void *)&endless_loop_wrap, NULL, 0, "test", low_medium);

            if (p_rqs[rq].pid == -1) {
                printf("test_processes: ERROR creating process\n");
                return -1;
            } else {
                p_rqs[rq].state = RUNNING;
                
                // printf("soy un proceso de pid: %d\n", p_rqs[rq].pid);
                alive++;
            }
        }
        printf("[test_processes] created %d processes\n", max_processes);
        
        // Randomly kills, blocks or unblocks processes until every one has been killed
        while (alive > 0) {

            for (rq = 0; rq < max_processes; rq++) {
                action = GetUniform(100) % 2;
                switch (action) {
                case 0:
                    if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
                        
                        if (sys_kill(p_rqs[rq].pid) == -1) {
                            printf("test_processes: ERROR killing process of pid: %d\n", p_rqs[rq].pid);
                            return -1;
                        }
                        // printf("No di error, voy a matar el proceso\n");
                        p_rqs[rq].state = KILLED;
                        alive--;
                        printf("[test_processes] killed process of pid: %d\n", p_rqs[rq].pid);
                    }
                    break;

                case 1:
                    if (p_rqs[rq].state == RUNNING) {
                        if (sys_block(p_rqs[rq].pid) == -1) {
                            printf("test_processes: ERROR blocking process of pid: %d\n", p_rqs[rq].pid);
                            return -1;
                        }
                        p_rqs[rq].state = BLOCKED;
                    }
                    break;
                }
            }

            // Randomly unblocks processes
            for (rq = 0; rq < max_processes; rq++)
                if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
                    if (sys_unblock(p_rqs[rq].pid) == -1) {
                        printf("test_processes: ERROR unblocking process\n");
                        return -1;
                    }
                    p_rqs[rq].state = RUNNING;
                }
        }

        printf("[test_processes] all processes have been killed correctly\n");
    }
    return 0;
}