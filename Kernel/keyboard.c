#include <keyboard.h>
#include <lib.h>
#include <video.h>
#include <time.h>
#define DELTA 3 

static char buffer = 0;
static char oldCharBuffer = 0;
static int lastTicks = 0;

static const char charHexMap[256] = 
    {   0,  0,  '1',  '2',  '3',  '4',  '5',  '6', '7',  '8',  '9', '0', '-',  '=',  '\b',  ' ',
        'q',  'w',  'e',  'r',  't',  'y',  'u',  'i', 'o',  'p',  '[', ']',  '\n',  0, 'a', 's',
        'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  '\'',  0,  0,  '\\', 'z',  'x', 'c', 'v',
        'b',  'n',  'm',  ',',  '.',  '/',  0,  '*', 0,  ' ',  0, 0, 0,  0, 0, 0};

void keyboard_handler(){
    buffer = getKeyPressed();
}

char getScancode() {
    int ticks = ticks_elapsed();
    if ((buffer == (buffer&0x7F) && (ticks - lastTicks > DELTA || (oldCharBuffer&0x7F) != (buffer&0x7F)))) { // La tecla esta siendo presionada
        oldCharBuffer = buffer;
        lastTicks = ticks;
        return buffer&0x7F;
    }
    return 0;
}

char getAscii(){
    return charHexMap[getScancode()];
}
