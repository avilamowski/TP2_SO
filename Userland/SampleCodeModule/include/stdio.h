#ifndef STDIO_H
#define STDIO_H

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define KBDIN 3

#define MAX_CHARS 256
#include <stdint.h>
#include <stdarg.h>
void putchar(char c);
void putcharErr(char c);
void puts(const char * s);
void printErr(const char * s);
void printNChars(char c, int n);
int getchar();
char getScanCode();
int scanf(char * fmt, ...);
void printf(char * fmt, ...);
void printRegisters(const uint64_t * rsp);
void printfc(uint8_t r, uint8_t g, uint8_t b, char * fmt, ...);
void vprintf(char * fmt, va_list args);

#endif
