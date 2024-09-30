#include "rjson.h"

int main() {
    rtest_banner("rjson");
    rjson_t *json = rjson();
    rjson_array_start(json);
    rjson_object_start(json);
    rjson_kv_string(json, "string", "value");
    rjson_kv_number(json, "number", 1337421984);
    rjson_kv_duration(json, "duration", 1337421984);
    rjson_kv_int(json, "ulonglong", 1337420);
    rjson_kv_bool(json, "bool", true);
    rjson_object_close(json);
    rjson_array_close(json);
    rassert(!strcmp(
        json->content,
        "[{\"string\":\"value\",\"number\":\"1.337.421.984\",\"duration\":\"1."
        "34s\",\"ulonglong\":1337420,\"bool\":true}]"));
    rjson_free(json);
    return rtest_end("");
}