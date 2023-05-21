#include <defs.h>
#include <interrupts.h>
#include <lib.h>
#include <memoryManager.h>
#include <process.h>
#include <string.h>

#define STACK_SIZE (1 << 12)

void processWrapper(MainFunction code, char **args) {
	int len = stringArrayLen(args);
	int retValue = code(len, args);

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
	process->stackPos = _initialize_stack_frame(&processWrapper, code, process->stackBase + STACK_SIZE, args);
	process->status = READY;
}
