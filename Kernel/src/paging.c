// paging.c

#include "paging.h"

// Define the page directory and page tables
uint64_t page_directory[512] __attribute__((aligned(4096)));
uint64_t page_tables[512][512] __attribute__((aligned(4096)));

void paging_init() {
    // Initialize page tables with identity mapping
    for(int i = 0; i < 512; i++) {
        for(int j = 0; j < 512; j++) {
            page_tables[i][j] = (j * 0x1000) | 0x3; // Present | Read/Write
        }
        page_directory[i] = ((uint64_t)page_tables[i]) | 0x3; // Present | Read/Write
    }

    // Load the page directory
    paging_load((uint64_t)page_directory);
}
