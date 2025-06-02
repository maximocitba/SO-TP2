#include "video.h"
#include "IO.h"
#include "time.h"
#include "lib.h"
#include "interrupts.h"
#include "sound.h"
#include "memman.h"
#include "scheduler.h"

#define SYS_HLT 0
#define SYS_SOUND 1
#define SYS_NOSOUND 12
#define SYS_SLEEP 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_CLEAR_SCREEN 5
#define SYS_PUT_PIXEL 6
#define SYS_NEW_SIZE 7
#define SYS_GET_TIME 8
#define SYS_GET_ELAPSED_SECONDS 9
#define SYS_GET_ELAPSED_TICKS 10
#define SYS_WIDTH_HEIGHT 11
#define SYS_GETREGS 13
#define SYS_MALLOC 14
#define SYS_FREE 15
#define SYS_EXEC 16
#define SYS_KILL 17
#define SYS_BLOCK 18
#define SYS_UNBLOCK 19
#define SYS_NICE 20

uint64_t registers[18] = {0};

uint64_t int80Dispacher(uint64_t id, uint64_t param_1, uint64_t param_2, uint64_t param_3, uint64_t param_4, uint64_t param_5) {
    switch (id) {
        case SYS_SOUND:
            play_sound(param_1);
            return 1;
        case SYS_NOSOUND:
            nosound();
            return 1;
        case SYS_WRITE:
            sys_write(param_1, param_2, param_3);
            return 1;
        case SYS_READ:
            return sys_read(param_1, param_2, param_3);
        case SYS_CLEAR_SCREEN:
            sys_clearScreen();
            return 1;
        case SYS_PUT_PIXEL:
            putPixel(param_1, param_2, param_3);
            return 1;
        case SYS_NEW_SIZE:
            sys_new_size(param_1);
            return 1;
        case SYS_GET_TIME:
            return sys_getTime(param_1);
        case SYS_GET_ELAPSED_SECONDS:
            return seconds_elapsed();
        case SYS_HLT:
            _hlt();
            return 1;
        case SYS_GET_ELAPSED_TICKS:
            return ticks_elapsed();
        case SYS_WIDTH_HEIGHT:
            return sys_getSize(param_1);
        case SYS_GETREGS:
            sys_registers();
            return 1;
        case SYS_MALLOC:
            return b_alloc(param_1);
        case SYS_FREE:
            b_free((void *)param_1);
            return 1;
        case SYS_EXEC:
            return create_process(param_1, (char **)param_2, param_3, (char *)param_4, param_5, 0);
        case SYS_KILL:
            return kill_process(param_1);
        case SYS_BLOCK:
            return block_process(param_1);
        case SYS_UNBLOCK:
            return unblock_process(param_1);
        case SYS_NICE:
            process_priority(param_1, (uint8_t)param_2);
            return 1;
    }
    return 0;
}