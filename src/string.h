#ifndef STRING_H
#define STRING_H

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, int n);
void strcpy(char *dest, const char *src);
int strlen(const char *s);
char *strncpy(char *dest, const char *src, int n);
char *strncat(char *dest, const char *src, int n);  // <-- Add this prototype

#endif
