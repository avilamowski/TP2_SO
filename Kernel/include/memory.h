#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdint.h>
#define QTY_REGS 17

void dump(uint64_t rip, uint64_t rsp, uint64_t * newRsp);
uint64_t * getLastRegSnapshot();

#endif