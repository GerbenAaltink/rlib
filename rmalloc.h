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
#include <locale.h>
#include "rtemp.h"
#ifdef _POSIX_C_SOURCE_TEMP
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE _POSIX_C_SOURCE_TEMP
#undef _POSIX_C_SOURCE_TEMP
#else
#undef _POSIX_C_SOURCE
#endif
ulonglong rmalloc_count = 0;
ulonglong rmalloc_alloc_count = 0;
ulonglong rmalloc_free_count = 0;
ulonglong rmalloc_total_bytes_allocated = 0;

void *_rmalloc_prev_realloc_obj = NULL;
size_t _rmalloc_prev_realloc_obj_size = 0;

void *rmalloc(size_t size) {
    void *result;
    while (!(result = malloc(size))) {
        fprintf(stderr, "Warning: malloc failed, trying again.\n");
    }
    rmalloc_count++;
    rmalloc_alloc_count++;
    rmalloc_total_bytes_allocated += size;
    return result;
}
void *rcalloc(size_t count, size_t size) {
    void *result;
    while (!(result = calloc(count, size))) {
        fprintf(stderr, "Warning: calloc failed, trying again.\n");
    }
    rmalloc_alloc_count++;
    rmalloc_count++;
    rmalloc_total_bytes_allocated += count * size;
    return result;
}
void *rrealloc(void *obj, size_t size) {
    if (!obj) {
        rmalloc_count++;
    }

    rmalloc_alloc_count++;
    if (obj == _rmalloc_prev_realloc_obj) {
        rmalloc_total_bytes_allocated += size - _rmalloc_prev_realloc_obj_size;
        _rmalloc_prev_realloc_obj_size = size - _rmalloc_prev_realloc_obj_size;

    } else {
        _rmalloc_prev_realloc_obj_size = size;
    }
    void *result;
    while (!(result = realloc(obj, size))) {
        fprintf(stderr, "Warning: realloc failed, trying again.\n");
    }
    _rmalloc_prev_realloc_obj = result;

    return result;
}

char *rstrdup(const char *s) {
    if (!s)
        return NULL;

    char *result;
    size_t size = strlen(s) + 1;

    result = rmalloc(size);
    memcpy(result, s, size);
    rmalloc_total_bytes_allocated += size;
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

char *rmalloc_lld_format(ulonglong num) {

    char res[100];
    res[0] = 0;
    sprintf(res, "%'lld", num);
    char *resp = res;
    while (*resp) {
        if (*resp == ',')
            *resp = '.';
        resp++;
    }
    return sbuf(res);
}

char *rmalloc_bytes_format(int factor, ulonglong num) {
    char *sizes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    if (num > 1024) {
        return rmalloc_bytes_format(factor + 1, num / 1024);
    }
    char res[100];
    sprintf(res, "%s %s", rmalloc_lld_format(num), sizes[factor]);
    return sbuf(res);
}

char *rmalloc_stats() {
    static char res[200];
    res[0] = 0;
    // int original_locale = localeconv();
    setlocale(LC_NUMERIC, "en_US.UTF-8");
    sprintf(res, "Memory usage: %s, %s (re)allocated, %s unqiue free'd, %s in use.", rmalloc_bytes_format(0, rmalloc_total_bytes_allocated),
            rmalloc_lld_format(rmalloc_alloc_count), rmalloc_lld_format(rmalloc_free_count),

            rmalloc_lld_format(rmalloc_count));
    // setlocale(LC_NUMERIC, original_locale);

    setlocale(LC_NUMERIC, "");
    return res;
}

#endif
