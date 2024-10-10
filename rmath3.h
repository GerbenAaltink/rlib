#ifndef RMATH_H
#define RMATH_H
#include <math.h>

#ifndef ceil
double ceil(double x) {
    if (x == (double)(long long)x) {
        return x;
    } else if (x > 0.0) {
        return (double)(long long)x + 1.0;
    } else {
        return (double)(long long)x;
    }
}
#endif

#ifndef floor
double floor(double x) {
    if (x >= 0.0) {
        return (double)(long long)x;
    } else {
        double result = (double)(long long)x;
        return (result == x) ? result : result - 1.0;
    }
}
#endif

#ifndef modf
double modf(double x, double *iptr) {
    double int_part = (x >= 0.0) ? floor(x) : ceil(x);
    *iptr = int_part;
    return x - int_part;
}
#endif
#endif