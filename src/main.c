/* src/main.c */
#include "malloc.h"
#include "stdio.h"
#include "stddef.h"
#include "gdt.h"  // Include for gdt_install
#include "idt.h"  // Include for idt_install
#include "string.h" // Include for memset

extern void shell_run();

// Function to get the value of DS using inline assembly
unsigned short get_ds(void) {
    unsigned short ds_val;
    asm volatile("mov %%ds, %0" : "=r"(ds_val));
    return ds_val;
}

int kmain(void) {
    gdt_install(); // Call GDT installation
    idt_install(); // Changed from idt_init to match implementation
    __asm__("sti");  // enable interrupts

    unsigned short ds_val = get_ds();
    printf("DS register: 0x%x\n", ds_val);
    if(ds_val == 0x10)
        printf("GDT loaded correctly!\n");
    else
        printf("GDT might not be working properly. DS is not 0x10!\n");

    int *ptr = (int *)malloc(sizeof(int) * 10);
    if (ptr == NULL) {
        printf("Out of heap memory!\n");
    } else {
        // Use the allocated memory
        for (int i = 0; i < 10; i++) {
            ptr[i] = i;
        }
        // Later print values, etc.
    }

    shell_run();
    while (1) { }
    return 0;
}



