#ifndef TPE_SHELL_H
#define TPE_SHELL_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/definitions.h"
#include <stddef.h>


#define max_cmds 2
#define max_args 10

typedef struct command {
	char* name; 
	char* description;
	function cmd;
} command_t;

typedef struct {
    char *cmd;
    char *argv[max_args];
    int argc;
} command_input_t;

typedef struct {
    command_input_t cmds[max_cmds];
    uint8_t cmd_count;
    uint8_t is_bg;
} parsed_input_t;


void shell();
int print_help();
int print_hola();
void printHeader();
// void execute(const char *inputBuffer);


#endif //TPE_SHELL_H