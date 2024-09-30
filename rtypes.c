#include "rtest.h"
#include "rtypes.h"

int main() {
    rtest_banner("rtypes");
    uint test1 = 1000;
    rassert(test1 == 1000) ulong test2 = -1000;
    rassert(test2 == -1000);
    byte test3 = 123;
    rassert(test3 == 123);
    byte test4 = 'A';
    rassert(test4 == 65);
    return rtest_end("");
}
