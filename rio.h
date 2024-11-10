#ifndef RLIB_RIO
#define RLIB_RIO
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <dirent.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <unistd.h>
#include "rstring_list.h"

bool rfile_exists(char *path) {
    struct stat s;
    return !stat(path, &s);
}

void rjoin_path(char *p1, char *p2, char *output) {
    output[0] = 0;
    strcpy(output, p1);

    if (output[strlen(output) - 1] != '/') {
        char slash[] = "/";
        strcat(output, slash);
    }
    if (p2[0] == '/') {
        p2++;
    }
    strcat(output, p2);
}

int risprivatedir(const char *path) {
    struct stat statbuf;

    if (stat(path, &statbuf) != 0) {
        perror("stat");
        return -1;
    }

    if (!S_ISDIR(statbuf.st_mode)) {
        return -2;
    }

    if ((statbuf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO)) == S_IRWXU) {
        return 1; // Private (owner has all permissions, others have none)
    }

    return 0;
}
bool risdir(const char *path) { return !risprivatedir(path); }

void rforfile(char *path, void callback(char *)) {
    if (!rfile_exists(path))
        return;
    DIR *dir = opendir(path);
    struct dirent *d;
    while ((d = readdir(dir)) != NULL) {
        if (!d)
            break;

        if ((d->d_name[0] == '.' && strlen(d->d_name) == 1) || d->d_name[1] == '.') {
            continue;
        }
        char full_path[4096];
        rjoin_path(path, d->d_name, full_path);

        if (risdir(full_path)) {
            callback(full_path);
            rforfile(full_path, callback);
        } else {
            callback(full_path);
        }
    }
    closedir(dir);
}

bool rfd_wait(int fd, int ms) {

    fd_set read_fds;
    struct timeval timeout;

    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    timeout.tv_sec = 0;
    timeout.tv_usec = 1000 * ms;

    int ret = select(fd + 1, &read_fds, NULL, NULL, &timeout);
    return ret > 0 && FD_ISSET(fd, &read_fds);
}

bool rfd_wait_forever(int fd) {
    while ((!rfd_wait(fd, 10))) {
    }
    return true;
}

size_t rfile_size(char *path) {
    struct stat s;
    stat(path, &s);
    return s.st_size;
}

size_t rfile_readb(char *path, void *data, size_t size) {
    FILE *fd = fopen(path, "r");
    if (!fd) {
        return 0;
    }
    size_t bytes_read = fread(data, sizeof(char), size, fd);

    fclose(fd);
    ((char *)data)[bytes_read] = 0;
    return bytes_read;
}

#endif