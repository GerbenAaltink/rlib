
#ifndef RLIB_TIME
#define RLIB_TIME

#ifndef _POSIX_C_SOURCE_199309L

#define _POSIX_C_SOURCE_199309L
#endif
#include <sys/time.h>
#include <time.h>
#undef _POSIX_C_SOURCE_199309L
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

typedef uint64_t nsecs_t;
void nsleep(nsecs_t nanoseconds);

void tick() { nsleep(1); }

typedef unsigned long long msecs_t;

nsecs_t nsecs() {
    unsigned int lo, hi;
    __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

msecs_t rnsecs_to_msecs(nsecs_t nsecs) { return nsecs / 1000 / 1000; }

nsecs_t rmsecs_to_nsecs(msecs_t msecs) { return msecs * 1000 * 1000; }

msecs_t usecs() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)(tv.tv_sec) * 1000000 + (long long)(tv.tv_usec);
}

msecs_t msecs() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)(tv.tv_sec) * 1000 + (tv.tv_usec / 1000);
}
char *msecs_strs(msecs_t ms) {
    static char str[22];
    str[0] = 0;
    sprintf(str, "%f", ms * 0.001);
    for (int i = strlen(str); i > 0; i--) {
        if (str[i] > '0')
            break;
        str[i] = 0;
    }
    return str;
}
char *msecs_strms(msecs_t ms) {
    static char str[22];
    str[0] = 0;
    sprintf(str, "%lld", ms);
    return str;
}
char *msecs_str(long long ms) {
    static char result[30];
    result[0] = 0;
    if (ms > 999) {
        char *s = msecs_strs(ms);
        sprintf(result, "%ss", s);
    } else {
        char *s = msecs_strms(ms);
        sprintf(result, "%sMs", s);
    }
    return result;
}

void nsleep(nsecs_t nanoseconds) {
    long seconds = 0;
    int factor = 0;
    while (nanoseconds > 1000000000) {
        factor++;
        nanoseconds = nanoseconds / 10;
    }
    if (factor) {
        seconds = 1;
        factor--;
        while (factor) {
            seconds = seconds * 10;
            factor--;
        }
    }

    struct timespec req = {seconds, nanoseconds};
    struct timespec rem;

    nanosleep(&req, &rem);
}

void ssleep(double s) {
    long nanoseconds = (long)(1000000000 * s);

    // long seconds = 0;

    // struct timespec req = {seconds, nanoseconds};
    // struct timespec rem;

    nsleep(nanoseconds);
}
void msleep(long miliseonds) {
    long nanoseconds = miliseonds * 1000000;
    nsleep(nanoseconds);
}

char *format_time(int64_t nanoseconds) {
    static char output[1024];
    size_t output_size = sizeof(output);
    output[0] = 0;
    if (nanoseconds < 1000) {
        // Less than 1 microsecond
        snprintf(output, output_size, "%ldns", nanoseconds);
    } else if (nanoseconds < 1000000) {
        // Less than 1 millisecond
        double us = nanoseconds / 1000.0;
        snprintf(output, output_size, "%.2fµs", us);
    } else if (nanoseconds < 1000000000) {
        // Less than 1 second
        double ms = nanoseconds / 1000000.0;
        snprintf(output, output_size, "%.2fms", ms);
    } else {
        // 1 second or more
        double s = nanoseconds / 1000000000.0;
        if (s > 60 * 60) {
            s = s / 60 / 60;
            snprintf(output, output_size, "%.2fh", s);
        } else if (s > 60) {
            s = s / 60;
            snprintf(output, output_size, "%.2fm", s);
        } else {
            snprintf(output, output_size, "%.2fs", s);
        }
    }
    return output;
}

#endif
