#include <globals.h>
#include <stdint.h>
#include <stdio.h>
#include <syscalls.h>

void nice(uint16_t pid, uint8_t priority) {
	changeProcessPriority(pid, priority);
}

int8_t block(uint16_t pid) {
	return changeProcessStatus(pid, BLOCKED);
}

int8_t unblock(uint16_t pid) {
	return changeProcessStatus(pid, READY);
}

int16_t createProcess(void *code, char **args, char *name, uint8_t priority) {
	int16_t fileDescriptors[] = {STDIN, STDOUT, STDERR};
	return createProcessWithFds(code, args, name, priority, fileDescriptors);
}

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

int mem(int argc, char **argv) {
	MemoryInfo *memoryInfo = getMemoryInfo();
	printf("%s MemoryManager:\ntotalBlocks:%d    freeBlocks:%d    usedBlocks:%d    totalMemory:%d    freeMemory:%d    usedMemory:%d\n", memoryInfo->name, memoryInfo->totalBlocks, memoryInfo->freeBlocks, memoryInfo->usedBlocks, memoryInfo->totalMemory, memoryInfo->freeMemory, memoryInfo->usedMemory);
	free(memoryInfo);
	return 0;
}