// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>

extern char bss;
extern char endOfBinary;


int main();

void * memset(void * destiny, int32_t c, uint64_t length);

int _start() {
	//Clean BSS
	memset(&bss, 0, &endOfBinary - &bss);

	return main();

}


