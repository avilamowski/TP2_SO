#include <string.h>

int strcpy(char * dest, char * origin) {
    return strcpychar(dest, origin, '\0');
}

int strcpychar(char * dest, char * origin, char limit) {
    int idx = 0;
    while (origin[idx] != limit && origin[idx] != '\0') {
        dest[idx] = origin[idx];
        idx++;
    }
    dest[idx] = 0;
    return idx;
}