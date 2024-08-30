#ifndef RLIB_RIO
#define RLIB_RIO
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/select.h>

bool rfd_wait(int fd, int ms) {
    fd_set read_fds;
    struct timeval timeout;

    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    timeout.tv_sec = 0;
    timeout.tv_usec = 1000 * ms; // 100 milliseconds timeout

    int ret = select(fd + 1, &read_fds, NULL, NULL, &timeout);
    return ret > 0 && FD_ISSET(fd, &read_fds);
}

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