// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/string.h"

int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

char* strcat(char* destination, const char* source) {
    // Find the end of the destination string
    char* ptr = destination + strlen(destination);

    // Append the source string to the destination string
    while (*source != '\0') {
        *ptr++ = *source++;
    }

    // Null-terminate the resulting string
    *ptr = '\0';

    return destination;
}

int strlen(const char * str) {
    int len = 0;
    while (*str != '\0') {
        len++;
        str++;
    }
    return len;
}

char*
strcpy(char* destination, const char* source)
{
	char* w;
	for (w = destination; *source != '\0'; *(w++) = *(source++))
		continue;
	*w = '\0';
	return destination;
}