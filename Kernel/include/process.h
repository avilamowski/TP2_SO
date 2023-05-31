#ifndef _PROCESS_H
#define _PROCESS_H
#include <linkedListADT.h>
#include <stdint.h>

typedef enum { BLOCKED = 0,
			   READY,
			   RUNNING,
			   ZOMBIE } ProcessStatus;
typedef int (*MainFunction)(int argc, char **args);

typedef struct Process { // PCB
	uint16_t pid;
	uint16_t parentPid;
	uint16_t waitingForPid;
	void *stackBase; // MemoryBlock
	void *stackPos;
	// void * heap;  // Lista de MemoryBlocks
	char *name;
	// uint32_t flags;
	uint8_t priority;
	ProcessStatus status;
	// uint8_t * fd;
	int32_t retValue;
	LinkedListADT zombieChildren;
} Process;

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

void initProcess(Process *process, uint16_t pid, uint16_t parentPid, MainFunction code, char **args, char *name, uint8_t priority);
ProcessSnapshot *loadSnapshot(ProcessSnapshot *snapshot, Process *process);
int processIsWaiting(Process *process, uint16_t pidToWait);
int getZombiesSnapshots(int processIndex, ProcessSnapshot psArray[], Process *nextProcess);
void freeProcess(Process *process);

#endif