#ifndef RMALLOC_H
#define RMALLOC_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int rmalloc_count = 0;
unsigned int rmalloc_alloc_count = 0;
unsigned int rmalloc_free_count = 0;

void *rmalloc(size_t size) {
    rmalloc_count++;
    rmalloc_alloc_count++;
    return malloc(size);
}
void *rrealloc(void *obj, size_t size) { return realloc(obj, size); }
void *rfree(void *obj) {
    rmalloc_count--;
    rmalloc_free_count++;
    free(obj);
    return NULL;
}

char *rmalloc_stats() {
    static char res[100] = {0};
    sprintf(res, "Memory usage: %d allocated, %d freed, %d in use.",
            rmalloc_alloc_count, rmalloc_free_count, rmalloc_count);
    return res;
}

char *rstrdup(char *str) {

    char *res = (char *)strdup(str);
    rmalloc_alloc_count++;
    rmalloc_count++;
    return res;
}

#endif