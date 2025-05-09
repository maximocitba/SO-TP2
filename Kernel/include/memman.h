#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>
#include <stdint.h>

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory, size_t managedSize);

void *allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate);

void freeMemory(MemoryManagerADT const restrict memoryManager, void *ptr);

void statusMemory(MemoryManagerADT const restrict memoryManager);

#endif