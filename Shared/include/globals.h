#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <stdint.h>

#define READ 0
#define WRITE 1

typedef enum { BLOCKED = 0,
			   READY,
			   RUNNING,
			   ZOMBIE,
			   DEAD } ProcessStatus;
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
	int64_t totalBlocks;
	int64_t freeBlocks;
	int64_t usedBlocks;
	int64_t totalMemory;
	int64_t freeMemory;
	int64_t usedMemory;
	char name[16];
} MemoryInfo;

/* File Descriptors*/
#define DEV_NULL -1
#define STDIN 0
#define STDOUT 1
#define STDERR 2
// #define KBDIN 3

#endif