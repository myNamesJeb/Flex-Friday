global keyboard_handler_stub
extern keyboard_handler   ; the C handler from keyboard.c

keyboard_handler_stub:
    pusha
    call keyboard_handler
    popa
    ; Send End Of Interrupt (EOI) to PIC (port 0x20)
    mov al, 0x20
    out 0x20, al
    iret