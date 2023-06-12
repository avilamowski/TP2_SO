#ifndef SHARED_MEMORY

#define SHARED_MEMORY
#include <stdint.h>
void initSharedMemory();
void *openSharedMemory(uint16_t id);

#endif