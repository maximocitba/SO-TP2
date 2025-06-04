#include "../include/shell.h"
#include "../include/syscalls.h"
#include "../include/command.h"
#include "../include/eliminator.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/string.h"
#include "../include/sounds.h"



#define MAX_BUF 1024

// se podría cambiar la implementación para que se acepten parámetros (y no haya que hacer size 1, size 2...)
// pero en este caso no hace falta porque hay pocos parámetros

// char * commands[] = {"clear", "dividebyzero", "help", "inforeg", "invalidopcode", "time", "eliminator",
//                      "size_1", "size_2", "size_3", "size_4", "size_5", "testmem", "testproc", "testprio", "testsync"};
// void (*commands_functions[])() = {clear,  divideByZero, print_help, sys_getRegs, invalidOpcode, time, eliminator,
//                                   changeSize_1, changeSize_2, changeSize_3, changeSize_4, changeSize_5, test_mem, 
//                                   test_proc, test_prior, test_syncro};

command_t commands[] = {
    {"help", "shows this help", (function) print_help},
    {"clear", "clears the screen", (function) clear},
    {"time", "shows the current time", (function) time},
};

static void parse_buffer(char *buff, parsed_input_t *parsed);
static int execute_command(parsed_input_t *parsed);

// ...existing includes and definitions...

void shell() {
    // play_song(2);
    // while (next_part());
    printHeader();

    char buff[MAX_BUF] = {0};
    parsed_input_t parsed;

    while (1) {
        printf_color("user", 0xcdff00, 0x000000);
        printf(":~$ ");

        int i = 0;
        memset(buff, 0, MAX_BUF);
        int break_line = 0;
        while (!break_line && i < MAX_BUF - 1) {
            char c = 0;
            if (sys_read(0, &c, 1) > 0) {
                if (c == '\n') {
                    putchar('\n');
                    break_line = 1;
                } else if (c == 0x08 && i > 0) { // Backspace
                    putchar(0x08);
                    i--;
                    buff[i] = 0;
                } else if (c == 0x08 && i == 0) {
                    // do nothing, can't backspace past start
                } else if (c >= 0x20 && c <= 0x7E) { // Printable
                    putchar(c);
                    buff[i++] = c;
                }
            }
        }
        parse_buffer(buff, &parsed);
        execute_command(&parsed);
    }
}

static void parse_buffer(char *buff, parsed_input_t *parsed) {
    if (!buff || !parsed) return;
    
    memset(parsed, 0, sizeof(parsed_input_t));
    
    int len = strlen(buff);
    while (len > 0 && (buff[len-1] == ' ' || buff[len-1] == '\n')) {
        buff[--len] = '\0';
    }
    
    if (len > 0 && buff[len-1] == '&') {
        parsed->is_bg = 1;
        buff[--len] = '\0';
        while (len > 0 && buff[len-1] == ' ') {
            buff[--len] = '\0';
        }
    }
    
    char *cmd_str = buff;
    char *pipe_pos;
    
    while (cmd_str && *cmd_str && parsed->cmd_count < max_cmds) {
        while (*cmd_str == ' ') cmd_str++;
        if (!*cmd_str) break;
        
        pipe_pos = strchr(cmd_str, '|');
        if (pipe_pos) {
            *pipe_pos = '\0';
        }
        
        command_input_t *current_cmd = &parsed->cmds[parsed->cmd_count];
        
        char *token = cmd_str;
        char *next_token = NULL;
        int arg_count = 0;

        while (*token == ' ') token++;
        next_token = strchr(token, ' ');
        if (next_token) {
            *next_token = '\0';
            next_token++;
        }
        current_cmd->cmd = token;
        
        if (next_token) {
            token = next_token;
            while (*token && arg_count < max_args - 1) { 
                while (*token == ' ') token++;
                if (!*token) break;
                
                next_token = strchr(token, ' ');
                if (next_token) {
                    *next_token = '\0';
                    next_token++;
                }
                
                current_cmd->argv[arg_count++] = token;
                
                if (!next_token) break;
                token = next_token;
            }
        }
        
        current_cmd->argc = arg_count;
        current_cmd->argv[arg_count] = NULL;
        
        parsed->cmd_count++;
        
        if (pipe_pos) {
            cmd_str = pipe_pos + 1;
        } else {
            break;
        }
    }
}

static int execute_command(parsed_input_t *parsed) {
    if (!parsed || parsed->cmd_count == 0) return 0;

    if (parsed->cmd_count == 1) {
        command_input_t *current = &parsed->cmds[0];
        
        for (int i = 0; i < sizeof(commands) / sizeof(command_t); i++) {
            if (strcmp(commands[i].name, current->cmd) == 0) {
                int pid;
                
                if (parsed->is_bg) {
                    pid = sys_exec((void *)commands[i].cmd, current->argv, current->argc, 
                             commands[i].name, DEFAULT_PRIORITY);
                
                    sys_block(pid);
                    // change_process_fd(pid, STDIN, DEV_NULL);
                    // set_bg(pid);
                    sys_unblock(pid);
                    printf("[%d] running in background\n", pid);
                } else {
                    // pid = sys_exec((void *)commands[i].cmd, current->argv, current->argc, 
                    //          commands[i].name, DEFAULT_PRIORITY);

                    // printf("Executing '%s' with PID %d\n", current->cmd, pid);
                    test_proc();
                    // sys_waitpid(pid);
                }
                return 0;
            }
        }
        printf("'%s' command not found\n", current->cmd);
        return 0;
    }

    // uint16_t pipe_id = create_pipe();

    // int pids[max_cmds] = {0};
    
    // for (int i = 0; i < parsed->cmd_count; i++) {
    //     command_input_t *current = &parsed->cmds[i];
    //     int found = 0;
    //     for (int j = 0; j < sizeof(commands) / sizeof(command_t); j++) {
    //         if (strcmp(commands[j].name, current->cmd) == 0) {
    //             pids[i] = exec((void *)commands[j].cmd, current->argv, current->argc, 
    //                          commands[j].name, DEFAULT_PRIORITY);
    //             block(pids[i]);
                
    //             if (i == 0) {
    //                 open_pipe(pids[i], pipe_id, write_mode);
    //                 change_process_fd(pids[i], STDOUT, pipe_id);
    //             }

    //             if (i == 1) {
    //                 open_pipe(pids[i], pipe_id, read_mode);
    //                 change_process_fd(pids[i], STDIN, pipe_id);
    //             }

    //             found = 1;
    //             break;
    //         }
    //     }
    //     if (!found) {
    //         printf("'%s' command not found", current->cmd);
    //         return 0;
    //     }
    // }

    // for (int i = 0; i < parsed->cmd_count; i++) {
    //     unblock(pids[i]);
    // }
    
    // if (!parsed->is_bg) {
    //     waitpid(pids[0]);
    //     close_pipe_by_pid(pids[0], pipe_id);
    //     waitpid(pids[1]);
    //     close_pipe_by_pid(pids[1], pipe_id);
    // } 
   
    return 0;
}


void printHeader() {
    printf_color("Bienvenido a la Shell!\n", 0xcdff00, 0x000000);
    printf("Ejecuta 'help' para ver una lista de comandos.\n");
    return;
}

// void execute(const char *inputBuffer) {
//     for (int i = 0; i < 16 ; i++)
//     {
//         if (strcmp(inputBuffer, commands[i]) == 0)
//         {
//             commands_functions[i]();
//             return;
//         }
//     }
//     printf("Invalid command, try again.\n");
// }

int print_help() {

    printf("command_t: description\n");
    int size = sizeof(commands) / sizeof(command_t);
    for (int i = 1; i < size; i++) {
        printf("%s: %s", commands[i].name, commands[i].description);
        if (i < size - 1) {
            putchar('\n');
        }
    }
    return 0;
}
