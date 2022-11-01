#ifndef _STDIO_H
#define _STDIO_H

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define KBDIN 3

/* Cantidad maxima de caracteres para los buffers */
#define MAX_CHARS 256

#include <stdint.h>
#include <stdarg.h>
#include <color.h>
/**
 * @brief Imprime un caracter en STDOUT
 * @param c: Caracter a imprimir
 */
void putchar(char c);

/**
 * @brief Imprime un caracter en STDERR
 * @param c: Caracter a imprimir
 */
void putcharErr(char c);

/**
 * @brief Imprime una cadena en STDOUT
 * @param s: Cadena a a imprimir
 */
void puts(const char * s);

/**
 * @brief Imprime una cadena en STDERR
 * 
 * @param s: Cadena a a imprimir
 */
void printErr(const char * s);

/**
 * @brief Imprime una n cantidad de caracteres el caracter c
 * 
 * @param c: Caracter a imprimir
 * @param n: Cantidad de veces que se imprimira el caracter
 */
void printNChars(char c, int n);

/**
 * @brief Devuelve un caracter escrito en STDIN
 * @return Caracter
 */
int getchar();

/**
 * @brief Devuelve un scancode escrito en KBDIN
 * @return Scancode 
 */
char getScanCode();

/**
 * @brief Permite leer varios tipos de datos de una sola vez (%d, %s, &c, %x, ...)
 * @param fmt: Formato de lo que se desea leer de STDIN
 * @param ... 
 * @return Cantidad de parametros que se leyeron
 */
int scanf(char * fmt, ...);

/**
 * @brief Imprime en STDOUT una "cadena de formato" que permite mezclar distintos tipos de datos para luego mostrarlos en la cadena final
 * @note La cantidad de parametros no es fija
 * @param fmt: Formato de lo que se desea escribir de STDOUT
 * @param ... 
 */
void printf(char * fmt, ...);

/* Deprecado */
void printRegisters(const uint64_t * rsp);

/**
 * @brief Analogo a printf pero permite asignar un color a la cadena de caracteres
 * @note La cantidad de parametros no es fija
 * @param color: color
 * @param fmt: Formato de lo que se desea escribir de STDOUT
 * @param ... 
 */
void printfc(Color color, char * fmt, ...);    

#endif
