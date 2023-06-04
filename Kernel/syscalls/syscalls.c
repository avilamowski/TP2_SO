#include <color.h>
#include <defs.h>
#include <keyboard.h>
#include <lib.h>
#include <memoryManager.h>
#include <pipeManager.h>
#include <process.h>
#include <registers.h>
#include <scheduler.h>
#include <semaphoreManager.h>
#include <speaker.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <video.h>

static int64_t syscall_read(int16_t fd, char *destinationBuffer, uint64_t len);
static int64_t syscall_write(int16_t fd, char *sourceBuffer, uint64_t len);
static uint32_t syscall_seconds();
static uint64_t *syscall_registerArray(uint64_t *regarr);
static void syscall_fontSize(uint8_t size);
static void syscall_drawRect(uint16_t x, uint16_t y, uint16_t width,
							 uint16_t height, uint32_t color);
static void syscall_getMemory(uint64_t pos, uint8_t *vec);
static void syscall_setFontColor(uint8_t r, uint8_t g, uint8_t b);
static uint32_t syscall_getFontColor();
static int16_t syscall_createProcess(MainFunction code, char **args, char *name, uint8_t priority, int16_t fileDescriptors[]);
static int32_t syscall_killProcess(uint16_t pid);
static int8_t syscall_changeProcessStatus(uint16_t pid, uint8_t status);
static void syscall_sleep(int seconds);

typedef uint64_t (*Syscall)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

uint64_t syscallDispatcher(uint64_t nr, uint64_t arg0, uint64_t arg1,
						   uint64_t arg2, uint64_t arg3, uint64_t arg4,
						   uint64_t arg5) {
	static Syscall syscalls[] = {
		(Syscall) syscall_read, (Syscall) syscall_write, (Syscall) videoClear,
		(Syscall) syscall_seconds, (Syscall) syscall_registerArray, (Syscall) syscall_fontSize,
		(Syscall) getScreenResolution, (Syscall) syscall_drawRect, (Syscall) ticksElapsed,
		(Syscall) syscall_getMemory, (Syscall) playSound, (Syscall) syscall_setFontColor,
		(Syscall) syscall_getFontColor, (Syscall) allocMemory, (Syscall) free,
		(Syscall) syscall_createProcess, (Syscall) killCurrentProcess, (Syscall) getpid,
		(Syscall) getProcessSnapshot, (Syscall) syscall_killProcess, (Syscall) syscall_changeProcessStatus,
		(Syscall) setPriority, (Syscall) yield, (Syscall) getZombieRetValue, // waitpid
		(Syscall) semInit, (Syscall) semOpen, (Syscall) semClose,
		(Syscall) semPost, (Syscall) semWait, (Syscall) pipeOpen,
		(Syscall) pipeClose, (Syscall) getLastFreePipe, (Syscall) syscall_sleep};
	return syscalls[nr](arg0, arg1, arg2, arg3, arg4, arg5);
}

static int64_t syscall_read(int16_t fd, char *destinationBuffer, uint64_t len) {
	if (fd == DEV_NULL) {
		destinationBuffer[0] = EOF;
		return 0;
	}
	else if (fd < DEV_NULL)
		return -1;

	int16_t fdValue = fd < BUILT_IN_DESCRIPTORS ? getCurrentProcessFileDescriptor(fd) : fd;
	if (fdValue >= BUILT_IN_DESCRIPTORS) {
		return readPipe(fdValue, destinationBuffer, len);
	}
	else if (fdValue == STDIN) {
		for (uint64_t i = 0; i < len; i++) {
			destinationBuffer[i] = getAscii();
			if (destinationBuffer[i] == EOF)
				return i + 1;
		}
		return len;
	}
	return -1;
}

static int64_t syscall_write(int16_t fd, char *sourceBuffer, uint64_t len) {
	if (fd == DEV_NULL)
		return 0;
	else if (fd < DEV_NULL)
		return -1;
	int16_t fdValue = fd < BUILT_IN_DESCRIPTORS ? getCurrentProcessFileDescriptor(fd) : fd;
	if (fdValue >= BUILT_IN_DESCRIPTORS)
		return writePipe(getpid(), fdValue, sourceBuffer, len);
	else if (fdValue == STDOUT || fdValue == STDERR) {
		Color prevColor = getFontColor();
		if (fd == STDERR)
			setFontColor(ERROR_COLOR);
		for (uint64_t i = 0; i < len; i++)
			printChar(sourceBuffer[i]);
		setFontColor(prevColor);
		return len;
	}
	return -1;
}

// Get time in seconds
static uint32_t syscall_seconds() {
	uint8_t h, m, s;
	getTime(&h, &m, &s);
	return s + m * 60 + ((h + 24 - 3) % 24) * 3600;
}

static void syscall_sleep(int seconds) {
	uint32_t limit = syscall_seconds() + seconds;
	while (syscall_seconds() < limit)
		yield();
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

static void syscall_drawRect(uint16_t x, uint16_t y, uint16_t width,
							 uint16_t height, uint32_t color) {
	ColorInt myColor = {bits: color};
	drawRect(x, y, width, height, myColor.color);
}

static void syscall_getMemory(uint64_t pos, uint8_t *vec) {
	memcpy(vec, (uint8_t *) pos, 32);
}

static void syscall_setFontColor(uint8_t r, uint8_t g, uint8_t b) {
	setFontColor((Color){b, g, r});
}

static uint32_t syscall_getFontColor() {
	ColorInt c = {color: getFontColor()};
	return c.bits;
}

static int16_t syscall_createProcess(MainFunction code, char **args, char *name, uint8_t priority, int16_t fileDescriptors[]) {
	return createProcess(code, args, name, priority, fileDescriptors, 0);
}

static int32_t syscall_killProcess(uint16_t pid) {
	return killProcess(pid, -1);
}

static int8_t syscall_changeProcessStatus(uint16_t pid, uint8_t status) {
	int8_t newStatus = setStatus(pid, status);
	if (newStatus != -1 && getpid() == pid)
		yield();
	return newStatus;
}