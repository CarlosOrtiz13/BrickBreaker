#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

void* memset(void* ptr, int c, size_t size);
int memcmp(void* s1, void* s2, int count);
void* memcpy(void* dest, const void* src, size_t len);  // FIXED: size_t
void* memmove(void* dest, const void* src, size_t n);  // ADDED: Move memory (handles overlap)

#endif