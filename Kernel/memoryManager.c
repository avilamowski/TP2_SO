#include <defs.h>
#include <lib.h>
#include <memoryManager.h>
#include <stdint.h>

#define MIN_EXP 5 // Tamaño del MemoryBlock
typedef struct MemoryBlock {
  uint8_t exp;
  uint8_t used;
  struct MemoryBlock *prev;
  struct MemoryBlock *next;
} MemoryBlock;

typedef struct MemoryManagerCDT {
  uint8_t currentMaxExp;
  uint8_t maxExp;
  void *firstAddress;
  MemoryBlock *blocks[64];
} MemoryManagerCDT;

static void split(MemoryBlock *blocks[], uint8_t exp);
static MemoryBlock *createMemoryBlock(void *ptrToAllocate, uint8_t exp,
                                      MemoryBlock *prev);
static MemoryBlock *removeMemoryBlock(MemoryBlock *memoryBlock);

MemoryManagerADT
createMemoryManager(void *const restrict memoryForMemoryManager,
                    void *const restrict managedMemory, uint64_t memAmount) {
  MemoryManagerADT memoryManager = (MemoryManagerADT)memoryForMemoryManager;
  memoryManager->firstAddress = managedMemory;
  memoryManager->maxExp = log(memAmount, 2);
  if (memoryManager->maxExp < MIN_EXP)
    return NULL;

  for (int i = 0; i < 64; i++)
    memoryManager->blocks[i] = NULL;

  memoryManager->blocks[memoryManager->maxExp - 1] =
      createMemoryBlock(managedMemory, memoryManager->maxExp, NULL);
  return memoryManager;
}

MemoryManagerADT getMemoryManager() {
  return (MemoryManagerADT)MEMORY_MANAGER_ADDRESS;
}

void *allocMemory(const uint64_t size) {
  MemoryManagerADT memoryManager = getMemoryManager();
  uint8_t expToAlloc = log(size, 2);
  if (expToAlloc < MIN_EXP || expToAlloc > memoryManager->maxExp)
    return NULL;
  void *allocation = NULL;

  if (memoryManager->blocks[expToAlloc] ==
      NULL) { // TODO: Ver si cambia con headers
    uint8_t closestExp = 0;
    for (uint8_t i = expToAlloc + 1; i < memoryManager->maxExp && !closestExp;
         i++)
      if (memoryManager->blocks[i] != NULL)
        closestExp = i;
    for (; closestExp > expToAlloc; closestExp--)
      split(memoryManager->blocks, closestExp);
  }
  MemoryBlock *block = memoryManager->blocks[expToAlloc];
  memoryManager->blocks[expToAlloc] = removeMemoryBlock(block);
  allocation = (void *)block + sizeof(MemoryBlock);

  return (void *)allocation;
}

static void split(MemoryBlock *blocks[], uint8_t exp) {
  MemoryBlock *block = blocks[exp - 1];
  blocks[exp - 1] = removeMemoryBlock(block);
  MemoryBlock *buddyBlock =
      (MemoryBlock *)(block +
                      (1 << (exp - 1))); // TODO: Si se rompe es por el & xd
  blocks[exp - 2] =
      createMemoryBlock((void *)buddyBlock, exp - 1, blocks[exp - 2]);
  blocks[exp - 2] = createMemoryBlock((void *)block, exp - 1, blocks[exp - 2]);
}

static MemoryBlock *removeMemoryBlock(MemoryBlock *memoryBlock) {
  if (memoryBlock->prev != NULL) {
    memoryBlock->prev->next = memoryBlock->next;
  }
  if (memoryBlock->next != NULL) {
    memoryBlock->next->prev = memoryBlock->prev;
  }
  return memoryBlock->next;
}

static MemoryBlock *createMemoryBlock(void *ptrToAllocate, uint8_t exp,
                                      MemoryBlock *next) {
  MemoryBlock *memoryBlock = (MemoryBlock *)ptrToAllocate;
  memoryBlock->exp = exp;
  memoryBlock->used = 0;
  memoryBlock->prev = NULL;
  memoryBlock->next = next;
  return memoryBlock;
}