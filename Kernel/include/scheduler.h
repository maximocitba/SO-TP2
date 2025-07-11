#ifndef scheduler_h
#define scheduler_h

#include "../include/definitions.h"
#include <IO.h>
#include <process.h>
#include <stdint.h>

typedef int (*function)(int argc, char **args);
typedef struct scheduler_cdt *scheduler_adt;

extern void asm_do_timer_tick();

int32_t create_process(function code, char **args, int argc, char *name, uint8_t priority, uint8_t unkillable);
int kill_process(uint32_t pid);
int block_process(uint64_t pid);
int unblock_process(uint64_t pid);
scheduler_adt init_scheduler();
int32_t get_next_ready_pid();
void kill_current_process();
void yield();
void *scheduler(void *stack_pointer);
void process_priority(uint64_t pid, uint8_t new_prio);
int get_current_pid();
process_t *get_current_process();
void waitpid(uint32_t child_pid);
process_t *get_process_by_pid(uint32_t pid);
int16_t get_current_process_file_descriptor(uint8_t fd_index);
uint16_t change_process_fd(uint32_t pid, uint16_t fd_index, int16_t new_fd);
void kill_current_ppid();
int get_all_processes_info(ps_info_t *buffer, int max_len); 
int toggle_process_block_state(uint32_t pid); 
void kill_fg_process();
void set_bg_process(uint32_t pid);

#endif // scheduler_h