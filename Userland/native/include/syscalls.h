#ifndef TPE_ARQUI_SYSCALLS_H
#define TPE_ARQUI_SYSCALLS_H

#include <stdint.h>
#include <stddef.h>

// System call numbers
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
#define SYS_MALLOC 20
#define SYS_FREE   21

extern uint64_t syscall(uint64_t id, uint64_t p1, uint64_t p2, uint64_t p3);

// System service wrappers
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

// Memory management functions
void *sys_malloc(size_t size);
void sys_free(void *ptr);

// Standard C memory allocation functions
void *malloc(size_t size);
void free(void *ptr);

#endif //TPE_ARQUI_SYSCALLS_H
