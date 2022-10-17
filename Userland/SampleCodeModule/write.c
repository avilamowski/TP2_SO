#include <stdint.h>

uint64_t write(int fd, const void * buf, uint64_t count){
    __asm__(
    "movq $1, %rax;"
    "int $0x80;"
    );
    return 0;
}