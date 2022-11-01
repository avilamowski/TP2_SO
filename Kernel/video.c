#include <video.h>
#include <lib.h>
#include <font.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#define RGB_SIZE 3
#define MAX_RESOLUTION (64 * 128)

struct vbe_mode_info_structure {
    uint16_t attributes;        // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
    uint8_t window_a;           // deprecated
    uint8_t window_b;           // deprecated
    uint16_t granularity;       // deprecated; used while calculating bank numbers
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr;      // deprecated; used to switch banks from protected mode without returning to real mode
    uint16_t pitch;         // number of bytes per horizontal line
    uint16_t width;         // width in pixels
    uint16_t height;            // height in pixels
    uint8_t w_char;         // unused...
    uint8_t y_char;         // ...
    uint8_t planes;
    uint8_t bpp;            // bits per pixel in this mode
    uint8_t banks;          // deprecated; total number of banks in this mode
    uint8_t memory_model;
    uint8_t bank_size;      // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
    uint8_t image_pages;
    uint8_t reserved0;

    uint8_t red_mask;
    uint8_t red_position;
    uint8_t green_mask;
    uint8_t green_position;
    uint8_t blue_mask;
    uint8_t blue_position;
    uint8_t reserved_mask;
    uint8_t reserved_position;
    uint8_t direct_color_attributes;

    uint32_t framebuffer;       // physical address of the linear frame buffer; write here to draw to the screen
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;   // size of memory in the framebuffer but not being displayed on the screen
    uint8_t reserved1[206];
} __attribute__ ((packed));

struct vbe_mode_info_structure* _screenData = (void*)0x5C00;
uint16_t _X = 0, _Y = 0;
Color _fontColor = DEFAULT_COLOR;
uint8_t _charWidth = CHAR_WIDTH_12;
uint8_t _charHeight = CHAR_HEIGHT_12;
char * _font = font_12;
char _charBuffer[MAX_RESOLUTION];
uint16_t _bufferIdx = 0;

static void renderFonts();
static void* getPtrToPixel(uint16_t x, uint16_t y);

static void* getPtrToPixel(uint16_t x, uint16_t y) {
    return (void*)(_screenData->framebuffer + RGB_SIZE * (x + (y * (uint64_t)_screenData->width)));
}

void videoClear() {
    void * pos = getPtrToPixel(0,0);
    memset(pos, 0, RGB_SIZE * (uint64_t)_screenData->width * _screenData->height);
    _X = _Y = 0;
    _bufferIdx = 0;
}

uint8_t coordinatesValid(uint16_t x, uint16_t y) {
    return x < _screenData->width && y < _screenData->height;
}

void setPixel(uint16_t x, uint16_t y, Color color) {
    if (coordinatesValid(x,y))
        *((Color*) getPtrToPixel(x,y)) = color;
}

void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color color) {
    if (!coordinatesValid(x, y))
        return;
    
    uint16_t maxWidth = _screenData->width - x;
    if (width > maxWidth) width = maxWidth;
    
    uint16_t maxHeight = _screenData->height - y;
    if (height > maxHeight) height = maxHeight;

    Color* ptr = (Color*)getPtrToPixel(x, y);
    uint16_t lineDiff = _screenData->width - width;
    for (int i=0; i<height; i++) {
        for (int c=0; c<width; c++)
            *(ptr++) = color;
        ptr += lineDiff;
    }
}

void setPosition(uint16_t x, uint16_t y) {
    uint16_t maxX = _screenData->width - _charWidth;
    uint16_t maxY = _screenData->height - _charHeight;

    _X = x < maxX ? x : maxX;
    _Y = y < maxY ? y : maxY;
}

void setFontColor(Color color) {
    _fontColor = color;
}

Color getFontColor(){
    return _fontColor;
}

void setFontSize(fontSize f){
    if(f >= FONT_12 && f <= FONT_36){
        _font = fonts[f];
        _charWidth = charWidths[f];
        _charHeight = charHeights[f];
        renderFonts();
    }
}

static void renderFonts() {
    int buffIdx = _bufferIdx;
    videoClear();
    printN(_charBuffer, buffIdx);
}

void printNewline(void) {
    _X = 0; 

    if (_Y + 2 * _charHeight <= _screenData->height) {
        _Y += _charHeight;
    } else {
        uint64_t len = RGB_SIZE * ((uint64_t)_screenData->width * (_screenData->height - _charHeight));
        memcpy(getPtrToPixel(0,0), getPtrToPixel(0, _charHeight), len);
        memset(getPtrToPixel(0, _screenData->height - _charHeight), 0, RGB_SIZE * (uint64_t)_screenData->width * _charHeight);
    }
}

void printChar(char c) {
    if (c == '\b') { // Pensado solo para shell
        if (_X < _charWidth && _Y > 0) { 
            _Y -= _charHeight;
            _X = (_screenData->width / _charWidth) * _charWidth - _charWidth;
        } else {
            _X -= _charWidth;
        }
        drawRect(_X, _Y, _charWidth, _charHeight, (Color){0, 0, 0});
        _bufferIdx--;
        return;
    }

    if (_bufferIdx == MAX_RESOLUTION) {
        videoClear();
        print("Buffer de video excedido, la pantalla ha sido limpiada\n");
    }

    _charBuffer[_bufferIdx++] = c;
    if (c == '\n') {
        printNewline();
        return;
    }
    
    if (c >= FIRST_CHAR && c <= LAST_CHAR) {
	    const char* data = _font + _charHeight * _charWidth * (c-FIRST_CHAR) / 8;
	    for (int h=0; h<_charHeight; h++) {
    		Color* ptr = (Color*)getPtrToPixel(_X, _Y + h);
            uint8_t mask = 1;
            for (uint8_t i = 0; i < _charWidth; i++) // TODO: Ver si se puede optimizar
            {
                if (*data & mask) {
                    ptr[i] = _fontColor;
                }
                
                if (mask & 0b10000000) {
                    mask = 0b00000001;
                    data++;
                } else {
                    mask <<= 1;
                }
            }
    	}
    }
    _X += _charWidth;
    if (_X > (_screenData->width / _charWidth) * _charWidth - _charWidth)
        printNewline();
}

void print(const char* s) {
    while (*s)
        printChar(*s++);
}

void printN(const char* s, uint32_t n) {
    if (!n)
        return;
    while (n-- && *s)
        printChar(*s++);
}

int printf(char * fmt, ...) {
    va_list v;
    va_start(v, fmt);
    char * buffer[256] = {0};
    char * fmtPtr = fmt;
    char * end;
    while (*fmtPtr) {
 	    if (*fmtPtr == '%') {
            fmtPtr++;
            int dx = strtoi(fmtPtr, &fmtPtr);
            int len;

            switch (*fmtPtr) {
                case 'c':
                    printChar(va_arg(v, char *));
                    break;
                case 'd':
                    len = itoa(va_arg(v, int *), buffer, 10);
                    printNChars('0', dx-len);
                    print(buffer);
                    break;
                case 'x':
                    len = itoa(va_arg(v, int *), buffer, 16);
                    printNChars('0', dx-len);
                    print(buffer);
                    break;
                case 's':
                    printNChars(' ', dx); // A diferencia %x y %d, la cantidad de espacios es igual al numero
                    print((char *) va_arg(v, char *));
                    break;
            }
        } else {
            printChar(*fmtPtr);
        }
        fmtPtr++;
    }
    va_end(v);
    return 1;
}

void printNChars(char c, int n) {
    for (int i = 0; i < n; i++)
        printChar(c);
}

uint32_t getScreenResolution(){
    return _screenData->width | _screenData->height << 16;
}
