#ifndef _PROCESS_H
#define _PROCESS_H
#include <globals.h>
#include <linkedListADT.h>
#include <stdint.h>
#define BUILT_IN_DESCRIPTORS 3

typedef struct Process { // PCB
	uint16_t pid;
	uint16_t parentPid;
	uint16_t waitingForPid;
	void *stackBase; // MemoryBlock
	void *stackPos;
	// void * heap;  // Lista de MemoryBlocks
	char **argv;
	char *name;
	// uint32_t flags;
	uint8_t priority;
	ProcessStatus status;
	int16_t fileDescriptors[BUILT_IN_DESCRIPTORS];
	int32_t retValue;
	LinkedListADT zombieChildren;
} Process;

void initProcess(Process *process, uint16_t pid, uint16_t parentPid, MainFunction code, char **args, char *name, uint8_t priority, int16_t fileDescriptors[]);
ProcessSnapshot *loadSnapshot(ProcessSnapshot *snapshot, Process *process);
int processIsWaiting(Process *process, uint16_t pidToWait);
int getZombiesSnapshots(int processIndex, ProcessSnapshot psArray[], Process *nextProcess);
void freeProcess(Process *process);

#endif