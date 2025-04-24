#ifndef DISK_H
#define DISK_H

#include <stdint.h>

#define SECTOR_SIZE 512

// Reads 'count' sectors starting at LBA 'lba' into 'buffer'.
// Returns 0 on success.
int ata_read(uint32_t lba, uint8_t count, uint8_t *buffer);

#endif