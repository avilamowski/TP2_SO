#include <defs.h>
#include <globals.h>
#include <lib.h>
#include <memoryManager.h>
#include <stdint.h>
#include <stdlib.h>
#include <video.h>

#define MIN_SIZE 32
#define FREE 0
#define USED 1

typedef struct MemoryBlock {
	uint8_t used;
	struct MemoryBlock *prev;
	struct MemoryBlock *next;
} MemoryBlock;

typedef struct MemoryManagerCDT {
	uint64_t currentMaxSize;
	uint64_t totalSize;
	MemoryBlock *firstAddress;
} MemoryManagerCDT;

static MemoryManagerADT getMemoryManager();
static MemoryBlock *split(MemoryManagerADT memoryManager, MemoryBlock *block, uint64_t realSize);
static MemoryBlock *createMemoryBlock(void *ptrToAllocate, MemoryBlock *prev, MemoryBlock *next);

MemoryManagerADT
createMemoryManager(void *const restrict managedMemory, uint64_t memAmount) {
	MemoryManagerADT memoryManager = (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
	memoryManager->totalSize = memAmount;
	memoryManager->firstAddress = createMemoryBlock(managedMemory, NULL, NULL);
	return memoryManager;
}

static MemoryManagerADT getMemoryManager() {
	return (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
}

static uint64_t getBlockSize(MemoryManagerADT memoryManager, MemoryBlock *block) {
	uint64_t end = block->next != NULL ? (uint64_t) block->next : ((uint64_t) memoryManager->firstAddress) + memoryManager->totalSize;
	return end - (uint64_t) block;
}

static uint64_t getRealSize(const uint64_t size) {
	uint64_t blockSize = size + sizeof(MemoryBlock);
	return blockSize + ((blockSize % MIN_SIZE) ? MIN_SIZE - blockSize % MIN_SIZE : 0);
}

void *allocMemory(const uint64_t size) {
	MemoryManagerADT memoryManager = getMemoryManager();
	uint64_t realSize = getRealSize(size);
	MemoryBlock *block = memoryManager->firstAddress;
	while (block != NULL && (block->used == USED || getBlockSize(memoryManager, block) < realSize))
		block = block->next;
	if (block == NULL)
		return NULL;

	block = split(memoryManager, block, realSize);
	block->used = USED;

	void *allocation = (void *) block + sizeof(MemoryBlock);
	return (void *) allocation;
}

static MemoryBlock *split(MemoryManagerADT memoryManager, MemoryBlock *block, uint64_t realSize) {
	// lineal
	if (getBlockSize(memoryManager, block) > realSize)
		block->next = createMemoryBlock((void *) block + realSize, block, block->next);

	// loco

	return block;
}

void free(void *ptrAllocatedMemory) {
	MemoryManagerADT memoryManager = getMemoryManager();
	MemoryBlock *block = (MemoryBlock *) (ptrAllocatedMemory - sizeof(MemoryBlock));
	block->used = FREE;

	while (block->next != NULL && block->next->used == FREE) {
		block->next = block->next->next;
	}

	while (block->prev != NULL && block->prev->used == FREE) {
		block->prev->next = block->next;
		block = block->prev;
	}
}

static MemoryBlock *createMemoryBlock(void *ptrToAllocate, MemoryBlock *prev, MemoryBlock *next) {
	MemoryBlock *memoryBlock = (MemoryBlock *) ptrToAllocate;
	memoryBlock->used = FREE;
	memoryBlock->prev = prev;
	memoryBlock->next = next;
	if (next != NULL)
		next->prev = memoryBlock;
	if (prev != NULL)
		prev->next = memoryBlock;
	return memoryBlock;
}

MemoryInfo *getMemoryInfo() {
	MemoryManagerADT memoryManager = getMemoryManager();
	MemoryInfo *memoryInfo = (MemoryInfo *) allocMemory(sizeof(MemoryInfo));

	memoryInfo->totalBlocks = 0;
	memoryInfo->freeBlocks = 0;
	memoryInfo->usedBlocks = 0;
	memoryInfo->totalMemory = 0;
	memoryInfo->freeMemory = 0;
	memoryInfo->usedMemory = 0;
	memoryInfo->maxUsedBlockSize = 0;
	memoryInfo->minUsedBlockSize = memoryManager->totalSize;
	memoryInfo->maxFreeBlockSize = 0;
	memoryInfo->minFreeBlockSize = memoryManager->totalSize;

	MemoryBlock *block = memoryManager->firstAddress;
	while (block != NULL) {
		uint64_t blockSize = getBlockSize(memoryManager, block);
		memoryInfo->totalBlocks++;
		memoryInfo->totalMemory += blockSize;
		if (block->used == FREE) {
			memoryInfo->freeBlocks++;
			memoryInfo->freeMemory += blockSize;
			memoryInfo->maxFreeBlockSize = blockSize > memoryInfo->maxFreeBlockSize ? blockSize : memoryInfo->maxFreeBlockSize;
			memoryInfo->minFreeBlockSize = blockSize < memoryInfo->minFreeBlockSize ? blockSize : memoryInfo->minFreeBlockSize;
		}
		else if (block->used == USED) {
			memoryInfo->usedBlocks++;
			memoryInfo->usedMemory += blockSize;
			memoryInfo->maxUsedBlockSize = blockSize > memoryInfo->maxUsedBlockSize ? blockSize : memoryInfo->maxUsedBlockSize;
			memoryInfo->minUsedBlockSize = blockSize < memoryInfo->minUsedBlockSize ? blockSize : memoryInfo->minUsedBlockSize;
		}
		block = block->next;
	}
	printf("\ntotalBlocks:%d\n freeBlocks:%d\n usedBlocks:%d\n totalMemory:%d o %d\n freeMemory:%d\n usedMemory:%d\n maxUsedBlockSize:%d\n minUsedBlockSize:%d\n maxFreeBlockSize:%d\n minFreeBlockSize:%d\n", memoryInfo->totalBlocks, memoryInfo->freeBlocks, memoryInfo->usedBlocks, memoryInfo->totalMemory, memoryManager->totalSize, memoryInfo->freeMemory, memoryInfo->usedMemory, memoryInfo->maxUsedBlockSize, memoryInfo->minUsedBlockSize, memoryInfo->maxFreeBlockSize, memoryInfo->minFreeBlockSize);

	return memoryInfo;
}