global timer_handler_stub

; Timer interrupt stub
timer_handler_stub:
    pusha
    ; (Optionally add code here for timer tasks)
    popa
    ; Send End Of Interrupt (EOI) to PIC (port 0x20)
    mov al, 0x20
    out 0x20, al
    iret