#include "rbench.h"
#include "rprint.h"
#include "rtree.h"
#include "rhashtable.h"
#include "rkeytable.h"
#include "rtest.h"

#include "hashmap.h"
#include <map>
#include <string>
#include <iostream>

using namespace std;

long long hashit(const char **key) {
    long long hash = 0;
    const char *k = *key;
    for (; *k; k++) {
        hash += *k * 31 + 5;
    }
    return hash;
}
long long hashit2(const char **s) {
    unsigned hashval;
    const char *key = *s;
    for (hashval = 0; *key != '\0'; key++)
        hashval = *key + 31 * hashval;
    return hashval % HASHSIZE;
}

bool keycmp(const char **key1, const char **key2) {
    return !strcmp(*key1, *key2);
}

HashMap(HashMap_name, char *, char *, long long, hashit, keycmp)

    void hashmap_print(const HashMap_name *map) {
    printf("Hash Map:\n");
    printf("\tBuckets: [");
    for (size_t i = 0; i < map->cap; i++) {
        if (i != 0)
            printf(",");
        printf(" %ld", map->buckets[i]);
    }
    printf(" ]\n");

    printf("\tEntries: [");
    for (size_t i = 0; i < map->cap; i++) {
        HashMap_name_entry_t *entry = map->entries + i;
        if (i != 0)
            printf(", ");
        printf("{\n");
        printf("\t\tIndex: %zd,\n", i);
        printf("\t\tKey: \"%s\",\n", entry->key);
        printf("\t\tHash: %lld,\n", entry->key_hash);
        printf("\t\tNext: %ld,\n", entry->next);
        printf("\t\tVal: %s,\n", entry->val);
        printf("\t}");
    }
    printf("]\n");
    printf("\tNext Empty: %zd\n", map->next_empty);
    printf("\tSize: %zd\n", map->size);
    printf("\tCapacity: %zd\n", map->cap);
}

void rbench_table_yuriimap() {
    HashMap_name *map = (HashMap_name *)rbf->data;
    if (rbf->first) {
        _r_generate_key_current = 0;
        HashMap_name *new_map = HashMap_name_new();
        HashMap_name_init(new_map, 0);
        rbf->data = (void *)new_map;
        map = new_map;
    }
    char *key = strdup(rgenerate_key());
    DS_codes_t code =
        HashMap_name_put(map, (const char **)&key, (const char **)&key);
    const char *res = *HashMap_name_get(map, (const char **)&key);
    rasserts(!strcmp(res, key));
    if (rbf->last) {
        HashMap_name_destroy(map);
    }
}

void rbench_table_cppmap() {
    std::map<char *, char *> *map = (std::map<char *, char *> *)rbf->data;
    if (rbf->first) {
        _r_generate_key_current = 0;
        std::map<char *, char *> *f = new std::map<char *, char *>;
        map = f;
        rbf->data = (void *)f;
    }
    for (int i = 0; i < 1; i++) {
        char *key = strdup(rgenerate_key());
        (*map)[key] = key;
        rasserts(!strcmp((*map)[key], key));
    }

    if (rbf->last) {
        delete map;
    }
}

void rbench_table_rtree() {
    rtree_t *tree = (rtree_t *)rbf->data;
    if (rbf->first) {
        _r_generate_key_current = 0;
        tree = rtree_new();
        rbf->data = (void *)tree;
    }
    for (int i = 0; i < 1; i++) {
        char *key = rgenerate_key();
        rtree_set(tree, key, key);
        rasserts(!strcmp((char *)rtree_get(tree, key), key));
    }
    if (rbf->last)
        rtree_free((rtree_t *)rbf->data);
}

void rbench_table_rhashtable() {
    if (rbf->first)
        _r_generate_key_current = 0;
    for (int i = 0; i < 1; i++) {
        char *key = rgenerate_key();
        rset(key, key);
        rasserts(!strcmp(rget(key), key));
    }
}

void rbench_table_rkeytable() {
    if (rbf->first)
        _r_generate_key_current = 0;
    for (int i = 0; i < 1; i++) {
        char *key = rgenerate_key();
        rkset(key, key);
        rasserts(!strcmp(rkget(key), key));
    }
}

nsecs_t total_execution_time = 0;
long total_times = 0;
bool show_progress = 1;

void bench_table(long times) {
    rbench_t *r;
    rprint("\\T B\\l Times: %ld\n", times);
    r = rbench_new();
    r->show_progress = show_progress;
    r->add_function(r, "hashmap", "yurii", rbench_table_yuriimap);
    r->add_function(r, "rtree", "retoor", rbench_table_rtree);
    r->add_function(r, "hashtable", "k&r", rbench_table_rhashtable);
    //  r->add_function(r, "keytable", "k*r", rbench_table_rkeytable);
    r->add_function(r, "map", "cpp", rbench_table_cppmap);
    r->execute(r, times);
    total_execution_time += r->execution_time;
    total_times += times * 2;
    rbench_free(r);
}

int main() {
    show_progress = true;
    long times = 900000;
    bench_table(times);

    return rtest_end("");
}