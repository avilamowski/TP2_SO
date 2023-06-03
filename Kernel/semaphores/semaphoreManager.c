#include <defs.h>
#include <lib.h>
#include <linkedListADT.h>
#include <memoryManager.h>
#include <process.h>
#include <scheduler.h>
#include <semaphoreManager.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_SEMAPHORES (1 << 12)

typedef struct Semaphore {
	uint32_t value;
	int mutex;					  // 0 libre, 1 ocupado
	LinkedListADT semaphoreQueue; // LinkedListADT de pids
	LinkedListADT mutexQueue;	  // LinkedListADT de pids
} Semaphore;

static Semaphore *createSemaphore(uint32_t initialValue);
static void freeSemaphore(Semaphore *sem);
static void acquireMutex(Semaphore *sem);
static void resumeFirstAvailableProcess(LinkedListADT queue);
static void releaseMutex(Semaphore *sem);
static int up(Semaphore *sem);
static int down(Semaphore *sem);

typedef struct SemaphoreManagerCDT {
	Semaphore *semaphores[MAX_SEMAPHORES];
	uint16_t qtySemaphores; // TODO: Sacar?
} SemaphoreManagerCDT;

SemaphoreManagerADT createSemaphoreManager() {
	SemaphoreManagerADT semManager = (SemaphoreManagerADT) SEMAPHORE_MANAGER_ADDRESS;
	for (int i = 0; i < MAX_SEMAPHORES; i++)
		semManager->semaphores[i] = NULL;
	semManager->qtySemaphores = 0;
	return semManager;
}

static SemaphoreManagerADT getSemaphoreManager() {
	return (SemaphoreManagerADT) SEMAPHORE_MANAGER_ADDRESS;
}

int8_t semInit(uint16_t id, uint32_t initialValue) {
	SemaphoreManagerADT semManager = getSemaphoreManager();
	if (semManager->semaphores[id] != NULL)
		return -1;
	semManager->semaphores[id] = createSemaphore(initialValue);
	return 0;
}

int8_t semOpen(uint16_t id) {
	SemaphoreManagerADT semManager = getSemaphoreManager();
	return -(semManager->semaphores[id] == NULL);
}

int8_t semClose(uint16_t id) {
	SemaphoreManagerADT semManager = getSemaphoreManager();
	if (semManager->semaphores[id] == NULL)
		return -1;

	freeSemaphore(semManager->semaphores[id]);
	semManager->semaphores[id] = NULL;
	return 0;
}

int8_t semPost(uint16_t id) {
	SemaphoreManagerADT semManager = getSemaphoreManager();
	if (semManager->semaphores[id] == NULL)
		return -1;
	return up(semManager->semaphores[id]);
}

int8_t semWait(uint16_t id) {
	SemaphoreManagerADT semManager = getSemaphoreManager();
	if (semManager->semaphores[id] == NULL)
		return -1;
	return down(semManager->semaphores[id]);
}

static Semaphore *createSemaphore(uint32_t initialValue) {
	Semaphore *sem = (Semaphore *) allocMemory(sizeof(Semaphore));
	sem->value = initialValue;
	sem->mutex = 0;
	sem->semaphoreQueue = createLinkedListADT();
	sem->mutexQueue = createLinkedListADT();
	return sem;
}

static void freeSemaphore(Semaphore *sem) {
	freeLinkedListADTDeep(sem->semaphoreQueue);
	freeLinkedListADTDeep(sem->mutexQueue);
	free(sem);
}

static void acquireMutex(Semaphore *sem) {
	if (_xchg(&(sem->mutex), 1)) {
		uint16_t pid = getpid();
		appendElement(sem->semaphoreQueue, (void *) ((uint64_t) pid));
		setStatus(pid, BLOCKED);
		yield();
	}
}

static void resumeFirstAvailableProcess(LinkedListADT queue) {
	Node *current;
	while ((current = getFirst(queue)) != NULL) {
		removeNode(queue, current);
		uint16_t pid = (uint16_t) ((uint64_t) current->data);
		free(current);
		if (processIsAlive(pid)) {
			setStatus(pid, READY);
			break;
		}
	}
}

static void releaseMutex(Semaphore *sem) {
	resumeFirstAvailableProcess(sem->mutexQueue);
	sem->mutex = 0;
}

static int up(Semaphore *sem) {
	acquireMutex(sem);
	sem->value++;
	resumeFirstAvailableProcess(sem->semaphoreQueue);
	releaseMutex(sem);
	return 0;
}

static int down(Semaphore *sem) {
	acquireMutex(sem);
	if (sem->value <= 0) {
		uint16_t pid = getpid();
		appendElement(sem->semaphoreQueue, (void *) ((uint64_t) pid));
		setStatus(pid, BLOCKED);
		releaseMutex(sem);
		yield();

		acquireMutex(sem);
	}
	sem->value--;
	releaseMutex(sem);

	return 0;
}