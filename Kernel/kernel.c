// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <idtLoader.h>
#include <lib.h>
#include <memman.h>
#include <moduleLoader.h>
#include <pipes.h>
#include <scheduler.h>
#include <stdint.h>

extern void test_int_80h();

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *)(uintptr_t)0x400000;
static void *const sampleDataModuleAddress = (void *)(uintptr_t)0x500000;

static void *const heap_start_address = (void *)(uintptr_t)0x1000000;
static void *const heap_end_address = (void *)(uintptr_t)0x2000000;

typedef int (*EntryPoint)();

void clearBSS(void *bssAddress, uint64_t bssSize) {
    memset(bssAddress, 0, bssSize);
}

void *getStackBase() {
    return (void *)((uint64_t)&endOfKernel + PageSize * 8 - sizeof(uint64_t));
}

void *initializeKernelBinary() {

    void *moduleAddresses[] = {
        sampleCodeModuleAddress,
        sampleDataModuleAddress};

    loadModules(&endOfKernelBinary, moduleAddresses);

    clearBSS(&bss, &endOfKernel - &bss);

    uint64_t size = (uint64_t)heap_end_address - (uint64_t)heap_start_address;
    b_init(heap_start_address, size);
    init_scheduler();
    init_pipe_manager();
    return getStackBase();
}

int main() {
    create_process((function)sampleCodeModuleAddress, NULL, 0, "shell", high, 1);

    load_idt();

    // sys_registers();     // descomentar para ver registros del kernel

    while (1)
        ;

    return 0;
}