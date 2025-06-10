// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "lib.h"
#include "memman.h"
#include <stdint.h>
#include <string.h>

#define HEAP_BITS_PER_BYTE 8
#define BYTE_ALIGNMENT 8
#define BYTE_ALIGNMENT_MASK 0x0007

typedef struct Node {
    struct Node *next;
    size_t block_size;
} MemoryBlock;

static MemoryBlock start_block;
static MemoryBlock end_block;
static size_t total_mem;
static size_t used_mem;
static unsigned int mem_chunks;
static size_t allocated_bit;
static const size_t struct_size = sizeof(MemoryBlock);

static void insert_block(MemoryBlock *block_to_insert);

void b_init(void *const start_address, size_t size) {
    if (size <= sizeof(MemoryBlock) * 2)
        return;

    size_t true_start = (size_t)start_address;
    if ((true_start & BYTE_ALIGNMENT_MASK) != 0) {
        true_start += (BYTE_ALIGNMENT - (true_start & BYTE_ALIGNMENT_MASK));
    }

    uint8_t *true_start_address = (uint8_t *)true_start;
    size_t usable_size = size - (true_start - (size_t)start_address);

    if (usable_size <= 2 * struct_size)
        return;

    usable_size -= struct_size;

    MemoryBlock *first_block = (MemoryBlock *)true_start_address;
    first_block->block_size = usable_size;
    first_block->next = &end_block;

    start_block.next = first_block;
    start_block.block_size = 0;

    end_block.block_size = 0;
    end_block.next = NULL;

    total_mem = usable_size;
    used_mem = 0;
    mem_chunks = 0;
    allocated_bit = ((size_t)1) << ((sizeof(size_t) * HEAP_BITS_PER_BYTE) - 1);
}

void *b_alloc(size_t size) {
    if (size == 0 || size & allocated_bit)
        return NULL;

    size += struct_size;
    if (size & BYTE_ALIGNMENT_MASK) {
        size += (BYTE_ALIGNMENT - (size & BYTE_ALIGNMENT_MASK));
    }

    if (size > (total_mem - used_mem))
        return NULL;

    MemoryBlock *prev = &start_block;
    MemoryBlock *curr = start_block.next;

    while (curr != &end_block && curr->block_size < size) {
        prev = curr;
        curr = curr->next;
    }

    if (curr == &end_block)
        return NULL;

    void *return_ptr = (uint8_t *)curr + struct_size;
    prev->next = curr->next;

    size_t remaining = curr->block_size - size;

    if (remaining >= (struct_size << 1)) {
        MemoryBlock *split_block = (MemoryBlock *)((uint8_t *)curr + size);
        split_block->block_size = remaining;
        curr->block_size = size;
        insert_block(split_block);
    }

    used_mem += curr->block_size;
    curr->block_size |= allocated_bit;
    curr->next = NULL;
    mem_chunks++;

    return return_ptr;
}

void b_free(void *ptr) {
    if (!ptr)
        return;

    MemoryBlock *block = (MemoryBlock *)((uint8_t *)ptr - struct_size);

    if ((block->block_size & allocated_bit) == 0 || block->next != NULL)
        return;

    block->block_size &= ~allocated_bit;
    used_mem -= block->block_size;
    mem_chunks--;
    insert_block(block);
}

static void insert_block(MemoryBlock *block_to_insert) {
    MemoryBlock *iterator = &start_block;

    while (iterator->next < block_to_insert && iterator->next != &end_block) {
        iterator = iterator->next;
    }

    uint8_t *iter_end = (uint8_t *)iterator + iterator->block_size;
    if (iter_end == (uint8_t *)block_to_insert) {
        iterator->block_size += block_to_insert->block_size;
        block_to_insert = iterator;
    }

    uint8_t *block_end = (uint8_t *)block_to_insert + block_to_insert->block_size;
    if (block_end == (uint8_t *)iterator->next && iterator->next != &end_block) {
        block_to_insert->block_size += iterator->next->block_size;
        block_to_insert->next = iterator->next->next;
    } else {
        block_to_insert->next = iterator->next;
    }

    if (iterator != block_to_insert) {
        iterator->next = block_to_insert;
    }
}

void *b_realloc(void *ptr, size_t size) {
    if (!ptr)
        return b_alloc(size);

    MemoryBlock *block = (MemoryBlock *)((uint8_t *)ptr - struct_size);
    size_t old_size = (block->block_size & ~allocated_bit) - struct_size;

    if (size <= old_size)
        return ptr;

    void *new_ptr = b_alloc(size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, old_size);
        b_free(ptr);
    }

    return new_ptr;
}

size_t *mem_info() {
    static size_t info[3];
    info[0] = total_mem;
    info[1] = used_mem;
    info[2] = mem_chunks;
    return info;
}
