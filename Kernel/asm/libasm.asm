EXTERN copyRegisters
GLOBAL cpuVendor
GLOBAL getTime
GLOBAL getKeyPressed
GLOBAL startSound
GLOBAL stopSound
GLOBAL saveRegisters

section .text
    
cpuVendor:
    push rbp
    mov rbp, rsp

    push rbx

    mov rax, 0
    cpuid


    mov [rdi], ebx
    mov [rdi + 4], edx
    mov [rdi + 8], ecx

    mov byte [rdi+13], 0

    mov rax, rdi

    pop rbx

    mov rsp, rbp
    pop rbp
    ret

getTime:
    push rbp
    mov rbp, rsp

    mov al, 0Bh
    out 70h, al
    in  al, 71h
    or al, 6h
    out 71h, al

    mov al, 0
    out 70h, al
    in al, 71h
    mov [rdx], al

    mov al, 2
    out 70h, al
    in al, 71h
    mov [rsi], al
    
    mov al, 4
    out 70h, al
    in al, 71h
    mov [rdi], al

    mov rsp, rbp
    pop rbp
    ret

getKeyPressed:
    push rbp
    mov rbp, rsp

    xor rax, rax
    in al, 60h

    mov rsp, rbp
    pop rbp
    ret

int_80:
    push rbp
    mov rbp, rsp

    xor rax, rax
    in al, 60h

    mov rsp, rbp
    pop rbp
    ret

startSound:
	push rbp
    push rdx
	mov rbp, rsp

	mov al, 0xB6
	out 43h, al

    mov rdx, 0
    mov rax, 1193180
    div rdi
    
	out 42h, al
	mov al, ah
	out 42h, al

	in al, 61h
 	or al, 03h
	out 61h, al

	mov rsp, rbp
    pop rdx
	pop rbp
	ret

stopSound:
  	in al, 61h
	and al, 0xFC
	out 61h, al
  	ret

saveRegisters:
    mov rdi, rbp 
    call copyRegisters
    ret