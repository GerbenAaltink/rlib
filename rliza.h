#ifndef RLIZA_H
#define RLIZA_H
#include <assert.h>
#include "rmalloc.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "rbuffer.h"
#include "rstring.h"

typedef enum rliza_type_t {
    RLIZA_STRING = 's',
    RLIZA_BOOLEAN = 'b',
    RLIZA_NUMBER = 'n',
    RLIZA_OBJECT = 'o',
    RLIZA_ARRAY = 'a',
    RLIZA_NULL = 0,
    RLIZA_KEY = 'k',
    RLIZA_INTEGER = 'i'
} rliza_type_t;

typedef struct rliza_t {
    rliza_type_t type;
    struct rliza_t *value;
    char *key;
    union {
        unsigned char *string;
        bool boolean;
        double number;
        struct rliza_t **map;
        long long integer;
    } content;
    unsigned int count;
    unsigned char *(*get_string)(struct rliza_t *, char *);
    long long (*get_integer)(struct rliza_t *, char *);
    double (*get_number)(struct rliza_t *, char *);
    bool (*get_boolean)(struct rliza_t *, char *);
    struct rliza_t *(*get_array)(struct rliza_t *, char *);
    struct rliza_t *(*get_object)(struct rliza_t *, char *);
    void (*set_string)(struct rliza_t *, char *, char *);
    void (*set_integer)(struct rliza_t *, char *, long long);
    void (*set_number)(struct rliza_t *, char *, double);
    void (*set_boolean)(struct rliza_t *, char *, bool);
    void (*set_array)(struct rliza_t *self, char *key, struct rliza_t *array);
    void (*set_object)(struct rliza_t *self, char *key, struct rliza_t *object);
} rliza_t;

void rliza_free(rliza_t *rliza) {
    if (rliza->key) {
        free(rliza->key);
        rliza->key = NULL;
    }
    if (rliza->value) {
        rliza_free(rliza->value);
        rliza->value = NULL;
    }
    // if (rliza->content.array) {
    //     printf("JAAAA\n");
    // }
    // if (rliza->content.object) {
    //     rliza_free(rliza->content.object);
    //    rliza->content.object = NULL;
    //}
    if (rliza->type == RLIZA_STRING) {
        if (rliza->content.string) {
            free(rliza->content.string);
            rliza->content.string = NULL;
            // else if (rliza->type == RLIZA_NUMBER) {
            //    printf("STDring freed\n");
        }
    } else if (rliza->type == RLIZA_OBJECT || rliza->type == RLIZA_ARRAY) {

        if (rliza->content.map) {
            for (unsigned int i = 0; i < rliza->count; i++) {
                rliza_free(rliza->content.map[i]);
            }
            free(rliza->content.map);
        }
    }
    // free(rliza->content.array);
    //}

    free(rliza);
}

rliza_t *rliza_new(rliza_type_t type);
rliza_t *rliza_new_string(char *string);
rliza_t *rliza_new_null();
rliza_t *rliza_new_boolean(bool value);
rliza_t *rliza_new_number(double value);
rliza_t *rliza_new_integer(long long value);
rliza_t *rliza_new_key_value(char *key, rliza_t *value);
rliza_t *rliza_new_key_string(char *key, char *string);
rliza_t *rliza_new_key_bool(char *key, bool value);
rliza_t *rliza_new_key_number(char *key, double value);
void rliza_push(rliza_t *self, rliza_t *obj);
void rliza_set_object(rliza_t *self, rliza_t *object);
void rliza_set_string(rliza_t *self, char *key, char *string);
void rliza_set_boolean(rliza_t *self, char *key, bool value);
void rliza_set_number(rliza_t *self, char *key, double value);
void rliza_set_integer(rliza_t *self, char *key, long long value);
unsigned char *rliza_get_string(rliza_t *self, char *key);
long long rliza_get_integer(rliza_t *self, char *key);
double rliza_get_number(rliza_t *self, char *key);
bool rliza_get_boolean(rliza_t *self, char *key);
rliza_t *rliza_get_array(rliza_t *self, char *key);
rliza_t *rliza_get_object(rliza_t *self, char *key);
void rliza_set_array(rliza_t *self, char *key, rliza_t *array);

unsigned char *rliza_get_string(rliza_t *self, char *key) {
    for (unsigned int i = 0; i < self->count; i++) {
        if (self->content.map[i]->key != NULL &&
            strcmp(self->content.map[i]->key, key) == 0) {
            if (self->content.map[i]->type == RLIZA_STRING ||
                self->content.map[i]->type == RLIZA_NULL) {
                return self->content.map[i]->content.string;
            }
        }
    }
    return NULL;
}
long long rliza_get_integer(rliza_t *self, char *key) {
    for (unsigned int i = 0; i < self->count; i++) {
        if (self->content.map[i]->key != NULL &&
            strcmp(self->content.map[i]->key, key) == 0) {
            if (self->content.map[i]->type == RLIZA_INTEGER ||
                self->content.map[i]->type == RLIZA_NULL) {
                return self->content.map[i]->content.integer;
            }
        }
    }
    return 0;
}

double rliza_get_number(rliza_t *self, char *key) {
    for (unsigned int i = 0; i < self->count; i++) {
        if (self->content.map[i]->key != NULL &&
            strcmp(self->content.map[i]->key, key) == 0) {
            if (self->content.map[i]->type == RLIZA_NUMBER ||
                self->content.map[i]->type == RLIZA_NULL) {
                return self->content.map[i]->content.number;
            }
        }
    }
    return 0;
}

bool rliza_get_boolean(rliza_t *self, char *key) {
    for (unsigned int i = 0; i < self->count; i++) {
        if (self->content.map[i]->key != NULL &&
            strcmp(self->content.map[i]->key, key) == 0) {
            if (self->content.map[i]->type == RLIZA_BOOLEAN ||
                self->content.map[i]->type == RLIZA_NULL) {
                return self->content.map[i]->content.boolean;
            }
        }
    }
    return false;
}

rliza_t *rliza_get_object(rliza_t *self, char *key) {
    for (unsigned int i = 0; i < self->count; i++) {
        if (self->content.map[i]->key != NULL &&
            strcmp(self->content.map[i]->key, key) == 0) {
            //  if(self->content.map[i]->type == RLIZA_OBJECT ||
            //  self->content.map[i]->type == RLIZA_NULL){
            return self->content.map[i];
            ;
            //}
        }
    }
    return NULL;
}

rliza_t *rliza_get_array(rliza_t *self, char *key) {
    for (unsigned int i = 0; i < self->count; i++) {
        if (self->content.map[i]->key != NULL &&
            strcmp(self->content.map[i]->key, key) == 0) {
            if (self->content.map[i]->type == RLIZA_ARRAY ||
                self->content.map[i]->type == RLIZA_NULL) {
                return self->content.map[i];
            }
        }
    }
    return NULL;
}

rliza_t *rliza_new_null() {
    rliza_t *rliza = rliza_new(RLIZA_NULL);
    return rliza;
}
rliza_t *rliza_new_string(char *string) {
    rliza_t *rliza = rliza_new(RLIZA_STRING);
    if (string == NULL) {
        rliza->type = RLIZA_NULL;
        rliza->content.string = NULL;
        return rliza;
    } else {
        rliza->content.string =
            (unsigned char *)strdup(string); // (unsigned char *)new_string;
    }
    return rliza;
}
rliza_t *rliza_new_boolean(bool value) {
    rliza_t *rliza = rliza_new(RLIZA_BOOLEAN);
    rliza->content.boolean = value;
    return rliza;
}

rliza_t *rliza_new_number(double value) {
    rliza_t *rliza = rliza_new(RLIZA_NUMBER);
    rliza->content.number = value;
    return rliza;
}

rliza_t *rliza_new_integer(long long value) {
    rliza_t *rliza = rliza_new(RLIZA_INTEGER);
    rliza->content.integer = value;
    return rliza;
}
rliza_t *rliza_new_key_array(char *key) {
    rliza_t *rliza = rliza_new(RLIZA_ARRAY);
    rliza->key = strdup(key);
    return rliza;
}

rliza_t *rliza_new_key_value(char *key, rliza_t *value) {
    rliza_t *rliza = rliza_new(RLIZA_OBJECT);
    if (key) {
        rliza->key = strdup(key);
    }
    rliza->value = value;
    return rliza;
}

rliza_t *rliza_new_key_string(char *key, char *string) {
    rliza_t *rliza = rliza_new_key_value(key, rliza_new_string(string));
    return rliza;
}
rliza_t *rliza_new_key_bool(char *key, bool value) {
    rliza_t *rliza = rliza_new_key_value(key, rliza_new_boolean(value));
    return rliza;
}
rliza_t *rliza_new_key_number(char *key, double value) {
    rliza_t *rliza = rliza_new_key_value(key, rliza_new_number(value));
    return rliza;
}

void rliza_set_null(rliza_t *self, char *key){
     rliza_t *obj = rliza_get_object(self, key);
     if(!obj){
        obj = rliza_new_null();
        obj->key = strdup(key);
        rliza_set_object(self, obj);
     }else if(obj->type == RLIZA_STRING){
        if(obj->content.string)
            free(obj->content.string);
        obj->content.string = NULL;
     }else if(obj->type == RLIZA_ARRAY || obj->type == RLIZA_OBJECT){
        for(unsigned int i = 0; i < obj->count; i++){
            rliza_free(obj->content.map[i]);
        }
     }else if(obj->type == RLIZA_NUMBER){
        obj->content.number = 0;
     }else if(obj->type == RLIZA_INTEGER){
        obj->content.integer = 0;
     }
     obj->type = RLIZA_NULL;
}
void rliza_set_string(rliza_t *self, char *key, char *string) {
    rliza_t *obj = rliza_get_object(self, key);

    if (!obj) {
        obj = rliza_new_string(string);
        obj->key = strdup(key);
        obj->type = RLIZA_STRING;
        rliza_set_object(self, obj);
    } else {
        char *new_string = (char *)malloc(strlen(string) * 2 + 1);
        new_string[0] = 0;
        rstraddslashes(string, new_string);
        if (obj->content.string)
            free(obj->content.string);
        obj->content.string = (unsigned char *)new_string;
    }
}

void rliza_set_array(rliza_t *self, char *key, rliza_t *array) {
    rliza_t *obj = rliza_get_object(self, key);
    if (obj)
        rliza_free(obj);
    if (array->key) {
        free(array->key);
        array->key = strdup(key);
    }
    rliza_set_object(self, array);
}

void rliza_set_number(rliza_t *self, char *key, double value) {
    rliza_t *obj = rliza_get_object(self, key);
    if (!obj) {
        obj = rliza_new_number(value);
        obj->key = strdup(key);
        obj->type = RLIZA_NUMBER;
        rliza_set_object(self, obj);
    } else {
        obj->content.number = value;
    }
}

void rliza_set_object(rliza_t *self, rliza_t *object) {
    self->content.map =
        realloc(self->content.map, sizeof(rliza_t *) * (self->count + 1));
    self->content.map[self->count] = object;
    self->count++;
}
void rliza_set_integer(rliza_t *self, char *key, long long value) {
    rliza_t *obj = rliza_get_object(self, key);
    if (!obj) {
        obj = rliza_new_integer(value);
        obj->key = strdup(key);
        obj->type = RLIZA_INTEGER;
        rliza_set_object(self, obj);
    } else {
        obj->content.integer = value;
    }
}

void rliza_set_boolean(rliza_t *self, char *key, bool value) {
    rliza_t *obj = rliza_get_object(self, key);
    if (!obj) {
        obj = rliza_new_boolean(value);
        obj->key = strdup(key);
        obj->type = RLIZA_BOOLEAN;

        rliza_set_object(self, obj);
    } else {
        obj->content.integer = value;
    }
}

rliza_t *rliza_new(rliza_type_t type) {
    rliza_t *rliza = calloc(1, sizeof(rliza_t));
    rliza->type = type;
    rliza->key = NULL;
    rliza->count = 0;
    rliza->get_boolean = rliza_get_boolean;
    rliza->get_integer = rliza_get_integer;
    rliza->get_number = rliza_get_number;
    rliza->get_string = rliza_get_string;
    rliza->get_array = rliza_get_array;
    rliza->get_object = rliza_get_object;
    rliza->set_string = rliza_set_string;
    rliza->set_number = rliza_set_number;
    rliza->set_boolean = rliza_set_boolean;
    rliza->set_integer = rliza_set_integer;
    rliza->set_array = rliza_set_array;
    rliza->content.map = NULL;
    rliza->value = NULL;

    return rliza;
}

void *rliza_coalesce(void *result, void *default_value) {
    if (result == NULL)
        return default_value;
    return result;
}

unsigned char *rliza_seek_string(char **content, char *options) {
    char *content_original = *content;
    while (**content) {
        if (**content == '\r' || **content == '\n' || **content == ' ' ||
            **content == '\t' || **content == '\\') {
            if (**content == '\\' && *(*content + 1) == '"') {
                while (true) {
                    (*content)++;
                    if (**content == '\\' && *(*content + 1) == '"') {
                        (*content)++;
                        break;
                    }
                }
            }
            (*content)++;
            continue;
        }

        char options_cpy[100] = {0};
        strcpy(options_cpy, options);
        char *options_ptr = options_cpy;
        char *state = options_ptr;
        char *option = NULL;
        while ((option = strtok_r(option == NULL ? options_ptr : NULL, "|",
                                  &state))) {
            if (!strcmp(option, "\\b")) {
                if (!strncmp(*content, "true", 4) ||
                    !strncmp(*content, "false", 5)) {

                    // printf("Boolean match: %c\n", **content);
                    return (unsigned char *)*content;
                }
            }
            if (!strcmp(option, "\\d")) {
                if (**content >= '0' && **content <= '9') {
                    // printf("Number match: %c\n", **content);
                    return (unsigned char *)*content;
                }
            }

            if (!strncmp(option, *content, strlen(option))) {
                // printf("Literal match: %c\n", **content);
                return (unsigned char *)*content;
            }
        }
        printf("PRE_MISMATCHH: <%s>\n", content_original);
        printf("MISMATCH: %s\n", *content);
        exit(0);
        (*content)++;
    }
    return (unsigned char *)*content;
}

unsigned char *rliza_extract_quotes(char **content) {
    rbuffer_t *buffer = rbuffer_new(NULL, 0);
    assert(**content == '"');
    char previous = 0;
    while (true) {

        (*content)++;

        if (**content == '"' && previous != '\\') {
            break;
        }
        rbuffer_push(buffer, **content);
        previous = **content;
    }
    assert(**content == '"');
    (*content)++;
    rbuffer_push(buffer, 0);
    unsigned char *result = rbuffer_to_string(buffer);
    return result;
}
unsigned char *rliza_object_to_string(rliza_t *rliza);
rliza_t *rliza_object_from_string(char **content) {
    char *token =
        (char *)rliza_seek_string(content, "[|{|\"|\\d|\\b|null|root");
    if (!token)
        return NULL;
    rliza_t *rliza = rliza_new(RLIZA_NULL);
    if (**content == '{') {
        rliza->type = RLIZA_OBJECT;
        (*content)++;
        char *result = NULL;
        while ((result = (char *)rliza_seek_string(content, "\"|,|null|}|object")) !=
                   NULL &&
               *result) {
            if (**content == ',') {
                (*content)++;
                continue;
            }
            unsigned char *key = NULL;
            if (**content == '"') {
                key = rliza_extract_quotes((char **)content);
                char *escaped_key = (char *)malloc(strlen((char *)key) * 2 + 1);
                rstrstripslashes((char *)key, escaped_key);
                assert(rliza_seek_string(content, ":|keystr"));
                (*content)++;

                rliza_t *value = rliza_object_from_string(content);
                if (value->key)
                    free(value->key);
                value->key = escaped_key;
                free(key);
                rliza_set_object(rliza, value);
                // printf("<<<<<<<<%s>>>>>>>>>>\n",value->content.string);
            } else if (**content == '}') {
                (*content)++;
            } else if (!strncmp(*content, "null", 4))  {
                (*content) += 4;
                /*
                unsigned char * key = (unsigned char *)malloc(5);
                strcpy((char *)key, "null");
                (*content) += 4;
                char *escaped_key = (char *)malloc(strlen((char *)key) * 2 + 1);
                rstrstripslashes((char *)key, escaped_key);
                assert(rliza_seek_string(content, ":|keystr"));
                (*content)++;

                rliza_t *value = rliza_object_from_string(content);
                if (value->key)
                    free(value->key);
                value->key = escaped_key;
                free(key);
                rliza_set_object(rliza, value);*/
            } else  {
                 assert(false && "Parse error.");
            }
        };
        return rliza;
    } else if (**content == '[') {
        rliza->type = RLIZA_ARRAY;
        (*content)++;
        char *result;
        while ((result = (char *)rliza_seek_string(
                    content, "{|[|\"|\\d|\\b|,|]|null|array")) != NULL &&
               *result) {
            if (**content == ',') {
                (*content)++;
                continue;

            } else if (**content == ']') {
                (*content)++;
                break;
            }
            rliza_t *obj = rliza_object_from_string(content);
            rliza_push(rliza, obj);
        }
        return rliza;
    } else if (**content >= '0' && **content <= '9') {
        char *ptr = *content;
        bool is_decimal = false;
        while (*ptr) {
            if (*ptr == '.') {
                is_decimal = true;
            } else if (!isdigit(*ptr)) {
                break;
            }
            ptr++;
        }
        if (is_decimal) {
            rliza->type = RLIZA_NUMBER;
            rliza->content.number = strtod((char *)*content, NULL);
        } else {
            rliza->type = RLIZA_INTEGER;
            rliza->content.integer = strtoll(*content, NULL, 10);
        }
        // printf("Parsed number: %lld\n", rliza->content.integer);
        while (isdigit(**content) || (is_decimal && **content == '.')) {
            (*content)++;
        }
        return rliza;
    } else if (!strncmp(*content, "true", 4)) {
        rliza->type = RLIZA_BOOLEAN;
        rliza->content.boolean = true;
        *content += 4;
        return rliza;
    } else if (!strncmp(*content, "false", 5)) {
        rliza->type = RLIZA_BOOLEAN;
        rliza->content.boolean = false;
        *content += 5;
        return rliza;
    } else if (!strncmp(*content, "null", 4)) {
        rliza->type = RLIZA_NULL;
        *content += 4;
        return rliza;
    } else if (**content == '"') {
        unsigned char *extracted = rliza_extract_quotes((char **)content);
        unsigned char *extracted_without_slashes =
            (unsigned char *)malloc(strlen((char *)extracted) + 1);
        extracted_without_slashes[0] = 0;
        rstrstripslashes((char *)extracted, (char *)extracted_without_slashes);
        rliza->type = RLIZA_STRING;
        ;
        rliza->content.string = extracted_without_slashes;
        free(extracted);
        return rliza;
    }

    assert(false && "Rliza Overflow.");
    return rliza;
}
unsigned char *rliza_object_to_string(rliza_t *rliza) {
    size_t size = 4096;
    unsigned char *content = (unsigned char *)malloc(size);
    content[0] = 0;
    if (rliza->type == RLIZA_INTEGER) {
        if (rliza->key) {
            sprintf((char *)content, "\"%s\":%lld", rliza->key,
                    rliza->content.integer);
        } else {
            sprintf((char *)content, "%lld", rliza->content.integer);
        }
    } else if (rliza->type == RLIZA_STRING) {
        char *escaped_string =
            (char *)malloc(strlen((char *)rliza->content.string) * 2 + 1);
        rstraddslashes((char *)rliza->content.string, escaped_string);

        if (size < strlen((char *)rliza->content.string) * 2 + 1) {
            size = strlen((char *)rliza->content.string) * 2 + 1;
            content = realloc(content, size + 1024);
        }
        if (rliza->key) {
            char *escaped_key =
                (char *)malloc(strlen((char *)rliza->key) * 2 + 1);
            rstraddslashes((char *)rliza->key, escaped_key);
            sprintf((char *)content, "\"%s\":\"%s\"", escaped_key,
                    escaped_string);
            free(escaped_key);
            // rliza->content.string);
        } else {

            sprintf((char *)content, "\"%s\"",
                    escaped_string); // rliza->content.string);
        }
        free(escaped_string);
    } else if (rliza->type == RLIZA_NUMBER) {
        if (rliza->key) {
            sprintf((char *)content, "\"%s\":%f", rliza->key,
                    rliza->content.number);
        } else {
            sprintf((char *)content, "%f", rliza->content.number);
        }
    } else if (rliza->type == RLIZA_BOOLEAN) {
        if (rliza->key) {
            sprintf((char *)content, "\"%s\":%s", rliza->key,
                    rliza->content.boolean ? "true" : "false");
        } else {
            sprintf((char *)content, "%s",
                    rliza->content.boolean ? "true" : "false");
        }
    } else if (rliza->type == RLIZA_OBJECT) {
        strcpy((char *)content, "{");
        for (unsigned i = 0; i < rliza->count; i++) {
            unsigned char *content_chunk =
                rliza_object_to_string(rliza->content.map[i]);
            if (strlen((char *)content_chunk) + strlen((char *)content) >
                size) {
                size += strlen((char *)content_chunk) + 1;
                realloc(content, size);
            }
            strcat((char *)content, (char *)content_chunk);
            free(content_chunk);
            strcat((char *)content, ",");
        }
        content[strlen((char *)content) - 1] = 0;
        strcat((char *)content, "}");
    } else if (rliza->type == RLIZA_ARRAY) {
        if (rliza->key) {
            char *escaped_key =
                (char *)malloc(strlen((char *)rliza->key) * 2 + 1);
            rstraddslashes((char *)rliza->key, escaped_key);

            sprintf((char *)content, "\"%s\":[", escaped_key);
            free(escaped_key);
        } else
            strcpy((char *)content, "[");

        for (unsigned i = 0; i < rliza->count; i++) {
            unsigned char *content_chunk =
                rliza_object_to_string(rliza->content.map[i]);
            if (strlen((char *)content_chunk) + strlen((char *)content) >
                size) {
                size += strlen((char *)content_chunk) + 1;
                realloc(content, size);
            }
            strcat((char *)content, (char *)content_chunk);
            free(content_chunk);
            strcat((char *)content, ",");
        }
        if (content[strlen((char *)content) - 1] != '[')
            content[strlen((char *)content) - 1] = 0;
        strcat((char *)content, "]");
    } else if (rliza->type == RLIZA_NULL) {
        
        if (rliza->key) {
            char *escaped_key =
                (char *)malloc(strlen((char *)rliza->key) * 2 + 1);
            rstraddslashes((char *)rliza->key, escaped_key);

            sprintf((char *)content, "\"%s\":null", escaped_key);
            free(escaped_key);
        } else
            strcpy((char *)content, "null");
    }
    return content;
}

void rliza_push(rliza_t *self, rliza_t *obj) {
    rliza_set_object(self, obj);
    // self->content.array =
    //     realloc(self->content.array, sizeof(rliza_t *) * (self->count + 1));
    // self->content.array[self->count] = obj;
    // self->count++;
}

#endif
