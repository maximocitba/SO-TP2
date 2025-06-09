#ifndef TPE_COMMAND_H
#define TPE_COMMAND_H

typedef struct {
    char * title;
    char * desc;
    void (*command)();
} command;

void print_time();
// void print_help();

void clear();
void divideByZero();
void invalidOpcode();
void time();
void changeSize_1();
void changeSize_2();
void changeSize_3();
void changeSize_4();
void changeSize_5();
void test_mem();
void test_proc();
void test_prior();
void test_syncro();

void ps_command();
int loop_command(int argc, char **argv);
int kill_command(int argc, char **argv);
int nice_command(int argc, char **argv);
int block_command(int argc, char **argv);
int cat(int argc, char **argv);
int echo(int argc, char **argv);
int wc(int argc, char **argv);
int filter(int argc, char **argv);
int phylo(int argc, char **argv);

//para excepciones (buscan en Assembler)
extern void runInvalidOpcodeException(void);
extern void runZeroDivisionException(void);

#endif //TPE_COMMAND_H