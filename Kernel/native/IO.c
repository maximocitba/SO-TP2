// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "IO.h"
#include "font.h"
#include "interrupts.h"
#include "pipes.h"
#include "scheduler.h"
#include "video.h"
#include <stdint.h>

#define SIZE_BUFFER 1000

void newLine(uint64_t *x, uint64_t *y);
void deleteCharAt(uint64_t *x, uint64_t *y, uint64_t backgroundColor);
void putCharAt(uint8_t c, uint64_t *x, uint64_t *y, uint64_t foreColor, uint64_t backgroundColor);
int yOutOfBounds(uint64_t *y);
int xOutOfBounds(uint64_t *x);

uint64_t x = 0;
uint64_t y = 0;
uint8_t fontSize = 1;

void putCharColoured(char c, uint64_t foreGround, uint64_t backGround) {
    switch (c) {
    case 0x0A:
        newLine(&x, &y);
        break;
    case 0x08:
        deleteCharAt(&x, &y, backGround);
        break;
    default:
        putCharAt(c, &x, &y, foreGround, backGround);
        break;
    }
}

void putChar(char c) {
    putCharColoured(c, 0xFFFFFF, BG_COLOR);
}

void printf(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        putChar(str[i]);
    }
}

static char stdinArr[SIZE_BUFFER];
static int sizeIn = 0;
static int startsIn = 0;

void putOut(char c, uint64_t foreground) {
    putCharColoured(c, foreground, BG_COLOR);
}

void putErr(char c) {
    putCharColoured(c, 0x00ff0000, BG_COLOR);
}

void putIn(char c) {

    if (sizeIn >= SIZE_BUFFER - 1)
        return;

    int pos = (startsIn + sizeIn) % SIZE_BUFFER;

    stdinArr[pos] = c;
    sizeIn++;
}

void clearIn() {
    sizeIn = 0;
}

void sys_write_color(int fd, const char *buf, int count, uint64_t foreground) {
    int16_t current_fd = get_current_process_file_descriptor(fd);

    if (current_fd == 1) {
        for (int i = 0; i < count; i++) {
            putOut(buf[i], foreground);
        }
    } else if (current_fd == 2) {
        for (int i = 0; i < count; i++) {
            putErr(buf[i]);
        }
    } else if (current_fd >= BUILTIN_FDS) {
        write_pipe(get_current_pid(), current_fd, buf, count);
        return;
    }
}

void sys_write(int fd, const char *buf, int count) {

    int16_t current_fd = get_current_process_file_descriptor(fd);

    if (current_fd == 1) {
        for (int i = 0; i < count; i++) {
            putOut(buf[i], 0xFFFFFF);
        }
    } else if (current_fd == 2) {
        for (int i = 0; i < count; i++) {
            putErr(buf[i]);
        }
    } else if (current_fd >= BUILTIN_FDS) {
        write_pipe(get_current_pid(), current_fd, buf, count);
        return;
    }
}

int sys_read(int fd, char *buf, int count) {
    int16_t current_fd = get_current_process_file_descriptor(fd);

    if (current_fd == DEV_NULL) {
        buf[0] = EOF;
        return 0;
    } else if (current_fd < DEV_NULL) {
        return -1;
    }

    if (current_fd >= BUILTIN_FDS) {
        read_pipe(get_current_pid(), current_fd, buf, count);
        return count;
    }

    int i = 0;
    if (current_fd == 0) {
        for (i = 0; i < count && i < sizeIn; i++) {
            buf[i] = stdinArr[(startsIn + i) % SIZE_BUFFER];
        }
        startsIn += i;
        startsIn = startsIn % SIZE_BUFFER;
        sizeIn -= i;
    }
    return i;
}

void sys_new_size(int newSize) {
    if (newSize < 1 || newSize > 5) {
        return;
    }
    _cli();
    fontSize = newSize;

    clearScreen(BG_COLOR);
    x = 0;
    y = 0;
    _sti();
}

void putCharAt(uint8_t c, uint64_t *x, uint64_t *y, uint64_t foreColor, uint64_t backgroundColor) {
    if (xOutOfBounds(x)) {
        newLine(x, y);
    }

    if (yOutOfBounds(y)) {
        clearScreen(backgroundColor);
        *y = 0;
    }

    uint8_t charMap[FONT_HEIGHT][FONT_WIDTH];
    getCharMap(c, charMap);

    for (int i = 0; i < FONT_HEIGHT; i++) {
        for (int j = 0; j < FONT_WIDTH; j++) {
            uint8_t squareIsOn = charMap[i][j];
            putSquare(squareIsOn ? foreColor : backgroundColor, *x + j * fontSize, *y + i * fontSize, fontSize);
        }
    }
    *x += FONT_WIDTH * fontSize;
}

void deleteCharAt(uint64_t *x, uint64_t *y, uint64_t backgroundColor) {
    if (*x == 0 && *y == 0)
        return;
    *x -= FONT_WIDTH * fontSize;
    if (xOutOfBounds(x)) {
        int lastX = ((getWidth() - 1) / (FONT_WIDTH * fontSize) - 1) * (FONT_WIDTH * fontSize);
        *x = lastX;
        *y -= FONT_HEIGHT * fontSize;
    }
    putCharAt(' ', x, y, backgroundColor, backgroundColor);
    *x -= FONT_WIDTH * fontSize;
}

void newLine(uint64_t *x, uint64_t *y) {
    *x = 0;
    *y += FONT_HEIGHT * fontSize;
}

int xOutOfBounds(uint64_t *x) {
    return *x + FONT_WIDTH * fontSize >= getWidth() || (int)*x < 0;
}

int yOutOfBounds(uint64_t *y) {
    return *y + FONT_HEIGHT * fontSize >= getHeight() || (int)*y < 0;
}

void sys_clearScreen() {
    clearScreen(BG_COLOR);
    y = 0;
    x = 0;
}
