/* src/stdio.h */
#ifndef STDIO_H
#define STDIO_H

void printf(const char *format, ...);
int my_sscanf(const char *str, const char *fmt, unsigned int *out);
#define sscanf my_sscanf

#endif
