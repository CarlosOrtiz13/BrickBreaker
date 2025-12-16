#include "memory.h"

void* memset(void* ptr, int c, size_t size)
{
    char* c_ptr = (char*) ptr;
    for (size_t i = 0; i < size; i++)  // FIXED: Use size_t instead of int
    {
        c_ptr[i] = (char) c;
    }
    return ptr;
}

int memcmp(void* s1, void* s2, int count)
{
    char* c1 = s1;
    char* c2 = s2;
    while(count-- > 0)
    {
        if (*c1++ != *c2++)
        {
            return c1[-1] < c2[-1] ? -1 : 1;
        }
    }

    return 0;
}

void* memcpy(void* dest, const void* src, size_t len)  // FIXED
{
    char* d = (char*)dest;
    const char* s = (const char*)src;
    for (size_t i = 0; i < len; i++)  // FIXED: size_t
    {
        d[i] = s[i];
    }
    return dest;
}

// ADDED: Move memory (handles overlapping regions)
void* memmove(void* dest, const void* src, size_t n)
{
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    
    if (d < s)
    {
        // ADDED: Copy forward (source is before destination)
        for (size_t i = 0; i < n; i++)
            d[i] = s[i];
    }
    else if (d > s)
    {
        // ADDED: Copy backward (destination is before source)
        for (size_t i = n; i > 0; i--)
            d[i - 1] = s[i - 1];
    }
    // ADDED: If d == s, do nothing (same address)
    
    return dest;
}