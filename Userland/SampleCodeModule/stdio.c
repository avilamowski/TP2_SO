#include <syscalls.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <sound.h>

#define CURSOR_FREQ 10  /* Frecuencia en Ticks del dibujo del cursor*/


/**
 * @brief Funcion auxiliar para printf y printfc
 * @note La cantidad de parametros no es fija
 * @param fmt Formato de lo que se desea escribir de STDOUT
 * @param args lista de argumentos
 */
static void vprintf(char * fmt, va_list args);

void putchar(char c) {
    write(STDOUT, c);
}

void putcharErr(char c) {
    write(STDERR, c);
}

void puts(const char * s) {
    while (*s)
        putchar(*s++); 
}

void printErr(const char * s) {
    while (*s) putcharErr(*s++); 
}

int getchar() {
    char c;
    c = read(STDIN);
    return c;
}

char getScanCode() {
    return read(KBDIN);
}

void printf(char * fmt, ...) {
    va_list v;
    va_start(v, fmt);
    vprintf(fmt, v);
    va_end(v);
}

void vprintf(char * fmt, va_list args) {
    char buffer[MAX_CHARS] = {0};
    char * fmtPtr = fmt;
    while (*fmtPtr) {
 	    if (*fmtPtr == '%') {
            fmtPtr++;
            int dx = strtoi(fmtPtr, &fmtPtr);
            int len;

            switch (*fmtPtr) {
                case 'c':
                    putchar(va_arg(args, int));
                    break;
                case 'd':
                    len = itoa(va_arg(args, uint64_t), buffer, 10);
                    printNChars('0', dx-len);
                    puts(buffer);
                    break;
                case 'x':
                    len = itoa(va_arg(args, uint64_t), buffer, 16);
                    printNChars('0', dx-len);
                    puts(buffer);
                    break;
                case 's':
                    printNChars(' ', dx); // A diferencia %x y %d, la cantidad de espacios es igual al numero
                    puts((char *) va_arg(args, char *));
                    break;
            }
        } else {
            putchar(*fmtPtr);
        }
        fmtPtr++;
    }
}

void printfc(Color color, char * fmt, ...){    
    Color prevColor = getFontColor();
    setFontColor(color.r, color.g, color.b);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    setFontColor(prevColor.r, prevColor.g, prevColor.b);
}


void printNChars(char c, int n) {
    for (int i = 0; i < n; i++)
        putchar(c);
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
    while((c = getchar()) != '\n' && bIdx < MAX_CHARS-1){
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
            } else {
                playSoundLimitReached();
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
            printErr("Error!!!"); 
        }
        fmtPtr++;
    }
    buffer[bIdx-1] = 0;
    va_end(v);
    return qtyParams;
}

static char * _regNames[] = {"RAX", "RBX", "RCX", "RDX", "RBP", "RDI", "RSI", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15"};
void printRegisters(const uint64_t * rsp) {
    for (int i = 0; i < sizeof(_regNames)/sizeof(char *); i++)
        printf("%s: 0x%x\n", _regNames[i], rsp[sizeof(_regNames)/sizeof(char *)-i-1]);
}