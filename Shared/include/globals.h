#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <stdint.h>

#define READ 0
#define WRITE 1

typedef enum { BLOCKED = 0,
			   READY,
			   RUNNING,
			   ZOMBIE } ProcessStatus;
typedef int (*MainFunction)(int argc, char **args);

typedef struct ProcessSnapshot {
	uint16_t pid;
	uint16_t parentPid;
	void *stackBase;
	void *stackPos;
	char *name;
	uint8_t priority;
	ProcessStatus status;
	uint8_t foreground;
} ProcessSnapshot;

typedef struct ProcessSnapshotList {
	uint16_t length;
	ProcessSnapshot *snapshotList;
} ProcessSnapshotList;

typedef struct MemoryInfo {
	uint64_t totalBlocks;
	uint64_t freeBlocks;
	uint64_t usedBlocks;
	uint64_t totalMemory;
	uint64_t freeMemory;
	uint64_t usedMemory;
	// uint64_t maxUsedBlockSize;
	// uint64_t minUsedBlockSize;
	// uint64_t maxFreeBlockSize;
	// uint64_t minFreeBlockSize;
} MemoryInfo;

/* File Descriptors*/
#define DEV_NULL -1
#define STDIN 0
#define STDOUT 1
#define STDERR 2
// #define KBDIN 3

#endif