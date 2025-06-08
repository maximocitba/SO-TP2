#ifndef definitions_h
#define definitions_h

#include <stdint.h>

#define MAX_PROCESSES (1<<15)
#define stack_size (1<<12)
#define SCHEDULER_ADDRESS 0x600000
#define MAX_PRIORITY 5
#define EOF (-1)
#define BUILTIN_FDS 3
#define DEV_NULL -1

#define DEFAULT_PRIORITY medium

#define MAX_PROC_NAME_LEN 32
#define MAX_PTR_STR_LEN 19 // "0x" + 16 hex chars for 64-bit address + null terminator

typedef enum { blocked = 0, ready, killed, running, waiting_for_child} state_t;
typedef enum { low = 1, low_medium, medium, high_medium, high } priority_t;
typedef enum { STDIN=0, STDOUT, STDERR } basic_fd_t;
typedef enum pipe_state { closed=0, opened } pipe_state;
typedef enum { foreground=0, background } fg_bg_t;
typedef enum pipe_mode { read_mode=0, write_mode } pipe_mode;

typedef struct {
    int32_t pid;
    priority_t priority;
    state_t state;
    char name[MAX_PROC_NAME_LEN];
    char stack_pointer_str[MAX_PTR_STR_LEN];
    char base_pointer_str[MAX_PTR_STR_LEN];
    fg_bg_t fg;
    int32_t parent_pid;
} ps_info_t;

typedef int (*function)(int argc, char **args);

typedef struct {
    int32_t pid;
    priority_t priority;
    state_t state;
    char* name;
    int argc;
    char** argv;
    char * stack_pointer;
    char * base_pointer;
    fg_bg_t fg;
} process_snapshot_t;

#endif