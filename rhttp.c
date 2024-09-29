#include "rhttp.h"
#include "rtest.h"
#include <pthread.h>

int request_handler(rhttp_request_t *r) {
    rhttp_send_drain(r->c,
                     "HTTP/1.1 200 OK\r\n"
                     "Content-Length: 2\r\n"
                     "Connection: close\r\n\r\n"
                     "Ok",
                     0);
    close(r->c);
}

void *rhttp_serve_thread(void *port_arg) {
    int port = *(int *)port_arg;
    rhttp_serve("0.0.0.0", port, 1024, 1, 1, request_handler);
    return NULL;
}

int main(int argc, char *argv[]) {

    rtest_banner("rhttp");
    int port = 9876;
    pthread_t st;
    pthread_create(&st, 0, rhttp_serve_thread, (void *)&port);

    char *response = NULL;
    int attempts = 0;
    while (response == NULL) {
        attempts++;
        response = rhttp_client_get("127.0.0.1", port, "/");
        if (attempts > 1) {
            // printf("Http request attempt: %d\n",attempts);
        }
    }
    rassert(!strcmp(response, "Ok"));
    pthread_cancel(st);
    // rhttp_main(argc, argv);
    return rtest_end("");
}
