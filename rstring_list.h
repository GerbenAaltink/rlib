#ifndef RSTRING_LIST_H
#define RSTRING_LIST_H
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct rstring_list_t {
    unsigned int size;
    unsigned int count;
    char **strings;
} rstring_list_t;

rstring_list_t *rstring_list_new() {
    rstring_list_t *rsl = (rstring_list_t *)malloc(sizeof(rstring_list_t));
    memset(rsl, 0, sizeof(rstring_list_t));
    return rsl;
}

void rstring_list_free(rstring_list_t *rsl) {
    for (unsigned int i = 0; i < rsl->size; i++) {
        free(rsl->strings[i]);
    }
    free(rsl);
    rsl = NULL;
}

void rstring_list_add(rstring_list_t *rsl, char *str) {
    if (rsl->count == rsl->size) {
        rsl->size++;
        rsl->strings = realloc(rsl->strings, sizeof(char *) * rsl->size);
    }
    rsl->strings[rsl->count] = (char *)malloc(strlen(str) + 1);
    strcpy(rsl->strings[rsl->count], str);
    rsl->count++;
}
bool rstring_list_contains(rstring_list_t *rsl, char *str) {
    for (unsigned int i = 0; i < rsl->count; i++) {
        if (!strcmp(rsl->strings[i], str))
            return true;
    }
    return false;
}

#endif