#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shell.h>
#include <stdint.h>

typedef enum {NO_PARAMS = 0, SINGLE_PARAM, DUAL_PARAM} functionType;
#define QTY_COMMANDS 8
typedef struct Command {
    char * name;
    char * description;
    union {
       int (*f)(void);
       int (*g)(int);
       int (*h)(int, int);
    };
    functionType ftype;
} Command;

static int getCommandIndex(char * command);

static Command commands[QTY_COMMANDS];

void init() {
    commands[0] = (Command){"help", "Listado de comandos", &help, NO_PARAMS};
    commands[1] = (Command){"inforeg", "Informacion de los registos en un momento arbitrario de ejecucion del sistema", 0, NO_PARAMS};
    commands[2] = (Command){"time", "Despliega la hora actual", &time, NO_PARAMS};
    commands[3] = (Command){ "div", "Divide dos numeros", &div, DUAL_PARAM};
    commands[4] = (Command){ "???", "que es esto!", 0, NO_PARAMS};
    commands[5] = (Command){ "tron.elf", "el tron", 0, NO_PARAMS};
    commands[6] = (Command){ "font-size", "hola", 0, SINGLE_PARAM};
    commands[7] = (Command){ "printmem", "hola", 0, SINGLE_PARAM};
    commands[8] = (Command){ "clear", "hola", 0, NO_PARAMS};
}

void run_shell() {
    init();
    int index;
    puts("Welcome to Cactiland OS\r\n");
    while(1){
        putchar('>');
        char command[100];
        int arg1, arg2;
        int qtyParams = scanf("%s %d %d", command, &arg1, &arg2); // TODO: Validar
        index = getCommandIndex(command);
        if (index == -1) {
            puts("Comando invalido!\r\n");
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

void help() {
    for (int i = 0; i < QTY_COMMANDS; i++)
        printf("%s: %s\r\n", commands[i].name, commands[i].description);
}

int div(int num, int div) {
    printf("%d/%d=%d\r\n", num, div, num/div);
    return 1;
}

void time(){
    uint32_t secs = getSeconds();
    int h = secs / 3600, m = secs % 3600 / 60, s = secs % 3600 % 60;
    printf("%d:%d:%d\r\n", h, m, s);
}