#include <video.h>
#include <lib.h>
#include <font.h>
#include <stdint.h>
#include <string.h>

#define RGB_SIZE 3
#define DEFAULT_COLOR {0x7F, 0x7F, 0x7F}

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

static void* getPtrToPixel(uint16_t x, uint16_t y) {
    return (void*)(_screenData->framebuffer + RGB_SIZE * (x + (y * (uint64_t)_screenData->width)));
}

uint16_t _X = 0, _Y = 0;
Color _fontColor = DEFAULT_COLOR;
uint8_t _charScaleFactor = 1;

void videoClear() {
    void * pos = getPtrToPixel(0,0);
    memset(pos, 0, RGB_SIZE * (uint64_t)_screenData->width * _screenData->height);
    _X = _Y = 0;
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
    uint16_t maxX = _screenData->width - CHAR_WIDTH;
    uint16_t maxY = _screenData->height - CHAR_HEIGHT;

    _X = x < maxX ? x : maxX;
    _Y = y < maxY ? y : maxY;
}

void setFontColor(Color color) {
    _fontColor = color;
}

void printNewline(void) {
    _X = 0; 

    if (_Y + 2 * CHAR_HEIGHT <= _screenData->height) {
        _Y += CHAR_HEIGHT;
    } else {
        uint64_t len = RGB_SIZE * ((uint64_t)_screenData->width * (_screenData->height - CHAR_HEIGHT));
        memcpy(getPtrToPixel(0,0), getPtrToPixel(0, CHAR_HEIGHT), len);
        memset(getPtrToPixel(0, _screenData->height - CHAR_HEIGHT), 0, RGB_SIZE * (uint64_t)_screenData->width * CHAR_HEIGHT);
    }
}

void printChar(char c) {
    if (c == '\n') {
        printNewline();
        return;
    }

    if (c == '\b') { // Pensado solo para shell
        if (_X < CHAR_WIDTH && _Y > 0) { 
            _Y -= CHAR_HEIGHT;
            _X = _screenData->width - CHAR_WIDTH;
        } else {
            _X -= CHAR_WIDTH;
        }
        drawRect(_X, _Y, CHAR_WIDTH, CHAR_HEIGHT, (Color){0, 0, 0});
        return;
    }

    if (c >= FIRST_CHAR && c <= LAST_CHAR) {
	    const char* data = font + 32*(c-33);
	    for (int h=0; h<16; h++) {
    		//Color* ptr = (Color*)getPtrToPixel(_X, _Y + h);
            uint8_t mask = 1;
            for (uint8_t i = 0; i < CHAR_WIDTH; i++)
            {
                if (*data & mask) {
                    drawRect(_X + i, _Y + h , 1, 1, _fontColor);
                    //ptr[i] = _fontColor;
                }
                
                if (mask & 0b10000000) {
                    mask = 0b00000001;
                    data++;
                } else {
                    mask <<= 1;
                }
            }
            data++;
    	}
    }

    _X += CHAR_WIDTH;
    if (_X > _screenData->width - CHAR_WIDTH)
        printNewline();
}

void print(const char* s) {
    while (*s)
        printChar(*s++);
}

void println(const char* s) {
    print(s);
    printNewline();
}
