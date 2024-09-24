// kernel.c
#include "kernel.h"
#include "gdt.h"
#include "paging.h"
#include "filesystem.h"

// Function prototypes
void print_string(const char* str);

// Entry point called by bootloader
void kernel_main() {
    print_string("hello from uninitialized kernel");
    // Initialize GDT
    gdt_init();

    // Initialize Paging
    paging_init();

    // Initialize Filesystem
    fs_init();

    // Print message to screen
    print_string("Hello From The Kernel!\n");

    // Create hello.txt and write "hello"
    fs_create_file("hello.txt", "hello");

    // Read and display the contents of hello.txt
    char buffer[100];
    fs_read_file("hello.txt", buffer);
    print_string("Contents of hello.txt: ");
    print_string(buffer);
    
    while (1) { /* Halt */ }
}

// Simple VGA text mode memory address
volatile unsigned char* vga = (unsigned char*)0xB8000;
unsigned int cursor_pos = 0;

// Function to print a string to the screen
void print_string(const char* str) {
    while(*str) {
        if(*str == '\n') {
            cursor_pos += 80 - (cursor_pos % 80);
        } else {
            vga[cursor_pos++] = *str++;
            vga[cursor_pos++] = 0x07; // Light grey color
        }
        if(cursor_pos >= 80 * 25) cursor_pos = 0; // Wrap around
    }
}
