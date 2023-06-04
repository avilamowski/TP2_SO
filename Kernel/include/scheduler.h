#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include <process.h>
#include <stdint.h>

typedef struct SchedulerCDT *SchedulerADT;

SchedulerADT createScheduler();
int16_t createProcess(MainFunction code, char **args, char *name, uint8_t priority, int16_t fileDescriptors[], uint8_t unkillable);
int32_t killProcess(uint16_t pid, int32_t retValue);
int32_t killCurrentProcess(int32_t retValue);
uint16_t getpid();
ProcessStatus getProcessStatus(uint16_t pid);
ProcessSnapshotList *getProcessSnapshot();
int32_t setPriority(uint16_t pid, uint8_t newPriority);
int8_t setStatus(uint16_t pid, uint8_t newStatus);
int32_t getZombieRetValue(uint16_t pid);
int32_t processIsAlive(uint16_t pid);
void yield();
int8_t changeFD(uint16_t pid, uint8_t position, int16_t newFd);
int16_t getCurrentProcessFileDescriptor(uint8_t fdIndex);
void killForegroundProcess();
#endif