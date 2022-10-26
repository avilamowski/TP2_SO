#include <syscalls.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

#define MAX_CHARS 256
#define CURSOR_FREQ 10
int putchar(int c) {
    write(STDOUT, &c, 1);
    return 1;
}

int puts(const char * s) {
    while (*s) putchar(*(s++)); 
    return 1;
}

int getchar() {
    char c;
    c = read(STDIN); // TODO: Ver cuestiones de buffer
    return c;
}

char getScanCode() {
    return read(KBDIN);
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
            switch (*fmtPtr) {
                case 'c':
                    putchar(va_arg(v, char *));
                    break;
                case 'd':
                    puts(itoa(va_arg(v, int *), buffer, 10));
                    break;
                case 'x':
                    puts(itoa(va_arg(v, int *), buffer, 16));
                    break;
                case 's':
                    puts((char *) va_arg(v, char *));
                    break;
            }
        } else {
            putchar(*fmtPtr);
        }
        fmtPtr++;
    }
    va_end(v);
    return 1;
}

int scanf(char * fmt, ...) {
    va_list v;
    va_start(v, fmt);
    char c;
    int ticks = getTicks();
    int cursorTicks = 0;
    char cursorDrawn = 0;
    char buffer[MAX_CHARS];
    uint64_t bIdx = 0;
    while((c = getchar()) != '\n' & bIdx < MAX_CHARS){
        cursorTicks = getTicks() - ticks;
         if(cursorTicks > CURSOR_FREQ){
            ticks = getTicks();
            cursorTicks = 0;
            if(cursorDrawn)
                putchar('\b');
            else
                putchar('_');
            cursorDrawn = !cursorDrawn;
        }
        if (c != 0) {
            if(cursorDrawn){
                putchar('\b');
                cursorDrawn = !cursorDrawn;
            }
            if (c != '\b'){
                buffer[bIdx++] = c;
                putchar(c);
            }
            else if(bIdx>0){
                bIdx--;
                putchar(c);
            }
        }
    }
    if(cursorDrawn)
        putchar('\b');
    putchar('\n');
    buffer[bIdx] = 0;
    char * fmtPtr = fmt;
    char * end;
    bIdx = 0;

    int qtyParams = 0;
    while (*fmtPtr && buffer[bIdx] && bIdx < MAX_CHARS) {
 	    if (*fmtPtr == '%') {
            fmtPtr++;
            switch (*fmtPtr) {
                case 'c':
                    *(char *) va_arg(v, char *) = buffer[bIdx];
                    end = &buffer[bIdx] + 1;
                    break;
                case 'd':
                    *(int *) va_arg(v, int *) = strtoi(&buffer[bIdx], &end);
                    break;
                case 'x':
                    *(int *) va_arg(v, int *) = strtoh(&buffer[bIdx], &end);
                    break;
                case 's':
                    end = &buffer[bIdx] + strcpychar((char *) va_arg(v, char *), &buffer[bIdx], ' ');
                    break;
            }
            bIdx += end - &buffer[bIdx];
            qtyParams++;
        } else if (*fmtPtr == buffer[bIdx]) {
           bIdx++; 
        } else {
            puts("Error!!!"); // TODO: Cambiar 
        }
        fmtPtr++;

    }
    va_end(v);
    return qtyParams;
}
