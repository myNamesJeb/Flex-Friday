#include "stdio.h"
#include "fs/fs.h"
#include "stddef.h"

typedef void (*entry_fn_t)(void);

void load_and_execute(const char *path) {
    // Load file into memory at 0x200000
    char *load_addr = (char*)0x200000;
    int bytes = fs_read(path, load_addr, 4096); // up to 4KB
    if (bytes <= 0) {
        printf("Could not load %s\n", path);
        return;
    }
    printf("Loaded %s (%d bytes) to 0x200000, jumping...\n", path, bytes);
    entry_fn_t entry = (entry_fn_t)0x200000;
    entry(); // Jump to loaded code
}