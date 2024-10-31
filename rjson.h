#ifndef RJSON_H
#define RJSON_H
#include "rmalloc.h"
#include "rtypes.h"
#include "rstring.h"
#include "rtemp.h"
#include "rtime.h"
#include "rtest.h"

typedef struct rjson_t {
    char *content;
    size_t length;
    size_t size;
} rjson_t;

rjson_t *rjson() {
    rjson_t *json = rmalloc(sizeof(rjson_t));
    json->size = 1024;
    json->length = 0;
    json->content = (char *)rmalloc(json->size);
    json->content[0] = 0;
    return json;
}

void rjson_write(rjson_t *rjs, char *content) {
    size_t len = strlen(content);
    while (rjs->size < rjs->length + len + 1) {
        rjs->content = realloc(rjs->content, rjs->size + 1024);
        rjs->size += 1024;
    }
    strcat(rjs->content, content);
    rjs->length += len;
}

void rjson_object_start(rjson_t *rjs) {
    if (rstrendswith(rjs->content, "}"))
        rjson_write(rjs, ",");
    rjson_write(rjs, "{");
}
void rjson_object_close(rjson_t *rjs) {
    if (rstrendswith(rjs->content, ",")) {
        rjs->content[rjs->length - 1] = 0;
        rjs->length--;
    }
    rjson_write(rjs, "}");
}
void rjson_array_start(rjson_t *rjs) {
    if (rjs->length && (rstrendswith(rjs->content, "}") || rstrendswith(rjs->content, "]")))
        rjson_write(rjs, ",");
    rjson_write(rjs, "[");
}
void rjson_array_close(rjson_t *rjs) {
    if (rstrendswith(rjs->content, ",")) {
        rjs->content[rjs->length - 1] = 0;
        rjs->length--;
    }
    rjson_write(rjs, "]");
}

void rjson_kv_string(rjson_t *rjs, char *key, char *value) {
    if (rjs->length && !rstrendswith(rjs->content, "{") && !rstrendswith(rjs->content, "[")) {
        rjson_write(rjs, ",");
    }
    rjson_write(rjs, "\"");
    rjson_write(rjs, key);
    rjson_write(rjs, "\":\"");
    char *value_str = (char *)rmalloc(strlen(value) + 4096);
    rstraddslashes(value, value_str);
    rjson_write(rjs, value_str);
    free(value_str);
    rjson_write(rjs, "\"");
}

void rjson_kv_int(rjson_t *rjs, char *key, ulonglong value) {
    if (rjs->length && !rstrendswith(rjs->content, "{") && !rstrendswith(rjs->content, "[")) {
        rjson_write(rjs, ",");
    }
    rjson_write(rjs, "\"");
    rjson_write(rjs, key);
    rjson_write(rjs, "\":");
    char value_str[100] = {0};
    sprintf(value_str, "%lld", value);
    rjson_write(rjs, value_str);
}
void rjson_kv_number(rjson_t *rjs, char *key, ulonglong value) {
    if (rjs->length && !rstrendswith(rjs->content, "{") && !rstrendswith(rjs->content, "[")) {
        rjson_write(rjs, ",");
    }
    rjson_write(rjs, "\"");
    rjson_write(rjs, key);
    rjson_write(rjs, "\":");
    rjson_write(rjs, "\"");

    rjson_write(rjs, sbuf(rformat_number(value)));
    rjson_write(rjs, "\"");
}

void rjson_kv_bool(rjson_t *rjs, char *key, int value) {
    if (rjs->length && !rstrendswith(rjs->content, "{") && !rstrendswith(rjs->content, "[")) {
        rjson_write(rjs, ",");
    }
    rjson_write(rjs, "\"");
    rjson_write(rjs, key);
    rjson_write(rjs, "\":");
    rjson_write(rjs, value > 0 ? "true" : "false");
}

void rjson_kv_duration(rjson_t *rjs, char *key, nsecs_t value) {
    if (rjs->length && !rstrendswith(rjs->content, "{") && !rstrendswith(rjs->content, "[")) {
        rjson_write(rjs, ",");
    }
    rjson_write(rjs, "\"");
    rjson_write(rjs, key);
    rjson_write(rjs, "\":");
    rjson_write(rjs, "\"");

    rjson_write(rjs, sbuf(format_time(value)));
    rjson_write(rjs, "\"");
}
void rjson_free(rjson_t *rsj) {
    free(rsj->content);
    free(rsj);
}

void rjson_key(rjson_t *rsj, char *key) {
    rjson_write(rsj, "\"");
    rjson_write(rsj, key);
    rjson_write(rsj, "\":");
}
#endif