global gdt_flush

section .text
gdt_flush:
    mov eax, [esp+4]  ; Get pointer to GDT passed as argument
    lgdt [eax]        ; Load new GDT pointer
    mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
    mov ds, ax        ; Load all data segment selectors
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush  ; 0x08 is the offset to our code segment
.flush:
    ret