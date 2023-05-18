#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>
#include <stdlib.h>

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT
createMemoryManager(void *const restrict memoryForMemoryManager,
                    void *const restrict managedMemory, uint64_t memAmount);

void *allocMemory(const size_t memoryToAllocate);

MemoryManagerADT getMemoryManager();

#endif