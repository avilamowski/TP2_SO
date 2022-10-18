#include <unistd.h>
#include <stdio.h>

int putchar(int c) {
    write(STDOUT, &c, 1);
    return 1;
}

int puts(const char * s) {
    while (*s) putchar(*(s++)); 
    return 1;
}

