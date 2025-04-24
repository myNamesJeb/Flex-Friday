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
extern gdt_ptr        ; from gdt.asm

_start:
    ; Load the GDT
    lgdt [gdt_ptr]
    ; Far jump: 0x08 is the selector for our code segment in the new GDT
    jmp 0x08:protected_mode_entry

protected_mode_entry:
    ; Set up segment registers with the data segment selector (0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Call the C kernel entry point
    call kmain

halt:
    cli
    hlt
    jmp halt
