#ifndef RMALLOC_H
#define RMALLOC_H
#ifndef RMALLOC_OVERRIDE
#define RMALLOC_OVERRIDE 1
#endif
#ifdef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE_TEMP _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif
#ifndef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif
#ifndef ulonglong
#define ulonglong unsigned long long
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _POSIX_C_SOURCE_TEMP
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE _POSIX_C_SOURCE_TEMP
#undef _POSIX_C_SOURCE_TEMP
#else
#undef _POSIX_C_SOURCE
#endif

static ulonglong rmalloc_count = 0;
static ulonglong rmalloc_alloc_count = 0;
static ulonglong rmalloc_free_count = 0;

void *rmalloc(size_t size) {
    void *result;
    while (!(result = malloc(size))) {
        fprintf(stderr, "Warning: malloc failed, trying again.\n");
    }
    rmalloc_count++;
    rmalloc_alloc_count++;
    return result;
}
void *rcalloc(size_t count, size_t size) {
    void *result;
    while (!(result = calloc(count, size))) {
        fprintf(stderr, "Warning: calloc failed, trying again.\n");
    }
    rmalloc_alloc_count++;
    rmalloc_count++;
    return result;
}
void *rrealloc(void *obj, size_t size) {
    if (!obj) {
        rmalloc_count++;
        rmalloc_alloc_count++;
    }
    void *result;
    while (!(result = realloc(obj, size))) {
        fprintf(stderr, "Warning: realloc failed, trying again.\n");
    }
    return result;
}

char *rstrdup(const char *s) {
    if (!s)
        return NULL;

    char *result;
    size_t size = strlen(s) + 1;
    result = rmalloc(size);
    memcpy(result, s, size);
    return result;
}
void *rfree(void *obj) {
    rmalloc_count--;
    rmalloc_free_count++;
    free(obj);
    return NULL;
}

#if RMALLOC_OVERRIDE
#define malloc rmalloc
#define calloc rcalloc
#define realloc rrealloc
#define free rfree
#define strdup rstrdup
#endif

char *rmalloc_stats() {
    static char res[200];
    res[0] = 0;
    sprintf(res, "Memory usage: %lld allocated, %lld freed, %lld in use.", rmalloc_alloc_count, rmalloc_free_count, rmalloc_count);
    return res;
}

#endif
