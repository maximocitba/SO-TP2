#ifndef MEMMAN_H
#define MEMMAN_H

#include <stddef.h>
#include <stdint.h>

void b_init(void *initial_addr, uint64_t size);
void *b_alloc(uint64_t size);
void b_free(void *ptr);
void *malloc(size_t size);
void free(void *ptr);
size_t *mem_info();

#endif