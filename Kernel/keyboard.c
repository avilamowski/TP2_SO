#include <keyboard.h>
#include <lib.h>
static char buffer = 0;
static const char charHexMap[256] = 
    {   0,  0,  '1',  '2',  '3',  '4',  '5',  '6', '7',  '8',  '9', '0', '-',  '=',  0Xf0,  ' ',
        'q',  'w',  'e',  'r',  't',  'y',  'u',  'i', 'o',  'p',  '[', ']',  '\n',  0, 'a', 's',
        'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  '\'',  0,  0,  '\\', 'z',  'x', 'c', 'v',
        'b',  'n',  'm',  ',',  '.',  '/',  0,  '*', 0,  ' ',  0, 0, 0,  0, 0, 0};

void keyboard_handler(){
    buffer = getKeyPressed();
}

char getScancode() {
    return buffer&0x7F;
}

char getAscii(){
    return charHexMap[buffer&0x7F];
}
