#ifndef _STDLIB_H
#define _STDLIB_H
#include <stdint.h>

/**
 * @brief Convierte un arreglo de caracteres en numero entero en formato decimal
 * @param s: Cadena de caracteres
 * @return Numero entero en formato decimal
 */
int atoi(char *s);

/**
 * @brief Convierte una cadena de caracteres en numero entero en formato decimal
 * @param s: Cadena de caracteres
 * @param end: Puntero al final de la cadena de caracteres
 * @return Numero entero en formato decimal
 */
int strtoi(char *s, char **end);

/**
 * @brief Convierte una cadena de caracteres en numero entero en formato
 * hexadecimal
 * @param s: Cadena de caracteres
 * @param end: Puntero al final de la cadena de caracteres
 * @return Numero entero en formato hexadecimal
 */
int strtoh(char *s, char **end);

/**
 * @brief Convierte un numero en una base en una cadena de caracteres
 * @param n: Numero que se desea convertir
 * @param buffer: Buffer para ir guardando la conversion
 * @param base: Base del numero que se desea convertir
 * @return Longitud del buffer (numero que se convirtio en cadena de caracteres)
 */
int itoa(uint64_t n, char *buffer, int base);

unsigned int log(uint64_t n, int base);

int stringArrayLen(char **array);

/**
 * @brief  Escribe 'character' en una direccion de memoria 'length' veces
 * @param  destination: Lugar a escribir
 * @param  character: Elemento a copiar
 * @param  length: Cantidad de veces que se desea escrbir el valor
 */
void *memset(void *destiation, int32_t c, uint64_t length);

#define toLower(n) ((n) >= 'A' && (n) <= 'Z' ? (n) - ('A' - 'a') : (n))
#define toUpper(n) ((n) >= 'a' && (n) <= 'z' ? (n) - ('a' - 'A') : (n))
#define isNumber(n) ((n) >= '0' && (n) <= '9')
#define isHex(n) ((n) >= 'a' && (n) <= 'f')

// clang-format off
#define NULL ((void *)0)
#define EOF (-1)
// clang-format on

#endif