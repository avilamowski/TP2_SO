#include <globals.h>
#include <memoryInfo.h>
#include <memoryManager.h>

MemoryInfo *createMemoryInfoCopy(MemoryInfo *memoryInfo) {
	MemoryInfo *copy = allocMemory(sizeof(MemoryInfo));
	copy->totalBlocks = memoryInfo->totalBlocks;
	copy->freeBlocks = memoryInfo->freeBlocks;
	copy->usedBlocks = memoryInfo->usedBlocks;
	copy->totalMemory = memoryInfo->totalMemory;
	copy->freeMemory = memoryInfo->freeMemory;
	copy->usedMemory = memoryInfo->usedMemory;
	return copy;
}

void initMemoryInfo(MemoryInfo *memoryInfo) {
	memoryInfo->totalBlocks = 0;
	memoryInfo->freeBlocks = 0;
	memoryInfo->usedBlocks = 0;
	memoryInfo->totalMemory = 0;
	memoryInfo->freeMemory = 0;
	memoryInfo->usedMemory = 0;
	// memoryInfo->maxUsedBlockSize = 0;
	// memoryInfo->minUsedBlockSize = memoryManager->totalSize;
	// memoryInfo->maxFreeBlockSize = 0;
	// memoryInfo->minFreeBlockSize = memoryManager->totalSize;
}