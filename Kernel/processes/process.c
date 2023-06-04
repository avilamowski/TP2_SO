#include <defs.h>
#include <interrupts.h>
#include <lib.h>
#include <linkedListADT.h>
#include <memoryManager.h>
#include <pipeManager.h>
#include <process.h>
#include <scheduler.h>
#include <stdlib.h>
#include <string.h>
#include <video.h>

#define STACK_SIZE (1 << 12)

static char **allocArguments();
static void assignFileDescriptor(Process *process, uint8_t fdIndex, int16_t fdValue, uint8_t mode);
static void closeFileDescriptor(uint16_t pid, int16_t fdValue);

void processWrapper(MainFunction code, char **args) {
	int len = stringArrayLen(args);
	// printf("qty: %d, argv[0]: %s, argv[1]: %s\n", len, args[0], args[1]);
	int retValue = code(len, args);
	killCurrentProcess(retValue);
}

void initProcess(Process *process, uint16_t pid, uint16_t parentPid,
				 MainFunction code, char **args, char *name,
				 uint8_t priority, int16_t fileDescriptors[], uint8_t unkillable) {
	process->pid = pid;
	process->parentPid = parentPid;
	process->waitingForPid = 0;
	process->stackBase = allocMemory(STACK_SIZE);
	process->argv = allocArguments(args);
	process->name = allocMemory(strlen(name) + 1);
	strcpy(process->name, name);
	process->priority = priority;
	void *stackEnd = (void *) ((uint64_t) process->stackBase + STACK_SIZE);
	process->stackPos = _initialize_stack_frame(&processWrapper, code, stackEnd, (void *) process->argv);
	process->status = READY;
	process->zombieChildren = createLinkedListADT();
	process->unkillable = unkillable;

	assignFileDescriptor(process, STDIN, fileDescriptors[STDIN], READ);
	assignFileDescriptor(process, STDOUT, fileDescriptors[STDOUT], WRITE);
	assignFileDescriptor(process, STDERR, fileDescriptors[STDERR], WRITE);
}

static void assignFileDescriptor(Process *process, uint8_t fdIndex, int16_t fdValue, uint8_t mode) {
	process->fileDescriptors[fdIndex] = fdValue;
	if (fdValue >= BUILT_IN_DESCRIPTORS)
		pipeOpenForPid(process->pid, fdValue, mode);
}

void closeFileDescriptors(Process *process) {
	closeFileDescriptor(process->pid, process->fileDescriptors[STDIN]);
	closeFileDescriptor(process->pid, process->fileDescriptors[STDOUT]);
	closeFileDescriptor(process->pid, process->fileDescriptors[STDERR]);
}

static void closeFileDescriptor(uint16_t pid, int16_t fdValue) {
	if (fdValue >= BUILT_IN_DESCRIPTORS)
		pipeCloseForPid(pid, fdValue);
}

static char **allocArguments(char **args) {
	int argc = stringArrayLen(args), totalArgsLen;
	int argsLen[argc];
	for (int i = 0; i < argc; i++) {
		argsLen[i] = strlen(args[i]) + 1;
		totalArgsLen += argsLen[i];
	}
	char **newArgsArray = (char **) allocMemory(totalArgsLen + sizeof(char **) * (argc + 1));
	char *charPosition = (char *) newArgsArray + (sizeof(char **) * (argc + 1));
	for (int i = 0; i < argc; i++) {
		newArgsArray[i] = charPosition;
		memcpy(charPosition, args[i], argsLen[i]);
		charPosition += argsLen[i];
	}
	newArgsArray[argc] = NULL;
	return newArgsArray;
}

void freeProcess(Process *process) {
	freeLinkedListADT(process->zombieChildren);
	free(process->stackBase);
	free(process->name);
	free(process->argv);
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
	snapshot->foreground = process->fileDescriptors[STDIN] == STDIN;
	return snapshot;
}

int processIsWaiting(Process *process, uint16_t pidToWait) {
	return process->waitingForPid == pidToWait && process->status == BLOCKED;
}

int getZombiesSnapshots(int processIndex, ProcessSnapshot psArray[], Process *nextProcess) {
	LinkedListADT zombieChildren = nextProcess->zombieChildren;
	begin(zombieChildren);
	while (hasNext(zombieChildren))
		loadSnapshot(&psArray[processIndex++], (Process *) next(zombieChildren));
	return processIndex;
}