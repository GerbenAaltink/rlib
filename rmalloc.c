#include "rtest.h"

#include "rmalloc.h"

void rtest_malloc() {
    rtest_banner("count");
    void *x = rmalloc(10);
    void *y = rmalloc(10);
    void *z = rmalloc(10);
    rtest_true(rmalloc_alloc_count == 3);

    rtest_banner("free") x = rfree(x);
    rtest_true(x == NULL);
    rtest_true(rmalloc_count == 2);

    rtest_banner("another free") y = rfree(y);
    rtest_true(y == NULL);
    rtest_true(rmalloc_count == 1);

    rtest_banner("third free") z = rfree(z);
    rtest_true(z == NULL);
    rtest_true(rmalloc_count == 0);

    rtest_banner("totals") rtest_true(rmalloc_alloc_count == 3);
    rtest_true(rmalloc_free_count == 3);
    rtest_true(rmalloc_count == 0);
}

int main() {
    rtest_banner("malloc.h");
    rtest_malloc();
    return rtest_end("rtest_malloc");
}