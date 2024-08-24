#ifndef RARENA_H
#define RARENA_H

#include "rmalloc.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct arena_t {
    unsigned char *memory;
    unsigned int pointer;
    unsigned int size;
} arena_t;

arena_t *arena_construct() {
    arena_t *arena = (arena_t *)rmalloc(sizeof(arena_t));
    arena->memory = NULL;
    arena->pointer = 0;
    arena->size = 0;
    return arena;
}

arena_t *arena_new(size_t size) {
    arena_t *arena = arena_construct();
    arena->memory = (unsigned char *)rmalloc(size);
    arena->size = size;
    return arena;
}

void *arena_alloc(arena_t *arena, size_t size) {
    if (arena->pointer + size > arena->size) {
        return NULL;
    }
    void *p = arena->memory + arena->pointer;
    arena->pointer += size;
    return p;
}

void arena_free(arena_t *arena) {
    // Just constructed and unused arena memory is NULL so no free needed
    if (arena->memory) {
        rfree(arena->memory);
    }
    rfree(arena);
}

void arena_reset(arena_t *arena) { arena->pointer = 0; }
#endif