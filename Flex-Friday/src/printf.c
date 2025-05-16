/* src/printf.c */
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "screen.h"  // use our scrolling routines

static void printstr(const char *str) {
    while (*str) {
        put_character(*str++);
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
                    for (int i = 0; i < num_pad; i++) {
                        put_character(pad);
                    }
                    printstr(buf);
                    break;
                }
                case 'd': {
                    int num = va_arg(args, int);
                    char buf[12];
                    int i = 0;
                    int is_negative = 0;
                    if (num < 0) {
                        is_negative = 1;
                        num = -num;
                    }
                    do {
                        buf[i++] = '0' + (num % 10);
                        num /= 10;
                    } while (num > 0);
                    if (is_negative)
                        buf[i++] = '-';
                    buf[i] = '\0';
                    for (int j = 0; j < i/2; j++) {
                        char tmp = buf[j];
                        buf[j] = buf[i - 1 - j];
                        buf[i - 1 - j] = tmp;
                    }
                    int len = strlen(buf);
                    int num_pad = (width > len) ? width - len : 0;
                    for (int i = 0; i < num_pad; i++) {
                        put_character(pad);
                    }
                    printstr(buf);
                    break;
                }
                case 's': {
                    char *s = va_arg(args, char *);
                    printstr(s);
                    break;
                }                
                case 'c': {
                    put_character((char)va_arg(args, int));
                    break;
                }
                default:
                    put_character(*p);
                    break;
            }
        } else {
            put_character(*p);
        }
    }
    
    va_end(args);
}
