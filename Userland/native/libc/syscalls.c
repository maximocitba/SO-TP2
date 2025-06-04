#include "../include/syscalls.h"
#include "../include/stdio.h"

void sys_hlt() {
    syscall(SYS_HLT, 0, 0, 0, 0, 0);
}

int sys_read(int fd, char *buf, int count) {
    return syscall(SYS_READ, fd, buf, count, 0, 0);
}

void sys_write(int fd, char *buf, int count) {
    syscall(SYS_WRITE, fd, buf, count, 0, 0);
}

uint64_t sys_time(int d) {
    return syscall(SYS_TIME, d, 0, 0, 0, 0);
}

void sys_sleep() {
    syscall(SYS_SLEEP, 0, 0, 0, 0, 0);
}

void sys_sound(int frec) {
    syscall(SYS_SOUND, frec, 0, 0, 0, 0);
}

void sys_nosound() {
    syscall(SYS_NOSOUND, 0, 0, 0, 0, 0);
}

void sys_clear() {
    syscall(SYS_CLEAR, 0, 0, 0, 0, 0);
}

void sys_new_size(int newSize) {
    syscall(SYS_NEW_SIZE, newSize, 0, 0, 0, 0);
}

void sys_putPixel(uint32_t hexColor, uint64_t x, uint64_t y) {
    syscall(SYS_PUT_PIXEL, hexColor, x, y, 0, 0);
}

int sys_secondsElapsed() {
    return syscall(SYS_GET_ELAPSED_SECONDS, 0, 0, 0, 0, 0);
}

int sys_ticks_elapsed() {
    return syscall(SYS_GET_ELAPSED_TICKS, 0, 0, 0, 0, 0);
}

int sys_getWindowSize(int elem) {
    return syscall(SYS_WIDTH_HEIGHT, elem, 0, 0, 0, 0);
}

void sys_getRegs() {
    syscall(SYS_GETREGS, 0, 0, 0, 0, 0);
}

void *sys_malloc(uint64_t size) {
    return (void *)syscall(SYS_MALLOC, size, 0, 0, 0, 0);
}

void sys_free(void *ptr) {
    syscall(SYS_FREE, (uint64_t)ptr, 0, 0, 0, 0);
}

int64_t sys_exec(void *code, char **argv, int argc, char *name, uint8_t priority) {
    printf("Executing '%s' with priority %d\n", name, priority);
    syscall(SYS_EXEC, (uint64_t)code, (uint64_t)argv, argc, (uint64_t)name, priority);
}

int sys_kill(uint64_t pid) {
    return syscall(SYS_KILL, pid, 0, 0, 0, 0);
}

int sys_block(uint64_t pid) {
    // printf("Blocking process %d\n", pid);
    return syscall(SYS_BLOCK, pid, 0, 0, 0, 0);
}

int sys_unblock(uint64_t pid) {
    return syscall(SYS_UNBLOCK, pid, 0, 0, 0, 0);
}

int sys_nice(uint64_t pid, uint8_t priority) {
    return syscall(SYS_NICE, pid, priority, 0, 0, 0);
}

int sys_get_pid() {
    return syscall(SYS_GET_PID, 0, 0, 0, 0, 0);
}

void sys_waitpid(uint64_t child_pid) {
    syscall(SYS_WAIT_PID, child_pid, 0, 0, 0, 0);
}

int sys_sem_open(int64_t id, int64_t initial_value) {
    syscall(SYS_SEM_OPEN, id, initial_value, 0, 0, 0);
}

void sys_sem_wait(uint64_t id) {
    syscall(SYS_SEM_WAIT, id, 0, 0, 0, 0);
}

void sys_sem_post(uint64_t id) {
    syscall(SYS_SEM_POST, id, 0, 0, 0, 0);
}

void sys_sem_close(uint64_t id) {
    syscall(SYS_SEM_CLOSE, id, 0, 0, 0, 0);
}

void sys_yield() {
    syscall(SYS_YIELD, 0, 0, 0, 0, 0);
}