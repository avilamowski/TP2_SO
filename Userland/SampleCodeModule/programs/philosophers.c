// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <globals.h>
#include <philosophers.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscalls.h>

#define MAX_QTY 32
#define MIN_QTY 3
#define MUTEX_SEM_ID 42
#define MAX_PHILO_NUMBER_BUFFER 3

#define EAT_TIME 3
#define THINK_TIME 2
#define CLEAR_RENDERS 5

#define COMMAND_QUIT 'q'
#define COMMAND_ADD 'a'
#define COMMAND_REMOVE 'r'
#define COMMAND_PS 'p'
#define COMMAND_CLEAR 'c'

#define left(i) (((i) + qtyPhilosophers - 1) % qtyPhilosophers)
#define right(i) (((i) + 1) % qtyPhilosophers)
#define philosopherSemaphore(i) (MUTEX_SEM_ID + (i) + 1)

typedef enum { NONE = 0,
			   EATING,
			   HUNGRY,
			   THINKING } PHILOSOPHER_STATE;

static uint8_t qtyPhilosophers = 0;
static PHILOSOPHER_STATE philosopherStates[MAX_QTY];
static int16_t philosopherPids[MAX_QTY];
static uint8_t singleLine = 0;

static void render();
static int philosopher(int argc, char **argv);
static int8_t addPhilosopher(int index);
static int8_t removePhilosopher(int index);
static void takeForks(int i);
static void putForks(int i);
static void test(int i);

static const char *philosopherNames[] = {
	"Aristoteles", "Kant", "Spinoza", "Marx",
	"Russell", "Kierkegaard", "Wittgenstein", "Locke",
	"Hume", "Descartes", "Confucio", "Socrates",
	"Epicuro", "Platon", "Nietzsche", "Hegel",
	"Aquinas", "Bunge", "Sartre", "Thoreau",
	"de Beauvoir", "Foucault", "Leibniz", "Rawls",
	"Hobbes", "Bentham", "Mill", "Kripke",
	"Frege", "Popper", "Montesquieu", "Rousseau"};

int runPhilosophers(int argc, char **argv) {
	if (semInit(MUTEX_SEM_ID, 1) == -1)
		return -1;

	for (int i = 0; i < MAX_QTY; i++) {
		philosopherStates[i] = NONE;
		philosopherPids[i] = -1;
	}

	for (int i = 0; i < MIN_QTY; i++)
		addPhilosopher(i);

	char command = '\0';
	while ((command = getchar()) != COMMAND_QUIT) {
		switch (command) {
			case COMMAND_ADD:
				if (qtyPhilosophers < MAX_QTY) {
					if (addPhilosopher(qtyPhilosophers) == -1)
						printErr("No se pudo agregar un filosofo\n");
				}
				else
					printf("La mesa esta llena\n");
				break;
			case COMMAND_REMOVE:
				if (qtyPhilosophers > MIN_QTY)
					removePhilosopher(qtyPhilosophers - 1);
				else
					printf("Como minimo debe haber %d filosofos para empezar el banquete\n", MIN_QTY);
				break;
			case COMMAND_PS:
				psPrint();
				break;
			case COMMAND_CLEAR:
				singleLine = !singleLine;
				break;
		}
	}

	for (int i = qtyPhilosophers - 1; i >= 0; i--) {
		removePhilosopher(i);
	}
	semClose(MUTEX_SEM_ID);
	return 0;
}

static void render() {
	if (singleLine)
		clear();
	const static char letters[] = {' ', 'E', '.', '.'};
	uint8_t somethingToWrite = 0;
	for (int i = 0; i < qtyPhilosophers; i++) {
		if (letters[philosopherStates[i]] != ' ') {
			somethingToWrite = 1;
			printf("%c%2s", letters[philosopherStates[i]], "");
		}
	}
	if (somethingToWrite)
		putchar('\n');
	// sleep(1);
}

static int8_t addPhilosopher(int index) {
	semWait(MUTEX_SEM_ID);
	char philoNumberBuffer[MAX_PHILO_NUMBER_BUFFER] = {0}; // TODO: nombres de filosofos
	if (semInit(philosopherSemaphore(index), 0) == -1)
		return -1;
	itoa(index, philoNumberBuffer, 10);
	char *params[] = {"philosopher", philoNumberBuffer, NULL};
	int16_t fileDescriptors[] = {DEV_NULL, STDOUT, STDERR};
	philosopherPids[index] = createProcessWithFds(&philosopher, params, "philosopher", 4, fileDescriptors);
	if (philosopherPids[index] != -1)
		qtyPhilosophers++;
	render();
	semPost(MUTEX_SEM_ID);
	return -1 * !(philosopherPids[index] + 1);
}

static int8_t removePhilosopher(int index) {
	printf("Se invita a retirarse a %s\n", philosopherNames[index]);
	semWait(MUTEX_SEM_ID);
	if (philosopherStates[left(index)] == EATING && philosopherStates[right(index)] == EATING) { // Estado invalido para remover
		semPost(MUTEX_SEM_ID);																	 // Bloquearse hasta que el filosofo este comiendo
		semWait(philosopherSemaphore(index));
	}
	else
		semPost(MUTEX_SEM_ID);
	semWait(MUTEX_SEM_ID);
	killProcess(philosopherPids[index]);
	waitpid(philosopherPids[index]);
	printf("Se retira %s\n", philosopherNames[index]);
	semClose(philosopherSemaphore(index));
	philosopherPids[index] = -1;
	philosopherStates[index] = NONE;
	qtyPhilosophers--;
	render();
	semPost(MUTEX_SEM_ID);
	return 0;
}

static int philosopher(int argc, char **argv) {
	int i = atoi(argv[1]);
	printf("Entra %s\n", philosopherNames[i]);
	philosopherStates[i] = THINKING;
	while (1) {
		sleep(THINK_TIME);
		takeForks(i);
		sleep(EAT_TIME);
		putForks(i);
	}
	return 0;
}

static void takeForks(int i) {
	semWait(MUTEX_SEM_ID);
	philosopherStates[i] = HUNGRY;
	test(i);
	semPost(MUTEX_SEM_ID);
	semWait(philosopherSemaphore(i));
}

static void putForks(int i) {
	semWait(MUTEX_SEM_ID);
	philosopherStates[i] = THINKING;
	render();
	test(left(i));
	test(right(i));
	semPost(MUTEX_SEM_ID);
}

static void test(int i) {
	if (philosopherStates[i] == HUNGRY && philosopherStates[left(i)] != EATING && philosopherStates[right(i)] != EATING) {
		philosopherStates[i] = EATING;
		semPost(philosopherSemaphore(i));
		render();
	}
}