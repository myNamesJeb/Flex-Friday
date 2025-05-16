#include "drivers/disk.h"
#include "fs/io.h"

static void ata_wait() {
    // Wait for BSY bit to clear
    while(inb(0x1F7) & 0x80);
}

int ata_read(uint32_t lba, uint8_t count, uint8_t *buffer) {
    ata_wait();
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F) | 0); // drive 0 (master)
    outb(0x1F2, count);
    outb(0x1F3, (lba & 0xFF));
    outb(0x1F4, (lba >> 8) & 0xFF);
    outb(0x1F5, (lba >> 16) & 0xFF);
    outb(0x1F7, 0x20); // Read command

    for (int i = 0; i < count; i++) {
        ata_wait();
        for (int j = 0; j < SECTOR_SIZE / 2; j++) {
            uint16_t data = 0;
            __asm__ volatile ("inw %%dx, %%ax" : "=a"(data) : "d"(0x1F0));
            ((uint16_t*)buffer)[i * (SECTOR_SIZE/2) + j] = data;
        }
    }
    return 0;
}