#ifndef TEXTMODE_H
#define TEXTMODE_H

#include <stdint.h>

// Text mode functions for direct VGA text buffer access
void textmode_clear();
void textmode_putchar(char c);
void textmode_write(const char* str);

#endif // TEXTMODE_H