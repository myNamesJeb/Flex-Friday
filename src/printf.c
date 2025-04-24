/* src/printf.c */
#include "stdio.h"
#include "stdarg.h"
#include "io.h"  // we'll define an outb() in io.h (see below)

#define VIDEO_MEMORY ((volatile char*)0xb8000)
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

static int cursor_pos = 0;

static void update_cursor() {
    // The hardware cursor position is given by cursor_pos.
    unsigned short pos = cursor_pos;
    outb(0x3D4, 14);
    outb(0x3D5, pos >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, pos & 0xFF);
}

static void putc(char c) {
    if (c == '\n') {
        int col = cursor_pos % SCREEN_WIDTH;
        cursor_pos += (SCREEN_WIDTH - col);
    } else {
        VIDEO_MEMORY[cursor_pos * 2] = c;
        VIDEO_MEMORY[cursor_pos * 2 + 1] = 0x07;  // light grey on black
        cursor_pos++;
    }
    update_cursor();
}

static void printstr(const char *str) {
    while (*str) {
        putc(*str++);
    }
}

void printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    for (const char *p = format; *p != '\0'; p++) {
        if (*p == '%') {
            p++;
            if (*p == 'x') {
                unsigned int num = va_arg(args, unsigned int);
                char buf[9];
                buf[8] = '\0';
                for (int i = 7; i >= 0; --i) {
                    int digit = num & 0xF;
                    buf[i] = digit < 10 ? '0' + digit : 'A' + (digit - 10);
                    num >>= 4;
                }
                printstr(buf);
            }
            else if (*p == 'd') {
                int num = va_arg(args, int);
                char buf[12]; // enough for signed 32-bit
                int i = 0;
                int is_negative = 0;
                if(num < 0) {
                    is_negative = 1;
                    num = -num;
                }
                // Convert number to string (in reverse)
                do {
                    buf[i++] = '0' + (num % 10);
                    num /= 10;
                } while(num > 0);
                if(is_negative)
                    buf[i++] = '-';
                buf[i] = '\0';
                // Reverse string in place
                for (int j = 0; j < i/2; j++) {
                    char temp = buf[j];
                    buf[j] = buf[i-1-j];
                    buf[i-1-j] = temp;
                }
                printstr(buf);
            }
            else if (*p == 's') {
                char *str = va_arg(args, char*);
                printstr(str);
            }
            else if (*p == 'c') {
                char c = (char)va_arg(args, int);
                putc(c);
            } else {
                putc(*p);
            }
        } else {
            putc(*p);
        }
    }
    
    va_end(args);
}
