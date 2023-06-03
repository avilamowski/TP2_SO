#ifndef _PIPE_MANAGER_H
#define _PIPE_MANAGER_H

#include <stdint.h>

#define PIPE_SIZE (1 << 12)

typedef struct PipeManagerCDT *PipeManagerADT;
PipeManagerADT createPipeManager();
int16_t getLastFreePipe();
int8_t pipeOpen(uint16_t id, uint8_t mode);
int8_t pipeOpenForPid(uint16_t pid, uint16_t id, uint8_t mode);
int8_t pipeClose(uint16_t id);
int8_t pipeCloseForPid(uint16_t pid, uint16_t id);
int64_t readPipe(uint16_t id, char *destinationBuffer, uint64_t len);
int64_t writePipe(uint16_t pid, uint16_t id, char *sourceBuffer, uint64_t len);

#endif