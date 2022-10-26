/* sampleCodeModule.c */
#include <stdint.h>
#include <syscalls.h>
#include <stdio.h>
#include <shell.h>
#include <tron.h>
int main() {
	run_shell();
	return 0xDEADBEEF;
}