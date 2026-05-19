[org 0x7c00]
bits 16

start:
    mov ah, 0x0e
    mov si, message

print_loop:
    lodsb
    cmp al, 0
    je hang

    int 0x10
    jmp print_loop
hang:
    jmp hang

message db 'Hello from MyOS!', 0

times 510 - ($ - $$) db 0
dw 0xaa55
