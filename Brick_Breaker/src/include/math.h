#ifndef MATH_H
#define MATH_H

// ADDED: Math functions (Doom uses fixed-point, so these are rarely called)

static inline double fabs(double x) { return x < 0 ? -x : x; }
static inline double sqrt(double x) { return x; } // Stub
static inline double sin(double x) { (void)x; return 0.0; }
static inline double cos(double x) { (void)x; return 1.0; }
static inline double tan(double x) { (void)x; return 0.0; }
static inline double floor(double x) { return (double)(int)x; }
static inline double ceil(double x) { return (double)((int)x + (x > (int)x ? 1 : 0)); }
static inline double pow(double x, double y) { (void)x; (void)y; return 1.0; }

#define M_PI 3.14159265358979323846

#endif