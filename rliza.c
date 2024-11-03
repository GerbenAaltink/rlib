#define RMALLOC_OVERRIDE 1
#include "rtest.h"
#include "rliza.h"
#include "rio.h"
#include "rbench.h"
void performance_test() {
    size_t size = rfile_size("resources/large.json");
    char *data = malloc(size + 1);
    rfile_readb("resources/large.json", data, size);
    data[size] = 0;
    RBENCH(1, {
        int length = rliza_validate(data);
        (void)length;
    });
    free(data);
}

int main() {
    rtest_banner("rliza");
    rtest_banner("performance test");
    performance_test();
    rtest_banner("serialize/deserialize");
    char *json_content = "{\"error\":\"not \\\"aaa\\\" "
                         "found\",\"rows\":[{\"a\":true},{\"b\":false},{\"c\":null},[1,23],[1,23],[1,23,true,false,5.5,5.505,6.1]]}";

    rassert(rliza_validate("{\"error\":\"not \\\"aaa\\\" found\",\"rows\":[[1,23],[1,23],[1,23,true,false,5.5,5.505]]}"));
    rassert(!rliza_validate("{\"error\":\"not \\\"aaa\\\" found\",\"rows\":[[1,23],[1,23],[1,23,true,false,5.5,5.505,6.]]}"));
    rassert(!rliza_validate("{\"error\":\"not \\\"aaa\\\" found\",\"rows\""));
    rassert(!rliza_validate("{\"error\":\"not \\\"aaa\\\" found\",\"rows\":["));
    rassert(!rliza_validate("{\"error\":\"not \\\"aaa\\\" found\",\"rows"));
    rassert(!rliza_validate("{\"error\":\"not \\\"aaa\\\" found\",\""));
    rassert(!rliza_validate("{\"error\":\"not \\\"aaa\\\" found\","));

    char *double_content = "{}{}[]";
    free(rliza_loads(&double_content));
    rassert(!strcmp(double_content, "{}[]"));
    free(rliza_loads(&double_content));
    rassert(!strcmp(double_content, "[]"));
    char *error_content = "{}*{}";
    free(rliza_loads(&error_content));
    rassert(!strcmp(error_content, "*{}"));
    rliza_loads(&error_content);
    rassert(!strcmp(error_content, "*{}"));

    rassert(rliza_validate("{}"));
    rassert(!rliza_validate("{"));
    rassert(!rliza_validate("}"));

    rassert(!rliza_validate("["));
    rassert(!rliza_validate("]"));
    rassert(!rliza_validate("\\"));
    rassert(!rliza_validate("*"));
    rassert(!rliza_validate("!"));
    char *json_contentp = json_content;
    rliza_t *to_object = rliza_loads(&json_contentp);
    char *to_string = (char *)rliza_dumps(to_object);
    rassert(!strcmp(to_string, json_content));
    printf("\n<%s>\n", to_string);
    printf("<%s>\n", json_content);
    free(to_string);

    rliza_free(to_object);

    // rliza_free(to_object);
    rtest_banner("manually building new object");
    rliza_t *rliza = rliza_new(RLIZA_OBJECT);
    rliza_set_integer(rliza, "a", 1);
    rliza_set_integer(rliza, "b", 2);
    rliza_set_integer(rliza, "c", 3);
    rliza_set_integer(rliza, "d", 4);
    rliza_set_integer(rliza, "e", 5);
    rliza_set_integer(rliza, "f", 6);
    rliza_set_string(rliza, "str1", "str1value");

    char *true_string = strdup("{\"aa\":123}");
    char *true_stringp = true_string;
    rliza_t *true_value = rliza_loads(&true_stringp);
    free(true_string);
    rliza->set_object(rliza, "obj", true_value);
    rliza_free(true_value);
    rliza_t *val = rliza->get_object(rliza, "obj");
    (void)val;
    // printf("val: %d\n", val);
    rliza_set_null(rliza, "q");

    char *original_content = rliza_dumps(rliza);
    printf("%s\n", original_content);
    char *content = original_content;

    printf("%s\n", content, content[strlen((char *)content)] == 0);

    rliza_t *rliza2 = rliza_loads((char **)&content);

    char *content2 = rliza_dumps(rliza2);

    content = original_content;
    rassert(!(strcmp((char *)content,
                     (char *)content2))); // strcmp(content,content2);
    char *content2p = original_content;
    content = original_content;
    rliza_t *rliza3 = rliza_loads((char **)&content2p);
    char *content3 = rliza_dumps(rliza2);

    rtest_banner("compare several serilizations. Should be equal.\n");
    content = original_content;
    printf("content1:<%s>\n", content);
    printf("content2:<%s>\n", content2);
    printf("content3:<%s>\n", content3);
    rassert(!strncmp(content2, content3, strlen((char *)content2)));
    rassert(!strncmp(content, content2, strlen((char *)content)));
    rliza_free(rliza2);
    rliza_free(rliza3);
    free(original_content);
    free(content2);
    free(content3);
    printf("Coalesce %s\n", (char *)rliza_coalesce(rliza_get_string(rliza, "a"), "#1"));
    printf("Coalesce %s\n", (char *)rliza_coalesce(rliza_get_string(rliza, "b"), "#2"));

    rliza_free(rliza);

    return rtest_end("");
}
