#ifndef STDLIB_H
#define STDLIB_H
int atoi(char* s);
int strtoi(char* s, char ** end);
int strtoh(char* s, char ** end);
char* itoa(int n, char* buffer, int base);
#endif