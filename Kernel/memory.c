#include <stdint.h>
#include <video.h>


void dump(uint64_t *rip, uint64_t *rsp) {
    uint64_t *oldRSP = rip + 8;  // Apunta al RAX pusheado por el pushState
    printf("RIP: %x, RSP: %x\n", rip, rsp);
    printf("RDI: %x, RSI: %x, RAX: %x, RBX: %x, RCX: %x, RDX: %x\n",
           oldRSP[8 * 5], oldRSP[8 * 6], oldRSP[8 * 0], oldRSP[8 * 1],
           oldRSP[8 * 2], oldRSP[8 * 3]);
    printf( "R8: %x, R9: %x, R10: %x, R11: %x, R12: %x, R13: %x, R14: %x, R15: %x\n",
        oldRSP[8 * 7], oldRSP[8 * 8], oldRSP[8 * 9], oldRSP[8 * 10],
        oldRSP[8 * 11], oldRSP[8 * 12], oldRSP[8 * 13], oldRSP[8 * 14]);
}
