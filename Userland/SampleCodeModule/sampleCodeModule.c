/* sampleCodeModule.c */
#include <stdint.h>
#include <syscalls.h>
#include <stdio.h>
#include <shell.h>
#include <tron.h>
int main() {
	//run_shell();
	startTron();
	return 0xDEADBEEF;
}