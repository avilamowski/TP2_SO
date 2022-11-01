#include <stdint.h>
#include <video.h>
#include <keyboard.h>
#include <lib.h>
#include <color.h>
#include <speaker.h>
#include <time.h>
#include <memory.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define KBDIN 3

static uint8_t syscall_read(uint32_t fd);
static void syscall_write(uint32_t fd, char c);
static void syscall_clear();
static uint32_t syscall_seconds();
static uint64_t * syscall_registerarray(uint64_t * regarr);
static void syscall_fontsize(uint8_t size);
static uint32_t syscall_resolution();
static void syscall_drawrect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
static uint64_t syscall_getticks();
static void syscall_getmemory(uint64_t pos, uint8_t * vec);
static void syscall_setfontcolor(uint8_t r, uint8_t g, uint8_t b);
static uint32_t syscall_getfontcolor();


uint64_t syscallDispatcher(uint64_t nr, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
	switch (nr) {
        case 0:
            return syscall_read((uint32_t)arg0);
		case 1:
			syscall_write((uint32_t)arg0, (char)arg1);
            break;
        case 2:
            syscall_clear();
            break;
        case 3:
            return syscall_seconds();
        case 4:
            return (uint64_t) syscall_registerarray((uint64_t *) arg0);
        case 5:
            syscall_fontsize((uint8_t)arg0);
            break;
        case 6:
            return syscall_resolution();
        case 7:
            syscall_drawrect((uint16_t)arg0, (uint16_t)arg1, (uint16_t)arg2, (uint16_t)arg3, (uint32_t)arg4);
            break;
        case 8:
            return syscall_getticks();
        case 9:
            syscall_getmemory((uint64_t) arg0, (uint8_t *) arg1);
            break;
        case 10:
            playSound(arg0, arg1);
            break;
        case 11:
            syscall_setfontcolor((uint8_t) arg0, (uint8_t) arg1, (uint8_t) arg2);
            break;
        case 12:
            return syscall_getfontcolor();
            
	}
	return 0;
}

// Read char
static uint8_t syscall_read(uint32_t fd){
    switch (fd){
        case STDIN:
            return getAscii();
        case KBDIN:
            return getScancode();
    }
    return 0;
}

// Write
static void syscall_write(uint32_t fd, char c){
    Color prevColor = getFontColor();
    if(fd == STDERR)
        setFontColor((Color){0, 0, 255});
    else if(fd != STDOUT)
        return;
    printChar(c);
    setFontColor(prevColor);
}

// Clear
static void syscall_clear(){
    videoClear();
}

// GetSeconds
static uint32_t syscall_seconds(){
    uint8_t h, m, s;
    getTime(&h, &m, &s);
    return s + m * 60 + ((h + 24 - 3) % 24) * 3600;
}

// Inforeg
static uint64_t * syscall_registerarray(uint64_t * regarr){
    uint64_t * snapshot = getLastRegSnapshot();
    for(int i = 0; i < QTY_REGS; i++)
        regarr[i] = snapshot[i];
    return regarr;
}

// FontSize
static void syscall_fontsize(uint8_t size){
    setFontSize(size - 1);
}

static uint32_t syscall_resolution(){
    return getScreenResolution();
}

static void syscall_drawrect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
    ColorInt myColor = { bits: color };
    drawRect(x, y, width, height, myColor.color);
}

static uint64_t syscall_getticks(){
    return ticksElapsed();
}

//PrintMem
static void syscall_getmemory(uint64_t pos, uint8_t * vec){
    memcpy(vec, (uint8_t *) pos, 32);
}

static void syscall_setfontcolor(uint8_t r, uint8_t g, uint8_t b){
    setFontColor((Color){b, g, r});
}

static uint32_t syscall_getfontcolor(){
    ColorInt c = { color: getFontColor() };
    return c.bits;
}