; loader.asm
; Assembly functions for the kernel

BITS 64
GLOBAL gdt_flush, paging_load

extern kernel_main

section .text

gdt_flush:
    lgdt [rdi]               ; Load the GDT
    mov ax, 0x10             ; Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp flush_pipeline       ; Far jump removed for 64-bit compatibility

flush_pipeline:
    call kernel_main         ; Call the kernel's main function
    ret                      ; Return if kernel_main returns

paging_load:
    mov cr3, rdi             ; Load page directory base address
    ; Enable paging
    mov rax, cr0
    or rax, 0x80000000       ; Set PG bit
    mov cr0, rax
    ret
