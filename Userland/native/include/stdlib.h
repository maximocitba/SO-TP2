#ifndef TPE_STDLIB_H
#define TPE_STDLIB_H

#include <stdint.h>

#define MAX_BUF 1024

void putchar(char c);
void putcharColoured(char c, uint64_t foreground, uint64_t background);
uint64_t getchar();

uint64_t gets(char * buf, uint64_t length);

uint64_t atoi(char * str);
void itoa(uint64_t num, char * buf);
// Convert integer to string with specified base (e.g., 10 for decimal, 16 for hex)
void itoa_base(uint64_t num, char * buf, int base);
uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);
void putSquare(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t size);
char getKey();

#endif //TPE_STDLIB_H
