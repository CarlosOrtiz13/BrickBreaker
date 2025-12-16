#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>
#include <stdarg.h>

typedef struct { int unused; } FILE;
#define EOF (-1)

int putchar(int c);
int printf(const char* fmt, ...);

// ADDED: Standard streams (stubs)
extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

// ADDED: Function declarations (implementations in stdio_impl.c)
int putchar(int c);
int printf(const char* fmt, ...);
int fprintf(FILE* f, const char* fmt, ...);
int sprintf(char* str, const char* fmt, ...);
int snprintf(char* str, size_t sz, const char* fmt, ...);
int vsnprintf(char* str, size_t size, const char* format, va_list ap);

#endif

// ADDED: Seek modes (if not already present)
#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif