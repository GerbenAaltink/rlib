#include "rtest.h"
#include "rstring_list.h"

void test_rstring_list() {
    rtest_banner("new");
    rstring_list_t *rsl = rstring_list_new();
    rassert(rsl->count == 0);
    rassert(rsl->count == 0);
    rtest_banner("add");
    rstring_list_add(rsl, "test1");
    rassert(rsl->count == 1);
    rassert(rsl->count == 1);
    rstring_list_add(rsl, "test2");
    rassert(rsl->count == 2);
    rassert(rsl->count == 2);
    rtest_banner("contains");
    rassert(rstring_list_contains(rsl, "test1"));
    rassert(rstring_list_contains(rsl, "test2"));
    rassert(!rstring_list_contains(rsl, "test3"));
    rtest_banner("free");
    rstring_list_free(rsl);
}

int main() {
    test_rstring_list();
    return rtest_end("");
}