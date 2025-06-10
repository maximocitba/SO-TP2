// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "time.h"
#include "interrupts.h"
#include "lib.h"
#include "naiveConsole.h"

static unsigned long ticks = 0;

void timer_handler() {
    ticks++;
}

int ticks_elapsed() {
    return ticks;
}

int seconds_elapsed() {
    return ticks / 18;
}

uint8_t getFormat(uint8_t num) {
    int dec = num & 240;
    dec = dec >> 4;
    int units = num & 15;
    return dec * 10 + units;
}

uint8_t sys_getTime(int descriptor) {
    uint8_t toReturn = getRTC(descriptor);
    return getFormat(toReturn);
}

void sleepms(uint64_t millis) {
    uint64_t start = ticks;
    while (ticks - start < millis / 51) {
        _hlt();
    }
}

void hlt() {
    _hlt();
}