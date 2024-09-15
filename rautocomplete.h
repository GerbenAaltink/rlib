#ifndef RAUTOCOMPLETE_H
#define RAUTOCOMPLETE_H
#define R4_DEBUG
#include "rrex4.h"
#include "rstring_list.h"
#define rautocomplete_new rstring_list_new
#define rautocomplete_free rstring_list_free
#define rautocomplete_add rstring_list_add
#define rautocomplete_find rstring_list_find
#define rautocomplete_t rstring_list_t
#define rautocomplete_contains rstring_list_contains

char *r4_escape(char *content) {
    size_t size = strlen(content) * 2 + 1;
    char *escaped = (char *)calloc(size, sizeof(char));
    char *espr = escaped;
    char *to_escape = "?*+()[]{}^$\\";
    *espr = '(';
    espr++;
    while (*content) {
        if (strchr(to_escape, *content)) {
            *espr = '\\';
            espr++;
        }
        *espr = *content;
        espr++;
        content++;
    }
    *espr = '.';
    espr++;
    *espr = '+';
    espr++;
    *espr = ')';
    espr++;
    *espr = 0;
    return escaped;
}

char *rautocomplete_find(rstring_list_t *list, char *expr) {
    if (!list->count)
        return NULL;
    if (!expr || !strlen(expr))
        return NULL;

    char *escaped = r4_escape(expr);

    for (unsigned int i = list->count - 1; i >= 0; i--) {
        if (i == -1)
            break;
        char *match;
        r4_t *r = r4(list->strings[i], escaped);
        if (r->valid && r->match_count == 1) {
            match = strdup(r->matches[0]);
        }
        r4_free(r);
        if (match) {

            free(escaped);
            return match;
        }
    }
    free(escaped);
    return NULL;
}
#endif