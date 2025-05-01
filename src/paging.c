#include <stdint.h>
#include <string.h>

uint32_t *page_directory;    // Define page_directory
uint32_t *first_page_table;  // Define first_page_table

void paging_enable() {
    asm volatile("mov %0, %%cr3" : : "r"(page_directory));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging
    asm volatile("mov %0, %%cr0" : : "r"(cr0));
}

void paging_install() {
    page_directory = (uint32_t *)0x9C000; // Example address
    first_page_table = (uint32_t *)0x9D000;

    memset(page_directory, 0, 4096);
    memset(first_page_table, 0, 4096);

    for (int i = 0; i < 1024; i++) {
        first_page_table[i] = (i * 0x1000) | 3; // Present, RW
    }

    page_directory[0] = ((uint32_t)first_page_table) | 3; // Present, RW

    paging_enable();
}
