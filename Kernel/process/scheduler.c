// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/scheduler.h"
#include "../include/linked_list.h"
#include "../include/queue_pid.h"

#include <IO.h>
#include <lib.h>
#include <math.h>
#include <memman.h>
#include <process.h>
#include <semaphore.h>
#include <string.h>

#define idle_pid 0
#define default_quantum 5

#define capped_priority(prio) ((prio) >= MAX_PRIORITY ? MAX_PRIORITY : (prio))

uint8_t sig_fg_kill = 0;
uint8_t idle_rotation = 0;

typedef struct scheduler_cdt {
    node_t *processes[MAX_PROCESSES];
    linked_list_adt blocked_process_list;
    linked_list_adt process_list;
    uint16_t bg_process_list[MAX_PROCESSES];
    int current_pid;
    uint16_t next_unused_pid;
    uint16_t remaining_processes;
    int32_t current_quantum;
    uint8_t num_bg_processes;
} scheduler_cdt;

scheduler_adt init_scheduler() {
    scheduler_adt scheduler = (scheduler_adt)SCHEDULER_ADDRESS;

    scheduler->process_list = create_linked_list();
    scheduler->blocked_process_list = create_linked_list();

    for (int i = 0; i < MAX_PROCESSES; i++) {
        scheduler->processes[i] = NULL;
        scheduler->bg_process_list[i] = foreground;
    }

    scheduler->remaining_processes = 0;
    scheduler->next_unused_pid = idle_pid;
    scheduler->current_pid = -1;
    scheduler->current_quantum = default_quantum;

    sig_fg_kill = 0;

    return scheduler;
}

scheduler_adt get_scheduler_adt() {
    return (scheduler_adt)SCHEDULER_ADDRESS;
}

int32_t get_next_ready_pid() {

    scheduler_adt scheduler = get_scheduler_adt();

    if (scheduler->remaining_processes <= 1) {
        return idle_pid;
    }

    if (scheduler->num_bg_processes < scheduler->remaining_processes && 
        scheduler->remaining_processes - scheduler->num_bg_processes <= 1) {
        idle_rotation = (idle_rotation + 1) % 2;
        if (idle_rotation == 0) {
            return idle_pid;
        }
    }

    start_iterator(scheduler->process_list);
    if (has_next(scheduler->process_list)) {
        process_t *process = (process_t *)get_next(scheduler->process_list);
        return process->pid;
    }

    return idle_pid;
}

void *scheduler(void *stack_pointer) {
    scheduler_adt scheduler = get_scheduler_adt();

    process_t *current_process = NULL;
    process_t *next_process = NULL;

    if (scheduler->current_pid != -1) {
        current_process = (process_t *)scheduler->processes[scheduler->current_pid]->process;
        current_process->stack_pointer = stack_pointer;
    }

    scheduler->current_quantum--;

    if (scheduler->current_quantum > 0 &&
        current_process != NULL &&
        (current_process->state == running || current_process->state == ready)) {
        return stack_pointer;
    }

    if (current_process == NULL) {
        int32_t next_pid = get_next_ready_pid();
        scheduler->current_pid = next_pid;
        next_process = (process_t *)scheduler->processes[next_pid]->process;
        next_process->state = running;
        scheduler->current_quantum = default_quantum;
        return next_process->stack_pointer;
    }

    switch (current_process->state) {
    case running:
        if (current_process->pid != idle_pid) {
            current_process->state = ready;
            swap_to_last(scheduler->process_list, current_process);
        } else {

            current_process->state = ready;
        }
        break;
    case killed:

        break;
    case blocked:
        break;
    case waiting_for_child:
        start_iterator(current_process->children);
        while (has_next(current_process->children)) {
            process_t *child = (process_t *)get_next(current_process->children);
            if (child == NULL || child->parent_pid != current_process->pid) {
                remove_node(current_process->children, child);
                start_iterator(current_process->children);
            }
        }
        if (is_empty_list(current_process->children)) {
            current_process->state = ready;
        }
        for (int i = 0; i < current_process->priority; i++) {
            swap_to_last(scheduler->process_list, current_process);
        }
        break;
    default:
        break;
    }

    int32_t next_pid = get_next_ready_pid();

    scheduler->current_pid = next_pid;
    next_process = (process_t *)scheduler->processes[next_pid]->process;
    next_process->state = running;

    scheduler->current_quantum = default_quantum;

    return next_process->stack_pointer;
}

static uint32_t get_next_unused_pid() {
    scheduler_adt scheduler = get_scheduler_adt();

    if (scheduler->remaining_processes >= MAX_PROCESSES) {
        printf("max processes reached\n");
        return -1;
    }

    int i = idle_pid + 1;
    while (scheduler->processes[i] != NULL) {
        i++;
    }
    return i;
}

int32_t create_process(function code, char **args, int argc, char *name, uint8_t priority, uint8_t unkilliable) {

    priority = capped_priority(priority);
    scheduler_adt scheduler = get_scheduler_adt();
    if (scheduler->remaining_processes >= MAX_PROCESSES) {
        printf("max processes reached\n");
        return -1;
    }

    process_t *process = (process_t *)b_alloc(sizeof(process_t));
    if (process == NULL) {
        printf("error creating process\n");
        return -1;
    }

    init_process(process, scheduler->next_unused_pid, code, args, argc, name, priority, unkilliable);

    if (scheduler->current_pid != -1 && process->pid != idle_pid) {
        process->parent_pid = scheduler->current_pid;
    } else {
        process->parent_pid = idle_pid;
    }

    node_t *process_node = b_alloc(sizeof(node_t));
    if (process_node == NULL) {
        printf("error creating process node\n");
        b_free(process);
        return -1;
    }

    process_node->process = (void *)process;
    if (process->pid != idle_pid) {
        for (int i = 0; i < process->priority; i++) {
            add_node(scheduler->process_list, (void *)process);
        }
    }

    scheduler->processes[process->pid] = process_node;
    scheduler->next_unused_pid = get_next_unused_pid();
    scheduler->remaining_processes++;

    return process->pid;
}


void waitpid(uint32_t child_pid) {
    process_t *parent = get_current_process();
    process_t *child = get_process_by_pid(child_pid);

    if (child == NULL || child->parent_pid != parent->pid) {
        return;
    }

    if (child->state == killed) {
        remove_node(parent->children, child);
        return;
    }

    if (get_node(parent->children, child) == NULL) {
        add_node(parent->children, child);
    }

    parent->state = waiting_for_child;

    while (1) {
        child = get_process_by_pid(child_pid);
        if (child == NULL || child->state == killed) break;
        yield();  
    }

    remove_node(parent->children, child);
    parent->state = ready;
}


process_t *get_current_process() {
    scheduler_adt scheduler = get_scheduler_adt();
    return (process_t *)scheduler->processes[scheduler->current_pid]->process;
}

void process_priority(uint64_t pid, uint8_t new_prio) {
    scheduler_adt scheduler = get_scheduler_adt();

    new_prio = capped_priority(new_prio);

    process_t *current_process = (process_t *)scheduler->processes[pid]->process;
    int priority_delta = new_prio - current_process->priority;

    if (priority_delta == 0) {
        for (int i = 0; i < current_process->priority; i++) {

            if (current_process->state != ready && current_process->state != running)
                add_node(scheduler->process_list, (void *)current_process);
        }
    }

    if (priority_delta > 0) {
        for (int i = 0; i < priority_delta; i++) {
            add_node(scheduler->process_list, (void *)current_process);
        }
    } else {
        for (int i = 0; i < abs(priority_delta); i++) {
            remove_node(scheduler->process_list, (void *)current_process);
        }
    }

    current_process->priority = new_prio;
}

uint8_t last_child(uint32_t parent_pid) {
    scheduler_adt scheduler = get_scheduler_adt();
    process_t *parent = (process_t *)scheduler->processes[parent_pid]->process;
    if (parent == NULL) {
        return 0;
    }

    return is_empty_list(parent->children);
}

int kill_process(uint32_t pid) {
    scheduler_adt scheduler = get_scheduler_adt();
    if (pid == idle_pid) {
        printf("\n_cannot kill idle process\n");
        return -1;
    }

    if (scheduler->bg_process_list[pid] == background) {
        scheduler->num_bg_processes--;
    }
    scheduler->bg_process_list[pid] = foreground;

    if (scheduler->processes[pid] == NULL) {
        printf("process not found\n");
        return -1;
    }

    process_t *process_to_kill = (process_t *)scheduler->processes[pid]->process;
    if (process_to_kill->unkilliable) {
        printf("failed to kill process. process is unkilliable\n");
        return -1;
    }


    if (process_to_kill->state == blocked) {
        remove_all_nodes(scheduler->blocked_process_list, (void *)process_to_kill);
    } else {
        remove_all_nodes(scheduler->process_list, (void *)process_to_kill);
    }


    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (scheduler->processes[i] != NULL) {
            process_t *process = (process_t *)scheduler->processes[i]->process;
            if (process->parent_pid == pid) {
                process->parent_pid = idle_pid;
                if (process->state == running || process->state == ready) {
                    block_process(process->pid);
                    unblock_process(process->pid);
                }
            }
        }
    }

    // Marcar como killed y liberar
    scheduler->next_unused_pid = pid;
    process_to_kill->state = killed;
    free_process(process_to_kill);
    scheduler->remaining_processes--;

    // Notificar al padre si estaba esperando
    if (process_to_kill->parent_pid != idle_pid) {
        process_t *parent = get_process_by_pid(process_to_kill->parent_pid);
        if (parent != NULL) {
            remove_node(parent->children, process_to_kill);
            if (parent->state == waiting_for_child && is_empty_list(parent->children)) {
                unblock_process(parent->pid);
                parent->state = ready;
            }
        }
    }

    yield();

    scheduler->processes[pid]->process = NULL;
    scheduler->processes[pid] = NULL;
    return 0;
}

void kill_current_process() {
    scheduler_adt scheduler = get_scheduler_adt();
    kill_process(scheduler->current_pid);
}

void yield() {
    scheduler_adt scheduler = get_scheduler_adt();
    scheduler->current_quantum = 0;
    asm_do_timer_tick();
}

int block_process(uint64_t pid) {
    scheduler_adt scheduler = get_scheduler_adt();
    if (scheduler->processes[pid] == NULL || pid == idle_pid) {
        return -1;
    }

    process_t *process_to_block = (process_t *)scheduler->processes[pid]->process;
    process_to_block->state = blocked;
    remove_all_nodes(scheduler->blocked_process_list, process_to_block);
    add_node(scheduler->blocked_process_list, process_to_block);
    remove_all_nodes(scheduler->process_list, process_to_block);

    return 0;
}

int unblock_process(uint64_t pid) {
    scheduler_adt scheduler = get_scheduler_adt();
    if (scheduler->processes[pid] == NULL) {
        return -1;
    }

    process_t *process_to_unblock = (process_t *)scheduler->processes[pid]->process;
    process_to_unblock->state = ready;
    remove_all_nodes(scheduler->blocked_process_list, process_to_unblock);
    for (int i = 0; i < process_to_unblock->priority; i++) {
        add_node(scheduler->process_list, process_to_unblock);
    }
    return 0;
}

int get_current_pid() {
    scheduler_adt scheduler = get_scheduler_adt();
    return scheduler->current_pid;
}

process_t *get_process_by_pid(uint32_t pid) {
    scheduler_adt scheduler = get_scheduler_adt();
    if (scheduler->processes[pid] == NULL) {
        return NULL;
    }

    return (process_t *)scheduler->processes[pid]->process;
}

int16_t get_current_process_file_descriptor(uint8_t fd_index) {
    scheduler_adt scheduler = get_scheduler_adt();
    process_t *process = scheduler->processes[scheduler->current_pid]->process;
    return process->fds[fd_index];
}

uint16_t change_process_fd(uint32_t pid, uint16_t fd_index, int16_t new_fd) {
    scheduler_adt scheduler = get_scheduler_adt();
    process_t *process = (process_t *)scheduler->processes[pid]->process;
    if (process == NULL) {
        return -1;
    }
    process->fds[fd_index] = new_fd;
    return 0;
}

void kill_current_ppid() {
    process_t *process = get_current_process();
    if (process->parent_pid != idle_pid) {
        kill_process(process->parent_pid);
    } else {
        kill_current_process();
    }
}

void kill_fg_process() {
    process_t *current = get_current_process();

    if (current == NULL || current->pid == idle_pid) {
        return;
    }

    if (current->fds[STDIN] == STDIN) {
        printf("killing current process\n");
        if (current->parent_pid != idle_pid) {
            kill_process(current->parent_pid);
            print_number(current->parent_pid);
        } else {
            print_number(current->pid);
            kill_process(current->pid);
        }
    }
}

void set_bg_process(uint32_t pid) {
    scheduler_adt scheduler = get_scheduler_adt();
    ((process_t *)scheduler->processes[pid]->process)->fg = background;
    scheduler->bg_process_list[pid] = background;
    scheduler->num_bg_processes++;
}

int get_all_processes_info(ps_info_t *buffer, int max_len) {
    scheduler_adt scheduler = get_scheduler_adt();
    int count = 0;

    for (int i = 0; i < MAX_PROCESSES && count < max_len; i++) {
        if (scheduler->processes[i] == NULL || scheduler->processes[i]->process == NULL) {
            continue;
        }

        process_t *p = (process_t *)scheduler->processes[i]->process;

        if (p->state == killed || p->pid > 100) {
            continue;
        }

        buffer[count].pid = p->pid;
        buffer[count].priority = p->priority;
        buffer[count].state = p->state;
        memcpy(buffer[count].name, p->name, strlen(p->name) + 1);
        pointer_to_string(p->stack_pointer, buffer[count].stack_pointer_str, MAX_PTR_STR_LEN);
        pointer_to_string(p->stack_base, buffer[count].base_pointer_str, MAX_PTR_STR_LEN);
        buffer[count].fg = p->fg;
        buffer[count].parent_pid = p->parent_pid;

        count++;
    }

    return count;
}


int toggle_process_block_state(uint32_t pid) {
    scheduler_adt scheduler = get_scheduler_adt();
    if (pid == idle_pid || scheduler->processes[pid] == NULL || scheduler->processes[pid]->process == NULL) {
        return -1;
    }

    process_t *p = (process_t *)scheduler->processes[pid]->process;

    if (p->state == blocked) {
        return unblock_process(pid);
    } else if (p->state == ready || p->state == running) {
        return block_process(pid);
    }
    return -1;
}

#undef capped_priority