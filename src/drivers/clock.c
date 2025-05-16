/* src/clock.c */
#include "drivers/clock.h"
#include "lib/stdio.h"

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static unsigned char read_cmos(unsigned char reg) {
    outb(0x70, reg);
    return inb(0x71);
}

static int bcd_to_bin(int val) {
    return (val & 0x0F) + ((val >> 4) * 10);
}

Time get_rtc_time() {
    Time t;
    t.second = bcd_to_bin(read_cmos(0x00));
    t.minute = bcd_to_bin(read_cmos(0x02));
    t.hour   = bcd_to_bin(read_cmos(0x04));
    return t;
}

void print_time() {
    Time t = get_rtc_time();
    printf("Time: %02d:%02d:%02d\n", t.hour, t.minute, t.second);
}
