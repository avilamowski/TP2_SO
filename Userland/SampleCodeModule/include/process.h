#ifndef _PROCESS_H
#define _PROCESS_H
#include <stdint.h>
void psPrint();
int loop(int argc, char **argv);
void nice(uint16_t pid, uint8_t priority);
int8_t block(uint16_t pid);
int8_t unblock(uint16_t pid);
int testProgram(int argc, char **argv);
int16_t createProcess(void *code, char **args, char *name, uint8_t priority);
int filter(int argc, char **argv);
int wc(int argc, char **argv);
int cat(int argc, char **argv);

#endif