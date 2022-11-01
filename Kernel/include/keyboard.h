#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
/* Handler de interrupciones de teclado */
void keyboardHandler();

/* Devuelve el valor ascii del ultimo caracter en el buffer de teclado */
char getAscii();

/* Devuelve el scancode del ultimo caracter en el buffer de teclado */
char getScancode();
#endif
