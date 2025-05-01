global idt_load

idt_load:
    mov eax, [esp+4]  ; Get pointer to IDT
    lidt [eax]        ; Load IDT pointer
    ret