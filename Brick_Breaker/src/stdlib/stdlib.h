#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

// Use the string.h version of itoa
void itoa(int value, char* buffer);

void* malloc(size_t size);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);
int atoi(const char* str);
long atol(const char* str);

static inline void exit(int status) { while(1); }
static inline void abort(void) { while(1); }

extern unsigned int __rand_seed;
static inline int rand(void) { 
    __rand_seed = __rand_seed * 1103515245 + 12345;
    return (__rand_seed >> 16) & 0x7FFF;
}
static inline void srand(unsigned int seed) { __rand_seed = seed; }

int abs(int n);
int sscanf(const char* str, const char* format, ...);
double atof(const char* str);
int mkdir(const char* path, int mode);

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#endif
