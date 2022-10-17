/* sampleCodeModule.c */
#include <stdint.h>
extern uint64_t write(int fd, const void * buf, uint64_t count);
char * v = (char*)0xB8000 + 79 * 2;

int main() {
	//All the following code may be removed 
	write(1, "Novardo!", 10);
	return 0xDEADBEEF;
}