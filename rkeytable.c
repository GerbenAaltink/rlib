#include "rkeytable.h"
#include "rtest.h"
#include "rstring.h"

int main() {

    for (int i = 0; i < 1000; i++) {
        char *key = rgenerate_key();
        rkset(key, "tast");
        rasserts(!strcmp(rkget(key), "tast"));
    }
}