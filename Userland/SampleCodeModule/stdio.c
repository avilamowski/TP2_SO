#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define MAX_CHARS 256

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
    c = read(STDIN);
    if(c != 0)
        putchar(c);
    return c;
}

int printf(char * fmt, ...) {
    va_list v;
    va_start(v, fmt);
    //TODO implementar el buffer de teclado

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
    //TODO implementar el buffer de teclado
    char c;
    char buffer[MAX_CHARS];
    int bIdx = 0;
    while((c = getchar()) != '\n'){
        if (c != 0) {
            if (c != '\b')
                buffer[bIdx++] = c;
            else
                bIdx--;
        }
    }
    buffer[bIdx] = 0;
    char * fmtPtr = fmt;
    char * end;
    bIdx = 0;
    while (*fmtPtr && buffer[bIdx]) {
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
        } else if (*fmtPtr == buffer[bIdx]) {
           bIdx++; 
        } else {
            puts("Error!!!"); // TODO: Cambiar 
        }
        fmtPtr++;

    }
    va_end(v);
    return 1;
}
