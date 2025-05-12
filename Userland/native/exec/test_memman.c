#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/syscalls.h"
#include "../include/test_memman.h"
#include <stddef.h>
#include <stdint.h>

void test_memman() {
    printf("Testing memory manager...\n");
    
    // Allocate memory
    void *ptr = malloc(64);
    if (ptr == NULL) {
        printf("malloc failed - could not allocate memory\n");
        return;
    }
    
    // Print allocated address in hex format
    char addr_str[20];
    uint64_t addr = (uint64_t)ptr;
    printf("malloc succeeded - allocated 64 bytes at address: 0x");
    uintToBase(addr, addr_str, 16);
    printf("%s\n", addr_str);
    
    // Write a specific pattern to memory
    unsigned char *memory = (unsigned char *)ptr;
    for (int i = 0; i < 64; i++) {
        memory[i] = (i % 16) + 0x30; // Write recognizable pattern
    }
    
    // Verify memory content by reading first few bytes
    printf("Verifying memory content: ");
    for (int i = 0; i < 8; i++) {
        char hex_str[4];
        uintToBase(memory[i], hex_str, 16);
        printf("%s ", hex_str);
    }
    printf("...\n");
    
    // Free memory and report success
    free(ptr);
    printf("Memory freed successfully\n");
    printf("Memory test completed!\n");
}