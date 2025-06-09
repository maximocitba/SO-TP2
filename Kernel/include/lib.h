#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <string.h>
#include <unistd.h>

void *memset(void *destination, int32_t character, uint64_t length);
void *memcpy(void *destination, const void *source, uint64_t length);

int buflen(const char *s);
void getRegs();
void sys_registers();

// assembler (libasm.asm)
char *cpuVendor(char *result);

void outb(unsigned char __value, unsigned short int __port);
unsigned char inb(unsigned short int __port);

void int_to_string(int num, char *str);
void print_number(int number);
void pointer_to_string(void *ptr, char *buffer, size_t buffer_size);

#endif