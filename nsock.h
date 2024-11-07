#ifndef NSOCK_H
#define NSOCK_H
#include "rmalloc.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include "rio.h"

int *nsock_socks = NULL;
int *nsock_readable = NULL;
void **nsock_data = NULL;
int nsock_server_fd = 0;
int nsock_max_socket_fd = 0;

typedef enum nsock_type_t { NSOCK_NONE = 0, NSOCK_SERVER, NSOCK_CLIENT, NSOCK_UPSTREAM } nsock_type_t;

typedef struct nsock_it {
    int fd;
    int *upstreams;
    bool connected;
    bool downstream;
    unsigned int upstream_count;
    nsock_type_t type;
} nsock_t;

nsock_t **nsocks = NULL;
int nsocks_count = 0;

void (*nsock_on_connect)(int fd) = NULL;
void (*nsock_on_data)(int fd) = NULL;
void (*nsock_on_close)(int fd) = NULL;
void nsock_on_before_data(int fd);

nsock_t *nsock_get(int fd) {
    if (nsock_socks[fd] == 0) {
        return NULL;
    }
    if (fd >= nsocks_count || nsocks[fd] == NULL) {
        if (fd >= nsocks_count) {
            nsocks_count = fd + 1;
            nsocks = (nsock_t **)realloc(nsocks, sizeof(nsock_t *) * (nsocks_count));
            nsocks[fd] = (nsock_t *)calloc(1, sizeof(nsock_t));
        }
        nsocks[fd]->upstreams = NULL;
        nsocks[fd]->fd = fd;
        nsocks[fd]->connected = false;
        nsocks[fd]->downstream = false;
        nsocks[fd]->upstream_count = 0;
        nsocks[fd]->type = NSOCK_CLIENT;
        return nsocks[fd];
    }
    return nsocks[fd];
}

void nsock_close(int fd) {
    if (nsock_on_close)
        nsock_on_close(fd);
    nsock_t *sock = nsock_get(fd);
    if (sock) {
        for (unsigned int i = 0; i < sock->upstream_count; i++) {
            nsock_close(sock->upstreams[i]);
            sock->upstreams[i] = 0;
        }
        if (sock->upstream_count) {
            free(sock->upstreams);
        }
        sock->upstream_count = 0;
        sock->connected = false;
    }
    nsock_socks[fd] = 0;
    close(fd);
}

nsock_t *nsock_create(int fd, nsock_type_t type) {
    if (fd <= 0)
        return NULL;
    nsock_socks[fd] = fd;
    nsock_t *sock = nsock_get(fd);
    sock->connected = true;
    sock->downstream = false;
    sock->type = type;
    return sock;
}

int *nsock_init(int socket_count) {
    if (nsock_socks) {
        return nsock_socks;
    }
    nsock_socks = (int *)calloc(1, sizeof(int) * sizeof(int *) * socket_count + 1);
    if (nsock_data) {
        free(nsock_data);
        nsock_data = NULL;
    }
    nsock_data = (void **)malloc(sizeof(void **) * socket_count + 1);
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
        exit(1);
    }
}

void nsock_add_upstream(int source, int target, bool downstream) {
    if (!nsock_socks[target])
        return;
    if (!nsock_socks[source])
        return;
    nsock_t *sock = nsock_get(source);
    nsock_t *sock_target = nsock_get(target);
    sock_target->type = NSOCK_UPSTREAM;
    sock->upstreams = (int *)realloc(sock->upstreams, sizeof(int) * (sock->upstream_count + 1));
    sock->downstream = downstream;
    sock->upstreams[sock->upstream_count] = target;
    sock->upstream_count++;
}

void *nsock_get_data(int socket) { return nsock_data[socket]; }
void nsock_set_data(int socket, void *data) { nsock_data[socket] = data; }

int nsock_connect(const char *host, unsigned int port) {
    char port_str[10] = {0};
    sprintf(port_str, "%d", port);
    int status;
    int socket_fd = 0;
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *p;
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return false;
    }
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if ((status = getaddrinfo(host, port_str, &hints, &res)) != 0) {
        return 0;
    }
    for (p = res; p != NULL; p = p->ai_next) {
        if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }
        if (connect(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_fd);
            continue;
        }
        break;
    }
    if (p == NULL) {
        freeaddrinfo(res);
        return 0;
    }
    freeaddrinfo(res);
    if (socket_fd) {
        if (nsock_socks == NULL) {
            nsock_init(2048);
        }
        nsock_socks[socket_fd] = socket_fd;
        nsock_t *sock = nsock_create(socket_fd, NSOCK_CLIENT);
        sock->connected = true;
    }
    return socket_fd;
}

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
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        int new_socket = 0;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
        } else {
            nsock_socks[new_socket] = new_socket;
            nsock_create(new_socket, NSOCK_CLIENT);
            if (nsock_on_connect)
                nsock_on_connect(new_socket);
            if (new_socket > nsock_max_socket_fd)
                nsock_max_socket_fd = new_socket;
        }
    }
    if (nsock_readable) {
        free(nsock_readable);
    }
    nsock_readable = (int *)calloc(1, sizeof(int *) + sizeof(int)  * (nsock_max_socket_fd + 2));
    nsock_readable[nsock_max_socket_fd + 1] = -1;
    nsock_readable[0] = 0;
    int readable_count = 0;
    for (int i = 0; i < nsock_max_socket_fd + 1; i++) {
        nsock_t *sock = nsock_get(i);
        if (!sock)
            continue;
        if (FD_ISSET(i, &efds)) {
            nsock_close(nsock_socks[i]);
            nsock_socks[i] = 0;
            nsock_readable[i] = 0;
        } else if (FD_ISSET(i, &rfds) && i != server_fd) {
            nsock_readable[i] = i;
            readable_count++;
            nsock_on_before_data(i);
        } else {
            nsock_readable[i] = 0;
            sock->connected = false;
        }
    }
    return nsock_readable;
}

unsigned char *nsock_read(int fd, int length) {
    if (!nsock_socks[fd])
        return NULL;
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
    if (!nsock_socks[fd])
        return NULL;
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
    if (!nsock_socks[fd])
        return 0;
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

int nsock_execute_upstream(int source, size_t buffer_size) {
    int result = 0;
    nsock_t *sock = nsock_get(source);
    unsigned char data[buffer_size];
    memset(data, 0, buffer_size);
    int bytes_read = read(source, data, buffer_size);
    if (bytes_read <= 0) {
        nsock_close(source);
        return 0;
    }
    bool downstreamed = false;
    for (unsigned int i = 0; i < sock->upstream_count; i++) {
        if (!nsock_socks[sock->upstreams[i]])
            continue;
        int bytes_sent = nsock_write_all(sock->upstreams[i], data, bytes_read);
        if (bytes_sent <= 0) {
            nsock_close(sock->upstreams[i]);
            continue;
        }
        if (sock->downstream && downstreamed == false) {
            downstreamed = true;
            unsigned char data[4096];
            memset(data, 0, 4096);
            int bytes_read = read(sock->upstreams[i], data, 4096);
            if (bytes_read <= 0) {
                nsock_close(source);
                return 0;
            }
            int bytes_sent = nsock_write_all(sock->fd, data, bytes_read);
            if (bytes_sent <= 0) {
                nsock_close(sock->upstreams[i]);
                return 0;
            }
        }
        result++;
    }
    return result;
}

void nsock_on_before_data(int fd) {
    if (!nsock_socks[fd])
        return;
    nsock_t *sock = nsock_get(fd);
    if (sock->upstream_count) {
        int upstreamed_to_count = nsock_execute_upstream(fd, 4096);
        if (!upstreamed_to_count) {
            nsock_close(fd);
        }
        return;
    } else if (sock->type == NSOCK_UPSTREAM) {
        while (rfd_wait(sock->fd, 0)) {
            unsigned char *data = nsock_read(fd, 4096);
            (void)data;
        }
    }
    if (nsock_on_data)
        nsock_on_data(fd);
}

void nsock(int port, void (*on_connect)(int fd), void (*on_data)(int fd), void (*on_close)(int fd)) {
    nsock_init(2048);
    nsock_listen(port);
    nsock_on_connect = on_connect;
    nsock_on_data = on_data;
    nsock_on_close = on_close;
    int serve_in_terminal = nsock_on_connect == NULL && nsock_on_data == NULL && nsock_on_close == NULL;
    while (1) {
        int *readable = nsock_select(1000);
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
