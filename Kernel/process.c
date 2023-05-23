#include <defs.h>
#include <interrupts.h>
#include <lib.h>
#include <memoryManager.h>
#include <process.h>

#define STACK_SIZE (1 << 12)

void processWrapper(MainFunction code, char **args) {
	int len = stringArrayLen(args);
	int retValue = code(len, args);

	// TODO: Free de cada argumento?

	// TODO: Desbloquear padre con waitpid???
	// giveForeground(processList[getCurrentPID()]->parent);

	// killProcess(getCurrentPID(), retValue);
}

void initProcess(Process *process, uint16_t pid, uint16_t parentPid, int code(int argc, char **args), char **args, char *name, uint8_t priority) {
	process->pid = pid;
	process->parentPid = parentPid;
	process->stackBase = allocMemory(STACK_SIZE);
	process->name = allocMemory(strlen(name) + 1);
	strcpy(process->name, name);
	process->priority = priority;
	void *stackEnd = (void *) ((uint64_t) process->stackBase + STACK_SIZE);
	process->stackPos = _initialize_stack_frame(&processWrapper, code, stackEnd, args);
	process->status = READY;
}

void freeProcess(Process *process) {
	free(process->stackBase);
	free(process->name);
	free(process);
}