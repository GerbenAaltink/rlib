#include "rtemp.h"
#include "rtest.h"
#include <string.h>

char *classic(char *content) {
    sstring(result, 1024);
    strcpy(result, content);
    result++;
    return result;
}

void rtemp_test_rtempc() {
    rtest_banner("rtempc");
    char *res1 = sbuf("test1");
    char *res2 = sbuf("test2");
    char *res3 = sbuf("test3");
    char *res4 = sbuf("test4");
    char *res5 = sbuf("test5");
    rassert(!strcmp(res5, "test5"));
    rassert(!strcmp(res4, "test4"));
    rassert(!strcmp(res3, "test3"));
    rassert(!strcmp(res2, "test2"));
    rassert(!strcmp(res1, "test1"));
    char line[1024] = {0};
    sprintf(line, "%s%s%s", rtempc("test1"), rtempc("test2"), rtempc("test3"));
    rassert(!strcmp(line, "test1test2test3"));
    line[0] = 0;
    sprintf(line, "%s%s%s", classic("test1"), classic("test2"), classic("test3"));
    rassert(strcmp(line, "test1test2test3"));
}

int main() {
    rtest_banner("rtemp");

    rtemp_test_rtempc();

    return rtest_end("");
}
