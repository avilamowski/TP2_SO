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
  uint8_t idxToAlloc = log(size + sizeof(MemoryBlock), 2);
  if (idxToAlloc < MIN_EXP || idxToAlloc > memoryManager->maxExp)
    return NULL;
  void *allocation = NULL;

  if (memoryManager->blocks[idxToAlloc] ==
      NULL) { // TODO: Ver si cambia con headers
    uint8_t closestIdx = 0;
    for (uint8_t i = idxToAlloc + 1; i < memoryManager->maxExp && !closestIdx;
         i++)
      if (memoryManager->blocks[i] != NULL)
        closestIdx = i;
    for (; closestIdx > idxToAlloc; closestIdx--)
      split(memoryManager->blocks, closestIdx);
  }
  MemoryBlock *block = memoryManager->blocks[idxToAlloc];
  memoryManager->blocks[idxToAlloc] = removeMemoryBlock(block);
  allocation = (void *)block + sizeof(MemoryBlock);

  return (void *)allocation;
}

static void split(MemoryBlock *blocks[], uint8_t idx) {
  MemoryBlock *block = blocks[idx];
  blocks[idx] = removeMemoryBlock(block);
  MemoryBlock *buddyBlock =
      (MemoryBlock *)(block + (1 << (idx))); // TODO: Si se rompe es por el & xd
  blocks[idx - 1] = createMemoryBlock((void *)buddyBlock, idx, blocks[idx - 1]);
  blocks[idx - 1] = createMemoryBlock((void *)block, idx, blocks[idx - 1]);
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