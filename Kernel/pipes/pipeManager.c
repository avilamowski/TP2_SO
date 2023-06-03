#include <defs.h>
#include <memoryManager.h>
#include <pipeManager.h>
#include <process.h>
#include <scheduler.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_PIPES (1 << 12)
#define bufferPosition(pipe) (((pipe)->startPosition + (pipe)->currentSize) % PIPE_SIZE)

typedef struct Pipe {
	char buffer[PIPE_SIZE];
	uint16_t startPosition;
	uint16_t currentSize;
	int16_t inputPid, outputPid;
	uint8_t isBlocking;
} Pipe;

static int16_t getPipeIndexById(uint16_t id);
static Pipe *getPipeById(PipeManagerADT pipeManager, uint16_t id);
static void freePipe(Pipe *pipe);
static Pipe *createPipe();

typedef struct PipeManagerCDT {
	Pipe *pipes[MAX_PIPES];
	uint16_t lastFreePipe;
	uint16_t qtyPipes;
} PipeManagerCDT;

static PipeManagerADT getPipeManager() {
	return (PipeManagerADT) PIPE_MANAGER_ADDRESS;
}

static int16_t getPipeIndexById(uint16_t id) {
	int16_t index = (int16_t) id - BUILT_IN_DESCRIPTORS;
	if (index < 0 || index > MAX_PIPES)
		return -1;
	return index;
}

static Pipe *getPipeById(PipeManagerADT pipeManager, uint16_t id) {
	int16_t index;
	if ((index = getPipeIndexById(id)) == -1)
		return NULL;
	return pipeManager->pipes[index];
}

PipeManagerADT createPipeManager() {
	PipeManagerADT pipeManager = (PipeManagerADT) PIPE_MANAGER_ADDRESS;
	for (int i = 0; i < MAX_PIPES; i++)
		pipeManager->pipes[i] = NULL;
	pipeManager->lastFreePipe = MAX_PIPES - 1;
	pipeManager->qtyPipes = 0;
	return pipeManager;
}

int16_t getLastFreePipe() {
	PipeManagerADT pipeManager = getPipeManager();
	if (pipeManager->qtyPipes >= MAX_PIPES)
		return -1;
	while (pipeManager->pipes[pipeManager->lastFreePipe] != NULL)
		pipeManager->lastFreePipe = (pipeManager->lastFreePipe + MAX_PIPES - 1) % MAX_PIPES;
	Pipe *pipe = createPipe();
	pipeManager->pipes[pipeManager->lastFreePipe] = pipe;
	pipeManager->qtyPipes++;
	return pipeManager->lastFreePipe;
}

int8_t pipeOpen(uint16_t id, uint8_t mode) {
	return pipeOpenForPid(getpid(), id, mode);
}

int8_t pipeOpenForPid(uint16_t pid, uint16_t id, uint8_t mode) {
	int16_t index;
	if ((index = getPipeIndexById(id)) == -1)
		return -1;
	PipeManagerADT pipeManager = getPipeManager();
	Pipe *pipe = pipeManager->pipes[index];
	pipeManager->qtyPipes++;
	if (pipe == NULL) {
		pipe = createPipe();
		pipeManager->pipes[index] = pipe;
	}
	if (mode == READ && pipe->outputPid < 0)
		pipe->outputPid = pid;
	else if (mode == WRITE && pipe->inputPid < 0)
		pipe->inputPid = pid;
	else
		return -1;
	return 0;
}

int8_t pipeClose(uint16_t id) {
	PipeManagerADT pipeManager = getPipeManager();
	int16_t index;
	if ((index = getPipeIndexById(id)) == -1)
		return -1;
	Pipe *pipe = pipeManager->pipes[index];
	if (pipe == NULL)
		return -1;

	uint16_t pid = getpid();
	if (pid == pipe->inputPid) {
		char eofString[1] = {EOF};
		writePipe(id, eofString, 1);
	}
	else if (pid == pipe->outputPid) {
		freePipe(pipeManager->pipes[index]);
		pipeManager->pipes[index] = NULL;
		pipeManager->qtyPipes--;
	}
	else
		return -1;

	return 0;
}

static void freePipe(Pipe *pipe) {
	free(pipe);
}

static Pipe *createPipe() {
	Pipe *pipe = (Pipe *) allocMemory(sizeof(Pipe));
	pipe->startPosition = 0;
	pipe->currentSize = 0;
	pipe->inputPid = -1;
	pipe->outputPid = -1;
	pipe->isBlocking = 0;
	for (int i = 0; i < PIPE_SIZE; i++)
		pipe->buffer[i] = 0;
	return pipe;
}

int64_t writePipe(uint16_t id, char *sourceBuffer, uint64_t len) {
	PipeManagerADT pipeManager = getPipeManager();
	Pipe *pipe = getPipeById(pipeManager, id);
	if (pipe == NULL ||
		pipe->inputPid != getpid() ||
		len == 0)
		return -1;

	uint64_t writtenBytes = 0;
	while (writtenBytes < len && pipe->buffer[bufferPosition(pipe)] != EOF) {
		if (pipe->currentSize == PIPE_SIZE) {
			pipe->isBlocking = 1;
			setStatus((uint16_t) pipe->inputPid, BLOCKED);
			yield();
		}
		if (pipe != getPipeById(pipeManager, id)) // Validar que no haya muerto el pipe
			return -1;

		while (pipe->currentSize < PIPE_SIZE && writtenBytes < len) {
			pipe->buffer[bufferPosition(pipe)] = sourceBuffer[writtenBytes];
			if (sourceBuffer[writtenBytes++] == EOF)
				break;
			pipe->currentSize++;
		}
		if (pipe->isBlocking) {
			setStatus((uint16_t) pipe->outputPid, READY);
			pipe->isBlocking = 0;
		}
	}
	return writtenBytes;
}

int64_t readPipe(uint16_t id, char *destinationBuffer, uint64_t len) {
	PipeManagerADT pipeManager = getPipeManager();
	Pipe *pipe = getPipeById(pipeManager, id);
	if (pipe == NULL ||
		pipe->outputPid != getpid() ||
		len == 0)
		return -1;
	uint8_t eofRead = 0;
	uint64_t readBytes = 0;
	while (readBytes < len && !eofRead) {
		if (pipe->currentSize == 0 && pipe->buffer[pipe->startPosition] != EOF) {
			pipe->isBlocking = 1;
			setStatus((uint16_t) pipe->outputPid, BLOCKED);
			yield();
		}
		while ((pipe->currentSize > 0 || pipe->buffer[pipe->startPosition] == EOF) && readBytes < len) {
			destinationBuffer[readBytes] = pipe->buffer[pipe->startPosition];
			if (destinationBuffer[readBytes++] == EOF) {
				eofRead = 1;
				break;
			}
			pipe->currentSize--;
			pipe->startPosition = (pipe->startPosition + 1) % PIPE_SIZE;
		}
		if (pipe->isBlocking) {
			setStatus((uint16_t) pipe->inputPid, READY);
			pipe->isBlocking = 0;
		}
	}
	return readBytes;
}