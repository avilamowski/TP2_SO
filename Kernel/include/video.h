#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <stdint.h>
#include <color.h>

/* Tamaños de fuente disponibles */
typedef enum {FONT_12 = 0, FONT_24, FONT_36} fontSize;

/**
 * @brief  Pone todos los pixeles de la pantalla en negro y limpia el buffer de video
 */
void videoClear();

/**
 * @brief  Dibuja un rectangulo
 * @param  x: Origen en x 
 * @param  y: Origen en y 
 * @param  width: Ancho
 * @param  height: Alto
 * @param  color: Color de relleno
 */
void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color color);

/**
 * @brief  Cambiar el color de la letra para STDIN
 * @param  color: Nuevo color
 */
void setFontColor(Color color);

/**
 * @brief  Cambia el tamaño de la fuente
 * @param  f: (1|2|3)
 */
void setFontSize(fontSize f);

/**
 * @brief  Hace un salto de linea y pasa al principio de la siguiente
 */
void printNewline();

/**
 * @brief  Imprime un caracter en pantalla
 * @param  c: Caracter a imprimir 
 */
void printChar(char c);

/**
 * @brief  Imprime una cadena de caracteres en pantalla
 * @param  s: Cadena de caracteres a imprimir
 */
void print(const char* s);

/**
 * @brief  Imprime una cadena de caracteres con formato en pantalla
 * @note   No confundir con la funcion printf definida en userland 
 * @param  fmt: Formato
 */
void printf(char * fmt, ...);


/**
 * @brief  Imprime n caracteres de un string
 * @param  s: String origen
 * @param  n: Cantidad de caracteres a imprimir
 */
void printN(const char * s, uint32_t n);

/**
 * @brief  Imprime n veces un caracter
 * @param  c: Caracter a imprimir
 * @param  n: Cantidad de veces que se lo quiere imprimir
 */
void printNChars(char c, int n);

/**
 * @brief  Devuelve las dimensiones de la pantalla
 * @note   
 * @return 32 bits menos significativos el ancho, 32 el alto 
 */
uint32_t getScreenResolution();

/**
 * @brief  Cambia el color de la letra
 * @param  color: Nuevo color
 */
void setFontColor(Color color);

/**
 * @brief  Devuelve el color de la letra
 * @return Color de la letra
 */
Color getFontColor();

#endif
