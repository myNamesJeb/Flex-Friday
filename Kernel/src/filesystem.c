// filesystem.c
#include "filesystem.h"
#include "kernel.h"

// Simple in-memory filesystem for demonstration
typedef struct {
    char filename[12];
    char content[100];
} file_t;

#define MAX_FILES 10
file_t filesystem[MAX_FILES];
int file_count = 0;

// Initialize filesystem (empty)
void fs_init() {
    file_count = 0;
}

// Create a file with given name and content
int fs_create_file(const char* name, const char* content) {
    if (file_count >= MAX_FILES) return -1;
    // Copy filename
    for(int i = 0; i < 12 && name[i] != '\0'; i++) {
        filesystem[file_count].filename[i] = name[i];
    }
    // Copy content
    for(int i = 0; i < 100 && content[i] != '\0'; i++) {
        filesystem[file_count].content[i] = content[i];
    }
    file_count++;
    return 0;
}

// Read a file's content into buffer
int fs_read_file(const char* name, char* buffer) {
    for(int i = 0; i < file_count; i++) {
        int match = 1;
        for(int j = 0; j < 12; j++) {
            if (filesystem[i].filename[j] != name[j]) {
                match = 0;
                break;
            }
            if (name[j] == '\0') break;
        }
        if (match) {
            // Copy content to buffer
            for(int j = 0; j < 100; j++) {
                buffer[j] = filesystem[i].content[j];
                if (filesystem[i].content[j] == '\0') break;
            }
            return 0;
        }
    }
    return -1; // File not found
}
