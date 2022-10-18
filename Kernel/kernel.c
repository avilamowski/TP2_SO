#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <interrupts.h>
#include <video.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	void * moduleAddresses[] = { sampleCodeModuleAddress, sampleDataModuleAddress };
	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);
	return getStackBase();
}

int main()
{	
	load_idt();
	((EntryPoint)sampleCodeModuleAddress)();
	/*scr_drawRect(10,10,8,32,(Color){255,255,0});
	scr_drawRect(0,0,8,16,(Color){200,200,50});
	scr_drawRect(90,90,8,8,(Color){0,255,0});*/
	while(1) _hlt();
	return 0;
}

