#include "rlib.h"

void dummy_function() {
    for (long i = 0; i < 100000; i++) {
        long a = i * 2;
    }
}

int main() {
    rbench_t *r = rbench_new();
    r->add_function(r, "function", "dummy_function", dummy_function);
    r->execute(r, 10000);

    rbench_free(r);

    for (int i = 0; i < 10000; i++) {

        // rprintr("\\l\\T message\n");
    }
}