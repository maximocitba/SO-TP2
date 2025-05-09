#include "memman.h"
// Buddy Memory Manager

#define MAX_ORDER 32
#define MIN 4
#define taken 0
#define freed 1

typedef struct block_t {
    uint8_t order;
    uint8_t state;
    struct block_t *prev;
    struct block_t *next;
} block_t;

typedef struct {
    void *base;
    uint8_t max;
    block_t *free_lists[MAX_ORDER];
    size_t total_mem;
    size_t used_mem;
} buddy_t;

typedef struct MemoryManagerCDT {
    buddy_t buddy;
} MemoryManagerCDT;

// Helpers
static uint8_t get_order(size_t size);
static block_t *new_block_t(void *address, uint8_t order, block_t *next);
static void free_lists_remove(buddy_t *buddy, block_t *header);
static void split(buddy_t *buddy, uint8_t idx);
static block_t *merge_block(buddy_t *buddy, block_t *block, block_t *buddy_block);
static inline void *hide_block_t(block_t *block);
static inline block_t *show_block_t(void *ptr);
static inline size_t block_size(uint8_t order);
static inline block_t *buddy_of(block_t *block);

// Inicializador del TAD
MemoryManagerADT createMemoryManager(void *memoryForManager, void *heapStart, size_t heapSize) {
    MemoryManagerADT manager = (MemoryManagerADT)memoryForManager;
    buddy_t *buddy = &manager->buddy;

    buddy->base = heapStart;
    buddy->max = get_order(heapSize);
    buddy->total_mem = heapSize;
    buddy->used_mem = 0;

    for (int i = 0; i < MAX_ORDER; i++)
        buddy->free_lists[i] = NULL;

    buddy->free_lists[buddy->max - 1] = new_block_t(buddy->base, buddy->max, NULL);

    return manager;
}

// Reserva memoria
void *allocMemory(MemoryManagerADT manager, size_t size) {
    buddy_t *buddy = &manager->buddy;
    uint8_t order = get_order(size + sizeof(block_t));

    if (order < MIN - 1)
        order = MIN - 1;
    if (order >= buddy->max)
        return NULL;

    uint8_t to_split = order;
    while (to_split < buddy->max && buddy->free_lists[to_split] == NULL)
        to_split++;

    if (to_split == buddy->max)
        return NULL;

    while (to_split > order) {
        split(buddy, to_split);
        to_split--;
    }

    block_t *block = buddy->free_lists[order];
    free_lists_remove(buddy, block);
    block->state = taken;
    buddy->used_mem += block_size(block->order);
    return hide_block_t(block);
}

// Libera memoria
void freeMemory(MemoryManagerADT manager, void *ptr) {
    if (!ptr) return;

    buddy_t *buddy = &manager->buddy;
    block_t *block = show_block_t(ptr);
    block->state = freed;
    buddy->used_mem -= block_size(block->order);

    while (1) {
        block_t *b = buddy_of(block);
        if (b->state != freed || b->order != block->order || block->order >= buddy->max)
            break;
        block = merge_block(buddy, block, b);
    }

    buddy->free_lists[block->order - 1] = new_block_t(block, block->order, buddy->free_lists[block->order - 1]);
}

// Estado de memoria
void statusMemory(MemoryManagerADT manager) {
    buddy_t *buddy = &manager->buddy;

    // Reemplazar por printf/printk si estás en modo kernel
    printf("Memory status:\n");
    printf("  Total: %zu bytes\n", buddy->total_mem);
    printf("  Used : %zu bytes\n", buddy->used_mem);
    printf("  Free : %zu bytes\n", buddy->total_mem - buddy->used_mem);
}

// Helpers
static uint8_t get_order(size_t size) {
    uint8_t i = 0;
    size--;
    while (size >>= 1) i++;
    return i;
}

static block_t *new_block_t(void *address, uint8_t order, block_t *next) {
    block_t *header = (block_t *)address;
    header->order = order;
    header->state = freed;
    header->prev = NULL;
    header->next = next;
    if (next) next->prev = header;
    return header;
}

static void free_lists_remove(buddy_t *buddy, block_t *header) {
    if (header->prev == NULL)
        buddy->free_lists[header->order - 1] = header->next;
    else
        header->prev->next = header->next;

    if (header->next)
        header->next->prev = header->prev;
}

static void split(buddy_t *buddy, uint8_t idx) {
    block_t *block = buddy->free_lists[idx];
    free_lists_remove(buddy, block);

    block_t *buddy_block = (block_t *)((char *)block + (1ULL << idx));
    new_block_t(buddy_block, idx, buddy->free_lists[idx - 1]);
    buddy->free_lists[idx - 1] = new_block_t(block, idx, buddy_block);
}

static block_t *merge_block(buddy_t *buddy, block_t *block, block_t *buddy_block) {
    free_lists_remove(buddy, buddy_block);
    return (block < buddy_block ? block : buddy_block)->order++, block < buddy_block ? block : buddy_block;
}

static inline void *hide_block_t(block_t *block) {
    return (void *)((char *)block + sizeof(block_t));
}

static inline block_t *show_block_t(void *ptr) {
    return (block_t *)((char *)ptr - sizeof(block_t));
}

static inline size_t block_size(uint8_t order) {
    return 1ULL << order;
}

static inline block_t *buddy_of(block_t *block) {
    uintptr_t addr = (uintptr_t)block;
    buddy_t *buddy = &((MemoryManagerADT)((uintptr_t)block & ~(0xFFFFFFFF)))->buddy;
    uintptr_t base = (uintptr_t)buddy->base;
    uintptr_t rel = addr - base;
    uintptr_t buddy_rel = rel ^ (1ULL << block->order);
    return (block_t *)(base + buddy_rel);
}
