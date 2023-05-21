#include <defs.h>
#include <lib.h>
#include <memoryManager.h>
#include <stdint.h>
#include <video.h>

#define MIN_EXP 5 // Tamaño del MemoryBlock
#define FREE 0
#define USED 1
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
static MemoryBlock *createMemoryBlock(void *ptrToAllocate, uint8_t exp, MemoryBlock *prev);
static MemoryBlock *removeMemoryBlock(MemoryBlock *blocks[], MemoryBlock *memoryBlock);
static MemoryBlock *merge(MemoryBlock *blocks[], MemoryBlock *block, MemoryBlock *buddy);

MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory, uint64_t memAmount) {
	MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager; // TODO ver si ponemos la constante
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
	return (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
}

void *allocMemory(const uint64_t size) {
	MemoryManagerADT memoryManager = getMemoryManager();
	uint8_t idxToAlloc = log(size + sizeof(MemoryBlock), 2);

	idxToAlloc = idxToAlloc < MIN_EXP - 1 ? MIN_EXP - 1 : idxToAlloc;

	if (idxToAlloc < MIN_EXP - 1 || idxToAlloc >= memoryManager->maxExp)
		return NULL;
	void *allocation = NULL;

	if (memoryManager->blocks[idxToAlloc] == NULL) {
		uint8_t closestIdx = 0;
		for (uint8_t i = idxToAlloc + 1; i < memoryManager->maxExp && !closestIdx; i++)
			if (memoryManager->blocks[i] != NULL)
				closestIdx = i;
		if (closestIdx == 0)
			return NULL;
		for (; closestIdx > idxToAlloc; closestIdx--)
			split(memoryManager->blocks, closestIdx);
	}
	MemoryBlock *block = memoryManager->blocks[idxToAlloc];
	removeMemoryBlock(memoryManager->blocks, block);
	block->used = USED;
	block->prev = NULL; // Reutilizar punteros para heap
	block->next = NULL;
	allocation = (void *) block + sizeof(MemoryBlock);

	return (void *) allocation;
}

static void split(MemoryBlock *blocks[], uint8_t idx) {
	MemoryBlock *block = blocks[idx];
	removeMemoryBlock(blocks, block);
	MemoryBlock *buddyBlock =
		(MemoryBlock *) ((void *) block + (1 << idx)); // TODO: Si se rompe es por el & xd
	createMemoryBlock(buddyBlock, idx, blocks[idx - 1]);
	blocks[idx - 1] = createMemoryBlock(block, idx, buddyBlock);
}

void free(void *ptrAllocatedMemory) {
	MemoryManagerADT memoryManager = getMemoryManager();
	MemoryBlock *block = (MemoryBlock *) (ptrAllocatedMemory - sizeof(MemoryBlock));
	block->used = FREE;
	uint64_t relativePosition = (uint64_t) ((void *) block - memoryManager->firstAddress);
	MemoryBlock *buddyBlock = (MemoryBlock *) ((uint64_t) memoryManager->firstAddress + (((uint64_t) relativePosition) ^ (1 << block->exp)));
	while (buddyBlock->used == FREE && buddyBlock->exp == block->exp) {
		block = merge(memoryManager->blocks, block, buddyBlock);
		relativePosition = (uint64_t) ((void *) block - memoryManager->firstAddress);
		buddyBlock = (MemoryBlock *) ((uint64_t) memoryManager->firstAddress + (((uint64_t) relativePosition) ^ (1 << block->exp)));
	}
	memoryManager->blocks[block->exp - 1] = createMemoryBlock((void *) block, block->exp, memoryManager->blocks[block->exp - 1]);
}

static MemoryBlock *merge(MemoryBlock *blocks[], MemoryBlock *block, MemoryBlock *buddy) {
	removeMemoryBlock(blocks, buddy);
	MemoryBlock *leftBlock = block < buddy ? block : buddy;
	leftBlock->exp++;
	return leftBlock;
}

static MemoryBlock *removeMemoryBlock(MemoryBlock *blocks[], MemoryBlock *memoryBlock) {
	if (memoryBlock->prev != NULL)
		memoryBlock->prev->next = memoryBlock->next;
	else
		blocks[memoryBlock->exp - 1] = memoryBlock->next;

	if (memoryBlock->next != NULL)
		memoryBlock->next->prev = memoryBlock->prev;
	return memoryBlock->next;
}

static MemoryBlock *createMemoryBlock(void *ptrToAllocate, uint8_t exp,
									  MemoryBlock *next) {
	MemoryBlock *memoryBlock = (MemoryBlock *) ptrToAllocate;
	memoryBlock->exp = exp;
	memoryBlock->used = FREE;
	memoryBlock->prev = NULL;
	memoryBlock->next = next;
	if (next != NULL) {
		next->prev = memoryBlock;
	}
	return memoryBlock;
}