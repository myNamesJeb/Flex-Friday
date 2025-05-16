/* src/shell.c */
#include "stdio.h"
#include "fs/fs.h"
#include "drivers/keyboard.h"
#include "string.h"
#include "kernel/editor.h"
#include "drivers/clock.h"
#include "stddef.h"

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
        else if (strcmp(cmd, "ls") == 0) {
            // No argument => list the current directory
            char path[128];
            fs_get_path(cwd, path, sizeof(path));
            fs_ls(path);
        }
        else if (strncmp(cmd, "ls ", 3) == 0) {
            // "ls <dir>" => parse <dir>, then resolve relative to cwd
            char *dir = cmd + 3;
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
        else {
            // Attempt to load a command from /cmd/<command>
            char full_path[128];
            strcpy(full_path, "/cmd/");
            strncat(full_path, cmd, sizeof(full_path) - strlen(full_path) - 1);
            // Assume load_and_execute() is implemented (see loader.c example)
            printf("Attempting to execute %s...\n", full_path);
            load_and_execute(full_path);
        }
    }
}
