#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>  // 👈 para los prints

#include "CuTest.h"
#include "memman.h"
#include "MemoryManagerTest.h"

#define MANAGED_MEMORY_SIZE 20480
#define ALLOCATION_SIZE 1024
#define WRITTEN_VALUE 'a'

void testAllocMemory(CuTest *const cuTest);
void testTwoAllocations(CuTest *const cuTest);
void testWriteMemory(CuTest *const cuTest);

static const size_t TestQuantity = 3;
static const Test MemoryManagerTests[] = {testAllocMemory, testTwoAllocations, testWriteMemory};

static inline void givenAMemoryManager(CuTest *const cuTest);
static inline void givenAMemoryAmount();
static inline void givenAnAllocation();

static inline void whenMemoryIsAllocated();
static inline void whenMemoryIsWritten();

static inline void thenSomeMemoryIsReturned(CuTest *const cuTest);
static inline void thenTheTwoAdressesAreDifferent(CuTest *const cuTest);
static inline void thenBothDoNotOverlap(CuTest *const cuTest);
static inline void thenMemorySuccessfullyWritten(CuTest *const cuTest);

static MemoryManagerADT memoryManager;

static size_t memoryToAllocate;
static void *allocatedMemory = NULL;
static void *anAllocation = NULL;

CuSuite *getMemoryManagerTestSuite(void) {
    CuSuite *const suite = CuSuiteNew();

    for (size_t i = 0; i < TestQuantity; i++)
        SUITE_ADD_TEST(suite, MemoryManagerTests[i]);

    return suite;
}

void testAllocMemory(CuTest *const cuTest) {
    printf("== Test: testAllocMemory ==\n");
    givenAMemoryManager(cuTest);
    givenAMemoryAmount();

    whenMemoryIsAllocated();

    thenSomeMemoryIsReturned(cuTest);
}

void testTwoAllocations(CuTest *const cuTest) {
    printf("== Test: testTwoAllocations ==\n");
    givenAMemoryManager(cuTest);
    givenAMemoryAmount();
    givenAnAllocation();

    whenMemoryIsAllocated();

    thenSomeMemoryIsReturned(cuTest);
    thenTheTwoAdressesAreDifferent(cuTest);
    thenBothDoNotOverlap(cuTest);
}

void testWriteMemory(CuTest *const cuTest) {
    printf("== Test: testWriteMemory ==\n");
    givenAMemoryManager(cuTest);
    givenAMemoryAmount();
    givenAnAllocation();

    whenMemoryIsWritten();

    thenMemorySuccessfullyWritten(cuTest);
}

inline void givenAMemoryManager(CuTest *const cuTest) {
    void *memoryForMemoryManager = malloc(sizeof(void *));
    if (memoryForMemoryManager == NULL) {
        CuFail(cuTest, "[givenAMemoryManager] Memory for Memory Manager cannot be null");
    }

    void *managedMemory = malloc(MANAGED_MEMORY_SIZE);
    if (managedMemory == NULL) {
        CuFail(cuTest, "[givenAMemoryManager] Managed Memory cannot be null");
    }

    memoryManager = createMemoryManager(memoryForMemoryManager, managedMemory, MANAGED_MEMORY_SIZE);

    printf("[givenAMemoryManager] Memory Manager created with heap of %d bytes\n", MANAGED_MEMORY_SIZE);
}

inline void givenAMemoryAmount() {
    memoryToAllocate = ALLOCATION_SIZE;
    printf("[givenAMemoryAmount] Will allocate %zu bytes\n", memoryToAllocate);
}

inline void givenAnAllocation() {
    anAllocation = allocMemory(memoryManager, memoryToAllocate);
    printf("[givenAnAllocation] Allocated memory at %p\n", anAllocation);
}

inline void whenMemoryIsAllocated() {
    allocatedMemory = allocMemory(memoryManager, memoryToAllocate);
    printf("[whenMemoryIsAllocated] Allocated memory at %p\n", allocatedMemory);
}

inline void whenMemoryIsWritten() {
    *((char *) anAllocation) = WRITTEN_VALUE;
    printf("[whenMemoryIsWritten] Wrote '%c' to %p\n", WRITTEN_VALUE, anAllocation);
}

inline void thenSomeMemoryIsReturned(CuTest *const cuTest) {
    if (allocatedMemory == NULL) {
        printf("[thenSomeMemoryIsReturned] ❌ Allocation failed! NULL pointer returned\n");
    } else {
        printf("[thenSomeMemoryIsReturned] ✅ Allocation succeeded, pointer = %p\n", allocatedMemory);
    }
    CuAssertPtrNotNull(cuTest, allocatedMemory);
}

inline void thenTheTwoAdressesAreDifferent(CuTest *const cuTest) {
    if (anAllocation == allocatedMemory) {
        printf("[thenTheTwoAdressesAreDifferent] ❌ Addresses are the same (%p)\n", allocatedMemory);
    } else {
        printf("[thenTheTwoAdressesAreDifferent] ✅ Addresses are different (%p vs %p)\n", anAllocation, allocatedMemory);
    }
    CuAssertTrue(cuTest, anAllocation != allocatedMemory);
}

inline void thenBothDoNotOverlap(CuTest *const cuTest) {
    int distance = (char *) anAllocation - (char *) allocatedMemory;
    distance = abs(distance);
    printf("[thenBothDoNotOverlap] Distance between blocks = %d bytes\n", distance);

    if (distance < ALLOCATION_SIZE) {
        printf("[thenBothDoNotOverlap] ❌ Overlapping detected!\n");
    } else {
        printf("[thenBothDoNotOverlap] ✅ No overlapping\n");
    }

    CuAssertTrue(cuTest, distance >= ALLOCATION_SIZE);
}

inline void thenMemorySuccessfullyWritten(CuTest *const cuTest) {
    uint8_t writtenValue = WRITTEN_VALUE;
    uint8_t readValue = *((uint8_t *) anAllocation);
    printf("[thenMemorySuccessfullyWritten] Wrote '%c', read back '%c'\n", writtenValue, readValue);

    CuAssertIntEquals(cuTest, writtenValue, readValue);
}
