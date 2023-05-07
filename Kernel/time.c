#include <stdint.h>
#include <time.h>
static uint64_t ticks = 0;

void timerHandler() { ticks++; }

uint64_t ticksElapsed() { return ticks; }

int secondsElapsed() { return ticks / 18; }