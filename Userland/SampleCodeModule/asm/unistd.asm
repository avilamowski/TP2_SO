GLOBAL write
GLOBAL read
GLOBAL getSeconds

write:
    mov rax, 1
    int 80h
    ret

read:
    mov rax, 0
    int 80h
    ret

getSeconds:
    mov rax, 3
    int 80h
    ret

