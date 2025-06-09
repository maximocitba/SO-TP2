#include "../include/stdio.h"
#include "../include/command.h"
#include "../include/syscalls.h"
#include "../include/stdlib.h"
#include "../include/string.h"
#include "../include/definitions.h"

#define PHILOSOPHER_I_STR_MAX 16
#define SECONDS_THINK 2
#define SECONDS_EAT 1
#define MAX_PHILOSOPHER_COUNT 16
#define MIN_PHILOSOPHER_COUNT 3
#define INI_PHILOSOPHER_COUNT 5
#define CMD_ADD 'a'
#define CMD_REMOVE 'r'
#define CMD_PS 'p'
#define CMD_CLEAR 'c'
#define CMD_USAGE 'u'
#define CMD_EXIT 'q'
typedef enum {
    INVALID = 0,
    THINKING,
    HUNGRY,
    EATING,
} PHILOSOPHER_STATE;

int64_t mutex_sem_id = 0;
int64_t philosopher_count = 0;
PHILOSOPHER_STATE state[MAX_PHILOSOPHER_COUNT] = {0};
int64_t pids[MAX_PHILOSOPHER_COUNT] = {0};
int cleanup_in_progress = 0;

static int philosopher_sem_id(int i) 
{
    return mutex_sem_id + 1 + i;
}

static int left(int i) 
{
    return (i + philosopher_count - 1) % philosopher_count;
}

static int right(int i) 
{
    return (i + 1) % philosopher_count;
}

static void print_state() 
{
    for(int i = 0; i < philosopher_count; i++) 
    {
        if(state[i] == EATING)
            putchar('E');
        else if(state[i] == THINKING || state[i] == HUNGRY)
            putchar('.');
        else
            putchar('_');
        if(i < philosopher_count - 1)
            putchar(' ');
    }
    putchar('\n');
}

static void test(int i) 
{
    if(state[i] == HUNGRY && state[left(i)] != EATING && state[right(i)] != EATING) {
        state[i] = EATING;
        if (!cleanup_in_progress) {
            print_state();
        }
        sys_sem_post(philosopher_sem_id(i));
    }
}

static void take_forks(int i) 
{
    sys_sem_wait(mutex_sem_id);
    state[i] = HUNGRY;
    // print_state();
    test(i);
    sys_sem_post(mutex_sem_id);
    sys_sem_wait(philosopher_sem_id(i));
}

static void put_forks(int i) 
{
    sys_sem_wait(mutex_sem_id);
    state[i] = THINKING;
    if (!cleanup_in_progress) {
        print_state();
    }
    test(left(i));
    test(right(i));
    sys_sem_post(mutex_sem_id);
}

static int philosopher(int argc, char **argv)
{
    int i = atoi(argv[1]);
    while(1) {
        sys_sleep(SECONDS_THINK * 1000);
        take_forks(i);
        sys_sleep(SECONDS_EAT * 1000);
        put_forks(i);
    }
    return 0;
}

static void add_philosopher() 
{
    sys_sem_wait(mutex_sem_id);
    int i = philosopher_count;
    if(i >= MAX_PHILOSOPHER_COUNT)
    {
        sys_sem_post(mutex_sem_id);
        printf("max phi count\n");
        return;
    }
    if(sys_sem_open(philosopher_sem_id(i), 0) == -1) 
    {
        sys_sem_post(mutex_sem_id);
        printf("error\n");
        return;
    }
    char philosopher_i_str[PHILOSOPHER_I_STR_MAX] = {0};
    itoa(i, philosopher_i_str);
    char *argv_phi[] = {"philosopher", philosopher_i_str};
    pids[i] = sys_exec((void *)philosopher, argv_phi, 2, "philosopher", DEFAULT_PRIORITY);
    if(pids[i] == -1) 
    {
        sys_sem_post(mutex_sem_id);
        return;
    }
    state[i] = THINKING; // Set initial state for the new philosopher
    philosopher_count++;
    if (!cleanup_in_progress) {
        print_state();
    }
    sys_sem_post(mutex_sem_id);
}

static void remove_philosopher() {
    int i = philosopher_count - 1;
    sys_sem_wait(mutex_sem_id);
    printf("Echando al filosofo numero %d\n", i);
    while(state[left(i)] == EATING && state[right(i)] == EATING) 
    {
        sys_sem_post(mutex_sem_id);
        sys_sem_wait(philosopher_sem_id(i));
        sys_sem_wait(mutex_sem_id);
    }
    sys_kill(pids[i]);
    sys_waitpid(pids[i]);
    sys_sem_close(philosopher_sem_id(i));
    pids[i] = -1;
    state[i] = INVALID;
    philosopher_count--;
    print_state();
    sys_sem_post(mutex_sem_id);
}

static void print_usage() 
{
    printf("Usa '%c' para agregar a un filosofo (maximo %d)\n", CMD_ADD, MAX_PHILOSOPHER_COUNT);
    printf("Usa '%c' para remover a un filosofo (minimo %d)\n", CMD_REMOVE, MIN_PHILOSOPHER_COUNT);
    printf("Usa '%c' para imprimir el estado de los procesos (ps)\n", CMD_PS);
    printf("Usa '%c' para limpiar la pantalla\n", CMD_CLEAR);
    printf("Usa '%c' para mostrar esta lista\n", CMD_USAGE);
    printf("Usa '%c' para salir del programa\n", CMD_EXIT);
}

int phylo(int argc, char **argv)
{
    mutex_sem_id = sys_get_pid();
    print_usage();
    if(sys_sem_open(mutex_sem_id, 1) == -1) 
    {
        printf("phylo error: init sem\n");
        return -1;
    }

    for(int i = 0; i < INI_PHILOSOPHER_COUNT; i++) 
    {
        add_philosopher();
    }
    int cmd = 0;
    while((cmd = getchar()) != CMD_EXIT) 
    {
        switch (cmd)
        {
        case CMD_ADD:
            add_philosopher();
            break;
        case CMD_REMOVE:
            if(philosopher_count > MIN_PHILOSOPHER_COUNT)
                remove_philosopher();
            else 
                printf("min phi count\n");
            break;
        case CMD_PS:
            ps_command();
            break;
        case CMD_CLEAR:
            sys_clear();
            break;
        case CMD_USAGE:
            print_usage();
            break;
        default:
            break;
        }
    }
    printf("Echando a todos los filosofos\n");
    cleanup_in_progress = 1;  // Set cleanup flag to stop all printing
    while(philosopher_count > 0) 
    {
        remove_philosopher();
    }
    sys_sem_close(mutex_sem_id);

    return 0;
}