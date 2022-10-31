#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);
char* itoa(uint64_t n, char* buffer, int base);
char getKeyPressed();
char *cpuVendor(char *result);
void getTime(char * h, char * m, char * s);
uint64_t * getRegisterArray();
void startSound(uint64_t freq);
void stopSound();
#endif

