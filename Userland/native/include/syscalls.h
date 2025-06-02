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

#include <stdint.h>

extern uint64_t syscall(uint64_t id, uint64_t par1, void * par2, uint64_t par3, uint64_t par4, uint64_t par5);

void sys_hlt();

int sys_read(int fd, char * buf, int count);
void sys_write(int fd, char* buf, int count);

uint64_t sys_time(int d);

void sys_sleep();

void sys_sound(int frec);
void sys_nosound();

void sys_clear();

void print_help();
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

#endif //TPE_ARQUI_SYSCALLS_H
