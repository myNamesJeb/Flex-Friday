#ifndef DISK_FS_H
#define DISK_FS_H

#include <stdint.h>

// Initialize the disk filesystem (reads structures from disk)
void disk_fs_init();

// Reads the file with given path into buffer; returns file size or <0 on error
int disk_fs_read_file(const char *path, uint8_t *buffer, int max_size);

#endif