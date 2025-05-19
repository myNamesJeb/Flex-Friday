#include "lib/stdio.h"
#include "fs/fs.h"

int mkdir(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: mkdir <dir>\n");
        return 1;
    }
    if (fs_create_dir(argv[1]) == 0)
        printf("Directory created: %s\n", argv[1]);
    else
        printf("Error: Could not create directory %s\n", argv[1]);
    return 0;
}