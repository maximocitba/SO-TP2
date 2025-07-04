// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "naiveConsole.h"

// VGA text mode video memory address
#define VGA_VIDEO_MEMORY 0xB8000UL

static char buffer[64] = { '0' };  //PVS Intentionally initialize first element to '0' character
static uint8_t * const video = (uint8_t*)(uintptr_t)VGA_VIDEO_MEMORY; //PVS Ignore the warning about casting to void* from uintptr_t, as this is intentional for memory address representation.
static uint8_t * currentVideo = (uint8_t*)(uintptr_t)VGA_VIDEO_MEMORY; //PVS Ignore the warning about casting to void* from uintptr_t, as this is intentional for memory address representation.
static const uint32_t width = 80;
static const uint32_t height = 25 ;

void ncPrint(const char * string)
{
	int i;

	for (i = 0; string[i] != 0; i++)
		ncPrintChar(string[i]);
}

void writeCharacter(unsigned char c, unsigned char forecolour, unsigned char backcolour)
{
    uint16_t attrib = (backcolour << 4) | (forecolour & 0x0F);

    *currentVideo = c;
    *(currentVideo+1) = attrib;
    currentVideo+=2;
}

void writeStr(char * str, unsigned char forecolour, unsigned char backcolour) {
    for (int i = 0 ; *(str+i) != '\0' ; i++) {
        writeCharacter(*(str+i), forecolour, backcolour);
    }
}

void ncPrintChar(char character)
{
	*currentVideo = character;
	currentVideo += 2;
}

void ncNewline()
{
	do
	{
		ncPrintChar(' ');
	}
	while((uint64_t)(currentVideo - video) % (width * 2) != 0);
}

void ncPrintDec(uint64_t value)
{
	ncPrintBase(value, 10);
}

void ncPrintHex(uint64_t value)
{
	ncPrintBase(value, 16);
}

void ncPrintBin(uint64_t value)
{
	ncPrintBase(value, 2);
}

void ncPrintBase(uint64_t value, uint32_t base)
{
    uintToBase(value, buffer, base);
    ncPrint(buffer);
}

void ncClear()
{
	int i;

	for (i = 0; i < height * width; i++)
		video[i * 2] = ' ';
	currentVideo = video;
}

uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base) {
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}
