#include <stdlib.h>
#include <stdio.h>

#define isNumber(n) ((n) >= '0' && (n) <= '9')
#define isHex(n) ((n) >= 'a' && (n) <= 'f')

static unsigned int log(int n, int base) {
    unsigned int count = 1;
    while (n /= base)
        count++;
    return count;
}

int itoa(int n, char* buffer, int base)
{
 
    if (n == 0)
    {
        buffer[0] = '0';
        buffer[1] = '\0';
        return 1;
    }
 
    unsigned int len = 0;
    int i = 0;
    if (n < 0 && base == 10)
    {
        n = -n;
        buffer[i] = '-';
        len++;
        i++;
    }
 
    len += log(n, base);
    while (n != 0)
    {
        int r = n % base;
        buffer[len - i++ - 1] = (r > 9)? (r-10) + 'A' : r + '0';
        n /= base;
    }
    buffer[i] = '\0'; 
    return len;
}

int atoi(char * s)
{
    int num = 0;
    while (isNumber(*s))
        num = num * 10 + *(s++) - '0';
    return num;
}

int strtoi(char* s, char ** end) {
    int num = 0;
    while (isNumber(*s))
        num = num * 10 + *(s++) - '0';
    *end = s; 
    return num;
}

// Solo soporta mayusculas por ahora
int strtoh(char* s, char ** end) {
    int num = 0;
    while (isNumber(*s) || isHex(*s)) {
        num = num * 16 + isNumber(*s)*(*s - '0') + isHex(*s)*(*s - 'a' + 10);
        s++;
    }
    *end = s; 
    return num;
}