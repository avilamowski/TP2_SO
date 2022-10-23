#include <stdint.h>
#include <video.h>
#include <memory.h>
#define ZERO_EXCEPTION_ID 0
#define OPCODE_EXCEPTION_ID 6

extern void load_main();

static void printError(char * msg, uint64_t *rip, uint64_t *rsp);

char * ZERO_EXCEPTION_DESCRIPTION = "Division por cero";
char * OPCODE_EXCEPTION_DESCRIPTION = "Operacion invalida";

void exceptionDispatcher(int ex, uint64_t * rip, uint64_t * rsp) {
	char * msg;
	switch(ex) {
		case ZERO_EXCEPTION_ID:
			msg = ZERO_EXCEPTION_DESCRIPTION;
			break;
		case OPCODE_EXCEPTION_ID:
			msg = OPCODE_EXCEPTION_DESCRIPTION;
			break;
	}
	printError(msg, (uint64_t *)rip, (uint64_t *)rsp);
	load_main();
	
}
static void printError(char * msg, uint64_t *rip, uint64_t *rsp){
	setFontColor((Color) ERROR_COLOR);
    printf("Error: %s\n\n", msg);
	dump(rip, rsp);
    setFontColor((Color) DEFAULT_COLOR);
}