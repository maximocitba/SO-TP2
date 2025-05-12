#include "IO.h"
#include "video.h"
#include <stdint.h>
#include "font.h"
#include "interrupts.h"
#include "textmode.h"


#define SIZE_BUFFER 1000

uint64_t x = 0;
uint64_t y = 0;
uint8_t fontSize = 1;

// Flag to track if we're in text mode (no graphics)
static int text_mode = 1;  // Default to text mode since we switched to it

// putChar y printf facilitan el manejo de IO cuando el Kernel quiere escribir cosas
void putChar(char c) {
    if (text_mode) {
        textmode_putchar(c);
    } else {
        putCharColoured(c, 0xFFFFFF, BG_COLOR);
    }
}

void printf(char * str) {
    if (text_mode) {
        textmode_write(str);
    } else {
        for (int i = 0; str[i] != '\0'; i++) {
            putChar(str[i]);
        }
    }
}

// Implementation of putCharColoured function
void putCharColoured(char c, uint64_t foreGround, uint64_t backGround) {
    if (text_mode) {
        // In text mode, just use the base putChar functionality
        textmode_putchar(c);
    } else {
        // Handle special characters
        if (c == '\n') {
            newLine(&x, &y);
        } else if (c == '\t') {
            for (int i = 0; i < 4; i++) {
                putCharAt(' ', &x, &y, foreGround, backGround);
            }
        } else if (c == '\b') {
            deleteCharAt(&x, &y, backGround);
        } else {
            putCharAt(c, &x, &y, foreGround, backGround);
        }
    }
}

// Forward declaration
void newLine(uint64_t * x, uint64_t * y);
void deleteCharAt(uint64_t * x, uint64_t * y, uint64_t backgroundColor);
void putCharAt(uint8_t c, uint64_t * x, uint64_t * y, uint64_t foreColor, uint64_t backgroundColor);
int xOutOfBounds(uint64_t * x);
int yOutOfBounds(uint64_t * y);


// el stdout no se guarda. Solo se guardan las coordenadas de la última posición

// el stdin es un array cíclico
static char stdinArr[SIZE_BUFFER];  
static int sizeIn = 0;
static int startsIn = 0;

static char videoModeOn = 0;


// Funciones que manejan stdin, stdout y stderr
void putOut(char c){
    putCharColoured(c, 0x00ffffff, BG_COLOR);
}


void putErr(char c){
    putCharColoured(c, 0x00ff0000, BG_COLOR);
}

void putIn(char c){
    // caso especial donde se pasa del límite: no se pueden agregar caracteres
    if (sizeIn >= SIZE_BUFFER-1) return;

    // mete c en el vector cíclico
    int pos = (startsIn + sizeIn) % SIZE_BUFFER;

    stdinArr[pos] = c;
    sizeIn++;
}

void clearIn(){
    sizeIn = 0;
}



// inspirado en la función de la API de Linux
void sys_write(int fd, const char* buf, int count) {
    if (fd == 1) { // stdout
        for (int i = 0; i < count; i++) {
            putChar(buf[i]);
        }
    } else if (fd == 2) { // stderr
        for (int i = 0; i < count; i++) {
            putCharColoured(buf[i], 0xFF0000, BG_COLOR); // Red color for stderr
        }
    }
}

// inspirado en la función de la API de Linux
int sys_read(int fd, char* buf, int count){
    int i=0;
    if (fd==0){
        for(i=0; i<count && i<sizeIn; i++){
            buf[i] = stdinArr[(startsIn + i)%SIZE_BUFFER];

        }
        startsIn+=i;
        startsIn = startsIn%SIZE_BUFFER;
        sizeIn-=i;
    }
    return i;
}


void sys_new_size(int newSize){
    _cli();               //por las dudas paro los in/out (antes generaba bugs)
    if (newSize < 1 || newSize > 5)
        return;
    fontSize = newSize;
    sys_clearScreen();
    _sti();
}

// imprime caracter y modifica coordenadas. Usado para representar el stdout
void putCharAt(uint8_t c, uint64_t * x, uint64_t * y, uint64_t foreColor, uint64_t backgroundColor) {
    if (xOutOfBounds(x)) {
        newLine(x, y);
    }

    if (yOutOfBounds(y)) {
        clearScreen(backgroundColor);
        *y = 0;
    }

    uint8_t charMap[FONT_HEIGHT][FONT_WIDTH];
    getCharMap(c, charMap);

    for (int i = 0; i < FONT_HEIGHT ; i++) {
        for (int j = 0; j < FONT_WIDTH ; j++) {
            uint8_t squareIsOn = charMap[i][j];
            putSquare(squareIsOn ? foreColor : backgroundColor, *x + j * fontSize, *y + i * fontSize, fontSize);
        }
    }
    *x += FONT_WIDTH * fontSize;
}

void deleteCharAt(uint64_t * x, uint64_t * y, uint64_t backgroundColor) {
    if (*x == 0 && *y == 0) return;         // no borra si no hay nada
    *x -= FONT_WIDTH * fontSize;
    if (xOutOfBounds(x)) {
        int lastX = ((getWidth() -1) / (FONT_WIDTH * fontSize) - 1 ) * (FONT_WIDTH * fontSize);  // esto hay que ponerlo en otro lado
        *x = lastX;
        *y -= FONT_HEIGHT * fontSize;
    }
    putCharAt(' ', x, y, backgroundColor, backgroundColor);
    *x -= FONT_WIDTH * fontSize;
}


void newLine(uint64_t * x, uint64_t * y) {
    *x = 0;
    *y += FONT_HEIGHT * fontSize;
}


int xOutOfBounds(uint64_t * x) {
    return *x + FONT_WIDTH * fontSize >= getWidth() || (int)*x < 0;     // casteo a int para que me tome que existen los negativos
}


int yOutOfBounds(uint64_t * y) {
    return *y + FONT_HEIGHT * fontSize >= getHeight() || (int)*y < 0;
}

// resetea coordenadas
void sys_clearScreen(){
    if (text_mode) {
        textmode_clear();
    } else {
        clearScreen(BG_COLOR);
    }
    y = 0;
    x = 0;
}
