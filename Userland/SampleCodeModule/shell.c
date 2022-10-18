#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void run_shell() {
    puts("Bienvenido a Cactiland OS\r\n");
    putchar('>');
    char txt[100];
    scanf("%s", txt);
    puts(txt);
}