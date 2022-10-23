global loader
global load_main
extern main
extern initializeKernelBinary
extern getStackBase

loader:
	mov rax, 4
	push rax
	call initializeKernelBinary	; Set up the kernel binary, and get thet stack address
load_main:
	call getStackBase	
	mov rsp, rax				; Set up the stack with the returned address
	call main
hang:
	cli
	hlt	; halt machine should kernel return
	jmp hang
