// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "video.h"
#include "font.h"

#define OUT_BACK_COLOR 0x00ffffff

struct vbe_mode_info_structure {
    uint16_t attributes; // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
    uint8_t window_a; // deprecated
    uint8_t window_b; // deprecated
    uint16_t granularity; // deprecated; used while calculating bank numbers
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr; // deprecated; used to switch banks from protected mode without returning to real mode
    uint16_t pitch; // number of bytes per horizontal line
    uint16_t width; // width in pixels
    uint16_t height; // height in pixels
    uint8_t w_char; // unused...
    uint8_t y_char; // ...
    uint8_t planes;
    uint8_t bpp; // bits per pixel in this mode
    uint8_t banks; // deprecated; total number of banks in this mode
    uint8_t memory_model;
    uint8_t bank_size; // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
    uint8_t image_pages;
    uint8_t reserved0;

    uint8_t red_mask;
    uint8_t red_position;
    uint8_t green_mask;
    uint8_t green_position;
    uint8_t blue_mask;
    uint8_t blue_position;
    uint8_t reserved_mask;
    uint8_t reserved_position;
    uint8_t direct_color_attributes;

    uint32_t framebuffer; // physical address of the linear frame buffer; write here to draw to the screen
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size; // size of memory in the framebuffer but not being displayed on the screen
    uint8_t reserved1[206];
} __attribute__((packed));

typedef struct vbe_mode_info_structure *VBEInfoPtr;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr)0x0000000000005C00;

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y) {
    uint8_t *framebuffer = (uint8_t *)((uint64_t)VBE_mode_info->framebuffer);
    uint64_t offset = (x * ((VBE_mode_info->bpp) / 8)) + (y * VBE_mode_info->pitch);
    framebuffer[offset] = (hexColor) & 0xFF;
    framebuffer[offset + 1] = (hexColor >> 8) & 0xFF;
    framebuffer[offset + 2] = (hexColor >> 16) & 0xFF;
}

void putSquare(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            putPixel(hexColor, x + j, y + i);
        }
    }
}

void clearScreen(uint32_t hexColor) {
    for (int i = 0; i < VBE_mode_info->height; i++) {
        for (int j = 0; j < VBE_mode_info->width; j++) {
            putPixel(hexColor, j, i);
        }
    }
}

int getWidth() {
    return VBE_mode_info->width;
}

int getHeight() {
    return VBE_mode_info->height;
}

// 0 --> getWidth   (o cualquier cosa que no sea 1)
// 1 --> getHeight
int sys_getSize(int id) {
    return id == 1 ? getHeight() : getWidth();
}