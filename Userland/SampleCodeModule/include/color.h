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

#define LIGHT_GREEN ((Color){100, 255, 100})
#define DARK_GREEN ((Color){100, 150, 100})
#define PINK ((Color){255, 153, 184})
#define MAGENTA ((Color){255, 0, 184})
#define SILVER ((Color){173, 169, 170})
#define RED ((Color){0, 0, 255})
#define BLACK ((Color){0, 0, 0})
#endif