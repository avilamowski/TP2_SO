#include <keyboard.h>
#include <lib.h>
static char buffer = 0;
static const char charHexMap[256] = 
    {   0,  0,  '1',  '2',  '3',  '4',  '5',  '6', '7',  '8',  '9', '0', '-',  '=',  0xF0,  ' ',
        'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I', 'O',  'P',  '[', ']',  '\n',  0, 'A', 'S',
        'D',  'F',  'G',  'H',  'J',  'K',  'L',  ';',  '\'',  0,  0,  '\\', 'Z',  'X', 'C', 'V',
        'B',  'N',  'M',  ',',  '.',  '/',  0,  '*', 0,  ' ',  0, 0, 0,  0, 0, 0};

void keyboard_handler(){
    buffer = getKeyPressed();
}

char getAsciiPressed(){
    return charHexMap[buffer&0x7F];
}