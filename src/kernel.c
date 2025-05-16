#include "gdt.h"
#include "idt.h"
#include "paging.h"
#include "string.h" // Include for memset

void kernel_main() {
    idt_install();
    paging_install();

    // Additional kernel initialization code can be added here.
}