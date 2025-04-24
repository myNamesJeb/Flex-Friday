#include "idt.h"
#include "io.h"  // outb() already defined there

#define IDT_ENTRIES 256

struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idtp;

extern void idt_flush(uint32_t);
extern void keyboard_handler_stub();  // assembly stub
extern void timer_handler_stub();  // declared in timer.asm

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].offset_low  = base & 0xFFFF;
    idt[num].selector    = sel;
    idt[num].zero        = 0;
    idt[num].type_attr   = flags;
    idt[num].offset_high = (base >> 16) & 0xFFFF;
}

static void pic_remap() {
    /* Initialize command ports (ICW1) */
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    /* Set vector offsets (ICW2): 0x20 for master, 0x28 for slave */
    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    /* Tell Master PIC that there is a slave PIC at IRQ2 (ICW3) */
    outb(0x21, 0x04);
    /* Tell Slave PIC its cascade identity (ICW3) */
    outb(0xA1, 0x02);

    /* Set PICs to 8086/88 (MCS-80/85) mode (ICW4) */
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    /* Disable all interrupts on the PICs (optional) */
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

void idt_init(void) {
    idtp.limit = sizeof(struct idt_entry) * IDT_ENTRIES - 1;
    idtp.base  = (uint32_t)&idt;
    
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    
    pic_remap();

    // Install timer interrupt (IRQ0, vector 32)
    idt_set_gate(32, (uint32_t)timer_handler_stub, 0x08, 0x8E);
    
    // Install keyboard interrupt (IRQ1, vector 33)
    idt_set_gate(33, (uint32_t)keyboard_handler_stub, 0x08, 0x8E);
    
    idt_flush((uint32_t)&idtp);
}