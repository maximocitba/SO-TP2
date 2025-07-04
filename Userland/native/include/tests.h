#ifndef tests_h
#define tests_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

//memory_manager
uint64_t test_mm(uint64_t max_memory);

//processes
int64_t test_processes(uint64_t argc, char *argv[]);

//prio
void test_prio();

//sync
void slowInc(int64_t *p, int64_t inc); 
uint64_t my_process_inc(uint64_t argc, char *argv[]);
uint64_t test_sync(uint64_t argc, char *argv[]);



//utils
uint32_t get_uint();
uint32_t GetUniform(uint32_t max);
uint8_t memcheck(void *start, uint8_t value, uint32_t size);
int64_t satoi(char *str);
void bussy_wait(uint64_t n);
void endless_loop();
void endless_loop_print(uint64_t wait);

//syscall
int64_t my_getpid();
int64_t my_create_process(char *name, uint64_t argc, char *argv[]);
int64_t my_nice(uint64_t pid, uint64_t new_prio);
int64_t my_kill(uint64_t pid);
int64_t my_block(uint64_t pid);
int64_t my_unblock(uint64_t pid);
int64_t my_sem_open(char *sem_id, uint64_t initial_value);
int64_t my_sem_wait(char *sem_id);
int64_t my_sem_post(char *sem_id);
int64_t my_sem_close(char *sem_id);
int64_t my_yield();
int64_t my_wait(int64_t pid);

#endif