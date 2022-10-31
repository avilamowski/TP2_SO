#ifndef _COLOR_H_
#define _COLOR_H_

#include <stdint.h>

#define DEFAULT_COLOR {255, 255, 255}
#define ERROR_COLOR {0, 0, 127}

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