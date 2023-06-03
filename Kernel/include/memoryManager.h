#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <globals.h>
#include <stdint.h>
typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT
createMemoryManager(void *const restrict managedMemory, uint64_t memAmount);

void *allocMemory(const uint64_t size);
void free(void *ptrAllocatedMemory);
MemoryInfo *getMemoryInfo();

#endif