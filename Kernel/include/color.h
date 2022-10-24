#ifndef _COLOR_H_
#define _COLOR_H_

#include <stdint.h>

#define DEFAULT_COLOR {0x7F, 0x7F, 0x7F}
#define ERROR_COLOR {0x00, 0x00, 0xFF}

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