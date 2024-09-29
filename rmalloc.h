#ifndef RMALLOC_H
#define RMALLOC_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned long long rmalloc_count = 0;
static unsigned long long rmalloc_alloc_count = 0;
static unsigned long long int rmalloc_free_count = 0;

void *rstrdup(const char *s) {
    rmalloc_count++;
    rmalloc_alloc_count++;
    return strdup(s);
}
void *rmalloc(size_t size) {
    rmalloc_count++;
    rmalloc_alloc_count++;
    return malloc(size);
}
void *rrealloc(void *obj, size_t size) {
    if (!obj) {
        rmalloc_count++;
        rmalloc_alloc_count++;
    }
    return realloc(obj, size);
}
void *rfree(void *obj) {
    rmalloc_count--;
    rmalloc_free_count++;
    free(obj);
    return NULL;
}

#define malloc rmalloc
#define realloc rrealloc
#define free rfree
#define strdup rstrdup

char *rmalloc_stats() {
    static char res[200] = {0};
    sprintf(res, "Memory usage: %lld allocated, %lld freed, %lld in use.",
            rmalloc_alloc_count, rmalloc_free_count, rmalloc_count);
    return res;
}

#endif
