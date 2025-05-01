/* src/printf.c */
#include "stdio.h"
#include "stdarg.h"
#include "io.h"  // we'll define an outb() in io.h (see below)
#include "string.h"

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
            p++; // move past '%'
            int width = 0;
            char pad = ' '; 
            if (*p == '0') {
                pad = '0';
                p++;
            }
            while (*p >= '0' && *p <= '9') {
                width = width * 10 + (*p - '0');
                p++;
            }
            switch(*p) {
                case 'x': {
                    unsigned int num = va_arg(args, unsigned int);
                    char buf[9];
                    buf[8] = '\0';
                    unsigned int temp = num;
                    for (int i = 7; i >= 0; i--) {
                        int digit = temp & 0xF;
                        buf[i] = (digit < 10) ? '0' + digit : 'A' + (digit - 10);
                        temp >>= 4;
                    }
                    int len = strlen(buf);
                    int num_pad = (width > len) ? width - len : 0;
                    for (int i = 0; i < num_pad; i++)
                        putc(pad);
                    printstr(buf);
                    break;
                }
                case 'd': {
                    int num = va_arg(args, int);
                    char buf[12];
                    int i = 0;
                    int is_negative = 0;
                    if(num < 0) {
                        is_negative = 1;
                        num = -num;
                    }
                    do {
                        buf[i++] = '0' + (num % 10);
                        num /= 10;
                    } while(num > 0);
                    if(is_negative)
                        buf[i++] = '-';
                    buf[i] = '\0';
                    for (int j = 0; j < i/2; j++) {
                        char tmp = buf[j];
                        buf[j] = buf[i - 1 - j];
                        buf[i - 1 - j] = tmp;
                    }
                    int len = strlen(buf);
                    int num_pad = (width > len) ? width - len : 0;
                    for (int i = 0; i < num_pad; i++)
                        putc(pad);
                    printstr(buf);
                    break;
                }
                case 's':
                    printstr(va_arg(args, char *));
                    break;
                case 'c':
                    putc((char)va_arg(args, int));
                    break;
                default:
                    putc(*p);
                    break;
            }
        } else {
            putc(*p);
        }
    }
    
    va_end(args);
}
