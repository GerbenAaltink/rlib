#ifndef NSOCK_H
#define NSOCK_H
#include "rmalloc.h"
#include <sys/select.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>

int *nsock_socks = NULL;
int *nsock_readable = NULL;
void **nsock_data = NULL;
int nsock_server_fd = 0;
int nsock_max_socket_fd = 0;

void (*nsock_on_connect)(int fd) = NULL;
void (*nsock_on_data)(int fd) = NULL;
void (*nsock_on_close)(int fd) = NULL;

void nsock_close(int fd) {
    if (nsock_on_close)
        nsock_on_close(fd);
    nsock_socks[fd] = 0;
    close(fd);
}

int *nsock_init(int socket_count) {
    if (nsock_socks) {
        free(nsock_socks);
    }
    nsock_socks = (int *)calloc(1, sizeof(int) * socket_count + 1);
    if (nsock_data) {
        free(nsock_data);
        nsock_data = NULL;
    }
    nsock_data = (void **)malloc(sizeof(void *) * socket_count + 1);
    nsock_socks[socket_count] = -1;
    return nsock_socks;
}

void nsock_free() {
    if (nsock_socks)
        free(nsock_socks);
    if (nsock_readable)
        free(nsock_readable);
    nsock_server_fd = 0;
    nsock_max_socket_fd = 0;
    if (nsock_data) {
        printf("Clean data before freeing\n");
        exit(1);
    }
}

void *nsock_get_data(int socket) { return nsock_data[socket]; }
void nsock_set_data(int socket, void *data) { nsock_data[socket] = data; }

void nsock_listen(int port) {
    int server_fd;
    struct sockaddr_in address;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        printf("port %d already in use\n", port);
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 8096) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    nsock_server_fd = server_fd;
}

int *nsock_select(suseconds_t timeout) {
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout;
    int server_fd = nsock_server_fd;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(server_fd, &rfds);
    int *socks = nsock_socks;
    fd_set efds;
    FD_ZERO(&efds);
    nsock_max_socket_fd = server_fd;
    for (int i = 0; socks[i] != -1; i++) {
        if (i == server_fd)
            continue;
        ;
        if (!socks[i])
            continue;
        if (socks[i] > nsock_max_socket_fd) {
            nsock_max_socket_fd = socks[i];
        }
        FD_SET(socks[i], &rfds);
        FD_SET(socks[i], &efds);
    }

    int activity = select(nsock_max_socket_fd + 1, &rfds, NULL, &efds, timeout == 0 ? NULL : &tv);
    if ((activity < 0) && (errno != EINTR)) {
        perror("Select error\n");
        return NULL;
    } else if (activity == 0) {
        return NULL;
    }
    if (FD_ISSET(server_fd, &rfds)) {
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        address.sin_family = AF_INET;         // IPv4
        address.sin_addr.s_addr = INADDR_ANY; // Listen on any available network interface

        int new_socket = 0;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
        } else {
            nsock_socks[new_socket] = new_socket;
            if (nsock_on_connect)
                nsock_on_connect(new_socket);
            if (new_socket > nsock_max_socket_fd)
                nsock_max_socket_fd = new_socket;
        }
    }
    if (nsock_readable) {
        free(nsock_readable);
    }
    nsock_readable = (int *)calloc(1, sizeof(int) * (nsock_max_socket_fd + 2));
    nsock_readable[nsock_max_socket_fd + 1] = -1;
    nsock_readable[0] = 0;
    int readable_count = 0;
    for (int i = 0; i < nsock_max_socket_fd + 1; i++) {
        if (FD_ISSET(i, &efds)) {
            nsock_close(nsock_socks[i]);
            nsock_socks[i] = 0;
            nsock_readable[i] = 0;
        }
        if (FD_ISSET(i, &rfds) && i != server_fd) {

            nsock_readable[i] = i;
            readable_count++;
            if (nsock_on_data) {
                nsock_on_data(i);
            }
        } else {
            nsock_readable[i] = 0;
        }
    }
    return nsock_readable;
}

unsigned char *nsock_read(int fd, int length) {
    unsigned char *buffer = (unsigned char *)malloc(length + 1);
    int bytes_read = read(fd, buffer, length);
    if (bytes_read <= 0) {
        nsock_close(fd);
        return NULL;
    }
    buffer[bytes_read] = 0;
    return buffer;
}

unsigned char *nsock_read_all(int fd, int length) {
    unsigned char *buffer = (unsigned char *)malloc(length + 1);
    int bytes_read = 0;
    while (bytes_read < length) {
        int bytes_chunk = read(fd, buffer + bytes_read, length - bytes_read);
        if (bytes_chunk <= 0) {
            nsock_close(fd);
            return NULL;
        }
        bytes_read += bytes_chunk;
    }
    buffer[bytes_read] = 0;
    return buffer;
}

int nsock_write_all(int fd, unsigned char *data, int length) {
    int bytes_written = 0;
    while (bytes_written < length) {
        int bytes_chunk = write(fd, data + bytes_written, length - bytes_written);
        if (bytes_chunk <= 0) {
            nsock_close(fd);
            return 0;
        }
        bytes_written += bytes_chunk;
    }
    return bytes_written;
}
void nsock(int port, void (*on_connect)(int fd), void (*on_data)(int fd), void (*on_close)(int fd)) {
    nsock_init(2048);
    nsock_listen(port);
    nsock_on_connect = on_connect;
    nsock_on_data = on_data;
    nsock_on_close = on_close;
    int serve_in_terminal = nsock_on_connect == NULL && nsock_on_data == NULL && nsock_on_close == NULL;
    while (1) {
        int *readable = nsock_select(1000000000);
        if (!serve_in_terminal)
            continue;
        if (!readable)
            continue;
        for (int i = 0; readable[i] != -1; i++) {
            if (!readable[i])
                continue;
            char buffer[1024] = {0};

            int bytes_read = read(readable[i], buffer, 1);
            buffer[bytes_read] = 0;
            if (bytes_read <= 0) {
                nsock_close(readable[i]);
                continue;
            }
            if (write(readable[i], buffer, bytes_read) <= 0) {
                nsock_close(readable[i]);
                continue;
            }
        }
    }
}
#endif