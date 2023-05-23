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
	uint16_t qtyProcesses;
	uint8_t remainingQuantum;
} SchedulerCDT;

SchedulerADT createScheduler() {
	SchedulerADT scheduler = (SchedulerADT) SCHEDULER_ADDRESS;
	for (int i = 0; i < MAX_PROCESSES; i++)
		scheduler->processes[i] = NULL;
	for (int i = 0; i < QTY_LEVELS; i++)
		scheduler->levels[i] = createLinkedListADT();

	scheduler->nextUnusedPid = 0;
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
	if (newPriority < 0 || newPriority >= QTY_LEVELS)
		return;
	removeNode(scheduler->levels[process->priority], node);
	process->priority = newPriority;
	scheduler->processes[process->pid] = appendNode(scheduler->levels[process->priority], node);
}

void *schedule(void *prevStackPointer) {
	static firstTime = 1;
	SchedulerADT scheduler = getSchedulerADT();
	if (!scheduler->qtyProcesses)
		return prevStackPointer;

	Node *currentProcessNode = scheduler->processes[scheduler->currentPid];
	Process *currentProcess = (Process *) currentProcessNode->data;
	if (!firstTime)
		currentProcess->stackPos = prevStackPointer;
	else
		firstTime = 0;
	if (currentProcess->status == RUNNING)
		currentProcess->status = READY;

	// TODO: Analizar si vale la pena hacerlo ciclico
	uint8_t newPriority = currentProcess->priority > 0 ? currentProcess->priority - 1 : currentProcess->priority;
	setPriority(scheduler, currentProcessNode, newPriority);

	scheduler->currentPid = getNextPid(scheduler);
	currentProcess = scheduler->processes[scheduler->currentPid]->data;
	currentProcess->status = RUNNING;
	return currentProcess->stackPos;
}

uint16_t createProcess(MainFunction code, char **args, char *name, uint8_t priority) {
	SchedulerADT scheduler = getSchedulerADT();
	if (scheduler->qtyProcesses >= MAX_PROCESSES) // TODO: Agregar panic?
		return 0;
	Process *process = (Process *) allocMemory(sizeof(Process));
	initProcess(process, scheduler->nextUnusedPid, scheduler->currentPid, code, args, name, priority);

	Node *processNode = appendElement(scheduler->levels[process->priority], (void *) process);
	scheduler->processes[process->pid] = processNode;

	while (scheduler->processes[scheduler->nextUnusedPid] != NULL)
		scheduler->nextUnusedPid++;
	scheduler->qtyProcesses++;
	return process->pid;
}

void killProcess(uint16_t pid, int retValue) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *processToKillNode = scheduler->processes[scheduler->currentPid];
	Process *processToKill = (Process *) processToKillNode->data;
	scheduler->processes[pid] = NULL;

	scheduler->qtyProcesses--;
	removeNode(scheduler->levels[processToKill->priority], processToKillNode);
	// TODO: llamar al timer tick si el proceso es el current
	// TODO: free heap?
	free(processToKillNode);
	free(processToKill);
}