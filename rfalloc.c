#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

// r file memory
typedef struct rfm_t {
    char path[4096];
    void *data;
    size_t size;
    long ptr;
    FILE *f;
    int fd;
} rfm_t;

rfm_t rfm;
bool _initialized = false;

void rfm_destroy(rfm_t *r) {
    if (munmap(r->data, r->size) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }
    close(rfm.fd);
}

void *falloc(size_t s) {
    rfm_t *rr = &rfm;

    printf("hier\n");
    char *data = (char *)rr->data + rfm.ptr;
    // data+= rfm.ptr;
    rfm.ptr += s;
    return data;
}

void *finit(char *path, size_t size) {

    printf("HIERR\n");
    if (!_initialized) {
        rfm.ptr = 0;
        _initialized = true;
        rfm.size = size;

        printf("HIERR\n");
        memset(&rfm, 0, sizeof(rfm_t));
        rfm.size = size;
        rfm.ptr = 0;

        printf("HIERR\n");
        // rfm.fd = open(path, O_RDWR);
        // ftruncate(rfm.fd, size);

        if (path) {
            printf("HIERR\n");
            rfm.path[0] = 0;
            strcpy(rfm.path, path);
            // creat(path,F_)
            rfm.fd = open(path, O_RDWR);
            printf("OPEN %s\n", path);
        }
        if (!path) {
            rfm.f = tmpfile();
            rfm.fd = fileno(rfm.f);
        }
        // if (ftruncate(rfm.fd, size) == -1)
        // {
        //    perror("ftruncate");
        //        exit(EXIT_FAILURE);
        //   }
        // rfensurefile(path,1024*1024);
        rfm.data =
            mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, rfm.fd, 0);
        printf("HIERR\n");
    }
    /*
    if (rfm.data == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    char *data = (char *)rfm.data;
    printf(data);
    if (munmap(rfm.data, size) == -1)
    {
        perror("munmap");
        exit(EXIT_FAILURE);
    }
    fclose(rfm.f);
    */
}
int main() {
    // Step 1: Create a temporary file
    finit("tast.dat", 1024 * 1024 * 100);
    printf("gaa\n");
    char *data = (char *)falloc(10);
    strcpy(data, "ab");
    char *data2 = (char *)falloc(30);
    strcpy(data2, "ff\n");

    // for(int i = 0; i < 333333; i++)
    //  strcat(data,"ggggggggggggggggg");

    printf("%s\n", data);

    // printf("%s\n",data);;

    // strcpy(data2,"aaaa");

    return 0;
}