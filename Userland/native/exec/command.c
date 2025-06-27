// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/command.h"
#include "../include/eliminator.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/syscalls.h" // Includes definitions.h for ps_info_t and enums
#include "../include/tests.h"

#define REG_SIZE 17
#define SECONDS 0
#define MINUTES 2
#define HOURS 4
#define DAY_OF_THE_WEEK 6
#define DAY_OF_THE_MONTH 7
#define MONTH 8
#define YEAR 9

#define PS_COMMAND_MAX_PROCESS_DISPLAY 16 // Reduced from 64 to 16 to prevent stack overflow

// void print_help() {
//     printf("Presiona left alt para guardar registros en cualquier momento\n");
//     printf("Comandos disponibles:\n");
//     for (int i = 0; i < sizeof(commands) / sizeof(command); i++) {
//         printf(commands[i].title);
//         printf(" : ");
//         printf(commands[i].desc);
//         printf("\n");
//     }
// }

void clear() {
    sys_clear();
}

void divideByZero() {
    runZeroDivisionException();
}

void invalidOpcode() {
    runInvalidOpcodeException();
}

void changeSize_1() {
    sys_new_size(1);
}
void changeSize_2() {
    sys_new_size(2);
}
void changeSize_3() {
    sys_new_size(3);
}
void changeSize_4() {
    sys_new_size(4);
}
void changeSize_5() {
    sys_new_size(5);
}

void test_mem(uint64_t argc, char *argv[]) {
    if (argc != 1) {
        printf("uso: testmem <n>\n");
        return;
    }

    test_mm(argv[0] ? satoi(argv[0]) : 0); //{n, 0, 0}
}

void test_proc(uint64_t argc, char *argv[]) {
    if (argc != 1) {
        printf("uso: testproc <n>\n");
        return;
    }

    if (satoi(argv[0]) <= 0 || satoi(argv[0]) > 15) {
        printf("n debe estar entre 1 y %d\n", 15);
        return;
    }

    test_processes(satoi(argv[0]), NULL);
}

void test_prior() {
    // int pid = sys_exec((void *)&test_prio, NULL, 0, "test_prio", 1);
    // sys_waitpid(pid); // Espera a que el proceso termine
    test_prio();
}

void test_syncro(uint64_t argc, char *argv[]) {
    if (argc != 2) {
        printf("uso: testsync <n> <use_sem>\n");
        return;
    }
    test_sync(argc, argv); //{n, use_sem, 0}
    // my_test_sync();
}

void time() {

    int hours = sys_time(HOURS);
    // hours -= 3;          // el tema con pasar a horario de argentina es que habría que cambiar el día (y hasta el año si es 31/12) también, y es todito un lío
    // if(hours <= 0){
    //     hours += 24;
    // }
    printInt(hours);
    printf(":");
    int minutes = sys_time(MINUTES);
    if (minutes < 10)
        printf("0");
    printInt(minutes);
    printf(", ");
    printInt(sys_time(DAY_OF_THE_MONTH));
    printf("/");
    printInt(sys_time(MONTH));
    printf("/");
    printInt(sys_time(YEAR) + 2000);
    printf("\n");
}

void ps_command() {
    ps_info_t processes[PS_COMMAND_MAX_PROCESS_DISPLAY];
    int count = sys_get_all_processes_info(processes, PS_COMMAND_MAX_PROCESS_DISPLAY);

    if (count < 0) {
        printf("Error retrieving process list\n");
        return;
    }
    if (count == 0) {
        printf("No processes to display.\n");
        return;
    }

    printf("PID  | PPID | PRI | STATE   | FG/BG | SP                | BP                | NAME            \n");
    printf("-----|------|-----|---------|-------|-------------------|-------------------|-----------------\n");

    for (int i = 0; i < count; i++) {
        char *state_str;
        switch (processes[i].state) {
        case blocked:
            state_str = "BLOCKED";
            break;
        case ready:
            state_str = "READY";
            break;
        case killed:
            state_str = "KILLED";
            break;
        case running:
            state_str = "RUNNING";
            break;
        case waiting_for_child:
            state_str = "WAITING";
            break;
        default:
            state_str = "UNKNOWN";
            break;
        }
        char *fg_bg_str = (processes[i].fg == foreground) ? "FG" : "BG";

        printf("%d | %d | %d | %s | %s | %s | %s | %s \n",
            processes[i].pid,
            processes[i].parent_pid,
            processes[i].priority,
            state_str,
            fg_bg_str,
            processes[i].stack_pointer_str,
            processes[i].base_pointer_str,
            processes[i].name);
    }
}

void mem_command() {
    uint64_t *info = sys_get_mem_info();
    if (info == NULL) {
        printf("Error: Unable to get memory information\n");
        return;
    }

    printf("Memory State:\n");
    printf("=============\n");
    printf("Total Memory:     %u bytes\n", (unsigned int)info[0]);
    printf("Used Memory:      %u bytes\n", (unsigned int)info[1]);
    printf("Free Memory:      %u bytes\n", (unsigned int)(info[0] - info[1]));

    // Calculate percentages
    if (info[0] > 0) {
        uint64_t used_percent = (info[1] * 100) / info[0];
        uint64_t free_percent = 100 - used_percent;
        printf("Memory Usage:     %u%% used, %u%% free\n", (unsigned int)used_percent, (unsigned int)free_percent);
    }

    // Show in more readable units if the numbers are large
    if (info[0] >= 1024 * 1024) {
        printf("\nIn MiB:\n");
        printf("Total Memory:     %u MiB\n", (unsigned int)(info[0] / (1024 * 1024)));
        printf("Used Memory:      %u MiB\n", (unsigned int)(info[1] / (1024 * 1024)));
        printf("Free Memory:      %u MiB\n", (unsigned int)((info[0] - info[1]) / (1024 * 1024)));
    } else if (info[0] >= 1024) {
        printf("\nIn KiB:\n");
        printf("Total Memory:     %u KiB\n", (unsigned int)(info[0] / 1024));
        printf("Used Memory:      %u KiB\n", (unsigned int)(info[1] / 1024));
        printf("Free Memory:      %u KiB\n", (unsigned int)((info[0] - info[1]) / 1024));
    }

    // Memory chunks information (if available)
    if (info[2] > 0) {
        printf("Memory Chunks:    %u\n", (unsigned int)info[2]);
    }
}

int loop_command(int argc, char **argv) {
    if (argc != 0) {
        printf("Usage: loop (no arguments)\n");
        return 1; // Return non-zero for error
    }
    uint64_t pid = sys_get_pid();
    while (1) {
        printf("Process ID: %d says hello!\n", (int)pid);
        sys_sleep(30000); // Sleep for 30 seconds
    }
    return 0; // Should not be reached
}

int kill_command(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: kill <pid>\n");
        return 1; // Return non-zero for error
    }
    int64_t pid_to_kill = satoi(argv[0]);
    if (sys_kill(pid_to_kill) == 0) {
        printf("Process %d killed successfully.\n", (int)pid_to_kill);
    } else {
        printf("Failed to kill process %d. It might not exist or you lack permissions.\n", (int)pid_to_kill);
    }
    return 0;
}

int nice_command(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: nice <pid> <priority>\n");
        return 1; // Return non-zero for error
    }
    uint64_t pid = satoi(argv[0]);
    uint8_t new_priority = satoi(argv[1]);

    if (new_priority < low || new_priority > high) {
        printf("Invalid priority. Must be between %d and %d.\n", (int)low, (int)high);
        return 1; // Return non-zero for error
    }

    if (sys_nice(pid, new_priority) == 1) { // Assuming 1 is success for nice
        printf("Priority of process %d changed to %d.\n", (int)pid, (int)new_priority);
    } else {
        printf("Failed to change priority for process %d.\n", (int)pid);
    }
    return 0;
}

int block_command(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: block <pid>\n");
        return 1; // Return non-zero for error
    }
    int64_t pid = satoi(argv[0]);
    int result = sys_toggle_block_state(pid);

    if (result == 0) { // Assuming 0 is success for toggle
        printf("Process %d block/unblock toggled successfully.\n", (int)pid);
    } else {
        printf("Failed to toggle block state for process %d.\n", (int)pid);
    }
    return 0;
}

int echo(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        printf("%s", argv[i]);
        if (i < argc - 1) {
            printf(" ");
        }
    }
    printf("\n");
    return 0;
}

int cat(int argc, char **argv) {
    // Check if stdin is connected to a pipe
    if (sys_get_current_fd(STDIN) != STDIN) {
        // Reading from pipe - original behavior
        int input = 0;
        while ((input = getchar()) != EOF) {
            putchar(input);
        }
        return 0;
    }
    
    // Reading from terminal - interactive mode
    char buff[MAX_BUF];
    memset(buff, 0, MAX_BUF);
    int i = 0;
    int break_input = 0;
    
    while (!break_input && i < MAX_BUF - 1) {
        char c = 0;
        if (sys_read(0, &c, 1) > 0) {
            if (c == EOF) { // EOF from Ctrl+D
                break_input = 1;
            } else if (c == '\n') {
                buff[i++] = c;
                putchar('\n');
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
    
    if (i == 0) {
        return 0; // No input to echo
    }

    if (i > 0 && buff[i - 1] != '\n') {
        putchar('\n');
    }
    // Echo the collected input
    for (int j = 0; j < i; j++) {
        putchar(buff[j]);
    }
    if (i > 0 && buff[i - 1] != '\n') {
        putchar('\n'); // Ensure final newline if not already present
    }
    return 0;
}

int wc(int argc, char **argv) {
    // Check if stdin is connected to a pipe
    int lines = 0;
    if (sys_get_current_fd(STDIN) != STDIN) {
        // Reading from pipe - original behavior
        int input = 0;
        while ((input = getchar()) != EOF) {
            if (input == '\n') {
                lines++;
            }
        }
        printf("Lines: %d\n", lines);
        return 0;
    }
    
    // Reading from terminal - interactive mode
    char buff[MAX_BUF];
    memset(buff, 0, MAX_BUF);
    int i = 0;
    int break_input = 0;
    
    while (!break_input && i < MAX_BUF - 1) {
        char c = 0;
        if (sys_read(0, &c, 1) > 0) {
            if (c == EOF) { // EOF from Ctrl+D
                break_input = 1;
            } else if (c == '\n') {
                buff[i++] = c;
                putchar('\n');
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
    if (i == 0) {
        printf("Lines: 0\n");
        return 0; // No input, no lines
    }

    if (buff[i - 1] != '\n') {
        putchar('\n');
    }
    lines = 1; // Start with 1 line if there's any input
    for (int j = 0; j < i; j++) {
        if (buff[j] == '\n') {
            lines++;
        }
    }
    printf("Lines: %d\n", lines);
    return 0;
}

int filter(int argc, char **argv) {
    if (sys_get_current_fd(STDIN) != STDIN) {
    // Reading from pipe - original behavior
        int input = 0;
        while ((input = getchar()) != EOF) {
            if (!isVowel(input)) {
                putchar(input); // Echo the character if it matches the filter
            }
        }
        return 0;
    }
    // Reading from terminal - interactive mode
    char buff[MAX_BUF];
    memset(buff, 0, MAX_BUF);
    int i = 0;
    int break_input = 0;
    
    while (!break_input && i < MAX_BUF - 1) {
        char c = 0;
        if (sys_read(0, &c, 1) > 0) {
            if (c == EOF) { // EOF from Ctrl+D
                break_input = 1;
            } else if (c == '\n') {
                buff[i++] = c;
                putchar('\n');
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

    if (i == 0) {
        return 0; // No input to filter
    }

    if (i > 0 && buff[i - 1] != '\n') {
        putchar('\n');
    }
    // Filter the collected input
    for (int j = 0; j < i; j++) {
        if (!isVowel(buff[j]) || buff[j] == '\n') {
            putchar(buff[j]);
        } 
    }
    if (i > 0 && buff[i - 1] != '\n') {
        putchar('\n'); // Ensure final newline if not already present
    }
}