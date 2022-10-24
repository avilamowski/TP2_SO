#include <stdint.h>
#include <syscalls.h>
#include <stdio.h>

typedef struct {
	uint8_t b;
	uint8_t g;
	uint8_t r;
} Color;

typedef union {
	Color color;
	uint32_t bits;
} ColorInt;

void startTron() {
    uint32_t res = getScreenResolution();
    uint16_t width = res & 0x0000FFFF, height = res >> 16; 
    
    clear();
    puts("Bienvenido a tron");
    puts("Instrucciones: bla bla bla");
    puts("Presione espacio para comenzar");
    uint64_t initialTicks = getTicks();
    while (1) {
        if(getTicks() % 25){
            char i = getScanCode();
            printf("%x\n", i);
        }
    }
    //ColorInt c = {0,70,185};
    //drawRect(100,100,50,50,c.bits);
}