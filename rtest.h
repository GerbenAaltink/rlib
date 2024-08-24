#ifndef RTEST_H
#define RTEST_H
#include "rmalloc.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "rprint.h"
#define debug(fmt, ...) printf("%s:%d: " fmt, __FILE__, __LINE__, __VA_ARGS__);

char *rcurrent_banner;
int rassert_count = 0;
unsigned short rtest_is_first = 1;
unsigned int rtest_fail_count = 0;

int rtest_end(char *content) {
    // Returns application exit code. 0 == success
    printf("%s", content);
    printf("\n@assertions: %d\n", rassert_count);
    printf("@memory: %s\n", rmalloc_stats());

    if (rmalloc_count != 0) {
        printf("MEMORY ERROR\n");
        return rtest_fail_count > 0;
    }
    return rtest_fail_count > 0;
}

void rtest_test_banner(char *content, char *file) {
    if (rtest_is_first == 1) {
        char delimiter[] = ".";
        char *d = delimiter;
        char f[2048];
        strcpy(f, file);
        printf("%s tests", strtok(f, d));
        rtest_is_first = 0;
        setvbuf(stdout, NULL, _IONBF, 0);
    }
    printf("\n - %s ", content);
}

bool rtest_test_true_silent(char *expr, int res, int line) {
    rassert_count++;
    if (res) {
        return true;
    }
    rprintrf(stderr, "\nERROR on line %d: %s", line, expr);
    rtest_fail_count++;
    return false;
}

bool rtest_test_true(char *expr, int res, int line) {
    rassert_count++;
    if (res) {
        fprintf(stdout, ".");
        return true;
    }
    rprintrf(stderr, "\nERROR on line %d: %s", line, expr);
    rtest_fail_count++;
    return false;
}
bool rtest_test_false_silent(char *expr, int res, int line) {
    return rtest_test_true_silent(expr, !res, line);
}
bool rtest_test_false(char *expr, int res, int line) {
    return rtest_test_true(expr, !res, line);
}
void rtest_test_skip(char *expr, int line) {
    rprintgf(stderr, "\n @skip(%s) on line %d\n", expr, line);
}
bool rtest_test_assert(char *expr, int res, int line) {
    if (rtest_test_true(expr, res, line)) {
        return true;
    }
    rtest_end("");
    exit(40);
}

#define rtest_banner(content)                                                  \
    rcurrent_banner = content;                                                 \
    rtest_test_banner(content, __FILE__);
#define rtest_true(expr) rtest_test_true(#expr, expr, __LINE__);
#define rtest_assert(expr) rtest_test_true(#expr, expr, __LINE__);
#define rassert(expr) rtest_test_assert(#expr, expr, __LINE__);
#define rtest_asserts(expr) rtest_test_true_silent(#expr, expr, __LINE__);
#define rasserts(expr) rtest_test_true_silent(#expr, expr, __LINE__);
#define rtest_false(expr)                                                      \
    rprintf(" [%s]\t%s\t\n", expr == 0 ? "OK" : "NOK", #expr);                 \
    assert_count++;                                                            \
    assert(#expr);
#define rtest_skip(expr) rtest_test_skip(#expr, __LINE__);

FILE *rtest_create_file(char *path, char *content) {
    FILE *fd = fopen(path, "wb");

    char c;
    int index = 0;

    while ((c = content[index]) != 0) {
        fputc(c, fd);
        index++;
    }
    fclose(fd);
    fd = fopen(path, "rb");
    return fd;
}

void rtest_delete_file(char *path) { unlink(path); }
#endif