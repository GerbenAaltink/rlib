#ifndef RMALLOC_H
#define RMALLOC_H
#ifndef RMALLOC_OVERRIDE
#define RMALLOC_OVERRIDE 1
#endif
#include "rtypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ulonglong rmalloc_count = 0;
ulonglong rmalloc_alloc_count = 0;
ulonglong rmalloc_free_count = 0;

char *rstrdup(const char *s) {
    if (!s)
        return NULL;

    char *result;
    rmalloc_count++;
    rmalloc_alloc_count++;
    size_t size = strlen(s) + 1;
    while (!(result = (char *)malloc(size))) {
        fprintf(stderr, "Warning: strdup failed, trying again.\n");
    }
    memcpy(result, s, size);
    return result;
}
void *rmalloc(size_t size) {
    rmalloc_count++;
    rmalloc_alloc_count++;
    void *result;
    while (!(result = malloc(size))) {
        fprintf(stderr, "Warning: malloc failed, trying again.\n");
    }
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
void *rfree(void *obj) {
    rmalloc_count--;
    rmalloc_free_count++;
    free(obj);
    return NULL;
}

#if RMALLOC_OVERRIDE
#define malloc rmalloc
#define realloc rrealloc
#define free rfree
#define strdup rstrdup
#endif

char *rmalloc_stats() {
    static char res[200];
    res[0] = 0;
    sprintf(res, "Memory usage: %lld allocated, %lld freed, %lld in use.",
            rmalloc_alloc_count, rmalloc_free_count, rmalloc_count);
    return res;
}

#endif
