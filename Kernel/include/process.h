#ifndef _PROCESS_H
#define _PROCESS_H
#include <stdint.h>

typedef enum { BLOCKED = 0,
			   READY,
			   RUNNING,
			   ZOMBIE } ProcessStatus;
typedef int (*MainFunction)(int argc, char **args);

typedef struct Process { // PCB
	uint16_t pid;
	uint16_t parentPid;
	void *stackBase; // MemoryBlock
	void *stackPos;
	// void * heap;  // Lista de MemoryBlocks
	char *name;
	// uint32_t flags;
	uint8_t priority;
	ProcessStatus status;
	// uint8_t * fd;
} Process;

void initProcess(Process *process, uint16_t pid, uint16_t parentPid, MainFunction code, char **args, char *name, uint8_t priority);

#endif