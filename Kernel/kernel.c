#include <defs.h>
#include <interrupts.h>
#include <lib.h>
#include <memoryManager.h>
#include <moduleLoader.h>
#include <scheduler.h>
#include <stdint.h>
#include <video.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *) 0x400000;
static void *const sampleDataModuleAddress = (void *) 0x500000;
static void *const memAmount = (void *) (SYSTEM_VARIABLES + 132);

typedef int (*EntryPoint)();
int idle(int argc, char **argv);

void clearBSS(void *bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void *getStackBase() {
	return (void *) ((uint64_t) &endOfKernel +
					 PageSize * 8		// The size of the stack itself, 32KiB
					 - sizeof(uint64_t) // Begin at the top of the stack
	);
}

void initializeKernelBinary() {
	void *moduleAddresses[] = {sampleCodeModuleAddress, sampleDataModuleAddress};
	uint64_t userlandSize = loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);

	uint64_t memAmountBytes = *((uint32_t *) memAmount) * (1 << 20);

	uint64_t availableMem =
		1 << log(memAmountBytes - (uint64_t) (sampleDataModuleAddress + userlandSize), 2);

	createMemoryManager((void *) MEMORY_MANAGER_ADDRESS, sampleDataModuleAddress + userlandSize, availableMem);
	createScheduler();
}

int main() {
	char *argsIdle[3] = {"idle", "Hm?", NULL};
	createProcess((MainFunction) &idle, argsIdle, "idle", 4);
	load_idt();
	return 0;
}

int idle(int argc, char **argv) {
	char *argsShell[2] = {"shell", NULL};
	createProcess((MainFunction) sampleCodeModuleAddress, argsShell, "shell", 4);
	while (1) {
		uint8_t oldM;
		uint8_t h, m, s;
		getTime(&h, &m, &s);
		if (m != oldM) {
			printf("\n%d:%d:%d %d %s\n", h, m, s, argc, argv[1]);
		}
		oldM = m;
		_hlt();
	}
	return 0;
}
