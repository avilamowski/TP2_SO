#include <process.h>
#include <stdio.h>
#include <syscalls.h>
#include <test_util.h>

enum State { RUNNING,
			 BLOCKED,
			 KILLED };

typedef struct P_rq {
	int32_t pid;
	enum State state;
} p_rq;

int64_t test_processes(int argc, char *argv[]) {
	uint8_t rq;
	uint8_t alive = 0;
	uint8_t action;
	uint64_t max_processes;
	// char *argvAux[] = {0};

	if (argc != 1)
		return -1;

	if ((max_processes = satoi(argv[0])) <= 0)
		return -1;

	p_rq p_rqs[max_processes];

	while (1) {
		printf("x");
		// Create max_processes processes
		for (rq = 0; rq < max_processes; rq++) {
			char *args[] = {"endless_loop", 0};
			p_rqs[rq].pid = createProcess(&endless_loop, args, "endless_loop", 0);

			if (p_rqs[rq].pid == -1) {
				printf("test_processes: ERROR creating process\n");
				return -1;
			}
			else {
				p_rqs[rq].state = RUNNING;
				alive++;
			}
		}

		// Randomly kills, blocks or unblocks processes until every one has been killed
		while (alive > 0) {
			for (rq = 0; rq < max_processes; rq++) {
				action = GetUniform(100) % 2;

				switch (action) {
					case 0:
						if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
							if (killProcess(p_rqs[rq].pid) == -1) {
								printf("test_processes: ERROR killing process\n");
								return -1;
							}
							p_rqs[rq].state = KILLED;
							alive--;
						}
						break;

					case 1:
						if (p_rqs[rq].state == RUNNING) {
							if (block(p_rqs[rq].pid) == -1) {
								printf("test_processes: ERROR blocking process\n");
								return -1;
							}
							p_rqs[rq].state = BLOCKED;
						}
						break;
				}
			}

			// Randomly unblocks processes
			for (rq = 0; rq < max_processes; rq++)
				if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
					if (unblock(p_rqs[rq].pid) == -1) {
						printf("test_processes: ERROR unblocking process\n");
						return -1;
					}
					p_rqs[rq].state = RUNNING;
				}
		}
	}
}
