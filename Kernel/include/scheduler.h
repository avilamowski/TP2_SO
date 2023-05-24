#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include <process.h>
#include <stdint.h>

typedef struct SchedulerCDT *SchedulerADT;

SchedulerADT createScheduler();
uint16_t createProcess(MainFunction code, char **args, char *name, uint8_t priority);
int32_t killProcess(uint16_t pid, int32_t retValue);
uint16_t getpid();
ProcessSnapshot *getProcessSnapshot();
int32_t setPriority(uint16_t pid, uint8_t newPriority);
int8_t setState(uint16_t pid, uint8_t newState);
#endif