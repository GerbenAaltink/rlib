#ifndef RHASHTABLE_H
#define RHASHTABLE_H
/*
    ORIGINAL SOURCE IS FROM K&R
 */

#include "rmalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASHSIZE 101

// Structure for the table entries
typedef struct rnlist {
    struct rnlist *next;
    char *name;
    char *defn;
} rnlist;

// Hash table array
static rnlist *rhashtab[HASHSIZE];

// Hash function
unsigned rhash(char *s) {
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}

rnlist *rlget(char *s) {
    rnlist *np;
    for (np = rhashtab[rhash(s)]; np != NULL; np = np->next)
        if (strcmp(s, np->name) == 0)
            return np; // Found
    return NULL;       // Not found
}

// Lookup function
char *rget(char *s) {
    rnlist *np = rlget(s);
    return np ? np->defn : NULL;
}

// Install function (adds a name and definition to the table)
struct rnlist *rset(char *name, char *defn) {
    struct rnlist *np = NULL;
    unsigned hashval;

    if ((rlget(name)) == NULL) { // Not found
        np = (struct rnlist *)malloc(sizeof(*np));
        if (np == NULL || (np->name = strdup(name)) == NULL)
            return NULL;
        hashval = rhash(name);
        np->next = rhashtab[hashval];
        rhashtab[hashval] = np;
    } else {
        if (np->defn)
            free((void *)np->defn);
        np->defn = NULL;
    }
    if ((np->defn = strdup(defn)) == NULL)
        return NULL;
    return np;
}
#endif
