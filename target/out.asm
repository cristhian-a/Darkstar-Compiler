global _start

_start:
    ; exit
    ; int_lit
    mov rax, 0
    push rax

    mov rax, 60
    pop rdi
    syscall

    mov rax, 60
    mov rdi, 0
    syscall