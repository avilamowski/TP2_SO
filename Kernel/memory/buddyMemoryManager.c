// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//#ifdef BUDDY

#include <defs.h>
#include <globals.h>
#include <lib.h>
#include <memoryInfo.h>
#include <memoryManager.h>
#include <stdint.h>
#include <stdlib.h>
#include <video.h>

#define MIN_EXP 5 // Tamaño del MemoryBlock
#define FREE 0
#define USED 1
#define LEVELS 64

typedef struct MemoryBlock {
	uint8_t exp;
	uint8_t used;
	struct MemoryBlock *prev;
	struct MemoryBlock *next;
} MemoryBlock;

typedef struct MemoryManagerCDT {
	uint8_t maxExp;
	void *firstAddress;
	MemoryBlock *blocks[LEVELS];
	MemoryInfo memoryInfo;
} MemoryManagerCDT;

static MemoryManagerADT getMemoryManager();
static void split(MemoryManagerADT memoryManager, uint8_t idx);
static MemoryBlock *createMemoryBlock(void *ptrToAllocate, uint8_t exp, MemoryBlock *prev);
static MemoryBlock *removeMemoryBlock(MemoryBlock *blocks[], MemoryBlock *memoryBlock);
static MemoryBlock *merge(MemoryManagerADT memoryManager, MemoryBlock *block, MemoryBlock *buddy);

MemoryManagerADT createMemoryManager(void *const restrict managedMemory, uint64_t memAmount) {
	MemoryManagerADT memoryManager = (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
	memoryManager->firstAddress = managedMemory;
	memoryManager->maxExp = log(memAmount, 2);
	if (memoryManager->maxExp < MIN_EXP)
		return NULL;

	for (int i = 0; i < LEVELS; i++)
		memoryManager->blocks[i] = NULL;

	initMemoryInfo(&(memoryManager->memoryInfo));
	memoryManager->memoryInfo.freeMemory = memAmount;
	memoryManager->memoryInfo.totalMemory = memAmount;

	memoryManager->blocks[memoryManager->maxExp - 1] =
		createMemoryBlock(managedMemory, memoryManager->maxExp, NULL);
	return memoryManager;
}

static MemoryManagerADT getMemoryManager() {
	return (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
}

void *allocMemory(const uint64_t size) {
	MemoryManagerADT memoryManager = getMemoryManager();
	uint8_t idxToAlloc = log(size + sizeof(MemoryBlock), 2);

	idxToAlloc = idxToAlloc < MIN_EXP - 1 ? MIN_EXP - 1 : idxToAlloc;

	if (idxToAlloc >= memoryManager->maxExp)
		return NULL;

	if (memoryManager->blocks[idxToAlloc] == NULL) {
		uint8_t closestIdx = 0;
		for (uint8_t i = idxToAlloc + 1; i < memoryManager->maxExp && !closestIdx; i++)
			if (memoryManager->blocks[i] != NULL)
				closestIdx = i;
		if (closestIdx == 0)
			return NULL;
		for (; closestIdx > idxToAlloc; closestIdx--)
			split(memoryManager, closestIdx);
	}
	MemoryBlock *block = memoryManager->blocks[idxToAlloc];
	removeMemoryBlock(memoryManager->blocks, block);
	block->used = USED;
	block->prev = NULL; // Reutilizar punteros para heap
	block->next = NULL;

	MemoryInfo *memoryInfo = &(memoryManager->memoryInfo);
	uint64_t blockSize = 1L << block->exp;
	memoryInfo->usedMemory += blockSize;
	memoryInfo->freeMemory -= blockSize;
	memoryInfo->usedBlocks++;
	memoryInfo->freeBlocks--;

	void *allocation = (void *) block + sizeof(MemoryBlock);
	return (void *) allocation;
}

static void split(MemoryManagerADT memoryManager, uint8_t idx) {
	MemoryBlock *block = memoryManager->blocks[idx];
	removeMemoryBlock(memoryManager->blocks, block);
	MemoryBlock *buddyBlock =
		(MemoryBlock *) ((void *) block + (1L << idx));
	createMemoryBlock((void *) buddyBlock, idx, memoryManager->blocks[idx - 1]);
	memoryManager->blocks[idx - 1] = createMemoryBlock((void *) block, idx, buddyBlock);

	MemoryInfo *memoryInfo = &(memoryManager->memoryInfo);
	memoryInfo->freeBlocks++;
	memoryInfo->totalBlocks++;
}

void free(void *ptrAllocatedMemory) {
	MemoryManagerADT memoryManager = getMemoryManager();
	MemoryBlock *block = (MemoryBlock *) (ptrAllocatedMemory - sizeof(MemoryBlock));
	if (block->used == FREE)
		return;
	block->used = FREE;

	MemoryInfo *memoryInfo = &(memoryManager->memoryInfo);
	uint64_t blockSize = 1L << block->exp;
	memoryInfo->freeMemory += blockSize;
	memoryInfo->usedMemory -= blockSize;
	memoryInfo->freeBlocks++;
	memoryInfo->usedBlocks--;

	uint64_t relativePosition = (uint64_t) ((void *) block - memoryManager->firstAddress);
	MemoryBlock *buddyBlock = (MemoryBlock *) ((uint64_t) memoryManager->firstAddress + (((uint64_t) relativePosition) ^ (1L << block->exp)));
	while (buddyBlock->used == FREE && buddyBlock->exp == block->exp && block->exp < memoryManager->maxExp) {
		block = merge(memoryManager, block, buddyBlock);
		relativePosition = (uint64_t) ((void *) block - memoryManager->firstAddress);
		buddyBlock = (MemoryBlock *) ((uint64_t) memoryManager->firstAddress + (((uint64_t) relativePosition) ^ (1L << block->exp)));
	}
	memoryManager->blocks[block->exp - 1] = createMemoryBlock((void *) block, block->exp, memoryManager->blocks[block->exp - 1]);
}

static MemoryBlock *merge(MemoryManagerADT memoryManager, MemoryBlock *block, MemoryBlock *buddy) {
	removeMemoryBlock(memoryManager->blocks, buddy);
	MemoryBlock *leftBlock = block < buddy ? block : buddy;
	leftBlock->exp++;

	MemoryInfo *memoryInfo = &(memoryManager->memoryInfo);
	memoryInfo->freeBlocks--;
	memoryInfo->totalBlocks--;

	return leftBlock;
}

static MemoryBlock *removeMemoryBlock(MemoryBlock **blocks, MemoryBlock *memoryBlock) {
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

MemoryInfo *getMemoryInfo() {
	MemoryManagerADT memoryManager = getMemoryManager();
	return createMemoryInfoCopy(&(memoryManager->memoryInfo));
}

//#endif