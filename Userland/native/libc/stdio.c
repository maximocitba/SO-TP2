// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include <stdarg.h>

#define MAX_BUF 1024 // Used for itoa buffer

void scanf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char buf[MAX_BUF];

    for (int i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i++] == '%') {
            switch (fmt[i]) {
            case 's':
                gets(va_arg(args, char *), MAX_BUF);
                break;
            case 'c':
                gets(va_arg(args, char *), 1);
                break;
            case 'd':
                gets(buf, MAX_BUF);
                int value = atoi(buf);
                int *aux = va_arg(args, int *);
                *aux = value;
                break;
            default:
                break;
            }
        }
    }
    va_end(args);
}

// static void print_str(const char * str, uint64_t foreground, uint64_t background) {
//     uint64_t i = 0;
//     while (str[i]) {
//         putcharColoured(str[i++], foreground, background);
//     }
// }

uint64_t vprintf_color(const char *fmt, uint64_t foreground, uint64_t background, va_list args) {
    uint64_t char_count = 0;
    int i = 0;
    char num_buf[MAX_BUF];

    while (fmt[i]) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
            case 'd': {
                int val = va_arg(args, int);
                if (val < 0) {
                    putcharColoured('-', foreground, background);
                    char_count++;
                    val = -val;
                }
                itoa(val, num_buf);
                for (int k = 0; num_buf[k]; k++) {
                    putcharColoured(num_buf[k], foreground, background);
                    char_count++;
                }
                break;
            }
            case 'u': {
                unsigned int val = va_arg(args, unsigned int);
                uintToBase(val, num_buf, 10);
                for (int k = 0; num_buf[k]; k++) {
                    putcharColoured(num_buf[k], foreground, background);
                    char_count++;
                }
                break;
            }
            case 'x': {
                unsigned int val = va_arg(args, unsigned int);
                uintToBase(val, num_buf, 16);
                // Convert to lowercase
                for (int k = 0; num_buf[k]; k++) {
                    if (num_buf[k] >= 'A' && num_buf[k] <= 'F') {
                        num_buf[k] = num_buf[k] - 'A' + 'a';
                    }
                }
                for (int k = 0; num_buf[k]; k++) {
                    putcharColoured(num_buf[k], foreground, background);
                    char_count++;
                }
                break;
            }
            case 'X': {
                unsigned int val = va_arg(args, unsigned int);
                uintToBase(val, num_buf, 16);
                for (int k = 0; num_buf[k]; k++) {
                    putcharColoured(num_buf[k], foreground, background);
                    char_count++;
                }
                break;
            }
            case 'o': {
                unsigned int val = va_arg(args, unsigned int);
                uintToBase(val, num_buf, 8);
                for (int k = 0; num_buf[k]; k++) {
                    putcharColoured(num_buf[k], foreground, background);
                    char_count++;
                }
                break;
            }
            case 'p': {
                void *ptr = va_arg(args, void *);
                putcharColoured('0', foreground, background);
                char_count++;
                putcharColoured('x', foreground, background);
                char_count++;
                uintToBase((uint64_t)ptr, num_buf, 16);
                // Convert to lowercase
                for (int k = 0; num_buf[k]; k++) {
                    if (num_buf[k] >= 'A' && num_buf[k] <= 'F') {
                        num_buf[k] = num_buf[k] - 'A' + 'a';
                    }
                }
                for (int k = 0; num_buf[k]; k++) {
                    putcharColoured(num_buf[k], foreground, background);
                    char_count++;
                }
                break;
            }
            case 's': {
                char *s = va_arg(args, char *);
                if (s == ((char *)0)) {
                    putcharColoured('(', foreground, background);
                    char_count++;
                    putcharColoured('n', foreground, background);
                    char_count++;
                    putcharColoured('u', foreground, background);
                    char_count++;
                    putcharColoured('l', foreground, background);
                    char_count++;
                    putcharColoured('l', foreground, background);
                    char_count++;
                    putcharColoured(')', foreground, background);
                    char_count++;
                } else {
                    for (int k = 0; s[k]; k++) {
                        putcharColoured(s[k], foreground, background);
                        char_count++;
                    }
                }
                break;
            }
            case 'c': {
                char c = (char)va_arg(args, int); // char is promoted to int in va_arg
                putcharColoured(c, foreground, background);
                char_count++;
                break;
            }
            case '%': {
                putcharColoured('%', foreground, background);
                char_count++;
                break;
            }
            default: { // Print % and the char if unknown specifier
                putcharColoured('%', foreground, background);
                char_count++;
                if (fmt[i]) { // Check if fmt[i] is not the null terminator
                    putcharColoured(fmt[i], foreground, background);
                    char_count++;
                } else {
                    // If fmt[i] is null, it means format string ended with %
                    // Behavior here can be to just stop or print % then stop.
                    // Current loop structure will break, so i-- might not be needed.
                }
                break;
            }
            }
        } else {
            putcharColoured(fmt[i], foreground, background);
            char_count++;
        }
        if (!fmt[i])
            break; // Defensive break, though while(fmt[i]) should handle it.
        i++;
    }
    return char_count;
}

uint64_t printf_color(const char *fmt, uint64_t foreground, uint64_t background, ...) {
    va_list args;
    va_start(args, background);
    uint64_t ret = vprintf_color(fmt, foreground, background, args);
    va_end(args);
    return ret;
}

void printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf_color(fmt, 0xFFFFFF, 0x000000, args);
    va_end(args);
}

void printInt(int num) {
    char buffer[40];
    uintToBase(num, buffer, 10);
    printf(buffer);
}

// void simplePrintf(char* str){
//    for (int i=0; str[i]; i++) putchar(str[i]);
// }