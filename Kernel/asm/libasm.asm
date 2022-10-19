GLOBAL cpuVendor
GLOBAL getTime
GLOBAL getKeyPressed
GLOBAL getRegisterArray

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


getRegisterArray:
    ; TODO: Ver que falta
    mov [regarr], rax
    mov [regarr+8], rbx
    mov [regarr+8*2], rcx
    mov [regarr+8*3], rdx
    mov [regarr+8*4], rsi
    mov [regarr+8*5], rdi
    mov [regarr+8*6], rbp
    mov [regarr+8*7], r8
    mov [regarr+8*8], r9
    mov [regarr+8*9], r10
    mov [regarr+8*10], r11
    mov [regarr+8*11], r12
    mov [regarr+8*12], r13
    mov [regarr+8*13], r14
    mov [regarr+8*14], r15

    mov rax, regarr
    ret

int_80:
    push rbp
    mov rbp, rsp

    xor rax, rax
    in al, 60h

    mov rsp, rbp
    pop rbp
    ret

section .data
regarr: resq 18