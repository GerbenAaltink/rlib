#ifndef RCASE_H
#define RCASE_H
#include "rio.h"
#include "rmalloc.h"
#include "rprint.h"
#include "rstring.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define RCAMEL_CASE 1
#define RSNAKE_CASE 2
#define RINVALID_CASE 0
#define RCONST_TEST_T 4;

int rdetermine_case(const char *str) {
    int length = strlen(str);

    char p = 0;
    while (*str) {
        if (p == '_' && islower(*str))
            return RSNAKE_CASE;
        if (p != '_' && !isupper(p) && isupper(*str))
            return RCAMEL_CASE;
        p = *str;
        str++;
    }
    return RINVALID_CASE;

    if (length == 0) {
        return RINVALID_CASE;
    }
    if (strchr(str, '_')) {
        if (str[0] == '_' || str[length - 1] == '_' || strstr(str, "__")) {
            return RINVALID_CASE;
        }
        for (int i = 0; i < length; i++) {
            if (!islower(str[i]) && str[i] != '_') {
                return RINVALID_CASE;
            }
        }
        return RSNAKE_CASE;
    } else {

        if (!islower(str[0])) {
            return RINVALID_CASE;
        }
        for (int i = 1; i < length; i++) {
            if (str[i] == '_') {
                return RINVALID_CASE;
            }
            if (isupper(str[i]) && isupper(str[i - 1])) {
                return RINVALID_CASE;
            }
        }
        return RCAMEL_CASE;
    }
}

char *rsnake_to_camel(const char *snake_case) {
    int length = strlen(snake_case);
    char *camel_case = (char *)malloc(length + 1);
    int j = 0;
    int toUpper = 0;

    for (int i = 0; i < length; i++) {
        if (i > 0 && snake_case[i] == '_' && snake_case[i + 1] == 'T') {
            toUpper = 1;
            if (snake_case[i + 1] == 'T' && (snake_case[i + 2] != '\n' || snake_case[i + 2] != '\0' || snake_case[i + 2] != ' ')) {

                toUpper = 0;
            }
        }
        if (snake_case[i] == '_' && snake_case[i + 1] != 't') {
            toUpper = 1;
            if (snake_case[i + 1] == 't' && (snake_case[i + 2] != '\n' || snake_case[i + 2] != '\0' || snake_case[i + 2] != ' ')) {
                toUpper = 0;
            }
        } else if (snake_case[i] == '_' && snake_case[i + 1] == 't' && !isspace(snake_case[i + 2])) {
            toUpper = 1;
        } else if (snake_case[i] == '_' && snake_case[i + 1] == 'T' && !isspace(snake_case[i + 2])) {
            toUpper = 1;
            camel_case[j++] = '_';
            j++;
        } else {
            if (toUpper) {
                camel_case[j++] = toupper(snake_case[i]);
                toUpper = 0;
            } else {
                camel_case[j++] = snake_case[i];
            }
        }
    }

    camel_case[j] = '\0';
    return camel_case;
}
char *rcamel_to_snake(const char *camelCase) {
    int length = strlen(camelCase);
    char *snake_case = (char *)malloc(2 * length + 1);
    int j = 0;

    for (int i = 0; i < length; i++) {
        if (isupper(camelCase[i])) {
            if (i != 0) {
                snake_case[j++] = '_';
            }
            snake_case[j++] = tolower(camelCase[i]);
        } else {
            snake_case[j++] = camelCase[i];
        }
    }

    snake_case[j] = '\0';
    return snake_case;
}

char *rflip_case(char *content) {
    if (rdetermine_case(content) == RSNAKE_CASE) {
        return rcamel_to_snake(content);
    } else if (rdetermine_case(content) == RCAMEL_CASE) {
        return rsnake_to_camel(content);
    } else {
        rprintr("Could not determine case\n");
        return NULL;
    }
}

char *rflip_case_file(char *filepath) {
    size_t file_size = rfile_size(filepath);
    if (file_size == 0) {
        return NULL;
    }
    char *content = (char *)malloc(file_size);
    char *result = NULL;
    if (rfile_readb(filepath, content, file_size)) {
        result = rflip_case(content);
        if (result) {
            free(content);
            return result;
        } else {
            return content;
        }
    }
    return result;
}

int rcase_main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: rcase <file>\n");
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        char *result = rflip_case_file(argv[i]);
        if (result) {
            printf("%s\n", result);
            free(result);
        }
    }
    return 0;
}
#endif
