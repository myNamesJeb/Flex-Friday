global gdt_ptr

; Minimal GDT definitions

; GDT table with null, code, and data segments
section .data
gdt:
    ; Null descriptor
    dd 0
    dd 0

    ; Code Segment Descriptor: base=0, limit=4GB, access=0x9A, flags=0xCF
    dw 0xFFFF           ; Limit low
    dw 0x0000           ; Base low
    db 0                ; Base middle
    db 0x9A             ; Access: Code execute/read, accessed, present, ring 0
    db 0xCF             ; Granularity: 4K, 32-bit segment, and limit high bits
    db 0                ; Base high

    ; Data Segment Descriptor: base=0, limit=4GB, access=0x92, flags=0xCF
    dw 0xFFFF
    dw 0x0000
    db 0                ; Base middle
    db 0x92             ; Access: Data read/write, accessed, present, ring 0
    db 0xCF
    db 0                ; Base high

gdt_end:

; GDT pointer structure (limit followed by base)
gdt_ptr:
    dw gdt_end - gdt - 1
    dd gdt

; End of src/gdt.asm