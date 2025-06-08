#include "../include/syscalls.h"
#include "../include/stdio.h"

void sys_hlt() {
    syscall(SYS_HLT, 0, 0, 0, 0, 0);
}

int sys_read(int fd, char *buf, int count) {
    return syscall(SYS_READ, fd, buf, count, 0, 0);
}

void sys_write(int fd, char *buf, int count, uint64_t foreground) {
    syscall(SYS_WRITE, fd, buf, count, foreground, 0);
}

uint64_t sys_time(int d) {
    return syscall(SYS_TIME, d, 0, 0, 0, 0);
}

void sys_sleep(uint64_t millis) {
    syscall(SYS_SLEEP, millis, 0, 0, 0, 0);
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

uint16_t sys_create_pipe() {
    return syscall(SYS_CREATE_PIPE, 0, 0, 0, 0, 0);
}

uint16_t sys_open_pipe(uint16_t pid, uint16_t pipe_id, uint8_t mode) {
    return syscall(SYS_OPEN_PIPE, pid, pipe_id, mode, 0, 0);
}

uint16_t sys_close_pipe(uint16_t pipe_id) {
    return syscall(SYS_CLOSE_PIPE, pipe_id, 0, 0, 0, 0);
}

uint16_t sys_write_pipe(uint16_t pid, uint16_t pipe_id, char *data, uint16_t size) {
    return syscall(SYS_WRITE_PIPE, pid, pipe_id, (uint64_t)data, size, 0);
}

uint16_t sys_read_pipe(uint16_t pid, uint16_t pipe_id, char *data, uint16_t size) {
    return syscall(SYS_READ_PIPE, pid, pipe_id, (uint64_t)data, size, 0);
}

uint16_t sys_change_process_fd(uint32_t pid, uint16_t fd_index, int16_t new_fd) {
    return syscall(SYS_CHANGE_PROCESS_FD, pid, fd_index, new_fd, 0, 0);
}

void sys_set_bg_process(uint32_t pid) {
    syscall(SYS_SET_BG_PROCESS, pid, 0, 0, 0, 0);
}

uint16_t sys_close_pipe_by_pid(uint16_t pid, uint16_t pipe_id) {
    return syscall(SYS_CLOSE_PIPE_BY_PID, pid, pipe_id, 0, 0, 0);
}

int sys_get_all_processes_info(ps_info_t* buffer, int max_len) {
    return syscall(SYS_GET_ALL_PROCESSES_INFO, (uint64_t)buffer, max_len, 0, 0, 0);
}

int sys_toggle_block_state(uint32_t pid) {
    return syscall(SYS_TOGGLE_BLOCK_STATE, pid, 0, 0, 0, 0);
}