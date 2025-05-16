#ifndef STRING_H
#define STRING_H

#include <stddef.h> // Add this line

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, int n);
void strcpy(char *dest, const char *src);
int strlen(const char *s);
char *strncpy(char *dest, const char *src, int n);
char *strncat(char *dest, const char *src, int n);  // <-- Add this prototype
void *memset(void *s, int c, size_t n); // Add this line

#endif
