#include <stdint.h>
#include <video.h>


static char * _regNames[] = {"RAX", "RBX", "RCX", "RDX", "RBP", "RDI", "RSI", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15"};
void dump(uint64_t rip, uint64_t rsp, uint64_t * newRsp) {
    printf("RIP: 0x%x\nRSP: 0x%x\n", rip, rsp);
    for (int i = 0; i < sizeof(_regNames)/sizeof(char *); i++)
        printf("%s: 0x%x\n", _regNames[i], *(newRsp - i));
}
