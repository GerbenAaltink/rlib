#ifndef RLIB_MAIN
#define RLIB_MAIN
#include "rhttp.h"
#include "rmerge.h"

void forward_argument(int *argcc, char *argv[]) {
    int argc = *argcc;
    for (int i = 0; i < argc; i++) {
        argv[i] = argv[i + 1];
    }
    argc--;
    *argcc = argc;
}

int rlib_main(int argc, char *argv[]) {

    if (argc == 1) {
        printf("rlib\n\n");
        printf("options:\n");
        printf(" httpd - a http file server. Accepts port as argument.\n");
        printf(
            " rmerge - a merge tool. Converts c source files to one file \n"
            "          with local includes by giving main file as argument.\n");
        return 0;
    }

    forward_argument(&argc, argv);

    if (!strcmp(argv[0], "httpd")) {

        return rhttp_main(argc, argv);
    }
    if (!strcmp(argv[0], "rmerge")) {
        return rmerge_main(argc, argv);
    }

    return 0;
}

#endif