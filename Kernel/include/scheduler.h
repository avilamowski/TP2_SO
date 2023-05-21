#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include <process.h>
#include <stdint.h>

typedef struct SchedulerCDT *SchedulerADT;

SchedulerADT createScheduler();
uint16_t createProcess(MainFunction code, char **args, char *name, uint8_t priority);

#endif