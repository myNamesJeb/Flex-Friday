#include "lib/stdio.h"
#include "fs/fs.h"

int touch(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: touch <file>\n");
        return 1;
    }
    if (fs_create_file(argv[1]) == 0)
        printf("File created: %s\n", argv[1]);
    else
        printf("Error: Could not create file %s\n", argv[1]);
    return 0;
}