#ifndef GDT_H
#define GDT_H

#include <stdint.h>

struct gdt_ptr_struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void gdt_install(void);
extern void gdt_flush(struct gdt_ptr_struct*);

#endif // GDT_H
