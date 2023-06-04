#include <defs.h>
#include <lib.h>
#include <linkedListADT.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <stdlib.h>
#include <video.h>
#define QTY_READY_LEVELS 5
#define MAX_PRIORITY 4
#define MIN_PRIORITY 0
#define BLOCKED_INDEX QTY_READY_LEVELS
#define MAX_PROCESSES (1 << 12)
#define IDLE_PID 0
#define QUANTUM_COEF 2

typedef struct SchedulerCDT {
	Node *processes[MAX_PROCESSES];
	LinkedListADT levels[QTY_READY_LEVELS + 1];
	uint16_t currentPid;
	uint16_t nextUnusedPid;
	uint16_t qtyProcesses;
	int8_t remainingQuantum;
	int8_t killFgProcess;
} SchedulerCDT;

SchedulerADT createScheduler() {
	SchedulerADT scheduler = (SchedulerADT) SCHEDULER_ADDRESS;
	for (int i = 0; i < MAX_PROCESSES; i++)
		scheduler->processes[i] = NULL;
	for (int i = 0; i < QTY_READY_LEVELS + 1; i++)
		scheduler->levels[i] = createLinkedListADT();
	scheduler->nextUnusedPid = 0;
	scheduler->killFgProcess = 0;
	return scheduler;
}

SchedulerADT getSchedulerADT() {
	return (SchedulerADT) SCHEDULER_ADDRESS;
}

static uint16_t getNextPid(SchedulerADT scheduler) {
	Process *process = NULL;
	for (int lvl = QTY_READY_LEVELS - 1; lvl >= 0 && process == NULL; lvl--)
		if (!isEmpty(scheduler->levels[lvl]))
			process = (Process *) (getFirst(scheduler->levels[lvl]))->data;

	if (process == NULL)
		return IDLE_PID;
	return process->pid;
}

int32_t setPriority(uint16_t pid, uint8_t newPriority) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *node = scheduler->processes[pid];
	if (node == NULL || pid == IDLE_PID)
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
	if (node == NULL || pid == IDLE_PID)
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
	return newStatus;
}

void *schedule(void *prevStackPointer) {
	static int firstTime = 1;
	SchedulerADT scheduler = getSchedulerADT();

	scheduler->remainingQuantum--;
	if (!scheduler->qtyProcesses || scheduler->remainingQuantum > 0)
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

		uint8_t newPriority = currentProcess->priority > 0 ? currentProcess->priority - 1 : currentProcess->priority;
		setPriority(currentProcess->pid, newPriority);
	}

	scheduler->currentPid = getNextPid(scheduler);
	currentProcess = scheduler->processes[scheduler->currentPid]->data;

	if (scheduler->killFgProcess && currentProcess->fileDescriptors[STDIN] == STDIN) {
		scheduler->killFgProcess = 0;
		if (killCurrentProcess(-1) != -1)
			forceTimerTick();
	}
	scheduler->remainingQuantum = QUANTUM_COEF * (MAX_PRIORITY - currentProcess->priority);
	currentProcess->status = RUNNING;
	return currentProcess->stackPos;
}

int16_t createProcess(MainFunction code, char **args, char *name, uint8_t priority, int16_t fileDescriptors[], uint8_t unkillable) {
	SchedulerADT scheduler = getSchedulerADT();
	if (scheduler->qtyProcesses >= MAX_PROCESSES) // TODO: Agregar panic?
		return -1;
	Process *process = (Process *) allocMemory(sizeof(Process));
	initProcess(process, scheduler->nextUnusedPid, scheduler->currentPid, code, args, name, priority, fileDescriptors, unkillable);

	Node *processNode;
	if (process->pid != IDLE_PID)
		processNode = appendElement(scheduler->levels[process->priority], (void *) process);
	else {
		processNode = allocMemory(sizeof(Node));
		processNode->data = (void *) process;
	}
	scheduler->processes[process->pid] = processNode;

	while (scheduler->processes[scheduler->nextUnusedPid] != NULL)
		scheduler->nextUnusedPid = (scheduler->nextUnusedPid + 1) % MAX_PROCESSES;
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
	SchedulerADT scheduler = getSchedulerADT();
	Node *processToKillNode = scheduler->processes[pid];
	if (processToKillNode == NULL)
		return -1;
	Process *processToKill = (Process *) processToKillNode->data;
	if (processToKill->status == ZOMBIE || processToKill->unkillable)
		return -1;

	closeFileDescriptors(processToKill);

	uint8_t priorityIndex = processToKill->status != BLOCKED ? processToKill->priority : BLOCKED_INDEX;
	removeNode(scheduler->levels[priorityIndex], processToKillNode);
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
		yield();
	return 0;
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

	loadSnapshot(&psArray[processIndex++], (Process *) scheduler->processes[IDLE_PID]->data);
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
	Node *zombieNode = scheduler->processes[pid];
	if (zombieNode == NULL)
		return -1;
	Process *zombieProcess = (Process *) zombieNode->data;
	if (zombieProcess->parentPid != scheduler->currentPid)
		return -1;

	Process *parent = (Process *) scheduler->processes[scheduler->currentPid]->data;
	parent->waitingForPid = pid;
	if (zombieProcess->status != ZOMBIE) {
		setStatus(parent->pid, BLOCKED);
		yield();
	}
	removeNode(parent->zombieChildren, zombieNode);
	destroyZombie(scheduler, zombieProcess);
	return zombieProcess->retValue;
}

int32_t processIsAlive(uint16_t pid) {
	SchedulerADT scheduler = getSchedulerADT();
	Process *process = scheduler->processes[pid]->data;
	return process != NULL && process->status != ZOMBIE;
}

void yield() {
	SchedulerADT scheduler = getSchedulerADT();
	scheduler->remainingQuantum = 0;
	setPriority(scheduler->currentPid, MAX_PRIORITY);
	forceTimerTick();
}

int8_t changeFD(uint16_t pid, uint8_t position, int16_t newFd) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *processNode = scheduler->processes[pid];
	if (pid == IDLE_PID || processNode == NULL)
		return -1;
	Process *process = (Process *) processNode->data;
	process->fileDescriptors[position] = newFd;
	return 0;
}

int16_t getCurrentProcessFileDescriptor(uint8_t fdIndex) {
	SchedulerADT scheduler = getSchedulerADT();
	Process *process = scheduler->processes[scheduler->currentPid]->data;
	return process->fileDescriptors[fdIndex];
}

void killForegroundProcess() {
	SchedulerADT scheduler = getSchedulerADT();
	scheduler->killFgProcess = 1;
}