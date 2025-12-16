#ifndef TIME_H
#define TIME_H

#include <stddef.h>

// ADDED: Time types (all stubbed)
typedef long time_t;
typedef long clock_t;

struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

// ADDED: Stub functions
static inline time_t time(time_t* t) { 
    if (t) *t = 0; 
    return 0; 
}

static inline struct tm* localtime(const time_t* t) { 
    static struct tm tm = {0};
    (void)t;
    return &tm; 
}

static inline char* asctime(const struct tm* tm) {
    static char buf[26] = "Thu Jan  1 00:00:00 1970\n";
    (void)tm;
    return buf;
}

#define CLOCKS_PER_SEC 1000

#endif