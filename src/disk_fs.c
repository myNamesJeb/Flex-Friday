#include "disk_fs.h"
#include "disk.h"
#include "string.h"
#include "stdio.h"

// Example on‑disk file table entry structure:
typedef struct {
    char name[32];
    uint32_t start_lba;
    uint32_t sector_count;
} FileEntry;

// Assume 8 entries stored in sector 100 to 101 (for example)
#define FILE_TABLE_LBA 100
#define MAX_FILES 8

static FileEntry file_table[MAX_FILES];

void disk_fs_init() {
    // Read file table from disk
    ata_read(FILE_TABLE_LBA, 1, (uint8_t*)file_table);
}

int disk_fs_read_file(const char *path, uint8_t *buffer, int max_size) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(file_table[i].name, path) == 0) {
            int total_bytes = file_table[i].sector_count * SECTOR_SIZE;
            if (total_bytes > max_size)
                total_bytes = max_size;
            ata_read(file_table[i].start_lba, file_table[i].sector_count, buffer);
            return total_bytes;
        }
    }
    printf("disk_fs: File not found: %s\n", path);
    return -1;
}