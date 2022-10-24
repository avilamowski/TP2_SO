#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>

uint64_t write(int fd, const void * buf, uint64_t count);
uint64_t read(int fd);
uint32_t getSeconds();
int clear(void);
uint64_t * getInfoReg();
void setFontSize(uint8_t size);
uint32_t getScreenResolution();
void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
uint64_t getTicks();

#endif