#include <defs.h>
#include <linkedListADT.h>
#include <memoryManager.h>
#include <scheduler.h>
#define QTY_LEVELS 5
#define MAX_PROCESSES (1 << 10)

typedef struct SchedulerCDT {
	Node *processes[MAX_PROCESSES];
	LinkedListADT levels[QTY_LEVELS];
	uint16_t currentPid;
	uint16_t nextUnusedPid;
	uint8_t remainingQuantum;
} SchedulerCDT;

SchedulerADT createScheduler() {
	SchedulerADT scheduler = (SchedulerADT) SCHEDULER_ADDRESS;
	for (int i = 0; i < MAX_PROCESSES; i++)
		scheduler->processes[i] = NULL;
	for (int i = 0; i < QTY_LEVELS; i++)
		scheduler->levels[i] = NULL;

	scheduler->nextUnusedPid = 0;
	// Node * process = (Node *) allocMemory(sizeof(Node));
	return scheduler;
}

SchedulerADT getSchedulerADT() {
	return (SchedulerADT) SCHEDULER_ADDRESS;
}

static uint16_t getNextPid(SchedulerADT scheduler) {
	Process *process = NULL;
	for (int lvl = QTY_LEVELS - 1; lvl >= 0 && process == NULL; lvl--) {
		if (!isEmpty(scheduler->levels[lvl])) {
			process = (Process *) (getFirst(scheduler->levels[lvl]))->data;
		}
	}
	// TODO: Panic si process es null?
	return process->pid;
}

static void setPriority(SchedulerADT scheduler, Node *node, uint8_t newPriority) {
	Process *process = (Process *) node->data;
	if (newPriority <= 0 || newPriority > QTY_LEVELS)
		return;
	removeNode(scheduler->levels[process->priority], node);
	process->priority = newPriority;
	scheduler->processes[process->pid] = appendNode(scheduler->levels[process->priority], process);
}

void *schedule(void *prevStackPointer) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *currentProcessNode = scheduler->processes[scheduler->currentPid];
	Process *currentProcess = (Process *) currentProcessNode->data;
	currentProcess->stackPos = prevStackPointer;
	if (currentProcess->status == RUNNING)
		currentProcess->status = READY;

	// TODO: Analizar si vale la pena hacerlo ciclico
	uint8_t newPriority = currentProcess->priority > 0 ? currentProcess->priority - 1 : currentProcess->priority;
	setPriority(scheduler, currentProcessNode, newPriority);

	scheduler->currentPid = getNextPid(scheduler);
	currentProcess = scheduler->processes[scheduler->currentPid];
	currentProcess->status = RUNNING;
	return currentProcess->stackPos;
}

uint16_t createProcess(MainFunction code, char **args, char *name, uint8_t priority) {
	SchedulerADT scheduler = getSchedulerADT();
	Process *process = (Process *) allocMemory(sizeof(Process));
	initProcess(process, scheduler->nextUnusedPid, scheduler->currentPid, code, args, name, priority);

	Node *processNode = appendNode(scheduler->levels[process->priority], (void *) process);
	scheduler->processes[process->pid] = processNode;
	return process->pid;
}

void killProcess(uint16_t pid, int retValue) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *processToKillNode = scheduler->processes[scheduler->currentPid];
	Process *processToKill = (Process *) processToKillNode->data;
	scheduler->processes[pid] = NULL;
	// if (pid < scheduler->nextUnusedPid) scheduler->nextUnusedPid = pid;
	// TODO chequear si hay que buscar linealmente el siguiente

	free(processToKill);
}