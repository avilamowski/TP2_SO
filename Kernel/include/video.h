#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <stdint.h>
#include <color.h>

typedef enum {FONT_12 = 0, FONT_24} fontSize;

/* Clears the whole screen to black. */
void videoClear();

/* Sets the color of a specific pixel on the screen. */
void setPixel(uint16_t x, uint16_t y, Color color);

/* Sets a specified rectangle of pixels on the screen to the specified color. */
void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color color);

/* Sets the pen position for drawing characters on the screen as a console. */
void setPenPosition(uint16_t x, uint16_t y);

/* Sets the pen color for drawing characters on the screen as a console. */
void setFontColor(Color color);

/* Sets the font size for drawing characters on the screen as a console. */
void setFontSize(fontSize f);

/* Advances the pen to the beginning of the next line. */
void printNewline(void);

/* Prints a single character with the pen, wrapping around the end of the screen and pushing old lines up if necessary. */
void printChar(char c);

/* Prints a string of characters with the pen, wrapping around the end of the screen and pushing old lines up if necessary.
Returns the new pen position as a 32 bit number, where the 16 lowest bits are the x and the upper 16 bits are the y. */
void print(const char* s);

int printf(char * fmt, ...);

void printNChars(char c, int n);

uint32_t getScreenResolution();
#endif
