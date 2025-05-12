#include "textmode.h"
#include <stdint.h>

// VGA text mode buffer is at 0xB8000
static volatile uint16_t* const VGA_BUFFER = (uint16_t*)0xB8000;
static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 25;

// Current cursor position
static int cursor_x = 0;
static int cursor_y = 0;

// Default text color: white on black
static const uint8_t DEFAULT_COLOR = 0x07; // White text on black background

// Scroll the screen up one line
static void textmode_scroll() {
    // Move all lines up one line
    for (int y = 0; y < VGA_HEIGHT - 1; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[y * VGA_WIDTH + x] = VGA_BUFFER[(y + 1) * VGA_WIDTH + x];
        }
    }
    
    // Clear the last line
    for (int x = 0; x < VGA_WIDTH; x++) {
        VGA_BUFFER[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = (DEFAULT_COLOR << 8) | ' ';
    }
}

// Clear the screen
void textmode_clear() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[y * VGA_WIDTH + x] = (DEFAULT_COLOR << 8) | ' ';
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}

// Write a character to the screen at the current cursor position
void textmode_putchar(char c) {
    // Handle special characters
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        // Tab is 8 spaces
        cursor_x = (cursor_x + 8) & ~7;
    } else if (c == '\b') {
        // Backspace
        if (cursor_x > 0) {
            cursor_x--;
            VGA_BUFFER[cursor_y * VGA_WIDTH + cursor_x] = (DEFAULT_COLOR << 8) | ' ';
        } else if (cursor_y > 0) {
            cursor_y--;
            cursor_x = VGA_WIDTH - 1;
        }
    } else {
        // Normal character
        VGA_BUFFER[cursor_y * VGA_WIDTH + cursor_x] = (DEFAULT_COLOR << 8) | c;
        cursor_x++;
    }
    
    // Handle line wrap
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    
    // Handle scrolling
    if (cursor_y >= VGA_HEIGHT) {
        textmode_scroll();
        cursor_y = VGA_HEIGHT - 1;
    }
}

// Write a string to the screen
void textmode_write(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        textmode_putchar(str[i]);
    }
}