#include "arena.h"
#include "rtest.h"
#include <stdio.h>
#include <string.h>

int main() {
    rtest_banner("testing arena");
    arena_t *arena = arena_construct();
    // Test initial data
    rtest_banner("Initial values");
    rtest_assert(arena->memory == NULL);
    rtest_assert(arena->size == 0);
    rtest_assert(arena->pointer == 0);
    arena_free(arena);
    // New instance test
    rtest_banner("New instance defaults");
    arena = arena_new(1024);
    rtest_assert(arena->memory != NULL);
    rtest_assert(arena->size == 1024);
    rtest_assert(arena->pointer == 0);
    arena_free(arena);
    // Allocate test
    rtest_banner("Allocate");
    arena = arena_new(1024);
    int *int_one = (int *)arena_alloc(arena, sizeof(int));
    *int_one = 10;
    rtest_assert(*int_one == 10);
    rtest_assert(arena->pointer == sizeof(int));
    int *int_two = (int *)arena_alloc(arena, sizeof(int));
    *int_two = 20;
    rtest_assert(*int_two == 20);
    rtest_assert(arena->pointer == sizeof(int) * 2);
    arena_free(arena);
    return rtest_end("");
}