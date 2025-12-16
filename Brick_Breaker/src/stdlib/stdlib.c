#include "stdlib.h"
#include "../memory/heap/kheap.h"
#include <stddef.h>


void* malloc(size_t size)
{
    return kzalloc(size);
}

void free(void* ptr)
{
    kfree(ptr);
}

void* calloc(size_t nmemb, size_t size)
{
    size_t total = nmemb * size;
    void* ptr = kzalloc(total);
    return ptr;
}

void* realloc(void* ptr, size_t size)
{
    if (!ptr) return kzalloc(size);
    if (size == 0) { 
        kfree(ptr);
        return NULL;
    }
    
    void* new_ptr = kzalloc(size);
    if (!new_ptr) return NULL;
    
    char* src = (char*)ptr;
    char* dst = (char*)new_ptr;
    for (size_t i = 0; i < size; i++) {
        dst[i] = src[i];
    }
    
    kfree(ptr);
    return new_ptr;
}

int atoi(const char* str)
{
    int result = 0;
    int sign = 1;
    
    while (*str == ' ') str++;
    if (*str == '-') { sign = -1; str++; }
    else if (*str == '+') str++;
    
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return sign * result;
}

int abs(int n)
{
    return n < 0 ? -n : n;
}

// ADDED: sscanf stub (Doom rarely uses this)
int sscanf(const char* str, const char* format, ...)
{
    // Stub: just return 0 (no items matched)
    // Doom will handle this gracefully
    (void)str;
    (void)format;
    return 0;
}

// Simple atof - converts string to float
double atof(const char* str)
{
    double result = 0.0;
    double sign = 1.0;
    double decimal = 0.1;
    int seen_decimal = 0;
    
    while (*str == ' ' || *str == '\t') str++;
    
    if (*str == '-')
    {
        sign = -1.0;
        str++;
    }
    else if (*str == '+')
    {
        str++;
    }
    
    while (*str)
    {
        if (*str >= '0' && *str <= '9')
        {
            if (seen_decimal)
            {
                result += (*str - '0') * decimal;
                decimal *= 0.1;
            }
            else
            {
                result = result * 10.0 + (*str - '0');
            }
        }
        else if (*str == '.' && !seen_decimal)
        {
            seen_decimal = 1;
        }
        else
        {
            break;
        }
        str++;
    }
    
    return result * sign;
}

long atol(const char* str)
{
    return (long)atoi(str);
}

int mkdir(const char* path, int mode)
{
    (void)path;
    (void)mode;
    return 0; // Stub - always succeed
}

unsigned int __rand_seed = 1;

// ADDED: long absolute value
long labs(long n)
{
    return n < 0 ? -n : n;
}

// ADDED: Integer to ASCII string
void itoa(int value, char* buffer)
{
    char temp[32];
    int i = 0;
    int is_negative = 0;
    
    if (value == 0)
    {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }
    
    if (value < 0)
    {
        is_negative = 1;
        value = -value;
    }
    
    while (value > 0)
    {
        temp[i++] = '0' + (value % 10);
        value /= 10;
    }
    
    int j = 0;
    if (is_negative)
        buffer[j++] = '-';
    
    while (i > 0)
        buffer[j++] = temp[--i];
    
    buffer[j] = '\0';
}