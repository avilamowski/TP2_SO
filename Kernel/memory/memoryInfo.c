// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <globals.h>
#include <memoryInfo.h>
#include <memoryManager.h>
#include <string.h>
MemoryInfo *createMemoryInfoCopy(MemoryInfo *memoryInfo) {
	MemoryInfo *copy = allocMemory(sizeof(MemoryInfo));
	strcpy(copy->name, memoryInfo->name);
	copy->totalBlocks = memoryInfo->totalBlocks;
	copy->freeBlocks = memoryInfo->freeBlocks;
	copy->usedBlocks = memoryInfo->usedBlocks;
	copy->totalMemory = memoryInfo->totalMemory;
	copy->freeMemory = memoryInfo->freeMemory;
	copy->usedMemory = memoryInfo->usedMemory;
	return copy;
}

void initMemoryInfo(MemoryInfo *memoryInfo) {
#if defined BUDDY
	strcpy(memoryInfo->name, "Buddy");
#elif defined MAD
	strcpy(memoryInfo->name, "Mad");
#else
	strcpy(memoryInfo->name, "Linear");
#endif
	memoryInfo->totalBlocks = 1;
	memoryInfo->freeBlocks = 1;
	memoryInfo->usedBlocks = 0;
	memoryInfo->totalMemory = 0;
	memoryInfo->freeMemory = 0;
	memoryInfo->usedMemory = 0;
}