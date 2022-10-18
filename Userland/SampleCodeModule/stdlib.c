#include <stdlib.h>
#define isNumber(n) (n >= '0' && n <= '9')
#define isHex(n) (n >= 'A' && n <= 'F')

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
        num = num * 16 + isNumber(*s)*(*s - '0') + isHex(*s)*(*s - 'A' + 10);
        s++;
    }
    *end = s; 
    return num;
}