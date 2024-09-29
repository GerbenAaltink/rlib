#include "rtree.h"
#include "rbench.h"
#include "rtest.h"
#include <string.h>

typedef struct rtree_bench_data_t {
    rtree_t *rtree;
    char *value;
} rtree_bench_data_t;

void rtree_bench_f() {
    rtree_bench_data_t *bd = rbf->data;
    if (rbf->first) {
        static rtree_bench_data_t bds;
        bds.rtree = rtree_new();
        bds.value = strdup(rgenerate_key());
        rbf->data = &bds;
        bd = rbf->data;
    }
    char *key = rgenerate_key();
    rtree_set(bd->rtree, key, bd->value);
    char *value = (char *)rtree_get(bd->rtree, key);
    rassert(!strcmp(value, bd->value));
    if (rbf->last) {
        free(bd->value);
        rtree_free(bd->rtree);
    }
}

void rtree_bench(long item_count) {
    rbench_t *b = rbench_new();
    b->stdout = false;
    b->silent = true;
    b->add_function(b, "random_key", "rtree_rw", rtree_bench_f);
    b->execute(b, 1000000);
    // rassert(rnsecs_to_msecs(b->execution_time) < 700); // faster than 700ms
    printf("r/w %ld items and deallocated in %s\n", item_count,
           format_time(b->execution_time));
    rbench_free(b);
}

int main() {
    rtest_banner("rtree");
    rtree_t *rtree = rtree_new();
    // Test new object default valuess
    rtest_banner("New object default values");
    rtest_assert(rtree->c == 0);
    rtest_assert(rtree->next == NULL);
    rtest_assert(rtree->children == NULL);
    rtest_assert(rtree->data == NULL);
    // Test set
    rtest_banner("Set");
    rtree_set(rtree, "a", "data");
    rtest_assert(rtree->c == 'a');
    rtest_assert(!strcmp(rtree->data, "data"));
    // Second element should be filled
    rtree_set(rtree, "b", "data2");
    rtest_assert(rtree->next->c == 'b');
    rtest_assert(!strcmp(rtree->next->data, "data2"));
    // First child of second element should be filled
    rtree_set(rtree, "bc", "data3");
    rtest_assert(rtree->next->children->c == 'c');
    rtest_assert(!strcmp(rtree->next->children->data, "data3"));
    rtree_free(rtree);
    rtest_banner("Benchmark");
    rtree_bench(1000000);
    return rtest_end("");
}
