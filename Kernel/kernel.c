#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <idtLoader.h>
#include <memman.h>
#include <scheduler.h>

extern void test_int_80h();

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void*)(uintptr_t)0x400000;
static void *const sampleDataModuleAddress = (void*)(uintptr_t)0x500000;

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
    char buffer[10];

    void *moduleAddresses[] = {
            sampleCodeModuleAddress,
            sampleDataModuleAddress
    };

    loadModules(&endOfKernelBinary, moduleAddresses);

    clearBSS(&bss, &endOfKernel - &bss);

    uint64_t size = (void *)(uintptr_t)0x2000000 - (void*)(uintptr_t)0x1000000;
    b_init((void*)(uintptr_t)0x1000000, size);
    init_scheduler();
    return getStackBase();
}

int main() {
    create_process((function)sampleCodeModuleAddress, NULL, 0, "shell", 14, 1);

    load_idt();

    // sys_registers();     // descomentar para ver registros del kernel

    // ((EntryPoint) sampleCodeModuleAddress)();

    while (1);

    return 0;
}