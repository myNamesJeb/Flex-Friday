/* src/editor.c */
#include "editor.h"
#include "stdio.h"
#include "fs.h"
#include "string.h"

void edit_file(const char* path) {
    FSNode* node = fs_find(path);
    if (!node) {
        if (fs_create_file(path) < 0) {
            printf("Error: Could not create file %s\n", path);
            return;
        }
        node = fs_find(path);
    }
    if (node->type != FS_NODE_FILE) {
        printf("%s is not a file.\n", path);
        return;
    }
    printf("Editing file: %s\n", path);
    printf("Enter text. End with a line containing only a single dot (.)\n");
    char line[128];
    char buffer[MAX_FILE_SIZE];
    int pos = 0;
    buffer[0] = '\0';
    while (1) {
        printf("> ");
        // Re-use shell's read_line; assume it's externally defined
        extern void read_line(char *buffer, int max_length);
        read_line(line, 128);
        if (strcmp(line, ".") == 0)
            break;
        int len = strlen(line);
        if (pos + len + 1 >= MAX_FILE_SIZE) {
            printf("File too long, truncating.\n");
            break;
        }
        strcpy(buffer + pos, line);
        pos += len;
        buffer[pos++] = '\n';
        buffer[pos] = '\0';
    }
    fs_write(path, buffer);
    printf("File saved.\n");
}
