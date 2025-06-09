#ifndef semaphore_h
#define semaphore_h

#include "../include/linked_list.h"
#include "../include/queue_pid.h"
#include <stdint.h>

#define MAX_SEMAPHORES 32

typedef int Semaphore;

Semaphore sem_open(const char *name, uint8_t init_value);
int sem_close(Semaphore sem);

int sem_wait(Semaphore sem);
int sem_post(Semaphore sem);

extern int asm_lock(int8_t *lock);
extern void asm_unlock(int8_t *lock);
#endif