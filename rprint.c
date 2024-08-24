#include "rprint.h"
#include "rbench.h"

void test_putc(void *arg) {
    char *str = (char *)arg;
    char c;
    while ((c = *str++)) {
        putc('\r', stdout);
    }
}

void test_putchar(void *arg) {
    char *str = (char *)arg;
    char c;
    while ((c = *str++)) {
        putchar('\r');
    }
}
void test_fwrite(void *arg) {
    int length;
    if (rbf->first) {
        length = strlen((char *)arg);
        rbf->data = length;
    } else {
        length = rbf->data;
    }
    fwrite((char *)arg, 1, length, stdout);
}
void test_printf(void *arg) { printf("%s", (char *)arg); }
void test_rprint(void *arg) { rprint("%s", (char *)arg); }
void test_rprintr(void *arg) { rprintr("%s", (char *)arg); }

int main() {

    rbench_t *r = rbench_new();
    r->stdout = false;
    long times = 1000000;
    r->add_function(r, "putc", "chrloop", test_putc);
    r->add_function(r, "putchar", "chrloop", test_putchar);
    r->add_function(r, "fwrite", "fileio", test_fwrite);
    r->add_function(r, "printf", "default", test_printf);
    r->add_function(r, "rprint", "custom", test_rprint);
    r->add_function(r, "rprintr", "color", test_rprintr);

    rprinty("Testing 20 spaces with cariage return\n");
    r->execute1(r, times, "                              \r");
    rprinty("Testing chairs only avaiable for custom functions on all write "
            "functions\n");
    r->execute1(r, times, "\\c\\T\\l\\L                  \r");
}
