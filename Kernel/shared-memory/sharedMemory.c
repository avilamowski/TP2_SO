#define MAX_SPACES 1024
#define SIZE 256
#include <memoryManager.h>
#include <stdlib.h>

void *sharedMemorySpaces[MAX_SPACES];

void initSharedMemory() {
	for (int i = 0; i < MAX_SPACES; i++) {
		sharedMemorySpaces[i] = NULL;
	}
}

void *openSharedMemory(uint16_t id) {
	if (sharedMemorySpaces[id] == NULL) {
		sharedMemorySpaces[id] = allocMemory(SIZE);
	}
	return sharedMemorySpaces[id];
}
