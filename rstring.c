
#include "rstring.h"
#include "rtest.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void rstring_test_whitespace() {
    char *str = malloc(30);
    str[0] = 0;
    char *str2 = rcat(10, 10);
    printf("Numbers: %s\n", str2);
    char *str3 = "Cool";
    rcat(str, str3);
    rcat(str, ' ');
    rcat(str, 13.37);
    printf("String: %s\n", str);
    free(str);

    rtest_banner("rstrip_whitespace");
    char output[1024];
    // Test 1
    char *string1 = "    Test 1";
    rstrip_whitespace(string1, output);
    rassert(strlen(output) == 6);

    char *string2 = "   Test 1";
    rstrip_whitespace(string2, output);
    rassert(strlen(output) == 6);

    char *string3 = "Test 1";
    rstrip_whitespace(string3, output);
    rassert(strlen(output) == 6);

    char *string4 = "";
    rstrip_whitespace(string4, output);
    rassert(strlen(output) == 0);
}

void rstring_test_rstrtokline() {
    rtest_banner("rstrtokline");
    char lines[1024] = "Line 1\nLine 2\nLine 3\nLine 4\n333.29\n3.2221";
    char line[1024];
    size_t offset = 0;
    // Test 1
    while ((offset = rstrtokline(lines, line, offset, true)) && *line) {
        rassert(strlen(line) == 6);
    }
    // Test 2
    offset = 0;
    int count = 0;
    while ((offset = rstrtokline(lines, line, offset, false)) && *line) {
        size_t expected_length = count < 5 ? 7 : 6;
        count++;
        rassert(strlen(line) == expected_length);
    }
    // Test 3
    offset = 0;
    strcat(lines, "\n");
    count = 0;
    while ((offset = rstrtokline(lines, line, offset, true)) && *line) {
        count++;
        size_t expected_length = 6;
        rassert(strlen(line) == expected_length);
    }
}

void sort_test(char *text, char *text_sort_expected) {
    char sorted_text[4096];
    rstrsort(text, sorted_text);
    rassert(!strcmp(text_sort_expected, sorted_text));
}

void rstring_test_rstrsort() {
    rtest_banner("Sorting string content");
    sort_test(
        "Line 3\nLine 2\nLine 4\nLine 1\nQQ 333.29\n3.22\n1337.29\n3.22\n",
        "Line 1\nLine 2\nLine 3\nLine 4\n3.22\n3.22\nQQ 333.29\n1337.29\n");
    sort_test("333.29\n3.22\n1337.29\n3.22\nLine 3\nThe original line 2\nLine "
              "4\nLine 1\n",
              "Line 1\nThe original line 2\nLine 3\nLine "
              "4\n3.22\n3.22\n333.29\n1337.29\n");
}

void rstring_test_rformat_number() {
    rtest_banner("Format number to human readable");
    rassert(!strcmp(rformat_number(100), "100"));
    rassert(!strcmp(rformat_number(1001), "1.001"));
    rassert(!strcmp(rformat_number(10001), "10.001"));
    rassert(!strcmp(rformat_number(100001), "100.001"));
    rassert(!strcmp(rformat_number(1000001), "1.000.001"));
    rassert(!strcmp(rformat_number(1000000001), "1.000.000.001"));
    rassert(!strcmp(rformat_number(1000000000001), "1.000.000.000.001"));
    rassert(!strcmp(rformat_number(1000000000000001), "1.000.000.000.000.001"));
    rassert(
        !strcmp(rformat_number(-1000000000000001), "-1.000.000.000.000.001"));
}

void rstring_test_rstraddslashes() {
    rtest_banner("Addslashes");
    char input[] = "\r\t\n\b\f test";
    char output[100];
    rstraddslashes(input, output);
    rassert(!strcmp((char *)output, "\\r\\t\\n\\b\\f test"));
}

void rstring_test_rstrstripslashes() {
    rtest_banner("Stripslashes");
    char input[] = "\\r\\t\\n\\b\\f test";
    char output[100];
    rstrstripslashes(input, output);
    rassert(!strcmp((char *)output, "\r\t\n\b\f test"));
}

void rstring_test_rstrstartswith() {
    rtest_banner("Starts with");
    rassert(rstrstartswith("abc", "abc"));
    rassert(rstrstartswith("abc", "ab"));
    rassert(rstrstartswith("abc", "a"));
    rassert(rstrstartswith("", ""));
    rassert(!rstrstartswith("abc", "abcdef"));
    rassert(!rstrstartswith("abc", "b"));
    rassert(!rstrstartswith("abc", "bc"));
    rassert(!rstrstartswith("abc", "c"));
}

void rstring_test_rstrendswith() {
    rtest_banner("Ends with");
    rassert(rstrendswith("abc", "abc"));
    rassert(rstrendswith("abc", "bc"));
    rassert(rstrendswith("abc", "c"));
    rassert(rstrendswith("", ""));
    rassert(!rstrendswith("abc", "a"));
    rassert(!rstrendswith("abc", "ab"));
    rassert(!rstrendswith("abc", "abcdef"));
}

void rstring_test_rstrmove() {
    rtest_banner("Move str");
    // Test 1
    char to_move_1[] = "abc?";
    rstrmove(to_move_1, 3, 1, 0);
    rassert(!strcmp(to_move_1, "?abc"));
    // Test 2
    char to_move_2[] = "abc?defgabc";
    rstrmove(to_move_2, 3, 5, 0);
    rassert(!strcmp(to_move_2, "?defgabcabc"));
    // Test 3
    char to_move_3[] = "abc?defg";
    rstrmove(to_move_3, 0, 3, 7);
    rassert(!strcmp(to_move_3, "?defgabc"));

    // Test 4
    char to_move_4[] = "abc?defgaa";
    rstrmove2(to_move_4, 3, 5, 0);
    rassert(!strcmp(to_move_4, "?defgabcaa"));

    // Test 5
    char to_move_5[] = "?defgabcaa";
    rstrmove2(to_move_5, 0, 5, 3);
    rassert(!strcmp(to_move_5, "abc?defgaa"));

    // Test 6
    char to_move_6[] = "?defgabcaa";
    rstrmove2(to_move_6, 0, 5, 6);
    rassert(!strcmp(to_move_6, "abcaa?defg"));
}

int main() {
    rtest_banner("rstring");
    rstring_test_whitespace();
    rstring_test_rstrtokline();
    rstring_test_rstrsort();
    rstring_test_rformat_number();
    rstring_test_rstraddslashes();
    rstring_test_rstrstripslashes();
    rstring_test_rstrstartswith();
    rstring_test_rstrendswith();
    rstring_test_rstrmove();
    return rtest_end("");
}
