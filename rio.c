#include "rio.h"

void cb(char *str) { printf("%s\n", str); }

int main() {
    rforfile("/tmp", cb);

    return 0;
}