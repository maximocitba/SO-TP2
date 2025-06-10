#ifndef _create_process_h
#define _create_process_h

#include "../include/definitions.h"
#include "linked_list.h"
#include <stdint.h>

typedef struct process_t {
    int32_t pid;
    priority_t priority;
    state_t state;
    void *stack_base;
    void *stack_pointer;
    char **argv;
    uint64_t argc;
    char *name;

    uint8_t unkilliable;
    int32_t parent_pid;
    int32_t exit_code;

    fg_bg_t fg;

    linked_list_adt children;

    basic_fd_t fds[3];
} process_t;

typedef int (*function)(int argc, char **args);

void process_handler(function code, char **argv, int argc);
void init_process(process_t *process, int32_t pid, function code, char **args, uint64_t argc, char *name, priority_t priority, uint8_t unkilliable);
extern void *create_process_stack_frame(void *rip, void *rsp, void *argv, uint64_t argc, void *process_handler);
void free_process(process_t *process);

process_snapshot_t *get_process_snapshot(uint32_t pid);

#endif