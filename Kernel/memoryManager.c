#include <defs.h>
#include <memoryManager.h>
#include <stdint.h>

typedef struct MemoryManagerCDT {
  char *nextAddress;
} MemoryManagerCDT;

MemoryManagerADT
createMemoryManager(void *const restrict memoryForMemoryManager,
                    void *const restrict managedMemory, uint64_t memAmount) {
  MemoryManagerADT memoryManager = (MemoryManagerADT)memoryForMemoryManager;
  memoryManager->nextAddress = managedMemory;

  return memoryManager;
}

MemoryManagerADT getMemoryManager() {
  return (MemoryManagerADT)MEMORY_MANAGER_ADDRESS;
}

void *allocMemory(const uint64_t memoryToAllocate) {
  MemoryManagerADT memoryManager = getMemoryManager();
  char *allocation = memoryManager->nextAddress;

  memoryManager->nextAddress += memoryToAllocate;

  return (void *)allocation;
}