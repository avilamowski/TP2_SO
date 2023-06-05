// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <globals.h>
#include <process.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>

static int spawnAndBlock(int argc, char **argv);

int loop(int argc, char **argv) {
	if (argc != 2) {
		printErr("Argumentos invalidos\n"); // TODO: Uniformizar?
		return -1;
	}
	uint16_t pid = getpid();
	while (1) {
		sleep(atoi(argv[1]));
		printf("Hola, soy el proceso: %d\n", pid);
	}
	return 0;
}

int testProgram(int argc, char **argv) {
	printf("Inicio %d\n", argc);
	char *args[2] = {"ResidentEvil", 0};
	int16_t pid = createProcess(&spawnAndBlock, args, "ResidentEvil", 4);
	uint32_t ret_value = waitpid(pid);
	printf("Finalizo! pid del hijo:%d %d\n", pid, ret_value);
	return 0;
}

static int spawnAndBlock(int argc, char **argv) {
	uint16_t pid = getpid();
	changeProcessStatus(pid, BLOCKED);
	return 0;
}

int filter(int argc, char **argv) {
	char c;
	while ((int) (c = getchar()) != EOF) {
		if (toLower(c) == 'a' || toLower(c) == 'e' || toLower(c) == 'i' || toLower(c) == 'o' || toLower(c) == 'u')
			putchar(c);
	}
	putchar('\n');
	return 0;
}

int wc(int argc, char **argv) {
	char c;
	int lineCounter = 0;
	while ((int) (c = getchar()) != EOF)
		lineCounter += (c == '\n');
	printf("La cantidad de lineas es: %d\n", lineCounter);
	return 0;
}

int cat(int argc, char **argv) {
	char c;
	while ((int) (c = getchar()) != EOF)
		putchar(c);
	return 0;
}