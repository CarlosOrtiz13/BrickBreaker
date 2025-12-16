#include "stdio.h"
#include "../stdlib/stdlib.h"
#include <stdarg.h>

// Forward declaration - print() is defined in kernel.c
extern void print(const char* str);

int putchar(int c)
{
    char str[2];
    str[0] = (char)c;
    str[1] = '\0';
    print(str);
    return 0;
}

int printf(const char *fmt, ...)
{
    va_list ap;
    const char *p;
    char* sval;
    int ival;
    char buffer[32];
    
    va_start(ap, fmt);
    for (p = fmt; *p; p++)
    {
        if (*p != '%')
        {
            putchar(*p);
            continue;
        }
        switch (*++p)
        {
        case 'i':
        case 'd':
            ival = va_arg(ap, int);
            itoa(ival, buffer);
            print(buffer);
            break;
        case 's':
            sval = va_arg(ap, char *);
            print(sval);
            break;
        default:
            putchar(*p);
            break;
        }
    }
    va_end(ap);
    return 0;
}
