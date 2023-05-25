#ifndef _PROCESS_H
#define _PROCESS_H
#include <stdint.h>
void psPrint();
int loop(int argc, char **argv);
void nice(char *pid, char *priority);
void block(char *pid);
void unblock(char *pid);
int testProgram(int argc, char **argv);
#endif