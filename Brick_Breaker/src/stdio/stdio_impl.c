#include "stdio.h"

// ADDED: Stub stream pointers
FILE* stdin = NULL;
FILE* stdout = NULL;
FILE* stderr = NULL;

// ADDED: fprintf
int fprintf(FILE* stream, const char* format, ...)
{
    (void)stream;
    (void)format;
    return 0;
}

// ADDED: sprintf (stub)
int sprintf(char* str, const char* format, ...)
{
    (void)str;
    (void)format;
    return 0;
}

// ADDED: snprintf (stub)
int snprintf(char* str, size_t size, const char* format, ...)
{
    (void)str;
    (void)size;
    (void)format;
    return 0;
}

// ADDED: vsnprintf (minimal)
int vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
    (void)str;
    (void)size;
    (void)format;
    (void)ap;
    return 0;
}

