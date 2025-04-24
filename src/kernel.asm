; src/kernel.asm
[section .multiboot]
    align 4
    dd 0x1BADB002        ; Multiboot magic number
    dd 0x0               ; Flags (none needed)
    dd -(0x1BADB002)     ; Checksum

[section .text]
    global _start
_start:
    mov esi, message
    mov edi, 0xb8000

.print_loop:
    lodsb
    cmp al, 0
    je .done
    mov byte [edi], al
    mov byte [edi+1], 0x07
    add edi, 2
    jmp .print_loop

.done:
    cli
.halt:
    hlt
    jmp .halt

[section .data]
message db 'Hello, World!', 0
