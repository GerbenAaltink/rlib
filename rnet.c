#include "rnet.h"


void on_client_connect(rnet_socket_t * sock){
    printf("%s connected\n",sock->name);
}
void on_client_read(rnet_socket_t * sock){
    char * data = net_socket_read(sock, 4096);
    if(!data)
        return;
    char * http_headers = "HTTP/1.1 200 OK\r\nContent-Length: 10\r\nConnection: close\r\n\r\n";
	net_socket_write(sock,http_headers,strlen(http_headers));
	rnet_safe_str(data,sock->bytes_received);
	data[11] = 0;
    printf("%s: %.30s\n",sock->name,data);
    net_socket_write(sock,data,strlen(data));
    if(!strncmp(data,"GET ",4))
    net_socket_close(sock);
}
void on_client_close(rnet_socket_t * sock){
    printf("%s disconnected\n",sock->name);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: [port].\n");
        return 1;
    }
    rnet_select_result_t *sockets = NULL;
    rnet_server_t  * server = net_socket_serve((unsigned int)atoi(argv[1]), 10);
    server->on_connect = on_client_connect;
    server->on_read = on_client_read;
    server->on_close = on_client_close;
    while (true) {
        if(net_socket_select(server)){
            printf("Handled all events.\n");
        }else{
            printf("No events to handle.\n");
        }
    }
    return 0;
}
