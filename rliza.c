#include "rliza.h"

int main() {

    rliza_t *rliza = rliza_new(RLIZA_OBJECT);
    rliza_set_integer(rliza, "a", 1);
    rliza_set_integer(rliza, "b", 2);
    rliza_set_integer(rliza, "c", 3);
    rliza_set_integer(rliza, "d", 4);
    rliza_set_integer(rliza, "e", 5);
    rliza_set_integer(rliza, "f", 6);
    rliza_set_string(rliza, "str1", "str1value");

    rliza_t *arr = rliza_new_key_array("testje");
    rliza_t *obj = rliza_new_integer(1337);
    rliza_push(arr, obj);
    obj = rliza_new_integer(420);
    rliza_push(arr, obj);
    obj = rliza_new_integer(42);
    rliza_push(arr, obj);
    obj = rliza_new_number(19.84);
    rliza_push(arr, obj);
    obj = rliza_new_boolean(true);
    rliza_push(arr, obj);
    rliza_set_object(rliza, arr);
    rliza_set_object(rliza, arr);
    rliza_set_object(rliza, arr);
    rliza_set_object(rliza, arr);
    unsigned char *content = rliza_object_to_string(rliza);
    unsigned char *original_content = content;

    printf("%s\n", content);
    rliza_t *rliza2 = rliza_object_from_string((char **)&content);
    unsigned char *content2 = rliza_object_to_string(rliza2);
    content = original_content;

    unsigned char *content3 = rliza_object_to_string(rliza2);
    printf("<content1:%s>\n", content);
    printf("<content2:%s>\n", content2);
    printf("<content3:%s>\n", content3);

    assert(!ustrncmp(content2, content3, strlen((char *)content2)));
    assert(!ustrncmp(content, content2, strlen((char *)content)));

    printf("%s\n", (unsigned char *)rliza_coalesce(rliza_get_string(rliza, "a"),
                                                   "WHOOPS"));
    printf("%s\n", (unsigned char *)rliza_coalesce(rliza_get_string(rliza, "b"),
                                                   "Whoops2"));
    printf("%s\n", (unsigned char *)rliza_coalesce(rliza_get_string(rliza, "c"),
                                                   "whoops3"));
    printf("%s\n", (unsigned char *)rliza_coalesce(rliza_get_string(rliza, "d"),
                                                   "whoops4"));
    printf("%s\n", (unsigned char *)rliza_coalesce(rliza_get_string(rliza, "e"),
                                                   "whoops5"));
    printf("%s\n", (unsigned char *)rliza_coalesce(rliza_get_string(rliza, "f"),
                                                   "woops6"));

    rliza_free(rliza);
    rliza_free(rliza2);
    return 0;
}