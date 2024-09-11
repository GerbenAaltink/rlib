#include "rrex4.h"
#include "rtest.h"
#include "rbench.h"
#include <regex.h>

bool bench_r4(unsigned int times, char *str, char *expr) {
    RBENCH(times, {
        r4_t *r = r4(str, expr);

        if (r->valid == false) {

            printf("Bench r4 error\n");
            exit(1);
        }

        r4_free(r);
    });
    return true;
}

void bench_c(unsigned int times, char *str, char *expr) {
    regex_t regex;
    if (regcomp(&regex, expr, REG_EXTENDED)) {
        printf("Creg: error in regular expression.\n");
        exit(1);
    }
    RBENCH(times, {
        if (regexec(&regex, str, 0, NULL, 0)) {
            printf("Creg: error executing regular expression.\n");
            exit(1);
        }
    });

    regfree(&regex);
}

bool bench(unsigned int times, char *str, char *expr) {
    // printf("%d:(%s)<%s>\n", times, str, expr);
    // printf("c:");
    bench_c(times, str, expr);
    // printf("r:");
    bench_r4(times, str, expr);
    return true;
}

void test_r4_next() {
    r4_t *r = r4_new();
    char *str = "abcdefghijklmnop";
    char *reg = "(\\w\\w\\w\\w)";
    r = r4(str, reg);
    rassert(r->valid);
    rassert(r->match_count == 1);
    rassert(!strcmp(r->matches[0], "abcd"));
    // Again with same regex as parameter
    r = r4_next(r, reg);
    rassert(r->valid);
    rassert(r->match_count == 1);
    rassert(!strcmp(r->matches[0], "efgh"));
    // Again with same regex as parameter
    r = r4_next(r, reg);
    rassert(r->valid);
    rassert(r->match_count == 1);
    rassert(!strcmp(r->matches[0], "ijkl"));
    // Reuse expression, NULL parameter
    r = r4_next(r, NULL);
    rassert(r->valid);
    rassert(r->match_count == 1);
    rassert(!strcmp(r->matches[0], "mnop"));
    // No results using r4_next
    r = r4_next(r, NULL);
    rassert(r->valid);
    rassert(r->match_count == 0);
    // Again no results using r4_next, Shouldn't crash
    r = r4_next(r, NULL);
    rassert(r->valid);
    rassert(r->match_count == 0);
    r4_free(r);
}

void bench_all(unsigned int times) {
    rassert(bench(times, "suvw",
                  "[abcdefghijklmnopqrstuvw][abcdefghijklmnopqrstuvw]["
                  "abcdefghijklmnopqrstuvw][abcdefghijklmnopqrstuvw]"));
    rassert(bench(times, "ponyyy", "^p+o.*yyy$$$$"));
    rassert(bench(times, "                   ponyyzd", "p+o.*yyzd$$$$"));
    rassert(bench(times, "abc", "def|gek|abc"));
    rassert(bench(times, "abc", "def|a?b?c|def"));
    rassert(bench(times, "NL18RABO0322309700",
                  "([A-Z]{2})([0-9]{2})([A-Z]{4}[0-9])([0-9]+)$"));
}

int main() {
    rtest_banner("Benchmark");
    unsigned int times = 1;
    bench_all(times);

    RBENCH(1, {
        rtest_banner("Validation");
        rassert(r4_match("ponyyy", "^p+o.*yyy$$$$"));
        rassert(!r4_match("ponyyy", "p%+o.*yyy$$$$"));
        rassert(!r4_match("ponyyyd", "^p+o.*yyz$$$$"));
        rassert(r4_match("123", "[0-2][2-2][1-3]$"));
        rassert(r4_match("aaaabC5", "(a)(\\w)a*(a)\\w[A-Z][0-9]$"));
        rassert(r4_match("abcdeeeeee", "ab(cdeee)e"));
        rassert(r4_match("1234567", "12(.*)67$"));
        rassert(r4_match("12111678993", "12(.*)67(.*)3$"));
        rassert(r4_match("NL17RABO0322309700", "NL(.*)R(.*)0(.*)0(.*)0$"));

        rassert(r4_match("NL18RABO0322309700",
                         "(\\w{2})(\\d{2})(\\w{4}\\d)(\\d+)$"));
        rassert(r4_match("NL18RABO0322309700garbage",
                         "(\\w{2})(\\d{2})(\\w{4}\\d)(\\d+)"));
        rassert(r4_match("NL18RABO0322309700",
                         "(\\w{2})(\\d{2})(\\w{4}\\d)(\\d+)$"));
        rassert(r4_match(" NL18RABO0322309700",
                         "(\\w{2})(\\d{2})(\\w{4}\\d)(\\d+)$"));
        rassert(r4_match("  NL18RABO0322309700",
                         "(\\w{2})(\\d{2})(\\w{4}\\d)(\\d+)$"));
        rassert(r4_match("NL18RABO0", "(\\w\\w)(\\d\\d)(\\w\\w\\w\\w\\d)$"));
        rassert(r4_match("q", "\\q$"));
        r4_t *r =
            r4("NL//18 - RABO0/322309700",
               "(\\w{2})[\\s/-]*(\\d{2})[\\s/-]*(\\w{4}\\d)[\\s/-]*(\\d+)$");
        rassert(r);
        rassert(r->match_count == 4);
        rassert(!strcmp(r->matches[0], "NL"));
        rassert(!strcmp(r->matches[1], "18"));
        rassert(!strcmp(r->matches[2], "RABO0"));
        rassert(!strcmp(r->matches[3], "322309700"));
        r4_free(r);
        rassert(r4_match("ab123", "[a-z0-9]+$"));
        rassert(r4_match("ppppony", "p*pppony"));
        rassert(r4_match("aa", "a{2}$"));
        rassert(r4_match("A23", "[0-2A-z][2-2][1-3]$"));
        rassert(r4_match("z23", "[0-2A-z][2-2][1-3]$"));
        rassert(r4_match("r23", "[0-2Ar][2-2][1-3]$"));
        rassert(r4_match("test", "\\w\\w\\w\\w$"));
        rassert(!r4_match("test", "\\W\\w\\w\\w$"));
        rassert(r4_match("1est", "\\W\\w\\w\\w$"));
        rassert(r4_match("1est", "\\d\\w\\w\\w$"));
        rassert(r4_match("Aest", "\\D\\w\\w\\w$"));
        rassert(r4_match("abc", "[ab]+"));
        rassert(!r4_match("abc", "[ab]+$"));
        rassert(r4_match("abc", "[abc]+$"));
        rassert(!r4_match("a", "[^ba]"));
        rassert(!r4_match("a", "[^ab]"));
        rassert(r4_match("                     ponyyzd", "p+o.*yyzd$$$$"));
        rassert(r4_match("abc", "def|gek|abc"));
        rassert(!r4_match("abc", "def|gek|abd"));
        rassert(r4_match("abc", "def|abc|def"));
        rassert(r4_match("suwv",
                         "[abcdesfghijklmnopqrtuvw][abcdefghijklmnopqrstuvw]["
                         "abcdefghijklmnopqrstuvw][abcdefghijklmnopqrstuvw]"));
        test_r4_next();
    });

    return 0;
}