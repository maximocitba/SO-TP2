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
        printf("usage: test_mm <n>\n");
        return;
    }

    test_mm(argv[0] ? satoi(argv[0]) : 0); //{n, 0, 0}
}

void test_proc() {

    test_processes(5, NULL);
}

void test_prior() {
    // int pid = sys_exec((void *)&test_prio, NULL, 0, "test_prio", 1);
    // sys_waitpid(pid); // Espera a que el proceso termine
    test_prio();
}

void test_syncro(uint64_t argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: test_synchro <n> <use_sem>\n");
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

        // Removed format specifiers for alignment and width
        printf("%d | %d | %d | %s | %s | %s | %s | %s\n",
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

int loop_command(int argc, char **argv) {
    if (argc != 0) {
        printf("Usage: loop (no arguments)\n");
        return 1; // Return non-zero for error
    }
    uint64_t pid = sys_get_pid();
    while (1) {
        printf("Process ID: %d says hello!\n", pid);
        sys_sleep(30000); // Sleep for 30 seconds
    }
    return 0; // Should not be reached
}

int kill_command(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: kill <pid>\n");
        return 1; // Return non-zero for error
    }
    uint64_t pid_to_kill = satoi(argv[0]);
    if (sys_kill(pid_to_kill) == 0) {
        printf("Process %d killed successfully.\n", pid_to_kill);
    } else {
        printf("Failed to kill process %d. It might not exist or you lack permissions.\n", pid_to_kill);
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
        printf("Invalid priority. Must be between %d and %d.\n", low, high);
        return 1; // Return non-zero for error
    }

    if (sys_nice(pid, new_priority) == 1) { // Assuming 1 is success for nice
        printf("Priority of process %d changed to %d.\n", pid, new_priority);
    } else {
        printf("Failed to change priority for process %d.\n", pid);
    }
    return 0;
}

int block_command(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: block <pid>\n");
        return 1; // Return non-zero for error
    }
    uint64_t pid = satoi(argv[0]);
    int result = sys_toggle_block_state(pid);

    if (result == 0) { // Assuming 0 is success for toggle
        printf("Process %d block/unblock toggled successfully.\n", pid);
    } else {
        printf("Failed to toggle block state for process %d.\n", pid);
    }
    return 0;
}

int cat(int argc, char **argv) {
    char c;

    while (1) {
        sys_read(0, &c, 1);
        if (c == EOF) {
            return 0; // Exit on EOF
        }
        putchar(c); // Echo the character
    }

    return 0; // This line is never reached, but keeps the compiler happy
}