#include <stdint.h>
#include <video.h>
#include <keyboard.h>
#include <lib.h>
#include <color.h>
#include <speaker.h>
#include <time.h>
#include <memory.h>
/* File Descriptors*/
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define KBDIN 3

/* IDs de syscalls */
#define READ 0
#define WRITE 1
#define CLEAR 2
#define SECONDS 3
#define GET_REGISTER_ARRAY 4
#define SET_FONT_SIZE 5
#define GET_RESOLUTION 6
#define DRAW_RECT 7
#define GET_TICKS 8
#define GET_MEMORY 9
#define PLAY_SOUND 10
#define SET_FONT_COLOR 11
#define GET_FONT_COLOR 12

static uint8_t syscall_read(uint32_t fd);
static void syscall_write(uint32_t fd, char c);
static void syscall_clear();
static uint32_t syscall_seconds();
static uint64_t * syscall_registerArray(uint64_t * regarr);
static void syscall_fontSize(uint8_t size);
static uint32_t syscall_resolution();
static void syscall_drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
static uint64_t syscall_getTicks();
static void syscall_getMemory(uint64_t pos, uint8_t * vec);
static void syscall_playSound(uint64_t frequency, uint64_t ticks);
static void syscall_setFontColor(uint8_t r, uint8_t g, uint8_t b);
static uint32_t syscall_getFontColor();


uint64_t syscallDispatcher(uint64_t nr, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
	switch (nr) {
        case READ:
            return syscall_read((uint32_t)arg0);
		case WRITE:
			syscall_write((uint32_t)arg0, (char)arg1);
            break;
        case CLEAR:
            syscall_clear();
            break;
        case SECONDS:
            return syscall_seconds();
        case GET_REGISTER_ARRAY:
            return (uint64_t) syscall_registerArray((uint64_t *) arg0);
        case SET_FONT_SIZE:
            syscall_fontSize((uint8_t)arg0);
            break;
        case GET_RESOLUTION:
            return syscall_resolution();
        case DRAW_RECT:
            syscall_drawRect((uint16_t)arg0, (uint16_t)arg1, (uint16_t)arg2, (uint16_t)arg3, (uint32_t)arg4);
            break;
        case GET_TICKS:
            return syscall_getTicks();
        case GET_MEMORY:
            syscall_getMemory((uint64_t) arg0, (uint8_t *) arg1);
            break;
        case PLAY_SOUND:
            syscall_playSound(arg0, arg1);
            break;
        case SET_FONT_COLOR:
            syscall_setFontColor((uint8_t) arg0, (uint8_t) arg1, (uint8_t) arg2);
            break;
        case GET_FONT_COLOR:
            return syscall_getFontColor();
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

// Write char
static void syscall_write(uint32_t fd, char c){
    Color prevColor = getFontColor();
    if(fd == STDERR)
        setFontColor(ERROR_COLOR);
    else if(fd != STDOUT)
        return;
    printChar(c);
    setFontColor(prevColor);
}

// Clear
static void syscall_clear(){
    videoClear();
}

// Get time in seconds
static uint32_t syscall_seconds(){
    uint8_t h, m, s;
    getTime(&h, &m, &s);
    return s + m * 60 + ((h + 24 - 3) % 24) * 3600;
}

// Get register snapshot array
static uint64_t * syscall_registerArray(uint64_t * regarr){
    uint64_t * snapshot = getLastRegSnapshot();
    for(int i = 0; i < QTY_REGS; i++)
        regarr[i] = snapshot[i];
    return regarr;
}

// Set fontsize
static void syscall_fontSize(uint8_t size){
    setFontSize(size - 1);
}

// Get screen resolution
static uint32_t syscall_resolution(){
    return getScreenResolution();
}

// DrawRect
static void syscall_drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
    ColorInt myColor = { bits: color };
    drawRect(x, y, width, height, myColor.color);
}

// GetTicks
static uint64_t syscall_getTicks(){
    return ticksElapsed();
}

//PrintMem
static void syscall_getMemory(uint64_t pos, uint8_t * vec){
    memcpy(vec, (uint8_t *) pos, 32);
}

//playSound
static void syscall_playSound(uint64_t frequency, uint64_t ticks){
    playSound(frequency, ticks);
}

//Set fontsize
static void syscall_setFontColor(uint8_t r, uint8_t g, uint8_t b){
    setFontColor((Color){b, g, r});
}

//Get fontsize
static uint32_t syscall_getFontColor(){
    ColorInt c = { color: getFontColor() };
    return c.bits;
}