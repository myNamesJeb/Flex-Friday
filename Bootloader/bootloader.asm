; Bootloader/bootloader.asm
; NASM syntax

BITS 16                  ; Bootloader operates in 16-bit real mode
ORG 0x7C00               ; BIOS loads the bootloader at address 0x7C00

start:
    cli                  ; Disable interrupts
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Set up the stack
    mov ax, stack_segment
    mov ss, ax
    mov sp, stack_pointer

    ; Display message: "Hello from bootloader"
    mov si, msg           ; Point SI to the message string
    call print_string     ; Call the print_string routine

    ; Load kernel (assume it's located at sector 2 onwards)
    mov ax, 0x1000          ; ES = 0x1000
    mov es, ax
    mov bx, 0x0000          ; BX = 0x0000
    mov cx, KERNEL_SECTORS  ; Number of sectors to load
    call load_kernel

    ; Switch to protected mode and jump to the kernel
    call enter_protected_mode

    ; If protected mode fails, hang
hang:
    jmp hang

; Function to load the kernel from disk
; Parameters:
;   ES:BX = destination (memory address to load the kernel)
;   CX = number of sectors to load
load_kernel:
    mov ah, 0x02            ; BIOS read sectors function
    mov al, cl              ; Number of sectors to read
    mov ch, 0               ; Cylinder number
    mov cl, 2               ; Starting sector (sector 2)
    mov dh, 0               ; Head number
    mov dl, 0x80            ; Drive number (first HDD)
    mov bx, 0x0000          ; BX = 0x0000 (offset within ES)
    int 0x13                ; BIOS interrupt to read sectors
    jc load_error           ; Jump if carry flag is set (error)
    ret

load_error:
    ; If there's an error loading the kernel, hang
    jmp hang

; Function to switch to protected mode
enter_protected_mode:
    cli
    lgdt [gdt_descriptor]    ; Load GDT
    mov eax, cr0
    or eax, 0x1              ; Set PE bit to enable protected mode
    mov cr0, eax
    jmp 0x08:protected_mode_jump ; Far jump to flush the pipeline

protected_mode_jump:
    ; Now in protected mode
    jmp 0x08:0x0000          ; Jump to the kernel's entry point at 0x100000

; Function to print a null-terminated string
; Input:
;   SI = pointer to the string
print_string:
    pusha                   ; Save all general-purpose registers
.print_loop:
    lodsb                   ; Load byte at DS:SI into AL and increment SI
    cmp al, 0               ; Check for null terminator
    je .print_done           ; If null, end of string
    mov ah, 0x0E            ; BIOS teletype function
    mov bh, 0x00            ; Display page number
    mov bl, 0x07            ; Text attribute (light grey on black)
    int 0x10                ; BIOS video interrupt
    jmp .print_loop
.print_done:
    popa                    ; Restore all general-purpose registers
    ret

; GDT setup
gdt_start:
    dq 0x0000000000000000     ; Null descriptor
    dq 0x00AF9A100000FFFF     ; Code segment: base=0x100000, limit=0xFFFFF, granularity=4KB, 32-bit
    dq 0x00AF9210000000FFFF    ; Data segment: base=0x100000, limit=0xFFFFF, granularity=4KB, 32-bit
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; Limit
    dq gdt_start               ; Base

; Stack setup
stack_segment equ 0x7000
stack_pointer equ 0x8000        ; Initialize stack pointer

; Kernel parameters
KERNEL_LOAD_ADDRESS equ 0x100000 ; Load kernel at 1MB
KERNEL_SECTORS equ 10             ; Number of sectors to load (adjust as needed)

; Message to display
msg db "Hello from bootloader", 0

; Boot signature
times 510 - ($ - $$) db 0
dw 0xAA55
