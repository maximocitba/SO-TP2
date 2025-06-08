#ifndef TPE_ARQUI_SYSCALLS_H
#define TPE_ARQUI_SYSCALLS_H

#define SYS_HLT 0
#define SYS_SOUND 1
#define SYS_NOSOUND 12
#define SYS_SLEEP 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_CLEAR 5
#define SYS_PUT_PIXEL 6
#define SYS_NEW_SIZE 7
#define SYS_TIME 8
#define SYS_GET_ELAPSED_SECONDS 9
#define SYS_GET_ELAPSED_TICKS 10
#define SYS_WIDTH_HEIGHT 11
#define SYS_GETREGS 13
#define SYS_MALLOC 14
#define SYS_FREE 15
#define SYS_EXEC 16
#define SYS_KILL 17
#define SYS_BLOCK 18
#define SYS_UNBLOCK 19
#define SYS_NICE 20
#define SYS_GET_PID 21
#define SYS_WAIT_PID 22
#define SYS_SEM_OPEN 23
#define SYS_SEM_WAIT 24
#define SYS_SEM_POST 25
#define SYS_SEM_CLOSE 26
#define SYS_YIELD 27
#define SYS_CREATE_PIPE 28
#define SYS_OPEN_PIPE 29
#define SYS_CLOSE_PIPE 30
#define SYS_WRITE_PIPE 31
#define SYS_READ_PIPE 32
#define SYS_CHANGE_PROCESS_FD 33
#define SYS_SET_BG_PROCESS 34
#define SYS_CLOSE_PIPE_BY_PID 35
#define SYS_GET_ALL_PROCESSES_INFO 36
#define SYS_TOGGLE_BLOCK_STATE 37

#include <stdint.h>
#include "../include/definitions.h"

extern uint64_t syscall(uint64_t id, uint64_t par1, uint64_t par2, uint64_t par3, uint64_t par4, uint64_t par5);

void sys_hlt();

int sys_read(int fd, char * buf, int count);
void sys_write(int fd, char* buf, int count, uint64_t foreground);

uint64_t sys_time(int d);

void sys_sleep(uint64_t millis);

void sys_sound(int frec);
void sys_nosound();

void sys_clear();


void test_int_80h();
void sys_putPixel(uint32_t hexColor, uint64_t x, uint64_t y);
int sys_ticks_elapsed();
int sys_secondsElapsed();

void sys_new_size(int newSize);
int sys_getWindowSize(int elem);

void sys_getRegs();

void* sys_malloc(uint64_t size);
void sys_free(void* ptr);

int64_t sys_exec(void *code, char **argv, int argc, char *name, uint8_t priority);

int sys_kill(uint64_t pid);

int sys_block(uint64_t pid);
int sys_unblock(uint64_t pid);

int sys_nice(uint64_t pid, uint8_t priority);

int sys_get_pid();

void sys_waitpid(uint64_t pid);

int sys_sem_open(int64_t id, int64_t initial_value);
void sys_sem_wait(uint64_t sem_id);
void sys_sem_post(uint64_t sem_id);
void sys_sem_close(uint64_t sem_id);

void sys_yield();

uint16_t sys_create_pipe();

uint16_t sys_open_pipe(uint16_t pid, uint16_t pipe_id, uint8_t mode);

uint16_t sys_close_pipe(uint16_t pipe_id);

uint16_t sys_write_pipe(uint16_t pid, uint16_t pipe_id, char * data, uint16_t size);

uint16_t sys_read_pipe(uint16_t pid, uint16_t pipe_id, char * data, uint16_t size);

uint16_t sys_change_process_fd(uint32_t pid, uint16_t fd_index, int16_t new_fd);

void sys_set_bg_process(uint32_t pid);

uint16_t sys_close_pipe_by_pid(uint16_t pid, uint16_t pipe_id);

int sys_get_all_processes_info(ps_info_t* buffer, int max_len);
int sys_toggle_block_state(uint32_t pid);

#endif //TPE_ARQUI_SYSCALLS_H
