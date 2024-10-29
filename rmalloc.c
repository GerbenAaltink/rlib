#include "rtest.h"

#include "rmalloc.h"

void rtest_malloc() {
    rtest_banner("count");
    void *x = malloc(10);
    void *y = malloc(10);
    void *z = malloc(10);
    void *ptr = NULL;
    realloc(ptr, 10);
    void *w = calloc(1, 10);
    rtest_true(rmalloc_alloc_count == 5);

    rtest_banner("free") x = free(x);
    rtest_true(x == NULL);
    rtest_true(rmalloc_count == 4);

    rtest_banner("another free") y = free(y);
    rtest_true(y == NULL);
    rtest_true(rmalloc_count == 3);

    rtest_banner("third free") z = free(z);
    rtest_true(z == NULL);
    rtest_true(rmalloc_count == 2);

    rtest_banner("third four") w = free(w);
    rtest_true(w == NULL);
    rtest_true(rmalloc_count == 1);

    rtest_banner("third five") ptr = free(ptr);
    rtest_true(ptr == NULL);
    rtest_true(rmalloc_count == 0);

    rtest_banner("totals") rtest_true(rmalloc_alloc_count == 5);
    rtest_true(rmalloc_free_count == 5);
    rtest_true(rmalloc_count == 0);
}

int main() {
    rtest_banner("malloc.h");
    rtest_malloc();
    return rtest_end("rtest_malloc");
}