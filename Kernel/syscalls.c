#include <stdint.h>
#include <video.h>
#include <keyboard.h>
static char syscall_0(uint32_t fd);
static uint64_t syscall_1(uint32_t fd, const char *buff , uint64_t count);

uint64_t syscallDispatcher(uint64_t nr, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
	switch (nr) {
        case 0:
            return syscall_0((uint32_t)arg0);
		case 1:
			return syscall_1((uint32_t)arg0, (char *)arg1, (uint64_t)arg2);
	}
	return -1;
}

// Read char
char syscall_0(uint32_t fd){
    switch (fd){
        case 1:
            return getAscii();
    }
}

// Write
uint64_t syscall_1(uint32_t fd, const char *buff , uint64_t count){
    Color color;
    switch (fd)
    {
    case 1:
        color = (Color){200, 200, 200};
        break;
    case 2:
        color = (Color){255, 0, 0};
        break;
    }
    scr_setPenColor(color);
    scr_print(buff);
    return count;
}