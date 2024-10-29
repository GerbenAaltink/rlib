#ifndef RSTRING_H
#define RSTRING_H
#include "rmalloc.h"
#include "rtypes.h"
#include "rmath.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *rstrtimestamp() {
    time_t current_time;
    time(&current_time);
    struct tm *local_time = localtime(&current_time);
    static char time_string[100];
    time_string[0] = 0;
    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", local_time);
    return time_string;
}

ulonglong _r_generate_key_current = 0;

char *_rcat_int_int(int a, int b) {
    static char res[20];
    res[0] = 0;
    sprintf(res, "%d%d", a, b);
    return res;
}
char *_rcat_int_double(int a, double b) {
    static char res[20];
    res[0] = 0;
    sprintf(res, "%d%f", a, b);
    return res;
}

char *_rcat_charp_int(char *a, int b) {
    char res[20];
    sprintf(res, "%c", b);
    return strcat(a, res);
}

char *_rcat_charp_double(char *a, double b) {
    char res[20];
    sprintf(res, "%f", b);
    return strcat(a, res);
}

char *_rcat_charp_charp(char *a, char *b) {
    ;
    return strcat(a, b);
}
char *_rcat_charp_char(char *a, char b) {
    char extra[] = {b, 0};
    return strcat(a, extra);
}
char *_rcat_charp_bool(char *a, bool *b) {
    if (b) {
        return strcat(a, "true");
    } else {
        return strcat(a, "false");
    }
}

#define rcat(x, y)                                                             \
    _Generic((x),                                                              \
        int: _Generic((y),                                                     \
        int: _rcat_int_int,                                                    \
        double: _rcat_int_double,                                              \
        char *: _rcat_charp_charp),                                            \
        char *: _Generic((y),                                                  \
        int: _rcat_charp_int,                                                  \
        double: _rcat_charp_double,                                            \
        char *: _rcat_charp_charp,                                             \
        char: _rcat_charp_char,                                                \
        bool: _rcat_charp_bool))((x), (y))

char *rgenerate_key() {
    _r_generate_key_current++;
    static char key[100];
    key[0] = 0;
    sprintf(key, "%lld", _r_generate_key_current);
    return key;
}

char *rformat_number(long long lnumber) {
    static char formatted[1024];

    char number[1024] = {0};
    sprintf(number, "%lld", lnumber);

    int len = strlen(number);
    int commas_needed = (len - 1) / 3;
    int new_len = len + commas_needed;

    formatted[new_len] = '\0';

    int i = len - 1;
    int j = new_len - 1;
    int count = 0;

    while (i >= 0) {
        if (count == 3) {
            formatted[j--] = '.';
            count = 0;
        }
        formatted[j--] = number[i--];
        count++;
    }
    if (lnumber < 0)
        formatted[j--] = '-';
    return formatted;
}

bool rstrextractdouble(char *str, double *d1) {
    for (size_t i = 0; i < strlen(str); i++) {
        if (isdigit(str[i])) {
            str += i;
            sscanf(str, "%lf", d1);
            return true;
        }
    }
    return false;
}

void rstrstripslashes(const char *content, char *result) {
    size_t content_length = strlen((char *)content);
    unsigned int index = 0;
    for (unsigned int i = 0; i < content_length; i++) {
        char c = content[i];
        if (c == '\\') {
            i++;
            c = content[i];
            if (c == 'r') {
                c = '\r';
            } else if (c == 't') {
                c = '\t';
            } else if (c == 'b') {
                c = '\b';
            } else if (c == 'n') {
                c = '\n';
            } else if (c == 'f') {
                c = '\f';
            } else if (c == '\\') {
                // No need tbh
                c = '\\';
            }
        }
        result[index] = c;
        index++;
    }
    result[index] = 0;
}

int rstrstartswith(const char *s1, const char *s2) {
    if (s1 == NULL)
        return s2 == NULL;
    if (s1 == s2 || s2 == NULL || *s2 == 0)
        return true;
    size_t len_s2 = strlen(s2);
    size_t len_s1 = strlen(s1);
    if (len_s2 > len_s1)
        return false;
    return !strncmp(s1, s2, len_s2);
}

bool rstrendswith(const char *s1, const char *s2) {
    if (s1 == NULL)
        return s2 == NULL;
    if (s1 == s2 || s2 == NULL || *s2 == 0)
        return true;
    size_t len_s2 = strlen(s2);
    size_t len_s1 = strlen(s1);
    if (len_s2 > len_s1) {
        return false;
    }
    s1 += len_s1 - len_s2;
    return !strncmp(s1, s2, len_s2);
}

void rstraddslashes(const char *content, char *result) {
    size_t content_length = strlen((char *)content);
    unsigned int index = 0;
    for (unsigned int i = 0; i < content_length; i++) {
        if (content[i] == '\r') {
            result[index] = '\\';
            index++;
            result[index] = 'r';
            index++;
            continue;
        } else if (content[i] == '\t') {
            result[index] = '\\';
            index++;
            result[index] = 't';
            index++;
            continue;
        } else if (content[i] == '\n') {
            result[index] = '\\';
            index++;
            result[index] = 'n';
            index++;
            continue;
        } else if (content[i] == '\\') {
            result[index] = '\\';
            index++;
            result[index] = '\\';
            index++;
            continue;
        } else if (content[i] == '\b') {
            result[index] = '\\';
            index++;
            result[index] = 'b';
            index++;
            continue;
        } else if (content[i] == '\f') {
            result[index] = '\\';
            index++;
            result[index] = 'f';
            index++;
            continue;
        } else if (content[i] == '"') {
            result[index] = '\\';
            index++;
            result[index] = '"';
            index++;
            continue;
        }
        result[index] = content[i];
        index++;
    }
    result[index] = 0;
}

int rstrip_whitespace(char *input, char *output) {
    output[0] = 0;
    int count = 0;
    size_t len = strlen(input);
    for (size_t i = 0; i < len; i++) {
        if (input[i] == '\t' || input[i] == ' ' || input[i] == '\n') {
            continue;
        }
        count = i;
        size_t j;
        for (j = 0; j < len - count; j++) {
            output[j] = input[j + count];
        }
        output[j] = '\0';
        break;
    }
    return count;
}

/*
 * Converts "pony" to \"pony\". Addslashes does not
 * Converts "pony\npony" to "pony\n"
 * 			    "pony"
 */
void rstrtocstring(const char *input, char *output) {
    int index = 0;
    char clean_input[strlen(input) * 2];
    char *iptr = clean_input;
    rstraddslashes(input, clean_input);
    output[index] = '"';
    index++;
    while (*iptr) {
        if (*iptr == '"') {
            output[index] = '\\';
            output++;
        } else if (*iptr == '\\' && *(iptr + 1) == 'n') {
            output[index] = '\\';
            output++;
            output[index] = 'n';
            output++;
            output[index] = '"';
            output++;
            output[index] = '\n';
            output++;
            output[index] = '"';
            output++;
            iptr++;
            iptr++;
            continue;
        }
        output[index] = *iptr;
        index++;
        iptr++;
    }
    if (output[index - 1] == '"' && output[index - 2] == '\n') {
        output[index - 1] = 0;
    } else if (output[index - 1] != '"') {
        output[index] = '"';
        output[index + 1] = 0;
    }
}

size_t rstrtokline(char *input, char *output, size_t offset, bool strip_nl) {

    size_t len = strlen(input);
    output[0] = 0;
    size_t new_offset = 0;
    size_t j;
    size_t index = 0;

    for (j = offset; j < len + offset; j++) {
        if (input[j] == 0) {
            index++;
            break;
        }
        index = j - offset;
        output[index] = input[j];

        if (output[index] == '\n') {
            index++;
            break;
        }
    }
    output[index] = 0;

    new_offset = index + offset;

    if (strip_nl) {
        if (output[index - 1] == '\n') {
            output[index - 1] = 0;
        }
    }
    return new_offset;
}

void rstrjoin(char **lines, size_t count, char *glue, char *output) {
    output[0] = 0;
    for (size_t i = 0; i < count; i++) {
        strcat(output, lines[i]);
        if (i != count - 1)
            strcat(output, glue);
    }
}

int rstrsplit(char *input, char **lines) {
    int index = 0;
    size_t offset = 0;
    char line[1024];
    while ((offset = rstrtokline(input, line, offset, false)) && *line) {
        if (!*line) {
            break;
        }
        lines[index] = (char *)malloc(strlen(line) + 1);
        strcpy(lines[index], line);
        index++;
    }
    return index;
}

bool rstartswithnumber(char *str) { return isdigit(str[0]); }

void rstrmove2(char *str, unsigned int start, size_t length,
               unsigned int new_pos) {
    size_t str_len = strlen(str);
    char new_str[str_len + 1];
    memset(new_str, 0, str_len);
    if (start < new_pos) {
        strncat(new_str, str + length, str_len - length - start);
        new_str[new_pos] = 0;
        strncat(new_str, str + start, length);
        strcat(new_str, str + strlen(new_str));
        memset(str, 0, str_len);
        strcpy(str, new_str);
    } else {
        strncat(new_str, str + start, length);
        strncat(new_str, str, start);
        strncat(new_str, str + start + length, str_len - start);
        memset(str, 0, str_len);
        strcpy(str, new_str);
    }
    new_str[str_len] = 0;
}

void rstrmove(char *str, unsigned int start, size_t length,
              unsigned int new_pos) {
    size_t str_len = strlen(str);
    if (start >= str_len || new_pos >= str_len || start + length > str_len) {
        return;
    }
    char temp[length + 1];
    strncpy(temp, str + start, length);
    temp[length] = 0;
    if (start < new_pos) {
        memmove(str + start, str + start + length, new_pos - start);
        strncpy(str + new_pos - length + 1, temp, length);
    } else {
        memmove(str + new_pos + length, str + new_pos, start - new_pos);
        strncpy(str + new_pos, temp, length);
    }
}

int cmp_line(const void *left, const void *right) {
    char *l = *(char **)left;
    char *r = *(char **)right;

    char lstripped[strlen(l) + 1];
    rstrip_whitespace(l, lstripped);
    char rstripped[strlen(r) + 1];
    rstrip_whitespace(r, rstripped);

    double d1, d2;
    bool found_d1 = rstrextractdouble(lstripped, &d1);
    bool found_d2 = rstrextractdouble(rstripped, &d2);

    if (found_d1 && found_d2) {
        double frac_part1;
        double int_part1;
        frac_part1 = modf(d1, &int_part1);
        double frac_part2;
        double int_part2;
        frac_part2 = modf(d2, &int_part2);
        if (d1 == d2) {
            return strcmp(lstripped, rstripped);
        } else if (frac_part1 && frac_part2) {
            return d1 > d2;
        } else if (frac_part1 && !frac_part2) {
            return 1;
        } else if (frac_part2 && !frac_part1) {
            return -1;
        } else if (!frac_part1 && !frac_part2) {
            return d1 > d2;
        }
    }
    return 0;
}

int rstrsort(char *input, char *output) {
    char **lines = (char **)malloc(strlen(input) * 10);
    int line_count = rstrsplit(input, lines);
    qsort(lines, line_count, sizeof(char *), cmp_line);
    rstrjoin(lines, line_count, "", output);
    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
    }
    free(lines);
    return line_count;
}

#endif
