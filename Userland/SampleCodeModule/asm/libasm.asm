EXTERN printRegisters
GLOBAL usrGetInfoReg
GLOBAL kaboom
; Para obtener los valores de los registros en tiempo real, usar esta funcion
usrGetInfoReg:
	push rax
	push rbx
	push rcx
	push rdx
    push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
    mov rbp, rsp

    mov rdi, rsp
    call printRegisters 

    mov rsp, rbp
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax

    ret

kaboom:
    UD2
    ret