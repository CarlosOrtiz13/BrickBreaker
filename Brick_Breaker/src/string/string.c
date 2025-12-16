#include "string.h"

int strlen(const char* ptr)
{
    int i = 0;
    while(*ptr != 0)
    {
        i++;
        ptr += 1;
    }

    return i;
}

int strnlen(const char* ptr, int max)
{
    int i = 0;
    for (i = 0; i < max; i++)
    {
        if (ptr[i] == 0)
            break;
    }

    return i;
}

int strnlen_terminator(const char* str, int max, char terminator)
{
    int i = 0;
    for(i = 0; i < max; i++)
    {
        if (str[i] == '\0' || str[i] == terminator)
            break;
    }

    return i;
}

int istrncmp(const char* s1, const char* s2, int n)
{
    unsigned char u1, u2;
    while(n-- > 0)
    {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;
        if (u1 != u2 && tolower(u1) != tolower(u2))
            return u1 - u2;
        if (u1 == '\0')
            return 0;
    }

    return 0;
}
int strncmp(const char* str1, const char* str2, int n)
{
    unsigned char u1, u2;

    while(n-- > 0)
    {
        u1 = (unsigned char)*str1++;
        u2 = (unsigned char)*str2++;
        if (u1 != u2)
            return u1 - u2;
        if (u1 == '\0')
            return 0;
    }

    return 0;
}

char* strcpy(char* dest, const char* src)
{
    char* res = dest;
    while(*src != 0)
    {
        *dest = *src;
        src += 1;
        dest += 1;
    }

    *dest = 0x00;

    return res;
}

char* strncpy(char* dest, const char* src, int count)
{
    int i = 0;
    for (i = 0; i < count-1; i++)
    {
        if (src[i] == 0x00)
            break;

        dest[i] = src[i];
    }

    dest[i] = 0x00;
    return dest;
}

int tonumericdigit(char c)
{
    return c - 48;
}

// ADDED: String comparison (case-sensitive)
int strcmp(const char* str1, const char* str2)
{
    while (*str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

// ADDED: Case-insensitive string comparison
int strcasecmp(const char* s1, const char* s2)
{
    while (*s1 && *s2)
    {
        char c1 = tolower(*s1);
        char c2 = tolower(*s2);
        if (c1 != c2)
            return c1 - c2;
        s1++;
        s2++;
    }
    return tolower(*s1) - tolower(*s2);
}

// ADDED: Case-insensitive string comparison with length
int strncasecmp(const char* s1, const char* s2, size_t n)
{
    if (n == 0)
        return 0;
    
    while (n-- && *s1 && *s2)
    {
        char c1 = tolower(*s1);
        char c2 = tolower(*s2);
        if (c1 != c2)
            return c1 - c2;
        s1++;
        s2++;
    }
    
    if (n == (size_t)-1)
        return 0;
    
    return tolower(*s1) - tolower(*s2);
}

// ADDED: String concatenation
char* strcat(char* dest, const char* src)
{
    char* ret = dest;
    while (*dest)
        dest++;
    while ((*dest++ = *src++))
        ;
    return ret;
}

// ADDED: String concatenation with length
char* strncat(char* dest, const char* src, size_t n)
{
    char* ret = dest;
    while (*dest)
        dest++;
    while (n-- && (*dest++ = *src++))
        ;
    if (n == (size_t)-1)
        *dest = '\0';
    return ret;
}

// ADDED: Find character in string
char* strchr(const char* s, int c)
{
    while (*s)
    {
        if (*s == (char)c)
            return (char*)s;
        s++;
    }
    return (c == '\0') ? (char*)s : NULL;
}

// ADDED: Find last occurrence of character
char* strrchr(const char* s, int c)
{
    const char* last = NULL;
    while (*s)
    {
        if (*s == (char)c)
            last = s;
        s++;
    }
    if (c == '\0')
        return (char*)s;
    return (char*)last;
}

// ADDED: Find substring
char* strstr(const char* haystack, const char* needle)
{
    if (!*needle)
        return (char*)haystack;
    
    for (; *haystack; haystack++)
    {
        const char* h = haystack;
        const char* n = needle;
        while (*h && *n && (*h == *n))
        {
            h++;
            n++;
        }
        if (!*n)
            return (char*)haystack;
    }
    return NULL;
}

// ADDED: Duplicate string (allocates memory)
char* strdup(const char* s)
{
    size_t len = strlen(s) + 1;
    char* new_str = (char*)malloc(len);
    if (new_str)
        memcpy(new_str, s, len);
    return new_str;
}

// ADDED: String span functions
size_t strspn(const char* s, const char* accept)
{
    size_t count = 0;
    while (*s && strchr(accept, *s++))
        count++;
    return count;
}

size_t strcspn(const char* s, const char* reject)
{
    size_t count = 0;
    while (*s && !strchr(reject, *s++))
        count++;
    return count;
}