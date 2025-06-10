// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "lib.h"
#include "IO.h"
#include "interrupts.h"
#include "naiveConsole.h"
#include "video.h"
#include <stdint.h>

void *memset(void *destination, int32_t c, uint64_t length) {
    uint8_t chr = (uint8_t)c;
    char *dst = (char *)destination;

    while (length--)
        dst[length] = chr;

    return destination;
}

void *memcpy(void *destination, const void *source, uint64_t length) {

    uint64_t i;

    if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
        (uint64_t)source % sizeof(uint32_t) == 0 &&
        length % sizeof(uint32_t) == 0) {
        uint32_t *d = (uint32_t *)destination;
        const uint32_t *s = (const uint32_t *)source;

        for (i = 0; i < length / sizeof(uint32_t); i++)
            d[i] = s[i];
    } else {
        uint8_t *d = (uint8_t *)destination;
        const uint8_t *s = (const uint8_t *)source;

        for (i = 0; i < length; i++)
            d[i] = s[i];
    }

    return destination;
}

int buflen(const char *s) {
    int i = 0;
    while (*s != '\0') {
        i++;
        s++;
    }
    return i;
}

#define REGS_SIZE 18

uint64_t *regs = {0};
int regsCaptured = 0;

char *regList[REGS_SIZE] = {
    "RAX", "RBX", "RCX", "RDX", "RDI", "RSI", "RBP", "RSP",
    "R08", "R09", "R10", "R11", "R12", "R13", "R14",
    "R15", "IP ", "RFLAGS "};

void getRegs() {
    regs = storeRegs();
    regsCaptured = 1;
}

void sys_registers() {
    if (!regsCaptured) {
        getRegs();
    } else {
        printf("Tenias estos registros guardados:\n");
    }

    for (int i = 0; i < REGS_SIZE; i++) {
        char buf[33];
        printf(regList[i]);
        uint32_t digits = uintToBase(regs[i], buf, 16);
        printf(" : 0x");
        int zeros = 15;
        while (zeros > digits) {
            printf("0");
            zeros--;
        }
        printf(buf);
        putChar('\n');
    }
    regsCaptured = 0;
}

void int_to_string(int num, char *str) {
    int i = 0;
    int is_negative = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    while (num != 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }

    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

void print_number(int number) {
    char buffer[20];
    int_to_string(number, buffer);
    printf(buffer);
}

void pointer_to_string(void *ptr, char *buffer, size_t buffer_size) {
    const size_t required_hex_digits = sizeof(uintptr_t) * 2;
    const size_t required_total_size = 2 /* "0x" */ + required_hex_digits + 1 /* '\0' */;

    if (buffer_size < required_total_size) {
        if (buffer_size > 0) {
            buffer[0] = '\0';
        }
        return;
    }

    uintptr_t address = (uintptr_t)ptr;
    int index = 0;
    buffer[index++] = '0';
    buffer[index++] = 'x';

    for (int i = required_hex_digits - 1; i >= 0; i--) {
        uintptr_t nibble_value = (address >> (i * 4)) & 0xF;
        if (nibble_value < 10) {
            buffer[index++] = '0' + nibble_value;
        } else {
            buffer[index++] = 'a' + (nibble_value - 10);
        }
    }
    buffer[index] = '\0';
}