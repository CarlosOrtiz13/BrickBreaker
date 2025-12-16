#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include "../memory/memory.h"  // ADDED: For memmove
#include "../stdlib/stdlib.h"  // ADDED: for malloc
#include "../memory/heap/kheap.h"  // ADDED: for actual heap allocation

// ADDED: Your existing functions
int strlen(const char* ptr);
int strnlen(const char* ptr, int max);
int strnlen_terminator(const char* str, int max, char terminator);
int istrncmp(const char* s1, const char* s2, int n);
int strncmp(const char* str1, const char* str2, int n);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, int count);

// ADDED: Missing functions Doom needs
int strcmp(const char* str1, const char* str2);
int strcasecmp(const char* s1, const char* s2);
int strncasecmp(const char* s1, const char* s2, size_t n);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t n);
char* strchr(const char* s, int c);
char* strrchr(const char* s, int c);
char* strstr(const char* haystack, const char* needle);
char* strdup(const char* s);
size_t strspn(const char* s, const char* accept);
size_t strcspn(const char* s, const char* reject);

#endif