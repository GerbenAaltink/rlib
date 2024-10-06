#ifndef RCAT_H
#define RCAT_H
#include <stdio.h>
#include <stdlib.h>

void rcat(char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("rcat: couldn't open \"%s\" for read.\n", filename);
        return;
    }
    unsigned char c;
    while ((c = fgetc(f)) && !feof(f)) {
        printf("%c", c);
    }
    fclose(f);
    fflush(stdout);
}

int rcat_main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: [filename]\n");
        return 1;
    }
    rcat(argv[1]);
    return 0;
}

#endif
