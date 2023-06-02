#ifndef _COLOR_H_
#define _COLOR_H_

#include <stdint.h>

#define DEFAULT_COLOR \
	{ 255, 255, 255 }					 // Color por defecto de la letra del monitor
#define ERROR_COLOR ((Color){0, 0, 255}) // Color por defecto para errores
#define BLACK ((Color){0, 0, 0})		 // Negro

/* Estructura que guarda el valor RGB de un pixel */
typedef struct {
	uint8_t b;
	uint8_t g;
	uint8_t r;
} Color;

/*
 * Union util para poder convertir un Color a un entero
 * y asi poder transportarlo facilmente
 */
typedef union {
	Color color;
	uint32_t bits;
} ColorInt;

#define LIGHT_GREEN ((Color){100, 255, 100})
#define DARK_GREEN ((Color){100, 150, 100})
#define PINK ((Color){255, 153, 184})
#define MAGENTA ((Color){255, 0, 184})
#define SILVER ((Color){173, 169, 170})
#define RED ((Color){0, 0, 255})
#define BLACK ((Color){0, 0, 0})
#endif