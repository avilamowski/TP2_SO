#include <libasm.h>
#include <man.h>
#include <process.h>
#include <shell.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syscalls.h>
#include <test_mm.h>
#include <test_prio.h>
#include <test_processes.h>
#include <test_sync.h>
#include <tron.h>

/* Enum para la cantidad de argumentos recibidos */
typedef enum { NO_PARAMS = 0,
			   SINGLE_PARAM,
			   DUAL_PARAM } functionType;
#define QTY_BYTES 32 /* Cantidad de bytes de respuesta del printmem */
#define DEFAULT_FONT_SIZE 1
#define MIN_FONT_SIZE 1
#define MAX_FONT_SIZE 3

#define WELCOME "Bienvenido a Cactiland OS!\n"
#define INVALID_COMMAND "Comando invalido!\n"
#define WRONG_PARAMS "La cantidad de parametros ingresada es invalida\n"
#define INVALID_FONT_SIZE "Dimension invalida de fuente\n"
#define CHECK_MAN "Escriba \"man %s\" para ver como funciona el comando\n"
#define CHECK_MAN_FONT \
	"Escriba \"man font-size\" para ver las dimensiones validas\n"

typedef struct {
	char *name;		   // Nombre del comando
	char *description; // Descripcion del comando (para help)
	union {			   // Puntero a la funcion
		int (*f)(void);
		int (*g)(char *);
		int (*h)(char *, char *);
	};
	functionType ftype; // Cantidad de argumentos del comando
} Command;

static void help();
static void man(char *command);
static void printInfoReg();
static void time();
static int div(char *num, char *div);
static void tron();
static void tronZen();
static void fontSize(char *size);
static void printMem(char *pos);
static int getCommandIndex(char *command);
static void test(char *name, char *param);
static void runLoop(char *delay);
static void runKill(char *pid);
static void runBlock(char *pid);
static void runUnblock(char *pid);
static void runNice(char *pid, char *priority);

static Command commands[QTY_COMMANDS];

void init() {
	commands[0] =
		(Command){"help", "Listado de comandos", .f = (void *) &help, NO_PARAMS};
	commands[1] = (Command){"man", "Manual de uso de los comandos",
							.g = (void *) &man, SINGLE_PARAM};
	commands[2] = (Command){"inforeg",
							"Informacion de los registos que fueron capturados "
							"en un momento arbitrario de ejecucion del sistema",
							.f = (void *) &printInfoReg, NO_PARAMS};
	commands[3] = (Command){"time", "Despliega la hora actual UTC - 3",
							.f = (void *) &time, NO_PARAMS};
	commands[4] = (Command){
		"div",
		"Hace la division entera de dos numeros naturales enviados por parametro",
		.h = (void *) &div, DUAL_PARAM};
	commands[5] = (Command){"kaboom", "Ejecuta una excepcion de Invalid Opcode",
							.f = (void *) &kaboom, NO_PARAMS};
	commands[6] = (Command){"tron", "Juego Tron Light Cycles", .f = (void *) &tron,
							NO_PARAMS};
	commands[7] =
		(Command){"tron-zen", "Juego Tron Light Cycles con un unico jugador",
				  .f = (void *) &tronZen, NO_PARAMS};
	commands[8] = (Command){"font-size",
							"Cambio de dimensiones de la fuente. Para hacerlo "
							"escribir el comando seguido de un numero",
							.g = (void *) &fontSize, SINGLE_PARAM};
	commands[9] = (Command){
		"printmem",
		"Realiza un vuelco de memoria de los 32 bytes posteriores a una "
		"direccion de memoria en formato hexadecimal enviada por parametro",
		.g = (void *) &printMem, SINGLE_PARAM};
	commands[10] = (Command){"clear", "Limpia toda la pantalla",
							 .f = (void *) &clear, NO_PARAMS};
	commands[11] = (Command){"test", "Permite ejecutar un programa de prueba",
							 .h = (void *) &test, DUAL_PARAM};
	commands[12] = (Command){"loop", "Loop", .g = (void *) &runLoop, SINGLE_PARAM};
	commands[13] = (Command){"kill", "Kill", .g = (void *) &runKill, SINGLE_PARAM};
	commands[14] = (Command){"ps", "ps", .f = (void *) &psPrint, NO_PARAMS};
	commands[15] = (Command){"nice", "69", .h = (void *) &runNice, DUAL_PARAM};
	commands[16] = (Command){"block", "bloquea el proceso con el pid recibido", .g = (void *) &runBlock, SINGLE_PARAM};
	commands[17] = (Command){"unblock", "desbloquea el proceso con el pid recibido", .g = (void *) &runUnblock, SINGLE_PARAM};
	commands[18] = (Command){"yield", "desbloquea el proceso con el pid recibido", .g = (void *) &unblock, SINGLE_PARAM};
}

void run_shell() {
	init();
	int index;
	puts(WELCOME);
	while (1) {
		putchar('>');
		char command[MAX_CHARS] = {0};
		char arg1[MAX_CHARS];
		char arg2[MAX_CHARS];
		int qtyParams = scanf("%s %s %s", command, arg1, arg2);
		index = getCommandIndex(command);
		if (index == -1) {
			if (command[0] != 0)
				printErr(INVALID_COMMAND);
			continue;
		}
		int funcParams = commands[index].ftype;
		if (qtyParams - 1 != funcParams) {
			printErr(WRONG_PARAMS);
			printf(CHECK_MAN, command);
			continue;
		}
		switch (commands[index].ftype) {
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

/**
 * @brief  Devuelve el indice del vector de comandos dado su nombre
 * @param  command: Nombre del comando a buscar
 * @return  Indice del comando
 */
static int getCommandIndex(char *command) {
	int idx = 0;
	for (; idx < QTY_COMMANDS; idx++) {
		if (!strcmp(commands[idx].name, command))
			return idx;
	}
	return -1;
}

static void help() {
	for (int i = 0; i < QTY_COMMANDS; i++)
		printf("%s: %s\r\n", commands[i].name, commands[i].description);
}

static int div(char *num, char *div) {
	printf("%s/%s=%d\r\n", num, div, atoi(num) / atoi(div));
	return 1;
}

static void time() {
	uint32_t secs = getSeconds();
	uint32_t h = secs / 3600, m = secs % 3600 / 60, s = secs % 3600 % 60;
	printf("%2d:%2d:%2d\r\n", h, m, s);
}

static void fontSize(char *size) {
	int s = atoi(size);
	if (s >= MIN_FONT_SIZE && s <= MAX_FONT_SIZE)
		setFontSize((uint8_t) atoi(size));
	else {
		printErr(INVALID_FONT_SIZE);
		puts(CHECK_MAN_FONT);
	}
}

static void tron() {
	setFontSize(2);
	startTron(2);
	setFontSize(1);
}

static void tronZen() {
	setFontSize(2);
	startTron(1);
	setFontSize(1);
}

static void printMem(char *pos) {
	uint8_t resp[QTY_BYTES];
	char *end;
	getMemory(strtoh(pos, &end), resp);
	for (int i = 0; i < QTY_BYTES; i++) {
		printf("0x%2x ", resp[i]);
		if (i % 4 == 3)
			putchar('\n');
	}
}

static char *_regNames[] = {"RIP", "RSP", "RAX", "RBX", "RCX", "RDX",
							"RBP", "RDI", "RSI", "R8", "R9", "R10",
							"R11", "R12", "R13", "R14", "R15"};
static void printInfoReg() {
	int len = sizeof(_regNames) / sizeof(char *);
	uint64_t regSnapshot[len];
	getInfoReg(regSnapshot);
	for (int i = 0; i < len; i++)
		printf("%s: 0x%x\n", _regNames[i], regSnapshot[i]);
}

static void man(char *command) {
	int idx = getCommandIndex(command);
	if (idx != -1)
		printf("%s\n", usages[idx]);
	else
		printErr(INVALID_COMMAND);
}

static void test(char *name, char *param) {
	if (!strcmp(name, "test-mm")) {
		char *args[] = {"test_mm", param, NULL};
		createProcess(&test_mm, args, "test_mm", 4);
		// test_mm(1, (char *[]){param});
	}
	else if (!strcmp(name, "test-processes")) {
		char *args[] = {"test_processes", param, NULL};
		createProcess(&test_processes, args, "test_processes", 4);
		// test_processes(1, (char *[]){param});
	}
	else if (!strcmp(name, "test-prio")) {
		char *args[] = {"test_prio", param, NULL};
		createProcess(&test_prio, args, "test_prio", 4);
	}
	else if (!strcmp(name, "test-sync")) {
		char *args[] = {"test_sync", param, "1", NULL};
		createProcess(&test_sync, args, "test_sync", 4);
	}
	else if (!strcmp(name, "test-no-sync")) {
		char *args[] = {"test_sync", param, "0", NULL};
		createProcess(&test_sync, args, "test_no_sync", 4);
	}
	else if (!strcmp(name, "test-filter")) {
		uint16_t filterPid, echoPid;
		char *argsFilter[] = {"test_filter", NULL};
		int16_t fdsFilter[3] = {404, STDOUT, STDERR};
		filterPid = createProcessWithFds(&filter, argsFilter, "test_filter", 4, fdsFilter);
		int16_t fdsEcho[3] = {DEV_NULL, 404, STDERR};
		char *argsEcho[] = {"test_echo", param, NULL};
		echoPid = createProcessWithFds(&echo, argsEcho, "test_echo", 4, fdsEcho);
		waitpid(filterPid);
		waitpid(echoPid);
	}
	else {
		printErr(INVALID_COMMAND);
		// TODO: Mover?
		char *args[] = {"ZombieCreator", NULL};
		createProcess(&testProgram, args, "ZombieCreator", (uint8_t) 4);
	}
}

static void runLoop(char *delay) {
	char *args[] = {"loop", delay, NULL};
	createProcess(&loop, args, "loop", (uint8_t) 4);
}

static void runKill(char *pid) {
	killProcess((uint16_t) atoi(pid));
}

static void runUnblock(char *pid) {
	unblock((uint16_t) atoi(pid));
}

static void runBlock(char *pid) {
	block((uint16_t) atoi(pid));
}

static void runNice(char *pid, char *priority) {
	nice((uint16_t) atoi(pid), (uint8_t) atoi(priority));
}