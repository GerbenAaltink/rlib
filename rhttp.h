#include <arpa/inet.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define BUFF_SIZE 1337
#define RHTTP_MAX_CONNECTIONS 100

int rhttp_opt_error = 1;
int rhttp_opt_warn = 1;
int rhttp_opt_info = 1;
int rhttp_opt_port = 8080;
int rhttp_opt_debug = 0;
int rhttp_opt_request_logging = 0;
int rhttp_sock = 0;
int rhttp_opt_buffered = 0;
int rhttp_c = 0;
char rhttp_opt_host[1024] = "0.0.0.0";
unsigned int rhttp_connections_handled = 0;

typedef struct rhttp_header_t {
    char *name;
    char *value;
    struct rhttp_header_t *next;
} rhttp_header_t;

typedef struct rhttp_request_t {
    int c;
    int closed;
    char *raw;
    char *line;
    char *body;
    char *method;
    char *path;
    char *version;
    unsigned int bytes_received;
    rhttp_header_t *headers;
} rhttp_request_t;

char *rhttp_current_timestamp() {
    // Create a time_t object to hold the current time
    time_t current_time;
    // Get the current time
    time(&current_time);

    // Convert the time to local time (struct tm)
    struct tm *local_time = localtime(&current_time);

    // Create a buffer to hold the formatted date/time string
    static char time_string[100];
    time_string[0] = 0;

    // Format the time as "YYYY-MM-DD HH:MM:SS"
    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", local_time);

    return time_string;
}

void rhttp_logs(const char *prefix, const char *level, const char *format,
                va_list args) {
    char buf[strlen(format) + BUFSIZ + 1];
    buf[0] = 0;
    sprintf(buf, "%s%s %s %s\e[0m", prefix, rhttp_current_timestamp(), level,
            format);
    vfprintf(stdout, buf, args);
}
void rhttp_log_info(const char *format, ...) {
    if (!rhttp_opt_info)
        return;
    va_list args;
    va_start(args, format);
    rhttp_logs("\e[32m", "INFO ", format, args);
    va_end(args);
}
void rhttp_log_debug(const char *format, ...) {
    if (!rhttp_opt_debug)
        return;
    va_list args;
    va_start(args, format);
    if (rhttp_opt_debug)
        rhttp_logs("\e[33m", "DEBUG", format, args);

    va_end(args);
}
void rhttp_log_warn(const char *format, ...) {
    if (!rhttp_opt_warn)
        return;
    va_list args;
    va_start(args, format);
    rhttp_logs("\e[34m", "WARN ", format, args);

    va_end(args);
}
void rhttp_log_error(const char *format, ...) {
    if (!rhttp_opt_error)
        return;
    va_list args;
    va_start(args, format);
    rhttp_logs("\e[35m", "ERROR", format, args);

    va_end(args);
}

void http_request_init(rhttp_request_t *r) {
    r->raw = NULL;
    r->line = NULL;
    r->body = NULL;
    r->method = NULL;
    r->path = NULL;
    r->version = NULL;
    r->headers = NULL;
    r->bytes_received = 0;
    r->closed = 0;
}

void rhttp_free_header(rhttp_header_t *h) {
    if (!h)
        return;
    rhttp_header_t *next = h->next;
    free(h->name);
    free(h->value);
    free(h);
    if (next)
        rhttp_free_header(next);
}
void rhttp_rhttp_free_headers(rhttp_request_t *r) {
    if (!r->headers)
        return;
    rhttp_free_header(r->headers);
    r->headers = NULL;
}

rhttp_header_t *rhttp_parse_headers(rhttp_request_t *s) {
    int first = 1;
    char *body = s->body;
    while (body && *body) {
        char *line = __strtok_r(first ? body : NULL, "\r\n", &body);
        if (!line)
            break;
        rhttp_header_t *h = (rhttp_header_t *)malloc(sizeof(rhttp_header_t));
        h->name = NULL;
        h->value = NULL;
        h->next = NULL;
        char *name = __strtok_r(line, ": ", &line);
        first = 0;
        if (!name) {
            rhttp_free_header(h);
            break;
        }
        h->name = strdup(name);
        char *value = __strtok_r(NULL, "\r\n", &line);
        if (!value) {
            rhttp_free_header(h);
            break;
        }
        h->value = value ? strdup(value + 1) : strdup("");
        h->next = s->headers;
        s->headers = h;
    }
    return s->headers;
}

void rhttp_free_request(rhttp_request_t *r) {
    if (r->bytes_received != 0) {

        free(r->raw);
        free(r->body);

        free(r->method);
        free(r->path);
        free(r->version);
        rhttp_rhttp_free_headers(r);
    }
    free(r);
}

void rhttp_print_header(rhttp_header_t *h) {
    rhttp_log_debug("Header: <%s> \"%s\"\n", h->name, h->value);
}
void rhttp_print_headers(rhttp_header_t *h) {
    while (h) {
        rhttp_print_header(h);
        h = h->next;
    }
}
void rhttp_print_request_line(rhttp_request_t *r) {
    rhttp_log_info("%s %s %s\n", r->method, r->path, r->version);
}
void rhttp_print_request(rhttp_request_t *r) {
    rhttp_print_request_line(r);
    if (rhttp_opt_debug)
        rhttp_print_headers(r->headers);
}

rhttp_request_t *rhttp_parse_request(int s) {
    rhttp_request_t *request =
        (rhttp_request_t *)malloc(sizeof(rhttp_request_t));
    http_request_init(request);
    char buf[BUFF_SIZE] = {0};
    request->c = s;
    int breceived = read(s, buf, BUFF_SIZE);
    if (breceived <= 0) {
        close(request->c);
        request->closed = 1;
        return request;
    }
    buf[breceived] = '\0';
    char *original_buf = buf;

    char *b = original_buf;
    request->raw = strdup(b);
    b = original_buf;
    char *line = strtok(b, "\r\n");
    b = original_buf;
    char *body = b + strlen(line) + 2;
    request->body = strdup(body);
    b = original_buf;
    char *method = strtok(b, " ");
    char *path = strtok(NULL, " ");
    char *version = strtok(NULL, " ");
    request->bytes_received = breceived;
    request->line = line;
    request->method = strdup(method);
    request->path = strdup(path);
    request->version = strdup(version);
    request->headers = NULL;
    rhttp_parse_headers(request);
    return request;
}

void rhttp_close_server() {
    close(rhttp_sock);
    close(rhttp_c);
    printf("Connections handled: %d\n", rhttp_connections_handled);
    printf("Gracefully closed\n");
    exit(0);
}

size_t rhttp_send_drain(int s, char *tsend, size_t to_send_len) {
    if (to_send_len == 0 && *tsend) {
        to_send_len = strlen(tsend) + 1;
    }
    char *to_send = (char *)malloc(to_send_len);
    char *to_send_original = to_send;

    memcpy(to_send, tsend, to_send_len);
    // to_send[to_send_len] = '\0';
    size_t bytes_sent = 0;
    size_t bytes_sent_total = 0;
    while (1) {
        bytes_sent = send(s, to_send, to_send_len, 0);
        if (bytes_sent <= 0) {
            bytes_sent_total = 0;
            break;
        }
        bytes_sent_total += bytes_sent;
        to_send += bytes_sent;
        if (bytes_sent_total == to_send_len) {
            break;
        } else {
            rhttp_log_info("Extra send of %d bytes.\n", to_send_len);
        }
    }

    free(to_send_original);
    return bytes_sent_total;
}

typedef int (*rhttp_request_handler_t)(rhttp_request_t *r);

void rhttp_serve(const char *host, int port, int backlog, int request_logging,
                 int request_debug, rhttp_request_handler_t handler) {
    at_quick_exit(rhttp_close_server);
    rhttp_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host);
    rhttp_opt_debug = request_debug;
    rhttp_opt_request_logging = request_logging;
    int opt = 1;
    setsockopt(rhttp_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(rhttp_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("Binding error\n");
        exit(1);
    }
    listen(rhttp_sock, backlog);
    while (1) {
        struct sockaddr_in client_addr;
        int addrlen = sizeof(client_addr);

        rhttp_c = accept(rhttp_sock, (struct sockaddr *)&client_addr,
                         (socklen_t *)&addrlen);
        rhttp_connections_handled++;
        rhttp_request_t *r = rhttp_parse_request(rhttp_c);
        if (!r->closed) {
            handler(r);
        }
        rhttp_free_request(r);
    }
}

unsigned int rhttp_calculate_number_char_count(unsigned int number) {
    unsigned int width = 1;
    unsigned int tcounter = number;
    while (tcounter / 10 >= 1) {
        tcounter = tcounter / 10;
        width++;
    }
    return width;
}

unsigned int counter = 100000000;
int rhttp_counter_request_handler(rhttp_request_t *r) {
    if (!strncmp(r->path, "/counter", strlen("/counter"))) {
        counter++;
        unsigned int width = rhttp_calculate_number_char_count(counter);
        char to_send2[1024] = {0};
        sprintf(to_send2,
                "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nConnection: "
                "close\r\n\r\n%d",
                width, counter);
        rhttp_send_drain(r->c, to_send2, 0);
        close(r->c);
        return 1;
    }
    return 0;
}
int rhttp_root_request_handler(rhttp_request_t *r) {
    if (!strcmp(r->path, "/")) {
        char to_send[1024] = {0};
        sprintf(to_send, "HTTP/1.1 200 OK\r\nContent-Length: 3\r\nConnection: "
                         "close\r\n\r\nHi!");
        rhttp_send_drain(r->c, to_send, 0);
        close(r->c);
        return 1;
    }
    return 0;
}
int rhttp_error_404_handler(rhttp_request_t *r) {
    char to_send[1024] = {0};
    sprintf(to_send, "HTTP/1.1 404 Document not found\r\nContent-Length: "
                     "0\r\nConnection: close\r\n\r\n");
    rhttp_send_drain(r->c, to_send, 0);
    close(r->c);
    return 1;
}

int rhttp_default_request_handler(rhttp_request_t *r) {
    if (rhttp_opt_debug || rhttp_opt_request_logging)
        rhttp_print_request(r);
    if (rhttp_counter_request_handler(r)) {
        // Counter handler
        rhttp_log_info("Counter handler found for: %s\n", r->path);

    } else if (rhttp_root_request_handler(r)) {
        // Root handler
        rhttp_log_info("Root handler found for: %s\n", r->path);

    } else if (rhttp_error_404_handler(r)) {
        rhttp_log_warn("Error 404 for: %s\n", r->path);

        // Error handler
    } else {
        rhttp_log_warn("No handler found for: %s\n", r->path);
        close(rhttp_c);
    }
    return 0;
}

int rhttp_main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
    int opt;
    while ((opt = getopt(argc, argv, "p:drh:bewi")) != -1) {
        switch (opt) {
        case 'i':
            rhttp_opt_info = 1;
            rhttp_opt_warn = 1;
            rhttp_opt_error = 1;
            break;
        case 'e':
            rhttp_opt_error = 1;
            rhttp_opt_warn = 0;
            rhttp_opt_info = 0;
            break;
        case 'w':
            rhttp_opt_warn = 1;
            rhttp_opt_error = 1;
            rhttp_opt_info = 0;
            break;
        case 'p':
            rhttp_opt_port = atoi(optarg);
            break;
        case 'b':
            rhttp_opt_buffered = 1;
            printf("Logging is buffered. Output may be incomplete.\n");
            break;
        case 'h':
            strcpy(rhttp_opt_host, optarg);
            break;
        case 'd':
            printf("Debug enabled\n");
            rhttp_opt_debug = 1;
            rhttp_opt_warn = 1;
            rhttp_opt_info = 1;
            rhttp_opt_error = 1;
            break;
        case 'r':
            printf("Request logging enabled\n");
            rhttp_opt_request_logging = 1;
            break;
        default:
            printf("Usage: %s [-p port] [-h host] [-b]\n", argv[0]);
            return 1;
        }
    }

    printf("Starting server on: %s:%d\n", rhttp_opt_host, rhttp_opt_port);
    if (rhttp_opt_buffered)
        setvbuf(stdout, NULL, _IOFBF, BUFSIZ);

    rhttp_serve(rhttp_opt_host, rhttp_opt_port, 1024, rhttp_opt_request_logging,
                rhttp_opt_debug, rhttp_default_request_handler);

    return 0;
}

/* CLIENT CODE */

typedef struct rhttp_client_request_t {
    char *host;
    int port;
    char *path;
    bool is_done;
    char *request;
    char *response;
    pthread_t thread;
    int bytes_received;
} rhttp_client_request_t;

rhttp_client_request_t *rhttp_create_request(const char *host, int port,
                                             const char *path) {
    rhttp_client_request_t *r =
        (rhttp_client_request_t *)malloc(sizeof(rhttp_client_request_t));
    char request_line[4096] = {0};
    sprintf(request_line,
            "GET %s HTTP/1.1\r\n"
            "Host: localhost:8000\r\n"
            "Connection: close\r\n"
            "Accept: */*\r\n"
            "User-Agent: mhttpc\r\n"
            "Accept-Language: en-US,en;q=0.5\r\n"
            "Accept-Encoding: gzip, deflate\r\n"
            "\r\n",
            path);
    r->request = strdup(request_line);
    r->host = strdup(host);
    r->port = port;
    r->path = strdup(path);
    r->is_done = false;
    r->response = NULL;
    r->bytes_received = 0;
    return r;
}

int rhttp_execute_request(rhttp_client_request_t *r) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(r->port);
    addr.sin_addr.s_addr = inet_addr(r->host);
    connect(s, (struct sockaddr *)&addr, sizeof(addr));
    send(s, r->request, strlen(r->request), 0);
    char buf[1024 * 1024] = {0};
    int ret = recv(s, buf, 1024 * 1024, 0);
    if (ret > 0) {
        r->response = strdup(buf);
    }

    close(s);
    return ret;
}
void rhttp_reset_request(rhttp_client_request_t *r) {
    free(r->response);
    r->is_done = false;
    r->response = NULL;
    r->bytes_received = 0;
}

void rhttp_client_bench(int workers, int times, const char *host, int port,
                        const char *path) {
    rhttp_client_request_t *requests[workers];
    while (times > 0) {

        for (int i = 0; i < workers && times; i++) {
            requests[i] = rhttp_create_request(host, port, path);
            rhttp_execute_request(requests[i]);
            times--;
        }
    }
}
rhttp_client_request_t *rhttp_client_get(char *host, int port, char *path) {
    rhttp_client_request_t *r = rhttp_create_request(host, port, path);
    rhttp_execute_request(r);
    r->is_done = true;
    return r;
}
/*END CLIENT CODE */