// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "keyboard.h"
#include "naiveConsole.h"
#include "time.h"
#include <stdint.h>
#include "semaphore.h"

static void int_20();
static void int_21();

void irqDispatcher(uint64_t irq) {
    switch (irq) {
    case 0:
        int_20();
        break;
    case 1:
        int_21();
        break;
    }
    return;
}

void int_20() {
    timer_handler();
}

void int_21() {
    keyboard_handler();
}
