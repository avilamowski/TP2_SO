// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "test_util.h"
#include <process.h>
#include <stdint.h>
#include <stdio.h>
#include <syscalls.h>

#define MINOR_WAIT "500000000"
#define WAIT 1000000000

#define TOTAL_PROCESSES 3
#define LOWEST 0
#define MEDIUM 2
#define HIGHEST 4

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio() {
	int64_t pids[TOTAL_PROCESSES];
	char *argv[] = {"endless_loop_print", MINOR_WAIT, 0};
	uint64_t i;

	for (i = 0; i < TOTAL_PROCESSES; i++)
		pids[i] = createProcess(&endless_loop_print, argv, "endless_loop_print", 0);

	bussy_wait(WAIT);
	clear();
	printf("\nCHANGING PRIORITIES...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		nice(pids[i], prio[i]);

	bussy_wait(WAIT);
	clear();
	printf("\nBLOCKING...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		block(pids[i]);

	printf("CHANGING PRIORITIES WHILE BLOCKED...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		nice(pids[i], MEDIUM);

	printf("UNBLOCKING...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		unblock(pids[i]);

	bussy_wait(WAIT);
	clear();
	printf("\nKILLING...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		killProcess(pids[i]);
}
