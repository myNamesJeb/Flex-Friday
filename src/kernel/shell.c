/* src/shell.c */
#include "lib/stdio.h"
#include "fs/fs.h"
#include "drivers/keyboard.h"
#include "lib/string.h"
#include "kernel/editor.h"
#include "drivers/clock.h"
#include "lib/stddef.h"

// Add prototype for load_and_execute:
extern void load_and_execute(const char *path);

#define MAX_LINE 128
static FSNode *cwd = NULL; // pointer to the current directory node
/* somewhere global, e.g. shell.c */
#include "fs/fs.h"
void shell_init() {
    fs_init();      // your existing FS init
    cwd = fs_find("/home");  // or fs_find("/")
    if (!cwd) {
        cwd = fs_root; // fallback if "/home" wasn't found
    }
}


// Read a line from the keyboard (echoing input with backspace support)
void read_line(char *buffer, int max_length) {
    int i = 0;
    char c;
    while (i < max_length - 1) {
        c = keyboard_getc();
        if (c == '\n') {
            buffer[i] = '\0';
            printf("\n");
            return;
        } else if (c == '\b') {
            if (i > 0) {
                i--;
                buffer[i] = '\0';
                // Erase from screen
                printf("\b \b");
            }
        } else {
            buffer[i++] = c;
            buffer[i] = '\0';
            printf("%c", c);
        }
    }
    buffer[i] = '\0';
}

/* Updated shell_run in shell.c */
void shell_run() {
    char line[MAX_LINE];
    shell_init(); // initialize FS & set cwd

    // Initialize the filesystem if not already done
    while (1) {

        char cwd_path[128];
        fs_get_path(cwd, cwd_path, sizeof(cwd_path)); 
        printf("%s> ", cwd_path);

        read_line(line, MAX_LINE);
   
        
        // Skip leading spaces
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (strncmp(line, "cd ", 3) == 0) {
            char *arg = line + 3;
            FSNode *target = fs_resolve(cwd, arg);
            if (!target) {
                printf("No such directory: %s\n", arg);
            } else if (target->type != FS_NODE_DIR) {
                printf("%s is not a directory\n", arg);
            } else {
                cwd = target;
            }
        }
        else if (strcmp(line, "cd") == 0) {
            // cd with no args => go home or root
            FSNode *home = fs_find("/home");
            if (home) cwd = home; 
            else cwd = fs_root;
        }
        else if (strncmp(cmd, "ls", 2) == 0) {
            char *dir = cmd + 2;
            while (*dir == ' ') dir++;
            if (*dir == '\0') {
                char path[128];
                fs_get_path(cwd, path, sizeof(path));
                fs_ls(path);
            } else {
                FSNode *target = fs_resolve(cwd, dir);
                if (!target) {
                    printf("No such directory: %s\n", dir);
                } else if (target->type != FS_NODE_DIR) {
                    printf("%s is not a directory\n", dir);
                } else {
                    char full_path[128];
                    fs_get_path(target, full_path, sizeof(full_path));
                    fs_ls(full_path);
                }
            }
        }
        else if (strncmp(cmd, "mkdir ", 6) == 0) {
            char *dir = cmd + 6;
            if (fs_create_dir(dir) == 0)
                printf("Directory created: %s\n", dir);
            else
                printf("Error: Could not create directory %s\n", dir);
        }
        else if (strncmp(cmd, "cd ", 3) == 0) {
            char *arg = cmd + 3;
            FSNode *target = fs_resolve(cwd, arg);
            if (!target) {
                printf("No such directory: %s\n", arg);
            } else if (target->type != FS_NODE_DIR) {
                printf("%s is not a directory\n", arg);
            } else {
                cwd = target;
            }
        }
        else if (strcmp(cmd, "cd") == 0) {
            FSNode *home = fs_find("/home");
            if (home) cwd = home; 
            else cwd = fs_root;
        }
        else if (strncmp(cmd, "rm ", 3) == 0) {
            char *file = cmd + 3;
            if (fs_rm(file) == 0)
                printf("Removed: %s\n", file);
            else
                printf("Error: Could not remove %s\n", file);
        }
        else if (strncmp(cmd, "mv ", 3) == 0) {
            char *args = cmd + 3;
            char *src = strtok(args, " ");
            char *dst = strtok(NULL, " ");
            if (!src || !dst) {
                printf("Usage: mv <src> <dst>\n");
            } else if (fs_mv(src, dst) == 0) {
                printf("Moved %s to %s\n", src, dst);
            } else {
                printf("Error: Could not move %s to %s\n", src, dst);
            }
        }
        else if (strcmp(cmd, "makefile") == 0) {
            if (fs_create_file("/home/Makefile") == 0) {
                fs_write("/home/Makefile", "all:\n\tgcc main.c -o main\n");
                printf("Created /home/Makefile\n");
            } else {
                printf("Could not create Makefile\n");
            }
        }
        else if (strncmp(cmd, "cat ", 4) == 0) {
            char *file = cmd + 4;
            char resolved[128];
            // If file is relative, resolve against cwd; otherwise use as-is.
            if (file[0] != '/') {
                fs_get_path(cwd, resolved, sizeof(resolved));
                if (strcmp(resolved, "/") != 0) {
                    strncat(resolved, "/", sizeof(resolved) - strlen(resolved) - 1);
                }
                strncat(resolved, file, sizeof(resolved) - strlen(resolved) - 1);
                fs_cat(resolved);
            } else {
                fs_cat(file);
            }
        }
        else if (strncmp(cmd, "touch ", 6) == 0) {
            char *file = cmd + 6;
            if (fs_create_file(file) == 0)
                printf("File created: %s\n", file);
            else
                printf("Error: Could not create file %s\n", file);
        }
        else if (strncmp(cmd, "edit ", 5) == 0) {
            char *file = cmd + 5;
            edit_file(file);
        }
        else if (strcmp(cmd, "clock") == 0) {
            print_time();
        }
        else if (strcmp(cmd, "boot") == 0) {
            printf("Bootstrapping OS from /boot/kernel.bin...\n");
            fs_cat("/boot/kernel.bin");
        }
        else if (strcmp(cmd, "") == 0) {
            // Do nothing on empty input
        }
        else if (strncmp(cmd, "hexwrite ", 9) == 0) {
            // Usage: hexwrite <file> <hex-bytes>
            char *args = cmd + 9;
            // Find first space (file name ends)
            char *space = strchr(args, ' ');
            if (!space) {
                printf("Usage: hexwrite <file> <hex-bytes>\n");
            } else {
                *space = '\0';
                char *file = args;
                char *hexstr = space + 1;
                // Parse hex string into bytes
                int hexlen = strlen(hexstr) / 2;
                char buf[256];
                int i;
                for (i = 0; i < hexlen && i < (int)sizeof(buf); i++) {
                    unsigned int byte;
                    if (sscanf(hexstr + 2*i, "%2x", &byte) != 1) {
                        printf("Invalid hex at position %d\n", 2*i);
                        break;
                    }
                    buf[i] = (char)byte;
                }
                if (i > 0) {
                    if (fs_write(file, buf) == 0)
                        printf("Wrote %d bytes to %s\n", i, file);
                    else
                        printf("Error: Could not write to %s\n", file);
                }
            }
        }
        else if (strncmp(cmd, "exec ", 5) == 0) {
            char *file = cmd + 5;
            char resolved[128];
            if (file[0] != '/') {
                fs_get_path(cwd, resolved, sizeof(resolved));
                if (strcmp(resolved, "/") != 0) {
                    strncat(resolved, "/", sizeof(resolved) - strlen(resolved) - 1);
                }
                strncat(resolved, file, sizeof(resolved) - strlen(resolved) - 1);
                load_and_execute(resolved);
            } else {
                load_and_execute(file);
            }
        }
        else {
            printf("Unknown command: %s\n", cmd);
        }
    }
}
