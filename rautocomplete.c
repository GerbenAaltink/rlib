#include "rautocomplete.h"

int main() {

    rautocomplete_t *ac = rautocomplete_new();
    rstring_list_add(ac, "first");
    rstring_list_add(ac, "test2");
    rstring_list_add(ac, "test3");
    rstring_list_add(ac, "test4");
    rstring_list_add(ac, "test5");
    rstring_list_add(ac, "test6");
    rstring_list_add(ac, "test7");
    rstring_list_add(ac, "test8");
    rstring_list_add(ac, "test9");
    rstring_list_add(ac, "test10");
    rstring_list_add(ac, "test11");
    rstring_list_add(ac, "test12");
    rstring_list_add(ac, "test13");
    rstring_list_add(ac, "test14");
    rstring_list_add(ac, "test15");
    rstring_list_add(ac, "test16");
    rstring_list_add(ac, "test17");
    rstring_list_add(ac, "test18");
    rstring_list_add(ac, "test19");
    rstring_list_add(ac, "test20");
    printf(r4_escape("test"));
    char *str = rautocomplete_find(ac, "firsta");
    if (str)
        printf("%s\n", str);
    rautocomplete_free(ac);
    return 0;
}