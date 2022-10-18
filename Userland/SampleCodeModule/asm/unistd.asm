GLOBAL write
GLOBAL read

write:
    mov rax, 1
    int 80h
    ret

read:
    mov rax, 0
    int 80h
    ret