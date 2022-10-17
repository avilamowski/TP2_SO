#include <stdint.h>
#include <video.h>
static uint64_t syscall_1();

uint64_t syscallDispatcher(uint64_t nr, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
	switch (nr) {
		case 1:
			return syscall_1((uint32_t)arg0, (char *)arg1, (uint64_t)arg2);
			break;
	}
	return -1;
}

uint64_t syscall_1(uint32_t fd, const char *buff , uint64_t count){
    Color color;
    switch (fd)
    {
    case 1:
        color = (Color){255, 0, 0};
        break;
    case 2:
        color = (Color){0, 0, 0};
        break;
    }
    scr_setPenColor(color);
    scr_print(buff);
    return count;
}



