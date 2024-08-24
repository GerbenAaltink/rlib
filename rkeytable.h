#ifndef RKEYTABLE_H
#define RKEYTABLE_H
/*
    DERIVED FROM HASH TABLE K&R
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct rnklist {
    struct rnklist *next;
    struct rnklist *last;
    char *name;
    char *defn;
} rnklist;

static rnklist *rkeytab = NULL;

rnklist *rlkget(char *s) {
    rnklist *np;
    for (np = rkeytab; np != NULL; np = np->next)
        if (strcmp(s, np->name) == 0)
            return np; // Found
    return NULL;       // Not found
}

char *rkget(char *s) {
    rnklist *np = rlkget(s);
    return np ? np->defn : NULL;
}

rnklist *rkset(char *name, char *defn) {
    rnklist *np;
    if ((np = (rlkget(name))) == NULL) { // Not found
        np = (rnklist *)malloc(sizeof(rnklist));
        np->name = strdup(name);
        np->next = NULL;
        np->last = NULL;

        if (defn) {
            np->defn = strdup(defn);
        } else {
            np->defn = NULL;
        }

        if (rkeytab == NULL) {
            rkeytab = np;
            rkeytab->last = np;
        } else {
            if (rkeytab->last)
                rkeytab->last->next = np;

            rkeytab->last = np;
        }
    } else {
        if (np->defn)
            free((void *)np->defn);
        if (defn) {
            np->defn = strdup(defn);
        } else {
            np->defn = NULL;
        }
    }
    return np;
}
#endif