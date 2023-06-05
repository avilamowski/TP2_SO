// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <globals.h>
#include <process.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int zombieCreator(int argc, char **argv) {
	char *args[2] = {"kill_me", NULL};
	int16_t pid = createProcess(&spawnAndBlock, args, "kill_me", 4);
	printf("Por favor mate al proceso: %d\n", pid);
	waitpid(pid);
	printf("Finalizo! pid del hijo:%d\n", pid);
	return 0;
}

static int spawnAndBlock(int argc, char **argv) {
	uint16_t pid = getpid();
	changeProcessStatus(pid, BLOCKED);
	return 0;
}

#define TEST_PIPE 200
static int testWriter(int argc, char **argv) {
	pipeOpen(TEST_PIPE, WRITE);
	char *message = "***Secreto***";
	puts("Soy el escritor, enviando mensaje...\n");
	write(TEST_PIPE, message, strlen(message) + 1);
	pipeClose(TEST_PIPE, WRITE);
	return 0;
}

#define TEST_BUFFER_LEN 100
static int testReader(int argc, char **argv) {
	pipeOpen(TEST_PIPE, READ);
	char received[TEST_BUFFER_LEN] = {0};
	read(TEST_PIPE, received, TEST_BUFFER_LEN);
	printf("Soy el lector, mensaje: %s\n", received);
	pipeClose(TEST_PIPE, READ);
	return 0;
}

int testNamedPipes(int argc, char **argv) {
	char *paramsReader[] = {"test_reader", NULL};
	uint16_t pidReader = createProcess((void *) &testReader, paramsReader, "test_reader", 4);
	char *paramsWriter[] = {"test_writer", NULL};
	uint16_t pidWriter = createProcess((void *) &testWriter, paramsWriter, "test_writer", 4);
	waitpid(pidReader);
	waitpid(pidWriter);
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