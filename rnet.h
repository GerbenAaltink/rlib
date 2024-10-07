#ifndef RNET_H
#define RNET_H
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define NET_SOCKET_MAX_CONNECTIONS 50000

typedef struct rnet_socket_t {
    int fd;
    char name[50];
    void *data;
    size_t bytes_received;
    size_t bytes_sent;
    void (*on_read)(struct rnet_socket_t *);
    void (*on_close)(struct rnet_socket_t *);
    void (*on_connect)(struct rnet_socket_t *);
} rnet_socket_t;

typedef struct rnet_select_result_t {
    int server_fd;
    rnet_socket_t **sockets;
    unsigned int socket_count;
} rnet_select_result_t;

typedef struct rnet_server_t {
    int socket_fd;
    rnet_socket_t **sockets;
    unsigned int socket_count;
    unsigned int port;
    unsigned int backlog;
    rnet_select_result_t * select_result;
    int max_fd;
    void (*on_connect)(rnet_socket_t *socket);
    void (*on_close)(rnet_socket_t *socket);
    void (*on_read)(rnet_socket_t *socket);
} rnet_server_t;

void rnet_select_result_free(rnet_select_result_t *result);
int net_socket_accept(int server_fd);
int net_socket_connect(const char *, unsigned int);
int net_socket_init();
rnet_server_t *net_socket_serve(unsigned int port, unsigned int backlog);
rnet_select_result_t *net_socket_select(rnet_server_t *server);
rnet_socket_t * net_socket_wait(rnet_socket_t * socket_fd);
bool net_set_non_blocking(int sock);
bool net_socket_bind(int sock, unsigned int port);
bool net_socket_listen(int sock, unsigned int backlog);
char *net_socket_name(int sock);
size_t net_socket_write(rnet_socket_t *, unsigned char *, size_t);
rnet_socket_t *get_net_socket_by_fd(int);
unsigned char *net_socket_read(rnet_socket_t *, unsigned int buff_size);
void _net_socket_close(int sock);
void net_socket_close(rnet_socket_t * sock);

rnet_server_t *rnet_server_new(int socket_fd, unsigned int port,
                               unsigned int backlog) {
    rnet_server_t *server = malloc(sizeof(rnet_server_t));
    server->socket_fd = socket_fd;
    server->sockets = NULL;
    server->socket_count = 0;
    server->port = port;
    server->backlog = backlog;
    server->max_fd = -1;
    server->select_result = NULL;
    server->on_connect = NULL;
    server->on_close = NULL;
    server->on_read = NULL; 
    return server;
}

rnet_server_t *rnet_server_add_socket(rnet_server_t *server,
                                      rnet_socket_t *sock) {
    server->sockets = realloc(server->sockets, sizeof(rnet_socket_t *) *
                                                   (server->socket_count + 1));
    server->sockets[server->socket_count] = sock;
    server->socket_count++;
    sock->on_read = server->on_read;
    sock->on_connect = server->on_connect;
    sock->on_close = server->on_close;
    return server;
}

rnet_socket_t sockets[NET_SOCKET_MAX_CONNECTIONS] = {0};
unsigned long sockets_connected = 0;
int net_socket_max_fd = 0;
unsigned long sockets_total = 0;
unsigned long sockets_disconnected = 0;
unsigned long sockets_concurrent_record = 0;
unsigned long sockets_errors = 0;

bool net_set_non_blocking(int sock) {
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags < 0) {
        perror("fcntl");
        return false;
    }

    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("fcntl");
        return false;
    }

    return true;
}

int net_socket_init() {
    int socket_fd = -1;
    memset(sockets, 0, sizeof(sockets));
    int opt = 1;
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed.\n");
        return false;
    }
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("Setsockopt failed.\n");
        close(socket_fd);
        return false;
    }
    net_set_non_blocking(socket_fd);
    return socket_fd;
}

char *net_socket_name(int fd) {
    rnet_socket_t *rnet_socket = get_net_socket_by_fd(fd);
    if (rnet_socket) {
        return rnet_socket->name;
        ;
    }

    // If socket disconnected or is no client from server
    return NULL;
}

bool net_socket_bind(int socket_fd, unsigned int port) {
    struct sockaddr_in address;

    address.sin_family = AF_INET;         // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Bind to any available address
    address.sin_port = htons(port);       // Convert port to network byte order

    if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(socket_fd);
        return false;
    }
    return true;
}

int net_socket_connect(const char *host, unsigned int port) {
    char port_str[10] = {0};
    sprintf(port_str, "%d", port);
    int status;
    int socket_fd = -1;
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
        return -1;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        if ((socket_fd =
                 socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
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
        return -1;
    }

    freeaddrinfo(res);
    return socket_fd;
}

bool net_socket_listen(int socket_fd, unsigned int backlog) {
    if (listen(socket_fd, backlog) < 0) { // '3' is the backlog size
        perror("Listen failed");
        close(socket_fd);
        return false;
    }
    return true;
}

rnet_server_t *net_socket_serve(unsigned int port, unsigned int backlog) {
    signal(SIGPIPE, SIG_IGN);
    int socket_fd = net_socket_init();
    net_socket_bind(socket_fd, port);
    net_socket_listen(socket_fd, backlog);
    return rnet_server_new(socket_fd, port, backlog);
}

int net_socket_accept(int net_socket_server_fd) {
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int new_socket = -1;
    if ((new_socket = accept(net_socket_server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0) {
        close(new_socket);
        return -1;
    } else {

        return new_socket;
    }
}
/*
static void net_socket_stats(WrenVM *vm)
{

    wrenSetSlotNewList(vm, 0);

    wrenSetSlotString(vm, 1, "sockets_total");
    wrenInsertInList(vm, 0, -1, 1);

    wrenSetSlotDouble(vm, 1, (double)sockets_total);
    wrenInsertInList(vm, 0, -1, 1);

    wrenSetSlotString(vm, 1, "sockets_concurrent_record");
    wrenInsertInList(vm, 0, -1, 1);

    wrenSetSlotDouble(vm, 1, (double)sockets_concurrent_record);
    wrenInsertInList(vm, 0, -1, 1);

    wrenSetSlotString(vm, 1, "sockets_connected");
    wrenInsertInList(vm, 0, -1, 1);

    wrenSetSlotDouble(vm, 1, (double)sockets_connected);
    wrenInsertInList(vm, 0, -1, 1);

    wrenSetSlotString(vm, 1, "sockets_disconnected");
    wrenInsertInList(vm, 0, -1, 1);

    wrenSetSlotDouble(vm, 1, (double)sockets_disconnected);
    wrenInsertInList(vm, 0, -1, 1);
}*/

size_t net_socket_write(rnet_socket_t * sock, unsigned char *message,size_t size) {
    ssize_t sent_total = 0;
    ssize_t sent = 0;
    ssize_t to_send = size;
    while ((sent = send(sock->fd, message, to_send, 0))) {
        if (sent == -1) {
            sockets_errors++;
            net_socket_close(sock);
            break;
        }
        if (sent == 0) {
            printf("EDGE CASE?\n");
            exit(1);
            sockets_errors++;
            net_socket_close(sock);
            break;
        }
        sent_total += sent;
        if (sent_total == to_send)
            break;
    }
    return sent_total;
}

unsigned char *net_socket_read(rnet_socket_t * sock, unsigned int buff_size) {
    if (buff_size > 1024 * 1024 + 1) {
        perror("Buffer too big. Maximum is 1024*1024.\n");
        exit(1);
    }
    static unsigned char buffer[1024 * 1024];
    buffer[0] = 0;
    size_t received = recv(sock->fd, buffer, buff_size, 0);
    if (received <= 0) {
        buffer[0] = 0;
        net_socket_close(sock);
        if (received < 0) {
            sockets_errors++;
            return NULL;
        }
    }
    buffer[received+1] = 0;
    sock->bytes_received = received;
    return buffer;
}

rnet_socket_t * net_socket_wait(rnet_socket_t *sock) {
    if(!sock)
        return NULL;
    if(sock->fd == -1)
        return NULL;
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(sock->fd, &read_fds);

    int max_socket_fd = sock->fd;
    int activity = select(max_socket_fd + 1, &read_fds, NULL, NULL, NULL);
    if ((activity < 0) && (errno != EINTR)) {
        // perror("Select error");
        net_socket_close(sock);
        return NULL;
    }
    if (FD_ISSET(sock->fd, &read_fds)) {
        return sock;
    }

    return NULL;
}

void rnet_safe_str(char *str, size_t length){
    for(unsigned int i = 0; i < length; i++){
        if(str[i] < 32 || str[i] > 126)
            if(str[i] != 0)
                str[i] = '.';
    }
}

rnet_select_result_t *rnet_new_socket_select_result(int socket_fd) {
    rnet_select_result_t *result =
        (rnet_select_result_t *)malloc(sizeof(rnet_select_result_t));
    memset(result, 0, sizeof(rnet_select_result_t));
    result->server_fd = socket_fd;
    result->socket_count = 0;
    return result;
}

void rnet_select_result_add(rnet_select_result_t *result, rnet_socket_t * sock) {
    result->sockets = realloc(result->sockets, sizeof(rnet_socket_t *) *
                                                   (result->socket_count + 1));
    result->sockets[result->socket_count] = sock;
    result->socket_count++;
}
void rnet_select_result_free(rnet_select_result_t *result) {
    free(result->sockets);
    free(result);
}
rnet_select_result_t *net_socket_select(rnet_server_t *server) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(server->socket_fd, &read_fds);

    server->max_fd = server->socket_fd;
    int socket_fd = -1;
    for (unsigned int i = 0; i < server->socket_count; i++) {
        socket_fd = server->sockets[i]->fd;
        if (socket_fd > 0) {
            FD_SET(socket_fd, &read_fds);
            if (socket_fd > server->max_fd) {
                server->max_fd = socket_fd;
            }
        }
    }
    int new_socket = -1;
    struct sockaddr_in address;
    int addrlen = sizeof(struct sockaddr_in);
    int activity = select(server->max_fd + 1, &read_fds, NULL, NULL, NULL);
    if ((activity < 0) && (errno != EINTR)) {
        // perror("Select error\n");
        return NULL;
    }
    if (FD_ISSET(server->socket_fd, &read_fds)) {
        if ((new_socket =
                 accept(server->socket_fd, (struct sockaddr *)&address,
                        (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed\n");
            return NULL;
        }

        net_set_non_blocking(new_socket);
        char name[50] = {0};
        sprintf(name, "fd:%.4d:ip:%12s:port:%.6d", new_socket,
                inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        rnet_socket_t *sock_obj = NULL;
        for (unsigned int i = 0; i < server->socket_count; i++) {
            if (server->sockets && server->sockets[i]->fd == -1) {
                sock_obj = server->sockets[i];
            }
        }
        if (!sock_obj) {
            sock_obj = (rnet_socket_t *)malloc(sizeof(rnet_socket_t));
            rnet_server_add_socket(server, sock_obj);
        }
        sock_obj->fd = new_socket;
        strcpy(sock_obj->name, name);
        sockets_connected++;
        sockets_total++;
        sockets_concurrent_record =
            sockets_connected > sockets_concurrent_record
                ? sockets_connected
                : sockets_concurrent_record;
        if (new_socket > net_socket_max_fd) {
            net_socket_max_fd = new_socket;
        }
        sock_obj->on_connect(sock_obj);
    }
    rnet_select_result_t *result =
        rnet_new_socket_select_result(server->socket_fd);
    unsigned int readable_count = 0;
    for (unsigned int i = 0; i < server->socket_count; i++) {
        if(server->sockets[i]->fd == -1)
            continue;
        if (FD_ISSET(server->sockets[i]->fd, &read_fds)) {
            rnet_select_result_add(result, server->sockets[i]);
            readable_count++;
            if(server->sockets[i]->on_read){
                server->sockets[i]->on_read(server->sockets[i]);
            }
        }
    }
    if(server->select_result)
    {
        rnet_select_result_free(server->select_result);
        server->select_result = NULL;
    }
    if(readable_count == 0)
        rnet_select_result_free(result);
    return readable_count ? result : NULL;
}

rnet_socket_t *get_net_socket_by_fd(int sock) {
    for (unsigned int i = 0; i < net_socket_max_fd; i++) {
        if (sockets[i].fd == sock) {
            return &sockets[i];
        }
    }
    return NULL;
}

void _net_socket_close(int sock) {
    if (sock > 0 ) {
        sockets_connected--;
        sockets_disconnected++;
        if (sock > 0) {
            if (close(sock) == -1) {
                perror("Error closing socket.\n");
            }
        }
    }
}

void net_socket_close(rnet_socket_t * sock) { 
    if(sock->on_close)
    sock->on_close(sock);
    _net_socket_close(sock->fd); 
    sock->fd = -1;
    }

#endif
