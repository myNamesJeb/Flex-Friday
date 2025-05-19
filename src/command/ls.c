#include "lib/stdio.h"
#include "fs/fs.h"
#include "lib/string.h"
#include "lib/stdio.h"
int ls(int argc, char **argv) {
    const char *path = (argc > 1) ? argv[1] : "/";
    fs_ls(path);
    printf("ITS WORKING");
    return 0;
}