#include <defs.h>
#include <interrupts.h>
#include <lib.h>
#include <linkedListADT.h>
#include <memoryManager.h>
#include <process.h>
#include <video.h>

#define STACK_SIZE (1 << 12)

void processWrapper(MainFunction code, char **args) {
	int len = stringArrayLen(args);
	// printf("qty: %d, argv[0]: %s, argv[1]: %s\n", len, args[0], args[1]);

	int retValue = code(len, args);

	// TODO: Free de cada argumento?
	// giveForeground(processList[getCurrentPID()]->parent);
	killCurrentProcess(retValue);
}

void initProcess(Process *process, uint16_t pid, uint16_t parentPid, int code(int argc, char **args), char **args, char *name, uint8_t priority) {
	process->pid = pid;
	process->parentPid = parentPid;
	process->waitingForPid = 0;
	process->stackBase = allocMemory(STACK_SIZE);
	process->name = allocMemory(strlen(name) + 1);
	strcpy(process->name, name);
	process->priority = priority;
	void *stackEnd = (void *) ((uint64_t) process->stackBase + STACK_SIZE);
	process->stackPos = _initialize_stack_frame(&processWrapper, code, stackEnd, args);
	process->status = READY;
	process->zombieChildren = createLinkedListADT();
}

void freeProcess(Process *process) {
	freeLinkedListADT(process->zombieChildren);
	free(process->stackBase);
	free(process->name);
	free(process);
}

ProcessSnapshot *loadSnapshot(ProcessSnapshot *snapshot, Process *process) {
	snapshot->name = allocMemory(strlen(process->name) + 1);
	strcpy(snapshot->name, process->name);
	snapshot->pid = process->pid;
	snapshot->parentPid = process->parentPid;
	snapshot->stackBase = process->stackBase;
	snapshot->stackPos = process->stackPos;
	snapshot->priority = process->priority;
	snapshot->status = process->status;
	snapshot->foreground = 1; // TODO: Poner un valor representativo
	return snapshot;
}

int processIsWaiting(Process *process, uint16_t pidToWait) {
	return process->waitingForPid = pidToWait && process->status == BLOCKED;
}

int getZombiesSnapshots(int processIndex, ProcessSnapshot psArray[], Process *nextProcess) {
	LinkedListADT zombieChildren = nextProcess->zombieChildren;
	begin(zombieChildren);
	while (hasNext(zombieChildren))
		loadSnapshot(&psArray[processIndex++], (Process *) next(zombieChildren));
	return processIndex;
}
