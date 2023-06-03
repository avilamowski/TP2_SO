#include <globals.h>
#include <process.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>

static int spawnAndBlock(int argc, char **argv);

void psPrint() {
	char *statusNames[4] = {"BLOCKED", "READY", "RUNNING", "WALKER"};
	ProcessSnapshotList *snapshots = ps();
	for (int i = 0; i < snapshots->length; i++) {
		ProcessSnapshot *snapshot = &snapshots->snapshotList[i];
		// printf("%s\n", statusNames[snapshot->status]);
		printf("PID: %d - PPID: %d - Nombre: %s - StackBase: %x - StackPos: %x",
			   snapshot->pid, snapshot->parentPid, snapshot->name, snapshot->stackBase, snapshot->stackPos);
		printf("- Prioridad: %d - Fg: %s - Estado: %s\n",
			   snapshot->priority, snapshot->foreground ? "Si" : "No", statusNames[snapshot->status]);
		free(snapshots->snapshotList[i].name);
	}
	free(snapshots->snapshotList);
	free(snapshots);
}

int loop(int argc, char **argv) {
	uint16_t pid = getpid();
	int seconds = atoi(argv[1]);
	// printf("sec: %d", seconds);
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

void nice(uint16_t pid, uint8_t priority) {
	changeProcessPriority(pid, priority);
}

int8_t block(uint16_t pid) {
	return changeProcessStatus(pid, BLOCKED);
}

int8_t unblock(uint16_t pid) {
	return changeProcessStatus(pid, READY);
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

int16_t createProcess(void *code, char **args, char *name, uint8_t priority) {
	int16_t fileDescriptors[] = {STDIN, STDOUT, STDERR};
	return createProcessWithFds(code, args, name, priority, fileDescriptors);
}

int filter(int argc, char **argv) {
	char c;
	while ((c = getchar()) != EOF) {
		if (toLower(c) == 'a' || toLower(c) == 'e' || toLower(c) == 'i' || toLower(c) == 'o' || toLower(c) == 'u')
			putchar(c);
	}
	putchar('\n');
	return 0;
}
