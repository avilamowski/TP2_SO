#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shell.h>
#include <stdint.h>
#include <syscalls.h>
#include <tron.h>
#include <man.h>
#include <libasm.h>

typedef enum {NO_PARAMS = 0, SINGLE_PARAM, DUAL_PARAM} functionType;
#define QTY_BYTES 32

typedef struct {
    char * name;
    char * description;
    union {
       int (*f)(void);
       int (*g)(char *);
       int (*h)(char *, char *);
    };
    functionType ftype;
} Command;

static void help();
static void man(char * command);
static void printInfoReg();
static void time();
static int div(char * num, char * div);
static void tron();
static void tronZen();
static void fontSize(char * size);
static void printMem(char * pos);
static int getCommandIndex(char * command);

static Command commands[QTY_COMMANDS];

void init() {
    commands[0] = (Command){"help", "Listado de comandos", &help, NO_PARAMS};
    commands[1] = (Command){ "man", "Manual de uso de los comandos", &man, SINGLE_PARAM};
    commands[2] = (Command){"inforeg", "Informacion de los registos en un momento arbitrario de ejecucion del sistema", &printInfoReg, NO_PARAMS}; 
    commands[3] = (Command){"time", "Despliega la hora actual", &time, NO_PARAMS};
    commands[4] = (Command){ "div", "Divide dos numeros", &div, DUAL_PARAM};
    commands[5] = (Command){ "kaboom", "que es esto!", &kaboom, NO_PARAMS};
    commands[6] = (Command){ "tron", "el tron", &tron, NO_PARAMS};
    commands[7] = (Command){ "tron-zen", "el snake", &tronZen, NO_PARAMS};
    commands[8] = (Command){ "font-size", "Cambio de tamanio de la fuente. Para hacerlo escribir el comando seguido de un numero", &fontSize, SINGLE_PARAM};
    commands[9] = (Command){ "printmem", "hola", &printMem, SINGLE_PARAM};
    commands[10] = (Command){ "clear", "Limpia toda la pantalla", &clear, NO_PARAMS};
}

void run_shell() {
    init();
    int index;
    puts("Bienvenido a Cactiland OS!\n");
    while(1){
        putchar('>');
        char command[MAX_CHARS] = {0};
        char arg1[MAX_CHARS];
        char arg2[MAX_CHARS];
        int qtyParams = scanf("%s %s %s", command, arg1, arg2); 
        index = getCommandIndex(command);
        if (index == -1) {
            if (command[0] != 0)
                printErr("Comando invalido!\n");
            continue;
        }
        int funcParams = commands[index].ftype;
        if(qtyParams - 1 != funcParams){
            printErr("La cantidad de parametros ingresada es invalida\n");
            printf("Escriba \"man %s\" para ver como funciona el comando.\n", command);
            continue;
        }
        switch (commands[index].ftype)
        {
            case NO_PARAMS: 
                commands[index].f();
                break;
            case SINGLE_PARAM:
                commands[index].g(arg1);
                break;
            case DUAL_PARAM:
                commands[index].h(arg1, arg2);
                break;
        }
    }
}

static int getCommandIndex(char * command) {
    int idx = 0;
    for(; idx < QTY_COMMANDS; idx++){
        if(!strcmp(commands[idx].name, command))
            return idx;
    }    
    return -1;
}

static void help() {
    for (int i = 0; i < QTY_COMMANDS; i++)
        printf("%s: %s\r\n", commands[i].name, commands[i].description);
}

static int div(char * num, char * div) {
    printf("%s/%s=%d\r\n", num, div, atoi(num)/atoi(div));
    return 1;
}

static void time(){
    uint32_t secs = getSeconds();
    int h = secs / 3600, m = secs % 3600 / 60, s = secs % 3600 % 60;
    printf("%2d:%2d:%2d\r\n", h, m, s);
}

static void fontSize(char * size) {
    setFontSize((uint8_t)atoi(size));
}

static void tron(){
    setFontSize(2);
    startTron(2);
    setFontSize(1);
}

static void tronZen(){
    setFontSize(2);
    startTron(1);
    setFontSize(1);
}


static void printMem(char * pos){
    uint8_t resp[QTY_BYTES];
    char * end[1];
    getMemory(strtoh(pos, &end), resp);
    for(int i = 0; i < QTY_BYTES; i++)
        printf("Byte numero %d: 0x%x\n", i, resp[i]);
}

static char * _regNames[] = {"RIP", "RSP", "RAX", "RBX", "RCX", "RDX", "RBP", "RDI", "RSI", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15"};
static void printInfoReg() {
    int len = sizeof(_regNames)/sizeof(char *);
    char * regSnapshot[len];
    getInfoReg(regSnapshot);
    for (int i = 0; i < len; i++)
        printf("%s: %x\n", _regNames[i], regSnapshot[i]);
    
}

static void man(char * command){
    int idx = getCommandIndex(command);
    if (idx != -1)
        printf("%s\n", usages[idx]);
    else
        puts("Comando desconocido\n");
}