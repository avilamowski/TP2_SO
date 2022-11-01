#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);
int itoa(uint64_t n, char* buffer, int base);
int strtoi(char* s, char ** end);
char getKeyPressed();
char *cpuVendor(char *result);
void getTime(uint8_t * h, uint8_t * m, uint8_t * s);
uint64_t * getRegisterArray();
void startSound(uint64_t freq);
void stopSound();
void saveRegisters();
void _hlt();
#endif

