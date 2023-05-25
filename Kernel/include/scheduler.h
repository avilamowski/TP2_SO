#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include <process.h>
#include <stdint.h>

typedef struct SchedulerCDT *SchedulerADT;

SchedulerADT createScheduler();
uint16_t createProcess(MainFunction code, char **args, char *name, uint8_t priority);
int32_t killProcess(uint16_t pid, int32_t retValue);
int32_t killCurrentProcess(int32_t retValue);
uint16_t getpid();
ProcessSnapshotList *getProcessSnapshot();
int32_t setPriority(uint16_t pid, uint8_t newPriority);
int8_t setStatus(uint16_t pid, uint8_t newStatus);
int32_t getZombieRetValue(uint16_t pid);
#endif