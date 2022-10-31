#ifndef COLOR_H
#define COLOR_H
#include <stdint.h>
typedef struct {
	uint8_t b;
	uint8_t g;
	uint8_t r;
} Color;

typedef union {
	Color color;
	uint32_t bits;
} ColorInt;
#endif