#if defined MAD || defined LINEAR

#include <defs.h>
#include <globals.h>
#include <lib.h>
#include <memoryInfo.h>
#include <memoryManager.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <video.h>

#define MIN_SIZE 32
#define FREE 0
#define USED 1

typedef struct MemoryBlock {
	uint8_t used;
	struct MemoryBlock *prev;
	struct MemoryBlock *next;
} MemoryBlock;

// Random
static uint32_t m_z = 362436069;
static uint32_t m_w = 521288629;

static uint32_t getUint() {
	m_z = (36969 - ticksElapsed()) * (m_z & 65535) + (m_z >> 16);
	m_w = 18000 * ticksElapsed() * (m_w & 65535) + (m_w >> 16);
	return (m_z << 16) + m_w;
}

static uint32_t getUniform(uint32_t max) {
	uint32_t u = getUint();
	return (u + 1.0) * 2.328306435454494e-10 * max;
}

typedef struct MemoryManagerCDT {
	uint64_t totalSize;
	MemoryBlock *firstAddress;
	MemoryInfo memoryInfo;
} MemoryManagerCDT;

static MemoryManagerADT getMemoryManager();
static MemoryBlock *split(MemoryManagerADT memoryManager, MemoryBlock *block, uint64_t realSize);
static MemoryBlock *merge(MemoryManagerADT memoryManager, MemoryBlock *left, MemoryBlock *right);
static MemoryBlock *createMemoryBlock(MemoryManagerADT memoryManager, void *ptrToAllocate, MemoryBlock *prev);

MemoryManagerADT
createMemoryManager(void *const restrict managedMemory, uint64_t memAmount) {
	MemoryManagerADT memoryManager = (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
	memoryManager->totalSize = memAmount;
	initMemoryInfo(&(memoryManager->memoryInfo));
	memoryManager->memoryInfo.freeMemory = memAmount;
	memoryManager->memoryInfo.totalMemory = memAmount;
	memoryManager->firstAddress = createMemoryBlock(memoryManager, managedMemory, NULL);
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
	while (block != NULL && (block->used == USED || getBlockSize(memoryManager, block) < realSize)) {
		block = block->next;
		uint64_t end = block->next != NULL ? (uint64_t) block->next : ((uint64_t) memoryManager->firstAddress) + memoryManager->totalSize;
	}
	if (block == NULL)
		return NULL;

	block = split(memoryManager, block, realSize);
	block->used = USED;

	MemoryInfo *memoryInfo = &(memoryManager->memoryInfo);
	memoryInfo->usedMemory += realSize;
	memoryInfo->freeMemory -= realSize;
	memoryInfo->usedBlocks++;
	memoryInfo->freeBlocks--;

	void *allocation = (void *) block + sizeof(MemoryBlock);
	return (void *) allocation;
}

static MemoryBlock *split(MemoryManagerADT memoryManager, MemoryBlock *block, uint64_t realSize) {
	uint64_t blockSize = getBlockSize(memoryManager, block);
	if (blockSize > realSize) {
#ifdef LINEAR
		block->next = createMemoryBlock(memoryManager, (void *) block + realSize, block);
#elif defined MAD
		uint64_t randomUnit = (uint64_t) getUniform((blockSize - realSize) / MIN_SIZE);
		MemoryBlock *randomBlock;
		if (randomUnit > 0)
			randomBlock = createMemoryBlock(memoryManager, (((void *) block) + (randomUnit * MIN_SIZE)), block);
		else
			randomBlock = block;
		if (getBlockSize(memoryManager, randomBlock) > realSize)
			randomBlock->next = createMemoryBlock(memoryManager, ((void *) randomBlock) + realSize, randomBlock);
		block = randomBlock;
#endif
	}
	return block;
}

void free(void *ptrAllocatedMemory) {
	MemoryManagerADT memoryManager = getMemoryManager();
	MemoryBlock *block = (MemoryBlock *) (ptrAllocatedMemory - sizeof(MemoryBlock));
	block->used = FREE;

	MemoryInfo *memoryInfo = &(memoryManager->memoryInfo);
	uint64_t blockSize = getBlockSize(memoryManager, block);
	memoryInfo->freeMemory += blockSize;
	memoryInfo->usedMemory -= blockSize;
	memoryInfo->freeBlocks++;
	memoryInfo->usedBlocks--;

	while (block->next != NULL && block->next->used == FREE)
		block = merge(memoryManager, block, block->next);

	while (block->prev != NULL && block->prev->used == FREE)
		block = merge(memoryManager, block->prev, block);
}

static MemoryBlock *merge(MemoryManagerADT memoryManager, MemoryBlock *left, MemoryBlock *right) {
	left->next = right->next;

	MemoryInfo *memoryInfo = &(memoryManager->memoryInfo);
	memoryInfo->freeBlocks--;
	memoryInfo->totalBlocks--;

	return left;
}

static MemoryBlock *createMemoryBlock(MemoryManagerADT memoryManager, void *ptrToAllocate, MemoryBlock *prev) {
	MemoryBlock *memoryBlock = (MemoryBlock *) ptrToAllocate;
	memoryBlock->used = FREE;
	memoryBlock->prev = prev;
	memoryBlock->next = NULL;
	if (memoryBlock->prev != NULL) {
		memoryBlock->next = prev->next;
		prev->next = memoryBlock;
	}
	if (memoryBlock->next != NULL)
		memoryBlock->next->prev = memoryBlock;

	MemoryInfo *memoryInfo = &(memoryManager->memoryInfo);
	memoryInfo->freeBlocks++;
	memoryInfo->totalBlocks++;

	return memoryBlock;
}

MemoryInfo *getMemoryInfo() {
	MemoryManagerADT memoryManager = getMemoryManager();
	createMemoryInfoCopy(&(memoryManager->memoryInfo));
}
#endif