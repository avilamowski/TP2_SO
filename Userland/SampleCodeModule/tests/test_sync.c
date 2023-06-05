// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <process.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>
#include <test_util.h>

#define SEM_ID 69
#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc) {
	int64_t aux = *p;
	yield(); // This makes the race condition highly probable
	aux += inc;
	*p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
	uint64_t n;
	int8_t inc;
	int8_t use_sem;

	if (argc != 4)
		return -1;

	if ((n = satoi(argv[1])) <= 0)
		return -1;
	if ((inc = satoi(argv[2])) == 0)
		return -1;
	if ((use_sem = satoi(argv[3])) < 0)
		return -1;

	if (use_sem) {
		if (semOpen(SEM_ID) == -1) {
			printf("test_sync: ERROR opening semaphore\n");
			return -1;
		}
	}

	uint64_t i;
	for (i = 0; i < n; i++) {
		if (use_sem)
			semWait(SEM_ID);
		slowInc(&global, inc);
		if (use_sem)
			semPost(SEM_ID);
	}
	return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
	uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

	if (argc != 3)
		return -1;

	// Agregado para que sea compatible con nuestra implementacion
	int8_t useSem = satoi(argv[2]);
	if (useSem) {
		if (semInit(SEM_ID, 1) == -1) {
			printf("test_sync: ERROR creating semaphore\n");
			return -1;
		}
	}

	char *argvDec[] = {"my_process_dec", argv[1], "-1", argv[2], NULL};
	char *argvInc[] = {"my_process_inc", argv[1], "1", argv[2], NULL};

	global = 0;

	uint64_t i;
	for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
		pids[i] = createProcess(&my_process_inc, argvInc, "my_process_inc", 4);
		pids[i + TOTAL_PAIR_PROCESSES] = createProcess(&my_process_inc, argvDec, "my_process_dec", 4);
	}

	for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
		waitpid(pids[i]);
		waitpid(pids[i + TOTAL_PAIR_PROCESSES]);
	}

	printf("Final value: %d\n", global);

	if (useSem)
		semClose(SEM_ID);

	return 0;
}