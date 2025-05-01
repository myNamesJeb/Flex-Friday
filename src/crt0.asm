; src/crt0.asm

; Ensure we are in 32-bit mode
bits 32

[section .multiboot]
    align 4
    dd 0x1BADB002        ; Multiboot magic number
    dd 0x0               ; Flags
    dd -(0x1BADB002)     ; Checksum

[section .text]
global _start
extern kmain

_start:
    ; Setup a basic stack
    mov esp, stack_top

    ; Call the C kernel entry point
    call kmain

halt:
    cli
    hlt
    jmp halt

[section .bss]
align 4
stack_bottom:
    resb 16384 ; 16 KB for stack
stack_top:
