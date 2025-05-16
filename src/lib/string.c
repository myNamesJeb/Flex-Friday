#include "lib/string.h"
#include <stddef.h> // Add this line

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

void *memset(void *s, int c, size_t n) {
    unsigned char *p = s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

// Minimal strchr implementation
char *strchr(const char *s, int c) {
    while (*s) {
        if (*s == (char)c)
            return (char *)s;
        s++;
    }
    return NULL;
}

// Minimal strtok implementation (not thread-safe, but fine for your shell)
char *strtok(char *str, const char *delim) {
    static char *next;
    if (str)
        next = str;
    if (!next)
        return NULL;
    // Skip leading delimiters
    while (*next && strchr(delim, *next))
        next++;
    if (!*next)
        return NULL;
    char *start = next;
    // Find end of token
    while (*next && !strchr(delim, *next))
        next++;
    if (*next) {
        *next = '\0';
        next++;
    } else {
        next = NULL;
    }
    return start;
}
