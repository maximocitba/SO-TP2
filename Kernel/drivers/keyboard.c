#include "keyboard.h"
#include "IO.h"
#include <stdint.h>

// PS/2 Set 1 Scan code definitions
#define PS2_KEY_RELEASE      0x80    // The high bit is set when a key is released
#define PS2_LEFT_SHIFT       0x2A
#define PS2_RIGHT_SHIFT      0x36
#define PS2_CAPS_LOCK        0x3A
#define PS2_EXTENDED         0xE0    // Extended key prefix

// Keyboard state
static int shift_active = 0;        // 1 if shift is currently pressed, 0 otherwise
static int caps_lock_active = 0;    // 1 if caps lock is active, 0 otherwise
static int extended_key = 0;        // 1 if processing an extended key sequence

// US keyboard layout scancodes to ASCII mapping (set 1)
static const uint8_t ascii_map[] = {
    0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0,   '*', 0,   ' '
};

static const uint8_t shift_ascii_map[] = {
    0,   0,   '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0,   '*', 0,   ' '
};

// Process a keyboard scan code and handle special keys like shift
static void process_scan_code(uint8_t scan_code) {
    // Check if it's a key release (bit 7 set)
    int is_release = (scan_code & PS2_KEY_RELEASE) != 0;
    uint8_t key = scan_code & ~PS2_KEY_RELEASE;  // Clear the release bit

    // Handle extended key sequences
    if (scan_code == PS2_EXTENDED) {
        extended_key = 1;
        return;
    }

    // Handle extended key release
    if (extended_key && is_release) {
        extended_key = 0;
        return;
    }

    // Handle extended keys
    if (extended_key) {
        extended_key = 0;
        return;  // For now we ignore extended keys
    }

    // Handle shift key press/release
    if (key == PS2_LEFT_SHIFT || key == PS2_RIGHT_SHIFT) {
        shift_active = !is_release;
        return;
    }

    // Handle caps lock (toggle on key press, not on release)
    if (key == PS2_CAPS_LOCK && !is_release) {
        caps_lock_active = !caps_lock_active;
        return;
    }

    // Only process key presses (not releases) for regular keys
    if (!is_release && key < sizeof(ascii_map)) {
        char c;
        
        // Apply shift and caps lock - caps lock only affects letters
        if (shift_active) {
            c = shift_ascii_map[key];
        } else {
            c = ascii_map[key];
            
            // Apply caps lock for letters only
            if (caps_lock_active && c >= 'a' && c <= 'z') {
                c = c - 'a' + 'A';  // Convert to uppercase
            }
        }
        
        // Send character to input buffer if it's not 0
        if (c != 0) {
            putIn(c);
        }
    }
}

void keyboard_handler() {
    // Get the scan code from the keyboard controller
    uint8_t scan_code = getPressedKey();
    
    // If scan_code is 0, no key was pressed/released
    if (scan_code == 0) {
        return;
    }
    
    // Process the scan code
    process_scan_code(scan_code);
}