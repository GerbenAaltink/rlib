#include "rtest.h"
#include "rliza.h"

int main() {
    rtest_banner("rliza");
    rtest_banner("serialize/deserialize");
    char *json_content =
        "{\"error\":\"not \\\"aaa\\\" found\",\"rows\":[[1,23],[1,23],[1,23]]}";
    char *json_contentp = json_content;
    rliza_t *to_object = rliza_object_from_string(&json_contentp);
    char *to_string = (char *)rliza_object_to_string(to_object);
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
    rliza_set_null(rliza,"q");

    unsigned char *original_content = rliza_object_to_string(rliza);
    unsigned char *content = original_content;

    printf("%s\n", content, content[strlen((char *)content)] == 0);

    rliza_t *rliza2 = rliza_object_from_string((char **)&content);

    unsigned char *content2 = rliza_object_to_string(rliza2);

    content = original_content;
    rassert(!(strcmp((char *)content,
                     (char *)content2))); // strcmp(content,content2);
    unsigned char *content2p = original_content;
    content = original_content;
    rliza_t *rliza3 = rliza_object_from_string((char **)&content2p);
    unsigned char *content3 = rliza_object_to_string(rliza2);

    rtest_banner("compare several serilizations. Should be equal.\n");
    content = original_content;
    printf("content1:<%s>\n", content);
    printf("content2:<%s>\n", content2);
    printf("content3:<%s>\n", content3);
    assert(!ustrncmp(content2, content3, strlen((char *)content2)));
    assert(!ustrncmp(content, content2, strlen((char *)content)));
    rliza_free(rliza2);
    rliza_free(rliza3);
    free(original_content);
    free(content2);
    free(content3);
    printf("Coalesce %s\n",
           (unsigned char *)rliza_coalesce(rliza_get_string(rliza, "a"), "#1"));
    printf("Coalesce %s\n",
           (unsigned char *)rliza_coalesce(rliza_get_string(rliza, "b"), "#2"));

    rliza_free(rliza);

    return rtest_end("");
}
