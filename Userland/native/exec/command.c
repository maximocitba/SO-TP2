#include "../include/command.h"
#include "../include/eliminator.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/syscalls.h"
#include "../include/tests.h"

#define REG_SIZE 17
#define SECONDS 0
#define MINUTES 2
#define HOURS 4
#define DAY_OF_THE_WEEK 6
#define DAY_OF_THE_MONTH 7
#define MONTH 8
#define YEAR 9

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

void test_mem() {
    test_mm(500);
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
        printf("usage: test_synchro <n> <sem>");
        return 0;
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