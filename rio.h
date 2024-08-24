#ifndef RLIB_RIO
#define RLIB_RIO
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

bool rfile_exists(char *path) {
    struct stat s;
    return !stat(path, &s);
}

size_t rfile_size(char *path) {
    struct stat s;
    stat(path, &s);
    return s.st_size;
}

size_t rfile_readb(char *path, void *data, size_t size) {
    FILE *fd = fopen(path, "rb");
    if (!fd) {
        return 0;
    }
    __attribute__((unused)) size_t bytes_read =
        fread(data, size, sizeof(char), fd);

    fclose(fd);
    return size;
}

#endif