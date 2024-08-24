#ifndef RTREE_H
#define RTREE_H
#include "rmalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct rtree_t {
    struct rtree_t *next;
    struct rtree_t *children;
    char c;
    void *data;
} rtree_t;

rtree_t *rtree_new() {
    rtree_t *b = (rtree_t *)rmalloc(sizeof(rtree_t));
    b->next = NULL;
    b->children = NULL;
    b->c = 0;
    b->data = NULL;
    return b;
}

rtree_t *rtree_set(rtree_t *b, char *c, void *data) {
    while (b) {
        if (b->c == 0) {
            b->c = *c;
            c++;
            if (*c == 0) {
                b->data = data;
                // printf("SET1 %c\n", b->c);
                return b;
            }
        } else if (b->c == *c) {
            c++;
            if (*c == 0) {
                b->data = data;
                return b;
            }
            if (b->children) {
                b = b->children;
            } else {
                b->children = rtree_new();
                b = b->children;
            }
        } else if (b->next) {
            b = b->next;
        } else {
            b->next = rtree_new();
            b = b->next;
            b->c = *c;
            c++;
            if (*c == 0) {
                b->data = data;
                return b;
            } else {
                b->children = rtree_new();
                b = b->children;
            }
        }
    }
    return NULL;
}

rtree_t *rtree_find(rtree_t *b, char *c) {
    while (b) {
        if (b->c == *c) {
            c++;
            if (*c == 0) {
                return b;
            }
            b = b->children;
            continue;
        }
        b = b->next;
    }
    return NULL;
}

void rtree_free(rtree_t *b) {
    if (!b)
        return;
    rtree_free(b->children);
    rtree_free(b->next);
    rfree(b);
}

void *rtree_get(rtree_t *b, char *c) {
    rtree_t *t = rtree_find(b, c);
    if (t) {
        return t->data;
    }
    return NULL;
}
#endif