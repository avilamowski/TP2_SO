#include <keyboard.h>
#include <stdint.h>
#include <lib.h>
#include <video.h>
#include <time.h>
#define BUFFER_CAPACITY 10                      /* Longitud maxima del vector _buffer */
#define HOTKEY 29                               /* Scancode para el snapshot de registros */
static uint8_t _bufferStart = 0;                /* Indice del comienzo de la cola */
static char _bufferSize = 0;                    /* Longitud de la cola */
static uint8_t _buffer[BUFFER_CAPACITY] = {0};  /* Vector ciclico que guarda las teclas 
                                                 * que se van leyendo del teclado */
static const char charHexMap[256] =             /* Mapa de scancode a ASCII */
    {   0,  0,  '1',  '2',  '3',  '4',  '5',  '6', '7',  '8',  '9', '0', '-',  '=',  '\b',  ' ',
        'q',  'w',  'e',  'r',  't',  'y',  'u',  'i', 'o',  'p',  '[', ']',  '\n',  0, 'a', 's',
        'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  '\'',  0,  0,  '\\', 'z',  'x', 'c', 'v',
        'b',  'n',  'm',  ',',  '.',  '/',  0,  '*', 0,  ' ',  0, 0, 0,  0, 0, 0};

/**
 * @brief  Obtiene el indice del elemento en la cola dado un corrimiento 
 * @param  offset: corrimiento
 * @return Indice del elemento en la cola
 */
static int getBufferIndex(int offset){
    return (_bufferStart+offset)%(BUFFER_CAPACITY);
}

void keyboardHandler(){
    uint8_t key = getKeyPressed();
    if(_bufferSize < BUFFER_CAPACITY -1){
        if(!(key & 0x80)){
            if (key == HOTKEY) {
                saveRegisters();
                return;
            }
            _buffer[getBufferIndex(_bufferSize)] = key;
            _bufferSize++;
        }
    }
}

char getScancode() {
    if(_bufferSize > 0){
        char c = _buffer[getBufferIndex(0)];
        _bufferStart = getBufferIndex(1);
        _bufferSize--;
        return c;
    }
    return 0;
}

char getAscii(){
    return charHexMap[(int) getScancode()];
}
