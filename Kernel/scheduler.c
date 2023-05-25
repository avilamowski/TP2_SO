#include <defs.h>
#include <lib.h>
#include <linkedListADT.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <video.h>
#define QTY_READY_LEVELS 5
#define BLOCKED_INDEX QTY_READY_LEVELS
#define MAX_PROCESSES (1 << 10)

typedef struct SchedulerCDT {
	Node *processes[MAX_PROCESSES];
	LinkedListADT levels[QTY_READY_LEVELS + 1];
	uint16_t currentPid;
	uint16_t nextUnusedPid;
	uint16_t qtyProcesses;
	uint8_t remainingQuantum;
} SchedulerCDT;

SchedulerADT createScheduler() {
	SchedulerADT scheduler = (SchedulerADT) SCHEDULER_ADDRESS;
	for (int i = 0; i < MAX_PROCESSES; i++)
		scheduler->processes[i] = NULL;
	for (int i = 0; i < QTY_READY_LEVELS + 1; i++)
		scheduler->levels[i] = createLinkedListADT();
	scheduler->nextUnusedPid = 0;
	return scheduler;
}

SchedulerADT getSchedulerADT() {
	return (SchedulerADT) SCHEDULER_ADDRESS;
}

static uint16_t getNextPid(SchedulerADT scheduler) {
	Process *process = NULL;
	for (int lvl = QTY_READY_LEVELS - 1; lvl >= 0 && process == NULL; lvl--) {
		if (!isEmpty(scheduler->levels[lvl])) {
			process = (Process *) (getFirst(scheduler->levels[lvl]))->data;
		}
	}
	// TODO: Panic si process es null?
	return process->pid;
}

int32_t setPriority(uint16_t pid, uint8_t newPriority) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *node = scheduler->processes[pid];
	if (node == NULL)
		return -1;
	Process *process = (Process *) node->data;
	if (newPriority < 0 || newPriority >= QTY_READY_LEVELS)
		return -1;
	if (process->status == READY || process->status == RUNNING) {
		removeNode(scheduler->levels[process->priority], node);
		scheduler->processes[process->pid] = appendNode(scheduler->levels[newPriority], node);
	}
	process->priority = newPriority;
	return newPriority;
}

int8_t setStatus(uint16_t pid, uint8_t newStatus) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *node = scheduler->processes[pid];
	if (node == NULL)
		return -1;
	Process *process = (Process *) node->data;
	ProcessStatus oldStatus = process->status;
	if (newStatus == RUNNING || newStatus == ZOMBIE || oldStatus == ZOMBIE)
		return -1;
	if (newStatus == process->status)
		return newStatus;
	if (newStatus == BLOCKED) {
		removeNode(scheduler->levels[process->priority], node);
		appendNode(scheduler->levels[BLOCKED_INDEX], node);
	}
	else if (oldStatus == BLOCKED) {
		removeNode(scheduler->levels[BLOCKED_INDEX], node);
		// Se asume que ya tiene un nivel predefinido
		appendNode(scheduler->levels[process->priority], node);
	}
	process->status = newStatus;
	if (oldStatus == RUNNING)
		forceTimerTick();
	return newStatus;
}

void *schedule(void *prevStackPointer) {
	static int firstTime = 1;
	SchedulerADT scheduler = getSchedulerADT();
	if (!scheduler->qtyProcesses)
		return prevStackPointer;

	Process *currentProcess;
	Node *currentProcessNode = scheduler->processes[scheduler->currentPid];

	if (currentProcessNode != NULL) {
		currentProcess = (Process *) currentProcessNode->data;
		if (!firstTime)
			currentProcess->stackPos = prevStackPointer;
		else
			firstTime = 0;
		if (currentProcess->status == RUNNING)
			currentProcess->status = READY;

		// TODO: Analizar si vale la pena hacerlo ciclico
		uint8_t newPriority = currentProcess->priority > 0 ? currentProcess->priority - 1 : currentProcess->priority;
		setPriority(currentProcess->pid, newPriority);
	}

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

static void destroyZombie(SchedulerADT scheduler, Process *zombie) {
	// TODO: free heap?
	Node *zombieNode = scheduler->processes[zombie->pid];
	scheduler->qtyProcesses--;
	scheduler->processes[zombie->pid] = NULL;
	freeProcess(zombie);
	free(zombieNode);
}

int32_t killCurrentProcess(int32_t retValue) {
	SchedulerADT scheduler = getSchedulerADT();
	return killProcess(scheduler->currentPid, retValue);
}

int32_t killProcess(uint16_t pid, int32_t retValue) {
	if (pid == 0)
		return -1;
	SchedulerADT scheduler = getSchedulerADT();
	Node *processToKillNode = scheduler->processes[pid];
	if (processToKillNode == NULL)
		return -1;
	Process *processToKill = (Process *) processToKillNode->data;
	removeNode(scheduler->levels[processToKill->priority], processToKillNode);
	processToKill->retValue = retValue;

	processToKill->status = ZOMBIE;

	begin(processToKill->zombieChildren);
	while (hasNext(processToKill->zombieChildren)) {
		destroyZombie(scheduler, (Process *) next(processToKill->zombieChildren));
	}

	Node *parentNode = scheduler->processes[processToKill->parentPid];
	if (parentNode != NULL && ((Process *) parentNode->data)->status != ZOMBIE) {
		Process *parent = (Process *) parentNode->data;
		appendNode(parent->zombieChildren, processToKillNode);
		if (processIsWaiting(parent, processToKill->pid))
			setStatus(processToKill->parentPid, READY);
	}
	else {
		destroyZombie(scheduler, processToKill);
	}
	if (pid == scheduler->currentPid)
		forceTimerTick();
	return retValue;
}

uint16_t getpid() {
	SchedulerADT scheduler = getSchedulerADT();
	return scheduler->currentPid;
}

ProcessSnapshotList *getProcessSnapshot() {
	SchedulerADT scheduler = getSchedulerADT();
	ProcessSnapshotList *snapshotsArray = allocMemory(sizeof(ProcessSnapshotList));
	ProcessSnapshot *psArray = allocMemory(scheduler->qtyProcesses * sizeof(ProcessSnapshot));
	int processIndex = 0;
	for (int lvl = QTY_READY_LEVELS; lvl >= 0; lvl--) { // Se cuentan tambien los bloqueados
		begin(scheduler->levels[lvl]);
		while (hasNext(scheduler->levels[lvl])) {
			Process *nextProcess = (Process *) next(scheduler->levels[lvl]);
			loadSnapshot(&psArray[processIndex], nextProcess);
			processIndex++;
			if (nextProcess->status != ZOMBIE) {
				getZombiesSnapshots(processIndex, psArray, nextProcess);
				processIndex += getLength(nextProcess->zombieChildren);
			}
		}
	}
	snapshotsArray->length = scheduler->qtyProcesses;
	snapshotsArray->snapshotList = psArray;
	return snapshotsArray;
}

int32_t getZombieRetValue(uint16_t pid) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *processNode = scheduler->processes[pid];
	if (processNode == NULL)
		return -1;
	Process *process = (Process *) processNode->data;
	if (process->parentPid != scheduler->currentPid)
		return -1;
	if (process->status != ZOMBIE) {
		setStatus(scheduler->currentPid, BLOCKED);
	}
	return process->retValue;
}