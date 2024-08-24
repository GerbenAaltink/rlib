#include "rtime.h"
#include "rtest.h"
#include <string.h>

int main() {
    rtest_banner("time");
    rtest_banner("Milliseconds tests");
    rtest_assert(!strcmp(msecs_str(0), "0Ms"));
    rtest_assert(!strcmp(msecs_str(1), "1Ms"));
    rtest_assert(!strcmp(msecs_str(12), "12Ms"));
    rtest_assert(!strcmp(msecs_str(123), "123Ms"));
    rtest_assert(!strcmp(msecs_str(999), "999Ms"));
    rtest_banner("Second tests");
    rtest_assert(!strcmp(msecs_str(1000), "1s"));
    rtest_assert(!strcmp(msecs_str(1100), "1.1s"));
    rtest_assert(!strcmp(msecs_str(1234), "1.234s"));
    rtest_assert(!strcmp(msecs_str(12345), "12.345s"));
    return rtest_end("successs");
}