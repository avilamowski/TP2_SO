#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
#define IO_SEM_ID 0
#include <stdint.h>
/* Handler de interrupciones de teclado */
void keyboardHandler();

/* Devuelve el valor ascii del ultimo caracter en el buffer de teclado */
int8_t getAscii();

/* Devuelve el scancode del ultimo caracter en el buffer de teclado */
int8_t getScancode();

void initializeKeyboardDriver();

#endif
