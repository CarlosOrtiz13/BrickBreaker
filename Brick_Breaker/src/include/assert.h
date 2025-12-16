#ifndef ASSERT_H
#define ASSERT_H

// ADDED: Assert macro (does nothing in release, hangs in debug)
#ifdef NDEBUG
#define assert(expr) ((void)0)
#else
#define assert(expr) \
    do { \
        if (!(expr)) { \
            while(1); \
        } \
    } while(0)
#endif

#endif