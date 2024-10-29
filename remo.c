#include "remo.h"

int main() {
    remo_print();
    printf("<%s>", remo_get("zany"));
    return 0;
}