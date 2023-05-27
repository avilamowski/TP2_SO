#include <process.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>

// TODO: Ver de mover a otro archivo
typedef enum { BLOCKED = 0,
			   READY,
			   RUNNING,
			   ZOMBIE } ProcessStatus;
typedef struct ProcessSnapshot {
	uint16_t pid;
	uint16_t parentPid;
	void *stackBase;
	void *stackPos;
	char *name;
	uint8_t priority;
	ProcessStatus status;
	uint8_t foreground;
} ProcessSnapshot;
typedef struct ProcessSnapshotList {
	uint16_t length;
	ProcessSnapshot *snapshotList;
} ProcessSnapshotList;

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
