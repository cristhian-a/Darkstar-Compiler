global _start

_start:
    ; var declaration
    ; int_lit
    mov rax, 69
    push rax

    ; var declaration
    ; int_lit
    mov rax, 15
    push rax

    ; var declaration
    ; int_lit
    mov rax, 22
    push rax

    ; exit
    ; ident
    push QWORD [rsp + 0]

    mov rax, 60
    pop rdi
    syscall

    mov rax, 60
    mov rdi, 0
    syscall