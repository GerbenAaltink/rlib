#include "rmalloc.h"
#include "rhttp.h"
#include "rtest.h"
#include <pthread.h>

int request_handler(rhttp_request_t *r) {
    rhttp_send_drain(r->c,
                     "HTTP/1.1 200 OK\r\n"
                     "Content-Length: 3\r\n"
                     "Connection: close\r\n\r\n"
                     "Ok!",
                     0);
    close(r->c);
    return 1;
}

rhttp_request_handler_t handler = request_handler;

void *rhttp_serve_thread(void *port_arg) {
    int port = *(int *)port_arg;
    rhttp_serve(rhttp_opt_host, port, 1024, rhttp_opt_request_logging,
                rhttp_opt_debug, handler);
    return NULL;
}

int main(int argc, char *argv[]) {
    bool do_test = true;
    int port = 9876;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--serve")) {
            printf("rhttp serve mode\n");
            printf("Handlers available:\n");
            printf(" - rhttp_root (/)\n");
            printf(" - rhttp_counter (/counter*)\n");
            printf(" - rhttp_404 (/*)\n");
            do_test = false;
        }
        if (!strcmp(argv[i], "--quiet")) {
            rhttp_opt_info = false;
            rhttp_opt_warn = false;
            rhttp_opt_request_logging = false;
            rhttp_opt_debug = false;
            printf("Quiet mode enabled\n");
        }
        if (atoi(argv[i])) {
            port = atoi(argv[i]);
        }
    }
    if (do_test) {
        rtest_banner("rhttp");
    } else {
        printf("Serving on %s:%d\n", rhttp_opt_host, port);
        handler = rhttp_default_request_handler;
    }

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
    if (do_test) {
        rassert(!strcmp(response, "Ok!"));
        pthread_cancel(st);
        // cleanup
    } else {
        pthread_join(st, NULL);
    }
    // rhttp_main(argc, argv);
    if (do_test)
        return rtest_end("");
    return 0;
}
