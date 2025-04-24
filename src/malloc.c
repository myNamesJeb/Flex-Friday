#include "malloc.h"
#include "stdint.h"
#include "stddef.h"

// Define your heap boundaries. Adjust the addresses based on your linker script!
#define HEAP_START 0x01000000  // e.g., 16MB (change as needed)
#define HEAP_SIZE  0x00100000  // e.g., 1MB heap size

static uintptr_t heap_current = HEAP_START;
static uintptr_t heap_limit = HEAP_START + HEAP_SIZE;

void *malloc(unsigned int size) {
    // Align size to 4 bytes
    size = (size + 3) & ~3;
    if (heap_current + size > heap_limit) {
        return NULL;  // Out of memory
    }
    void *ptr = (void *)heap_current;
    heap_current += size;
    return ptr;
}

void free(void *ptr) {
    // For now free is not implemented.
}