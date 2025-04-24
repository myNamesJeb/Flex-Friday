#include "string.h"

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

int strncmp(const char *s1, const char *s2, int n) {
    for (int i = 0; i < n; i++) {
         if (s1[i] != s2[i])
             return (unsigned char)s1[i] - (unsigned char)s2[i];
         if (s1[i] == '\0')
             return 0;
    }
    return 0;
}

void strcpy(char *dest, const char *src) {
    while ((*dest++ = *src++));
}

int strlen(const char *s) {
    int len = 0;
    while (s[len])
        len++;
    return len;
}

char *strncpy(char *dest, const char *src, int n) {
    int i = 0;
    for (; i < n && src[i] != '\0'; i++) {
         dest[i] = src[i];
    }
    for (; i < n; i++) {
         dest[i] = '\0';
    }
    return dest;
}

/* Custom implementation of strncat */
char *strncat(char *dest, const char *src, int n) {
    int dest_len = strlen(dest);
    int i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[dest_len + i] = src[i];
    }
    dest[dest_len + i] = '\0';
    return dest;
}
