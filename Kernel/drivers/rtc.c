#include <lib.h>
#include <scheduler.h>

uint32_t getSeconds() {
	uint8_t h, m, s;
	getTime(&h, &m, &s);
	return s + m * 60 + ((h + 24 - 3) % 24) * 3600;
}

void sleep(int seconds) {
	uint32_t limit = getSeconds() + seconds;
	while (getSeconds() < limit)
		yield();
}