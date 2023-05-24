#include <process.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>

void *psPrint(int argc, char **argv) {
	return 0;
}

int loop(int argc, char **argv) {
	uint16_t pid = getpid();
	int seconds = atoi(argv[1]);
	int currentTime = 0, oldTime = 0, initialTime = getSeconds();
	while (1) {
		currentTime = getSeconds();
		if (currentTime != oldTime && !((currentTime - initialTime) % seconds)) {
			printf("Hola, soy el proceso: %d\n", pid);
		}
		oldTime = currentTime;
	}
	return 0;
}

int nice(int argc, char **argv) {
	return 0;
}

int block(int argc, char **argv) {
	return 0;
}