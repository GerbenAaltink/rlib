#include <stdio.h>
#ifndef RLIB_RARGS_H
#define RLIB_RARGS_H
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

bool rargs_isset(int argc, char *argv[], char *key) {

    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], key)) {
            return true;
        }
    }
    return false;
}

char *rargs_get_option_string(int argc, char *argv[], char *key, const char *def) {

    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], key)) {
            if (i < argc - 1) {
                return argv[i + 1];
            }
        }
    }
    return (char *)def;
}

int rargs_get_option_int(int argc, char *argv[], char *key, int def) {

    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], key)) {
            if (i < argc - 1) {
                return atoi(argv[i + 1]);
            }
        }
    }
    return def;
}

bool rargs_get_option_bool(int argc, char *argv[], char *key, bool def) {

    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], key)) {
            if (i < argc - 1) {
                if (!strcmp(argv[i + 1], "false"))
                    return false;
                if (!strcmp(argv[i + 1], "0"))
                    return false;
                return true;
            }
        }
    }

    return def;
}
#endif