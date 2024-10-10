#include "rnet.h"

void on_client_connect(rnet_socket_t *sock) {
    printf("%s connected\n", sock->name);
}
void on_client_read(rnet_socket_t *sock) {
    unsigned char *data = net_socket_read(sock, 4096);
    if (!data)
        return;
    char *http_headers =
        "HTTP/1.1 200 OK\r\nContent-Length: 10\r\nConnection: close\r\n\r\n";
    net_socket_write(sock, (unsigned char *)http_headers, strlen(http_headers));
    rnet_safe_str((char *)data, sock->bytes_received);
    // data[11] = 0;
    printf("%s: %.30s\n", sock->name, data);
    net_socket_write(sock, data, strlen((char *)data));
    if (!strncmp((char *)data, "GET ", 4))
        net_socket_close(sock);
}
void on_client_close(rnet_socket_t *sock) {
    printf("%s disconnected\n", sock->name);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: [port].\n");
        return 1;
    }
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "test") == 0) {
            printf("Skipping rnet tests.\n");
            return 0;
        }
    }
    rnet_server_t *server = net_socket_serve((unsigned int)atoi(argv[1]), 10);
    server->on_connect = on_client_connect;
    server->on_read = on_client_read;
    server->on_close = on_client_close;
    while (true) {
        if (net_socket_select(server)) {
            printf("Handled all events.\n");
        } else {
            printf("No events to handle.\n");
        }
    }
    return 0;
}
