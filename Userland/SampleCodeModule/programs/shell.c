// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <globals.h>
#include <inputParserADT.h>
#include <libasm.h>
#include <man.h>
#include <philosophers.h>
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
#define continueIfInvalid(parser, index) \
	;                                    \
	if ((index) == -1) {                 \
		printErr(INVALID_COMMAND);       \
		return;                          \
	}
#define FATAL_ERROR_PIPES "Ocurrio un error fatal relacionado con los pipes"

typedef struct {
	char *name;		   // Nombre del comando
	char *description; // Descripcion del comando (para help)
	MainFunction code;
} Command;

static void help(int argc, char **argv);
static void echo(int argc, char **argv);
static void man(int argc, char **argv);
static void printInfoReg();
static void time();
static void div(int argc, char **argv);
static void tron();
static void tronZen();
static void fontSize(int argc, char **argv);
static void printMem(int argc, char **argv);
static void runKill(int argc, char **argv);
static void toggleBlock(int argc, char **argv);
static void runNice(int argc, char **argv);

static int getCommandIndex(char *command);
static void createSingleProcess(InputParserADT parser);
static void createPipedProcesses(InputParserADT parser);

const static Command commands[] = {
	{"help", "Listado de comandos", (MainFunction) help},
	{"man", "Manual de uso de los comandos", (MainFunction) man},
	{"inforeg", "Informacion de los registos que fueron capturados en un momento arbitrario de ejecucion del sistema", (MainFunction) printInfoReg},
	{"time", "Despliega la hora actual UTC - 3", (MainFunction) time},
	{"div", "Hace la division entera de dos numeros naturales enviados por parametro", (MainFunction) div},
	{"kaboom", "Ejecuta una excepcion de Invalid Opcode", (MainFunction) kaboom},
	{"tron", "Juego Tron Light Cycles", (MainFunction) tron},
	{"tron-zen", "Juego Tron Light Cycles con un unico jugador", (MainFunction) tronZen},
	{"font-size", "Cambio de dimensiones de la fuente. Para hacerlo escribir el comando seguido de un numero", (MainFunction) fontSize},
	{"printmem", "Realiza un vuelco de memoria de los 32 bytes posteriores a una direccion de memoria en formato hexadecimal enviada por parametro", (MainFunction) printMem},
	{"clear", "Limpia toda la pantalla", (MainFunction) clear},
	{"loop", "El proceso imprime su ID con un saludo cada una determinada cantidad de segundos", (MainFunction) loop},
	{"kill", "Mata a un proceso dado su PID", (MainFunction) runKill},
	{"ps", "Muestra la informacion de los procesos vivos y zombificados", (MainFunction) psPrint},
	{"mem", "Muestra informacion del MemoryManager", (MainFunction) mem},
	{"nice", "Cambia la prioridad de un proceso dado su ID y la nueva prioridad", (MainFunction) runNice},
	{"block", "Bloquea el proceso con el pid recibido", (MainFunction) toggleBlock},
	{"echo", "Imprime por pantalla su primer parametro", (MainFunction) echo},
	{"wc", "Cuenta la cantidad de lineas del input", (MainFunction) wc},
	{"cat", "Imprime el STDIN tal como lo recibe", (MainFunction) cat},
	{"filter", "Filtra las vocales del input", (MainFunction) filter},
	{"phylo", "Ejecuta la simulacion del problema de los filosofos comensales", (MainFunction) runPhilosophers},
	{"test-mm", "Ejecuta el test de memory manager", (MainFunction) test_mm},
	{"test-prio", "Ejecuta el test de prioridades", (MainFunction) test_prio},
	{"test-sync", "Ejecutar el test de sincronizacion", (MainFunction) test_sync},
	{"test-processes", "Ejecuta el test de procesos", (MainFunction) test_processes},
	{"test-named-pipes", "Crea dos procesos que se comunican por un pipe con nombre", (MainFunction) testNamedPipes}};

void run_shell() {
	puts(WELCOME);
	while (1) {
		putchar('>');
		char rawInput[MAX_CHARS] = {0};
		scanf("%S", rawInput);
		InputParserADT parser = parseInput(rawInput);
		if (parser == NULL) {
			printErr(INVALID_COMMAND);
			continue;
		}

		if (getProgramQuantity(parser) == 1)
			createSingleProcess(parser);
		else if (getProgramQuantity(parser) == 2)
			createPipedProcesses(parser);
		freeParser(parser);
	}
}

static void createSingleProcess(InputParserADT parser) {
	ShellProgram *program = getProgram(parser, 0);
	int index = getCommandIndex(program->name);
	continueIfInvalid(parser, index);
	if (isInBackground(parser)) {
		int16_t fileDescriptors[] = {DEV_NULL, STDOUT, STDERR};
		createProcessWithFds((void *) commands[index].code, program->params, program->name, 4, fileDescriptors);
	}
	else {
		int16_t pid = createProcess((void *) commands[index].code, program->params, program->name, 4);
		waitpid(pid);
	}
}

static void createPipedProcesses(InputParserADT parser) {
	ShellProgram *firstProgram = getProgram(parser, 0);
	int firstIndex = getCommandIndex(firstProgram->name);
	continueIfInvalid(parser, firstIndex);

	ShellProgram *secondProgram = getProgram(parser, 1);
	int secondIndex = getCommandIndex(secondProgram->name);
	continueIfInvalid(parser, secondIndex);

	int16_t newPipeId = pipeGet();
	if (newPipeId == -1) {
		printErr(FATAL_ERROR_PIPES);
		return;
	}

	int16_t fileDescriptors1[] = {isInBackground(parser) ? DEV_NULL : STDIN, newPipeId, STDERR};
	int16_t firstPid = createProcessWithFds((void *) commands[firstIndex].code, firstProgram->params, firstProgram->name, 4, fileDescriptors1);

	int16_t fileDescriptors2[] = {newPipeId, STDOUT, STDERR};
	createProcessWithFds((void *) commands[secondIndex].code, secondProgram->params, secondProgram->name, 4, fileDescriptors2);

	if (!isInBackground(parser))
		waitpid(firstPid);
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

static void help(int argc, char **argv) {
	if (argc != 1) {
		printErr(WRONG_PARAMS);
		return;
	}
	for (int i = 0; i < QTY_COMMANDS; i++)
		printf("%s: %s\n", commands[i].name, commands[i].description);
}

static void echo(int argc, char **argv) {
	if (argc != 2) {
		printErr(WRONG_PARAMS);
		return;
	}
	printf("%s\n", argv[1]);
}

static void div(int argc, char **argv) {
	if (argc != 3) {
		printErr(WRONG_PARAMS);
		return;
	}
	char *num = argv[1], *div = argv[2];
	printf("%s/%s=%d\n", num, div, atoi(num) / atoi(div));
}

static void time(int argc, char **argv) {
	uint32_t secs = getSeconds();
	uint32_t h = secs / 3600, m = secs % 3600 / 60, s = secs % 3600 % 60;
	printf("%2d:%2d:%2d\n", h, m, s);
}

static void fontSize(int argc, char **argv) {
	if (argc != 2) {
		printErr(WRONG_PARAMS);
		return;
	}
	int s = atoi(argv[1]);
	if (s >= MIN_FONT_SIZE && s <= MAX_FONT_SIZE)
		setFontSize((uint8_t) atoi(argv[1]));
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

static void printMem(int argc, char **argv) {
	if (argc != 2) {
		printErr(WRONG_PARAMS);
		return;
	}
	uint8_t resp[QTY_BYTES];
	char *end;
	getMemory(strtoh(argv[1], &end), resp);
	for (int i = 0; i < QTY_BYTES; i++) {
		printf("0x%2x ", resp[i]);
		if (i % 4 == 3)
			putchar('\n');
	}
}

static char *_regNames[] = {"RIP", "RSP", "RAX", "RBX", "RCX", "RDX",
							"RBP", "RDI", "RSI", "R8", "R9", "R10",
							"R11", "R12", "R13", "R14", "R15"};
static void printInfoReg(int argc, char **argv) {
	int len = sizeof(_regNames) / sizeof(char *);
	uint64_t regSnapshot[len];
	getInfoReg(regSnapshot);
	for (int i = 0; i < len; i++)
		printf("%s: 0x%x\n", _regNames[i], regSnapshot[i]);
}

static void man(int argc, char **argv) {
	if (argc != 2) {
		printErr(WRONG_PARAMS);
		return;
	}
	int idx = getCommandIndex(argv[1]);
	if (idx != -1)
		printf("%s\n", usages[idx]);
	else
		printErr(INVALID_COMMAND);
}

static void runKill(int argc, char **argv) {
	if (argc != 2) {
		printErr(WRONG_PARAMS);
		return;
	}
	killProcess((uint16_t) atoi(argv[1]));
}

static void toggleBlock(int argc, char **argv) {
	if (argc != 2) {
		printErr(WRONG_PARAMS);
		return;
	}
	uint16_t pid = (uint16_t) atoi(argv[1]);
	ProcessStatus status = getProcessStatus(pid);
	if (status == BLOCKED)
		changeProcessStatus(pid, READY);
	else if (status == READY)
		changeProcessStatus(pid, BLOCKED);
	else
		printErr("El proceso no se encuentra en un estado donde se pueda alternar entre READY y BLOCKED\n");
}

static void runNice(int argc, char **argv) {
	if (argc != 3) {
		printErr(WRONG_PARAMS);
		return;
	}
	nice((uint16_t) atoi(argv[1]), (uint8_t) atoi(argv[2]));
}