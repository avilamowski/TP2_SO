#include <color.h>
#include <defs.h>
#include <keyboard.h>
#include <lib.h>
#include <memory.h>
#include <memoryManager.h>
#include <pipeManager.h>
#include <process.h>
#include <scheduler.h>
#include <semaphoreManager.h>
#include <speaker.h>
#include <stdint.h>
#include <time.h>
#include <video.h>

/* IDs de syscalls */
#define READ 0
#define WRITE 1
#define CLEAR 2
#define SECONDS 3
#define GET_REGISTER_ARRAY 4
#define SET_FONT_SIZE 5
#define GET_RESOLUTION 6
#define DRAW_RECT 7
#define GET_TICKS 8
#define GET_MEMORY 9
#define PLAY_SOUND 10
#define SET_FONT_COLOR 11
#define GET_FONT_COLOR 12
#define MALLOC 13
#define FREE 14
#define CREATE_PROCESS 15
#define EXIT_PROCESS 16
#define GET_PID 17
#define PS 18
#define KILL_PROCESS 19
#define CHANGE_PROCESS_STATUS 20
#define CHANGE_PROCESS_PRIORITY 21
#define YIELD 22
#define WAITPID 23
#define SEM_INIT 24
#define SEM_OPEN 25
#define SEM_CLOSE 26
#define SEM_POST 27
#define SEM_WAIT 28
#define PIPE_OPEN 29
#define PIPE_CLOSE 30

static int64_t syscall_read(int16_t fd, char *destinationBuffer, uint64_t len);
static int64_t syscall_write(int16_t fd, char *sourceBuffer, uint64_t len);
static void syscall_clear();
static uint32_t syscall_seconds();
static uint64_t *syscall_registerArray(uint64_t *regarr);
static void syscall_fontSize(uint8_t size);
static uint32_t syscall_resolution();
static void syscall_drawRect(uint16_t x, uint16_t y, uint16_t width,
							 uint16_t height, uint32_t color);
static uint64_t syscall_getTicks();
static void syscall_getMemory(uint64_t pos, uint8_t *vec);
static void syscall_playSound(uint64_t frequency, uint64_t ticks);
static void syscall_setFontColor(uint8_t r, uint8_t g, uint8_t b);
static uint32_t syscall_getFontColor();
static void *syscall_malloc(uint64_t size);
static void syscall_free(void *ptr);
static int16_t syscall_createProcess(MainFunction code, char **args, char *name, uint8_t priority, int16_t fileDescriptors[]);
static void syscall_exitProcess(int32_t retValue);
static uint16_t syscall_getpid();
static ProcessSnapshotList *syscall_ps();
static int32_t syscall_killProcess(uint16_t pid);
static int8_t syscall_changeProcessPriority(uint16_t pid, uint8_t priority);
static int8_t syscall_changeProcessStatus(uint16_t pid, uint8_t status);
static int32_t syscall_waitpid(uint16_t pid);

uint64_t syscallDispatcher(uint64_t nr, uint64_t arg0, uint64_t arg1,
						   uint64_t arg2, uint64_t arg3, uint64_t arg4,
						   uint64_t arg5) {
	switch (nr) {
		case READ:
			return syscall_read((int16_t) arg0, (char *) arg1, (uint64_t) arg2);
		case WRITE:
			return syscall_write((int16_t) arg0, (char *) arg1, (uint64_t) arg2);
		case CLEAR:
			syscall_clear();
			break;
		case SECONDS:
			return syscall_seconds();
		case GET_REGISTER_ARRAY:
			return (uint64_t) syscall_registerArray((uint64_t *) arg0);
		case SET_FONT_SIZE:
			syscall_fontSize((uint8_t) arg0);
			break;
		case GET_RESOLUTION:
			return syscall_resolution();
		case DRAW_RECT:
			syscall_drawRect((uint16_t) arg0, (uint16_t) arg1, (uint16_t) arg2,
							 (uint16_t) arg3, (uint32_t) arg4);
			break;
		case GET_TICKS:
			return syscall_getTicks();
		case GET_MEMORY:
			syscall_getMemory((uint64_t) arg0, (uint8_t *) arg1);
			break;
		case PLAY_SOUND:
			syscall_playSound(arg0, arg1);
			break;
		case SET_FONT_COLOR:
			syscall_setFontColor((uint8_t) arg0, (uint8_t) arg1, (uint8_t) arg2);
			break;
		case GET_FONT_COLOR:
			return syscall_getFontColor();
		case MALLOC:
			return (uint64_t) syscall_malloc((uint64_t) arg0);
		case FREE:
			syscall_free((void *) arg0);
			break;
		case CREATE_PROCESS:
			return syscall_createProcess((MainFunction) arg0, (char **) arg1, (char *) arg2, (uint8_t) arg3, (int16_t *) arg4);
		case EXIT_PROCESS:
			syscall_exitProcess((int32_t) arg0);
			break;
		case GET_PID:
			return syscall_getpid();
		case PS:
			return (uint64_t) syscall_ps();
		case KILL_PROCESS:
			return syscall_killProcess((uint16_t) arg0);
		case CHANGE_PROCESS_PRIORITY:
			return syscall_changeProcessPriority((uint16_t) arg0, (uint8_t) arg1);
		case CHANGE_PROCESS_STATUS:
			syscall_changeProcessStatus((uint16_t) arg0, (uint8_t) arg1);
			break;
		case YIELD:
			yield();
			break;
		case WAITPID:
			return syscall_waitpid((uint16_t) arg0);
		case SEM_INIT:
			return semInit((uint16_t) arg0, (uint32_t) arg1);
		case SEM_OPEN:
			return semOpen((uint16_t) arg0);
		case SEM_CLOSE:
			return semClose((uint16_t) arg0);
		case SEM_POST:
			return semPost((uint16_t) arg0);
		case SEM_WAIT:
			return semWait((uint16_t) arg0);
		case PIPE_OPEN:
			return pipeOpen((uint16_t) arg0, (uint8_t) arg1);
		case PIPE_CLOSE:
			return pipeClose((uint16_t) arg0);
	}
	return 0;
}

static int64_t syscall_read(int16_t fd, char *destinationBuffer, uint64_t len) {
	if (fd == DEV_NULL)
		return 0;
	else if (fd < DEV_NULL)
		return -1;

	int16_t fdValue = fd < BUILT_IN_DESCRIPTORS ? getCurrentProcessFileDescriptor(fd) : fd;
	if (fdValue >= BUILT_IN_DESCRIPTORS) {
		return readPipe(fdValue, destinationBuffer, len);
	}
	else if (fdValue == STDIN) {
		for (uint64_t i = 0; i < len; i++)
			destinationBuffer[i] = getAscii();
		return len; // TODO: Ver si hay que acotar por Ctrl C o D
	}
	return -1;
}

static int64_t syscall_write(int16_t fd, char *sourceBuffer, uint64_t len) {
	if (fd == DEV_NULL)
		return 0;
	else if (fd < DEV_NULL)
		return -1;
	int16_t fdValue = fd < BUILT_IN_DESCRIPTORS ? getCurrentProcessFileDescriptor(fd) : fd;
	if (fdValue >= BUILT_IN_DESCRIPTORS) {
		return writePipe(fdValue, sourceBuffer, len);
	}
	else if (fdValue == STDOUT || fdValue == STDERR) {
		Color prevColor = getFontColor();
		if (fd == STDERR)
			setFontColor(ERROR_COLOR);
		for (uint64_t i = 0; i < len; i++)
			printChar(sourceBuffer[i]);
		setFontColor(prevColor);
		return len; // TODO: ver si hay que acotar por Ctrl C o D
	}
	return -1;
}

static void syscall_clear() {
	videoClear();
}

// Get time in seconds
static uint32_t syscall_seconds() {
	uint8_t h, m, s;
	getTime(&h, &m, &s);
	return s + m * 60 + ((h + 24 - 3) % 24) * 3600;
}

// Get register snapshot array
static uint64_t *syscall_registerArray(uint64_t *regarr) {
	uint64_t *snapshot = getLastRegSnapshot();
	for (int i = 0; i < QTY_REGS; i++)
		regarr[i] = snapshot[i];
	return regarr;
}

static void syscall_fontSize(uint8_t size) {
	setFontSize(size - 1);
}

// Get screen resolution
static uint32_t syscall_resolution() {
	return getScreenResolution();
}

static void syscall_drawRect(uint16_t x, uint16_t y, uint16_t width,
							 uint16_t height, uint32_t color) {
	ColorInt myColor = {bits: color};
	drawRect(x, y, width, height, myColor.color);
}

static uint64_t syscall_getTicks() {
	return ticksElapsed();
}

static void syscall_getMemory(uint64_t pos, uint8_t *vec) {
	memcpy(vec, (uint8_t *) pos, 32);
}

static void syscall_playSound(uint64_t frequency, uint64_t ticks) {
	playSound(frequency, ticks);
}

static void syscall_setFontColor(uint8_t r, uint8_t g, uint8_t b) {
	setFontColor((Color){b, g, r});
}

static uint32_t syscall_getFontColor() {
	ColorInt c = {color: getFontColor()};
	return c.bits;
}

static void *syscall_malloc(uint64_t size) {
	return allocMemory(size);
}

static void syscall_free(void *ptr) {
	free(ptr);
}

static int16_t syscall_createProcess(MainFunction code, char **args, char *name, uint8_t priority, int16_t fileDescriptors[]) {
	return createProcess(code, args, name, priority, fileDescriptors);
}

static void syscall_exitProcess(int32_t retValue) {
	killCurrentProcess(retValue);
}

static uint16_t syscall_getpid() {
	return getpid();
}

static ProcessSnapshotList *syscall_ps() {
	return getProcessSnapshot();
}

static int32_t syscall_killProcess(uint16_t pid) {
	return killProcess(pid, -1);
}

static int8_t syscall_changeProcessPriority(uint16_t pid, uint8_t priority) {
	return setPriority(pid, priority);
}

static int8_t syscall_changeProcessStatus(uint16_t pid, uint8_t status) {
	int8_t newStatus = setStatus(pid, status);
	if (newStatus != -1 && getpid() == pid)
		yield();
	return newStatus;
}

static int32_t syscall_waitpid(uint16_t pid) {
	return getZombieRetValue(pid);
}