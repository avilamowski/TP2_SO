#ifndef _SEMAPHORES_H
#define _SEMAPHORES_H

#include <stdint.h>
typedef struct SemaphoreManagerCDT *SemaphoreManagerADT;

SemaphoreManagerADT createSemaphoreManager();
int8_t semInit(uint16_t id, uint32_t initialValue);
int8_t semOpen(uint16_t id);
int8_t semClose(uint16_t id);
int8_t semPost(uint16_t id);
int8_t semWait(uint16_t id);

#endif