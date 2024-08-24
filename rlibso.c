#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void *rmalloc(size_t);
void *rfree(void *);
int rtest_end(char *);
void rprintgf(FILE *f, char *format, ...);

int main() {
    for (int i = 0; i < 100; i++) {
        void *data = rmalloc(5000);
        memset(data, 0, 5000);
        rfree(data);
    }
    rprintgf(stdout, "Hello from .so library!");
    return rtest_end("");
}