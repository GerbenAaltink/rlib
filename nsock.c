#include "nsock.h"

void on_connect(int fd) { printf("connect\n"); }
void on_data(int fd) { printf("data\n"); }
void on_close(int fd) { printf("close\n"); }

int main() {

    nsock(9999, on_connect, on_data, on_close);
    return 0;
}