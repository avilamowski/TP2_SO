// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <interrupts.h>
#include <lib.h>
#include <speaker.h>
#include <time.h>
void playSound(uint64_t frequency, uint64_t ticks) {
	if (frequency > 0) {
		startSound(frequency);
	}
	int initialTicks = ticksElapsed();
	while (ticksElapsed() - initialTicks < ticks) {
		_hlt();
	}
	if (frequency > 0)
		stopSound();
}