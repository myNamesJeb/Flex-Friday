#include "arch/gdt.h"
#include "arch/idt.h"
#include "arch/paging.h"
#include "string.h" // Include for memset

void kernel_main() {
    idt_install();
    paging_install();

    // Additional kernel initialization code can be added here.
}