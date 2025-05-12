#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <idtLoader.h>
#include <naiveConsole.h>
#include "textmode.h"
#include "memman.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

// HEAP_START will be initialized at runtime
void *HEAP_START;
#define HEAP_SIZE (16 * 1024 * 1024) // 16 MiB heap

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *) 0x400000;
static void *const sampleDataModuleAddress = (void *) 0x500000;

typedef int (*EntryPoint)();


void clearBSS(void *bssAddress, uint64_t bssSize) {
    memset(bssAddress, 0, bssSize);
}

void *getStackBase() {
    return (void *) (
            (uint64_t) & endOfKernel
                         + PageSize * 8                //The size of the stack itself, 32KiB
                         - sizeof(uint64_t)            //Begin at the top of the stack
    );
}

void *initializeKernelBinary() {
    void *moduleAddresses[] = {
            sampleCodeModuleAddress,
            sampleDataModuleAddress
    };

    loadModules(&endOfKernelBinary, moduleAddresses);

    clearBSS(&bss, &endOfKernel - &bss);
    HEAP_START = (void *)((((uint64_t)(uintptr_t)(void *)&endOfKernel) + 0xFFF) & ~0xFFF); // align to 4K
    b_init(HEAP_START, HEAP_SIZE);

    return getStackBase();
}

int main() {
    // Initialize the text mode screen
    textmode_clear();
    
    // Load interrupt descriptor table
    load_idt();
    
    // Print some debug info
    textmode_write("[Kernel] Starting in text mode...\n");
    textmode_write("[Kernel] Memory manager initialized\n");
    textmode_write("[Kernel] Starting userland...\n");
    
    // Call the userland entry point
    ((EntryPoint) sampleCodeModuleAddress)();
    
    // Should never reach here
    textmode_write("[Kernel] ERROR: Userland returned, halting system\n");
    
    while (1);
    return 0;
}