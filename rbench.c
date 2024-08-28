#include "rbench.h"
#include "rtest.h"
#include "rtree.h"
#include "rhashtable.h"
#include <math.h>
#include <string.h>
#include "rtime.h"

char *format_number_retoor(long lnumber) {
    static char formatted[1024];
    char number[1024];
    number[0] = 0;
    sprintf(number, "%ld", lnumber);
    size_t len = strlen(number);
    int comma_count = len / 3;
    int count = 0;
    int offset = 0;
    int i;
    formatted[comma_count + len] = 0;
    for (i = len + comma_count; i > 0; i--) {
        formatted[i - offset] = number[i - comma_count];
        if (count == 3) {
            count = 0;
            offset++;
            if (i > 1) {
                formatted[i - offset] = '.';
            }
        }
        count++;
    }
    return formatted;
}
char *format_number_yurii(long long num) {
    static char buf[1024];
    char *buff = buf;
    int isneg = num < 0;
    if (isneg)
        num = -num;
    long long rev = num;
    size_t count;
    for (count = 0; num; count++, num /= 10)
        rev = rev * 10 + num % 10;
    count += (count - 1) / 3;

    if (isneg)
        *buff++ = '-';
    for (size_t i = 0; i < count; i++) {
        if ((count - i) % 4 == 0) {
            *buff++ = '.';
        } else {
            *buff++ = (rev % 10 + '0');
            rev /= 10;
        }
    }
    *buff = '\0';
    return buf;
}

char *format_number_gpt(long lnumber) {
    static char formatted[1024];

    char number[1024];
    sprintf(number, "%ld", lnumber);

    int len = strlen(number);
    int commas_needed = (len - 1) / 3; // Determine how many dots are needed
    int new_len = len + commas_needed; // New length with dots included

    formatted[new_len] = '\0'; // Null-terminate the formatted string

    int i = len - 1;     // Index for original number
    int j = new_len - 1; // Index for formatted number
    int count = 0;       // Counter for placing dots

    while (i >= 0) {
        if (count == 3) {
            formatted[j--] = '.'; // Insert dot after every 3 digits
            count = 0;            // Reset the counter
        }
        formatted[j--] = number[i--]; // Copy digit from the original number
        count++;
    }
    return formatted;
}

int rstrcmp(char *l, char *r) {
    while (*l && *l == *r) {
        l++;
        r++;
    }
    return *l - *r;
}
int strcmp_gpt(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }

    return *(unsigned char *)str1 - *(unsigned char *)str2;
}
int strcmp_clib(p1, p2) const char *p1;
const char *p2;
{
    register const unsigned char *s1 = (const unsigned char *)p1;
    register const unsigned char *s2 = (const unsigned char *)p2;
    unsigned c1, c2;

    do {
        c1 = (unsigned char)*s1++;
        c2 = (unsigned char)*s2++;
        if (c1 == '\0')
            return c1 - c2;
    } while (c1 == c2);

    return c1 - c2;
}

void bench_rstrcmp(void *arg1, void *arg2) {
    __attribute__((unused)) int res = rstrcmp(arg1, arg2);
}
void bench_cstrcmp(void *arg1, void *arg2) {
    __attribute__((unused)) int res = strcmp(arg1, arg2);
}

bool bench_starts_with_r(const char *s1, const char *s2) {
    return rstrstartswith(s1, s2);
}
bool bench_ends_with_r(const char *s1, const char *s2) {
    return rstrendswith(s1, s2);
}

bool bench_starts_with_gpt(const char *str, const char *prefix) {
    while (*prefix) {
        if (*str != *prefix) {
            return false; // Mismatch found
        }
        str++;
        prefix++;
    }
    return true; // All characters matched
}

int bench_starts_with_yurii(const char *str, const char *start) {
    if (str == NULL)
        return start == NULL;
    if (str == start || start == NULL || *start == '\0')
        return 1;

    return strncmp(str, start, strlen(start)) == 0;
}

bool bench_ends_with_gpt(const char *str, const char *suffix) {
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);

    // If the suffix is longer than the string, it can't be a suffix
    if (suffix_len > str_len) {
        return false;
    }

    // Start comparing from the end of both strings
    const char *str_end = str + str_len - suffix_len;
    while (*suffix) {
        if (*str_end != *suffix) {
            return false; // Mismatch found
        }
        str_end++;
        suffix++;
    }

    return true; // All characters matched
}

int bench_ends_with_yurii(const char *str, const char *end) {
    size_t end_len;

    if (str == NULL)
        return end == NULL;
    if (str == end || end == NULL || *end == '\0')
        return 1;

    end_len = strlen(end);
    return strncmp(str + (strlen(str) - end_len), end, end_len) == 0;
}

void plus(int v1, int v2) { __attribute__((unused)) int v3 = v1 + v2; }
void min(int v1, int v2) { __attribute__((unused)) int v3 = v2 - v1; }

void bench_rstrmove_r() {
    char to_move_1[] = "abc?defgaa";
    rstrmove2(to_move_1, 3, 5, 0);
    rasserts(!strcmp(to_move_1, "?defgabcaa"));
    char to_move_2[] = "?defgabcaa";
    rstrmove2(to_move_2, 0, 5, 3);
    rasserts(!strcmp(to_move_2, "abc?defgaa"));
    char to_move_3[] = "?defgabcaa";
    rstrmove2(to_move_3, 0, 5, 6);
    rasserts(!strcmp(to_move_3, "abcaa?defg"));
}

void bench_rstrmove_gpt() {
    char to_move_1[] = "abc?defgaa";
    rstrmove(to_move_1, 3, 5, 0);
    rasserts(!strcmp(to_move_1, "?defgabcaa"));
    char to_move_2[] = "?defgabcaa";
    rstrmove(to_move_2, 0, 5, 2);
    // printf("BECAME: %s\n",to_move_2);
    //  Goes wrong!
    // rasserts(!strcmp(to_move_2, "ab?defgcaa"));
    char to_move_3[] = "?defgabcaa";
    rstrmove(to_move_3, 0, 5, 7);
    rasserts(!strcmp(to_move_3, "abc?defgaa"));
}

void rbench_table_rtree() {
    rtree_t *tree = (rtree_t *)rbf->data;
    if (rbf->first) {
        tree = rtree_new();
        rbf->data = (void *)tree;
    }
    for (int i = 0; i < 1; i++) {
        char *key = rgenerate_key();
        rtree_set(tree, key, key);
        rasserts(!strcmp(rtree_get(tree, key), key));
    }
    if (rbf->last)
        rtree_free(rbf->data);
}

void rbench_table_rhashtable() {
    for (int i = 0; i < 1; i++) {
        char *key = rgenerate_key();
        rset(key, key);
        rasserts(!strcmp(rget(key), key));
    }
}

nsecs_t total_execution_time = 0;
long total_times = 0;
bool show_progress = 1;
void bench_format_number(long times, long number) {
    rbench_t *r;
    rprint("\\T B\\l Times: %ld\n", times);
    r = rbench_new();
    r->show_progress = show_progress;
    r->add_function(r, "number_format", "retoor", format_number_retoor);
    r->add_function(r, "number_format", "yurii", format_number_yurii);
    r->add_function(r, "number_format", "gpt", format_number_gpt);
    r->execute1(r, times, (void *)number);
    total_execution_time += r->execution_time;
    total_times += times * 2;
    rbench_free(r);
}

void bench_table(long times) {
    rbench_t *r;
    rprint("\\T B\\l Times: %ld\n", times);
    r = rbench_new();
    r->show_progress = show_progress;
    r->add_function(r, "rtree", "retoor", rbench_table_rtree);
    r->add_function(r, "hashtable", "k*r", rbench_table_rhashtable);
    r->execute(r, times);
    total_execution_time += r->execution_time;
    total_times += times * 2;
    rbench_free(r);
}

void bench_rstrmove(long times) {
    rbench_t *r;
    rprint("\\T B\\l Times: %ld\n", times);
    r = rbench_new();
    r->show_progress = show_progress;
    r->add_function(r, "rstrmove2", "retoor", bench_rstrmove_r);
    r->add_function(r, "rstrmove", "gpt", bench_rstrmove_gpt);
    r->execute(r, times);
    total_execution_time += r->execution_time;
    total_times += times * 2;
    rbench_free(r);
}
void bench_math(long times) {
    rbench_t *r = rbench_new();
    r->show_progress = show_progress;
    rprint("\\T B\\l Times: %ld\n", times);
    r->add_function(r, "plus", "math", plus);
    r->add_function(r, "min", "math", min);
    r->execute2(r, times, (void *)5, (void *)5);
    total_execution_time += r->execution_time;
    total_times += times * 2;
    rbench_free(r);
}
void bench_strcmp(long times) {
    rbench_t *r = rbench_new();
    r->stdout = false;
    r->show_progress = show_progress;
    rprint("\\T B\\l Times: %ld\n", times);
    r->add_function(r, "strcmp_clib", "scmp", strcmp_clib);
    r->add_function(r, "strcmp", "scmp", strcmp);
    r->add_function(r, "rstrcmp", "scmp", rstrcmp);
    r->add_function(r, "strcmp_gpt", "scmp", strcmp_gpt);
    r->execute2(r, times, "abcdefghijklmnopqrstuvwxyz",
                "abcdefghijklmnopqrstuvwxyz");
    total_execution_time += r->execution_time;
    total_times += times * 2;
    rbench_free(r);
}

void printf_strcat() {
    char buffer[1000] = {0};
    for (int i = 0; i < 1000; i++) {
        strcat(buffer, "a");
    }
    printf("%s", buffer);
}
void printf_raw() {
    for (int i = 0; i < 1000; i++) {
        printf("%s", "a");
    }
}

void bench_sprintf(long times) {
    rbench_t *r = rbench_new();
    r->stdout = false;
    r->show_progress = show_progress;
    rprint("\\T B\\l Times: %ld\n", times);
    r->add_function(r, "strcat", "buffered", printf_strcat);
    r->add_function(r, "printf", "raw", printf_raw);
    r->execute(r, times);
    total_execution_time += r->execution_time;
    total_times += times * 2;
    rbench_free(r);
}

void bench_startswith(long times) {
    rbench_t *r = rbench_new();
    r->stdout = false;
    r->show_progress = show_progress;
    rprint("\\T B\\l Times: %ld\n", times);
    r->add_function(r, "startswith", "retoor", bench_starts_with_r);
    r->add_function(r, "startswith", "gpt", bench_starts_with_gpt);
    r->add_function(r, "startswith", "yurii", bench_starts_with_yurii);
    r->execute2(r, times, "abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnop");
    total_execution_time += r->execution_time;
    total_times += times * 2;
    rbench_free(r);
}

void bench_endswith(long times) {
    rbench_t *r = rbench_new();
    r->stdout = false;
    r->show_progress = show_progress;
    rprint("\\T B\\l Times: %ld\n", times);
    r->add_function(r, "endswith", "retoor", bench_ends_with_r);
    r->add_function(r, "endswith", "gpt", bench_ends_with_gpt);
    r->add_function(r, "endswith", "yurii", bench_ends_with_yurii);
    r->execute2(r, times, "abcdefghijklmnopqrstuvwxyzdef", "qrstuvwxyzdef");
    total_execution_time += r->execution_time;
    total_times += times * 2;
    rbench_free(r);
}

#define ifwhile(cond, action)                                                  \
    {                                                                          \
        bool _did_doit = false;                                                \
        while (cond) {                                                         \
            _did_doit = true;                                                  \
            { action }                                                         \
        }                                                                      \
        if (_did_doit)

#define endifwhile }

int main() {
    show_progress = true;
    long times = 900000000;

    printf("With %% progress times:\n");
    BENCH(times,
          { bench_starts_with_yurii("abcdefghijklmnopqrstuvw", "abcdef"); });
    BENCH(times, { bench_ends_with_yurii("abcdefghijklmnopqrstuvw", "uvw"); });

    printf("Without %% progress times:\n");
    BENCH(times * 1000,
          { bench_starts_with_yurii("abcdefghijklmnopqrstuvw", "abcdef"); });
    BENCH(times * 1000,
          { bench_ends_with_yurii("abcdefghijklmnopqrstuvw", "uvw"); });

    bench_table(times / 10000);
    bench_sprintf(times / 10000);
    bench_format_number(times / 100, 123456789);
    bench_rstrmove(times / 100);
    bench_math(times);
    bench_strcmp(times / 100);

    bench_startswith(times / 10);
    bench_endswith(times / 10);
    printf("\nTotal execution time:%s\n", format_time(total_execution_time));
    printf("Total times: %s\n", rformat_number(total_times));

    return 0;
}