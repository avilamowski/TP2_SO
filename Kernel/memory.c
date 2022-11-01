#include <stdint.h>
#include <memory.h>
#include <video.h>

uint64_t _regSnapshot[QTY_REGS] = {0};      /* Vector de valores de registros */
static char * _regNames[] = {"RAX", "RBX", "RCX", "RDX", "RBP", "RDI", "RSI", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15"};

uint64_t * getLastRegSnapshot() {
    return _regSnapshot;
}

/**
 * @brief Carga en regSnapshot los valores de los registros
 * @note  Se asume que todos los registros llegan por stack 
 */
void copyRegisters(uint64_t * rsp) {
    uint64_t * p = rsp + 23; 
    uint64_t * rspEnd = rsp+7; 
    int i = 0;

    _regSnapshot[0] = *p;
    _regSnapshot[1] = (uint64_t) p + 0x30;
    p--;
    while (p != rspEnd) {
        _regSnapshot[i+2] = *p;
        p--;
        i++;
    }
}

void dump(uint64_t rip, uint64_t rsp, uint64_t * newRsp) {
    printf("RIP: 0x%x\nRSP: 0x%x\n", rip, rsp);
    for (int i = 0; i < sizeof(_regNames)/sizeof(char *); i++)
        printf("%s: 0x%x\n", _regNames[i], *(newRsp - i));
}
