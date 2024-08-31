#include "rhashtable.h"
#include "rtest.h"
#include "rstring.h"

int main() {

    for (int i = 0; i < 1000; i++) {
        char *key = rgenerate_key();
        rset(key, "tast");
        rasserts(!strcmp(rget(key), "tast"));
    }
}