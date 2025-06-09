// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "syscall.h"

#include "../include/stdio.h"
#include "../include/tests.h"
#include "../include/stdlib.h"
#include <string.h>
#include <syscalls.h>

#define MAX_BLOCKS 128

typedef struct MM_rq {
    void *address;
    uint32_t size;
} mm_rq;

static mm_rq mm_rqs[MAX_BLOCKS];

uint64_t test_mm(uint64_t max_memory) {
    printf("test_mm start\n");

    uint8_t rq;
    uint32_t total;


    // printf("starting endless loop\n");
    int wh = 1;
    while (wh--) {
        rq = 0;
        total = 0;

        
        // Request as many blocks as we can
        while (rq < MAX_BLOCKS && total < max_memory) {
            mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
            mm_rqs[rq].address = malloc(mm_rqs[rq].size);

            if (mm_rqs[rq].address) {
                total += mm_rqs[rq].size;
                rq++;
            }
        }
        printf("allocated %d blocks\n", rq);

        // Set
        // printf("starting test set\n");
        uint32_t i;
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                memset(mm_rqs[i].address, i, mm_rqs[i].size);

        printf("set %d blocks\n", rq);

        // Check
        // printf("starting test check\n");
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
                    printf("test_mm ERROR\n");
                    return -1;
                }

        // printf("starting test free\n");
        // Free
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address){
                free(mm_rqs[i].address);
                printf("freed block %d of size %d\n", i, mm_rqs[i].size);
            }

    }
    printf("finished correctly\n");
    return 0;
}