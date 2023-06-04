#ifndef _PROCESS_H
#define _PROCESS_H
#include <stdint.h>

int loop(int argc, char **argv);
int testProgram(int argc, char **argv);
int filter(int argc, char **argv);
int wc(int argc, char **argv);
int cat(int argc, char **argv);

#endif