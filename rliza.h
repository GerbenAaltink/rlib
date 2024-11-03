#ifndef RLIZA_H
#define RLIZA_H
#include "rbuffer.h"
#include "rmalloc.h"
#include "rstring.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        char *string;
        bool boolean;
        double number;
        struct rliza_t **map;
        long long integer;
    } content;
    unsigned int count;
    char *(*get_string)(struct rliza_t *, char *);
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
void rliza_push_object(rliza_t *self, rliza_t *object);
void rliza_set_object(rliza_t *self, char *key, rliza_t *object);
void rliza_set_string(rliza_t *self, char *key, char *string);
void rliza_set_boolean(rliza_t *self, char *key, bool value);
void rliza_set_number(rliza_t *self, char *key, double value);
void rliza_set_integer(rliza_t *self, char *key, long long value);
char *rliza_get_string(rliza_t *self, char *key);
long long rliza_get_integer(rliza_t *self, char *key);
double rliza_get_number(rliza_t *self, char *key);
bool rliza_get_boolean(rliza_t *self, char *key);
rliza_t *rliza_get_array(rliza_t *self, char *key);
rliza_t *rliza_get_object(rliza_t *self, char *key);
void rliza_set_array(rliza_t *self, char *key, rliza_t *array);

char *rliza_dumps(rliza_t *rliza);
rliza_t *rliza_loads(char **content);
rliza_t *_rliza_loads(char **content);

char *rliza_get_string(rliza_t *self, char *key) {
    for (unsigned int i = 0; i < self->count; i++) {
        if (self->content.map[i]->key != NULL && strcmp(self->content.map[i]->key, key) == 0) {
            if (self->content.map[i]->type == RLIZA_STRING || self->content.map[i]->type == RLIZA_NULL) {
                return self->content.map[i]->content.string;
            }
        }
    }
    return NULL;
}
long long rliza_get_integer(rliza_t *self, char *key) {
    for (unsigned int i = 0; i < self->count; i++) {
        if (self->content.map[i]->key != NULL && strcmp(self->content.map[i]->key, key) == 0) {
            if (self->content.map[i]->type == RLIZA_INTEGER || self->content.map[i]->type == RLIZA_NULL) {
                return self->content.map[i]->content.integer;
            }
        }
    }
    return 0;
}

double rliza_get_number(rliza_t *self, char *key) {
    for (unsigned int i = 0; i < self->count; i++) {
        if (self->content.map[i]->key != NULL && strcmp(self->content.map[i]->key, key) == 0) {
            if (self->content.map[i]->type == RLIZA_NUMBER || self->content.map[i]->type == RLIZA_NULL) {
                return self->content.map[i]->content.number;
            }
        }
    }
    return 0;
}

bool rliza_get_boolean(rliza_t *self, char *key) {
    for (unsigned int i = 0; i < self->count; i++) {
        if (self->content.map[i]->key != NULL && strcmp(self->content.map[i]->key, key) == 0) {
            if (self->content.map[i]->type == RLIZA_BOOLEAN || self->content.map[i]->type == RLIZA_NULL) {
                return self->content.map[i]->content.boolean;
            }
        }
    }
    return false;
}

rliza_t *rliza_get_object(rliza_t *self, char *key) {

    for (unsigned int i = 0; i < self->count; i++) {
        if (self->content.map[i]->key != NULL && strcmp(self->content.map[i]->key, key) == 0) {
            //  if(self->content.map[i]->type == RLIZA_OBJECT ||
            //  self->content.map[i]->type == RLIZA_NULL){

            return self->content.map[i]->value;
            ;
            //}
        }
    }
    return NULL;
}

rliza_t *rliza_get_array(rliza_t *self, char *key) {
    for (unsigned int i = 0; i < self->count; i++) {
        if (self->content.map[i]->key != NULL && strcmp(self->content.map[i]->key, key) == 0) {
            if (self->content.map[i]->type == RLIZA_ARRAY || self->content.map[i]->type == RLIZA_NULL) {
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
        rliza->content.string = strdup(string); // (char *)new_string;
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

void rliza_set_null(rliza_t *self, char *key) {
    rliza_t *obj = rliza_get_object(self, key);
    if (!obj) {
        obj = rliza_new_null();
        obj->key = strdup(key);
        rliza_push_object(self, obj);
    }
    if (obj->type == RLIZA_OBJECT) {

        rliza_free(obj->value);
        obj->value = NULL;
    } else if (obj->type == RLIZA_STRING) {
        if (obj->content.string)
            free(obj->content.string);
        obj->content.string = NULL;
    } else if (obj->type == RLIZA_ARRAY) {
        for (unsigned int i = 0; i < obj->count; i++) {
            rliza_free(obj->content.map[i]);
        }
    } else if (obj->type == RLIZA_NUMBER) {
        obj->content.number = 0;
    } else if (obj->type == RLIZA_INTEGER) {
        obj->content.integer = 0;
    }
    obj->type = RLIZA_NULL;
}
rliza_t *rliza_new_object(rliza_t *obj) {
    rliza_t *rliza = rliza_new(RLIZA_OBJECT);
    rliza->value = obj;
    return rliza;
}

rliza_t *rliza_duplicate(rliza_t *rliza) {
    char *str = rliza_dumps(rliza);
    char *strp = str;
    rliza_t *obj = rliza_loads(&strp);
    free(str);
    return obj;
}
void rliza_set_object(rliza_t *self, char *key, rliza_t *value) {
    rliza_t *obj = rliza_duplicate(value);
    obj->key = strdup(key);
    obj->type = RLIZA_OBJECT;

    rliza_push(self, obj);
}

void rliza_set_string(rliza_t *self, char *key, char *string) {
    rliza_t *obj = rliza_get_object(self, key);

    if (!obj) {
        obj = rliza_new_string(string);
        obj->key = strdup(key);
        obj->type = RLIZA_STRING;
        rliza_push_object(self, obj);
    } else {
        obj->content.string = strdup(string);
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
    rliza_push_object(self, array);
}

void rliza_set_number(rliza_t *self, char *key, double value) {
    rliza_t *obj = rliza_get_object(self, key);
    if (!obj) {
        obj = rliza_new_number(value);
        obj->key = strdup(key);
        obj->type = RLIZA_NUMBER;
        rliza_push_object(self, obj);
    } else {
        obj->content.number = value;
    }
}

void rliza_push_object(rliza_t *self, rliza_t *object) {
    self->content.map = realloc(self->content.map, sizeof(rliza_t *) * (self->count + 1));
    self->content.map[self->count] = object;
    self->count++;
}
void rliza_set_integer(rliza_t *self, char *key, long long value) {
    rliza_t *obj = rliza_get_object(self, key);
    if (!obj) {
        obj = rliza_new_integer(value);
        obj->key = strdup(key);
        obj->type = RLIZA_INTEGER;
        rliza_push_object(self, obj);
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

        rliza_push_object(self, obj);
    } else {
        obj->content.integer = value;
    }
}

rliza_t *rliza_new(rliza_type_t type) {
    rliza_t *rliza = (rliza_t *)calloc(1, sizeof(rliza_t));
    rliza->type = type;
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
    rliza->set_object = rliza_set_object;

    return rliza;
}

void *rliza_coalesce(void *result, void *default_value) {
    if (result == NULL)
        return default_value;
    return result;
}

char *rliza_seek_string(char **content, char **options) {

    while (**content == ' ' || **content == '\n' || **content == '\t' || **content == '\r') {
        (*content)++;
    }
    if (**content == 0) {
        return NULL;
    }

    char *option = NULL;
    unsigned int option_index = 0;

    while (true) {
        option = options[option_index];
        if (option == NULL)
            break;
        option_index++;
        if (option[0] == 'd') {
            if (**content >= '0' && **content <= '9') {
                return (char *)*content;
            }
        } else if (!strncmp(option, *content, strlen(option))) {
            return (char *)*content;
        }
    }
    if (**content != 0) {
        //*((*content) + 2) = 0;
        // Works good
        // if((*(*content + 1) != 0))
        //   printf("Unexpected json: %c %d %s\n",**content, **content, (*content) + 0);
        // exit(0);
    }
    return *content;
}

char *rliza_extract_quotes(char **content) {
    rbuffer_t *buffer = rbuffer_new(NULL, 0);
    assert(**content == '"');
    char previous = 0;
    while (true) {

        (*content)++;
        if (!**content) {
            rbuffer_free(buffer);
            return NULL;
        }

        if (**content == '"' && previous != '\\') {
            break;
        }
        rbuffer_push(buffer, **content);
        previous = **content;
    }
    assert(**content == '"');
    (*content)++;
    rbuffer_push(buffer, 0);
    char *result = (char *)rbuffer_to_string(buffer);
    return result;
}

rliza_t *_rliza_loads(char **content) {
    static char *seek_for1[] = {"[", "{", "\"", "d", "true", "false", "null", NULL};
    char *token = (char *)rliza_seek_string(content, seek_for1);
    if (!token)
        return NULL;
    rliza_t *rliza = rliza_new(RLIZA_NULL);
    if (**content == '"') {
        char *extracted = rliza_extract_quotes(content);
        if (!extracted) {
            rliza_free(rliza);
            return NULL;
        }
        char *extracted_with_slashes = (char *)malloc(strlen((char *)extracted) + 1);
        rstraddslashes(extracted, extracted_with_slashes);
        rliza->type = RLIZA_STRING;
        rliza->content.string = extracted_with_slashes; // extracted_without_slashes;
        free(extracted);
        return rliza;
    } else if (**content == '{') {
        rliza->type = RLIZA_OBJECT;
        (*content)++;
        char *result = NULL;
        static char *seek_for2[] = {"\"", ",", "}", NULL};
        while ((result = (char *)rliza_seek_string(content, seek_for2)) != NULL && *result) {

            if (!**content) {
                rliza_free(rliza);
                return NULL;
            }
            if (**content == ',') {
                (*content)++;
                if (!**content) {
                    rliza_free(rliza);
                    return NULL;
                }
                continue;
            }
            char *key = NULL;
            if (**content == '"') {
                key = rliza_extract_quotes((char **)content);
                if (!key || !*key) {
                    rliza_free(rliza);
                    return NULL;
                }
                char *escaped_key = (char *)malloc(strlen((char *)key) * 2 + 1);
                rstrstripslashes((char *)key, escaped_key);
                static char *seek_for3[] = {":", NULL};
                char *devider = rliza_seek_string(content, seek_for3);

                if (!devider || !*devider) {
                    free(escaped_key);
                    free(key);
                    rliza_free(rliza);
                    return NULL;
                }
                (*content)++;
                if (!**content) {
                    free(key);
                    free(escaped_key);
                    rliza_free(rliza);
                    return NULL;
                }
                rliza_t *value = _rliza_loads(content);
                if (!value) {
                    free(key);
                    free(escaped_key);
                    rliza_free(rliza);
                    return NULL;
                }
                if (value->key)
                    free(value->key);
                value->key = escaped_key;
                free(key);
                rliza_push_object(rliza, value);
            } else if (**content == '}') {
                break;
            } else {
                // Parse error
                rliza_free(rliza);
                return NULL;
            }
        };
        if ((**content != '}')) {
            rliza_free(rliza);
            return NULL;
        }
        (*content)++;
        return rliza;
    } else if (**content == '[') {
        rliza->type = RLIZA_ARRAY;
        (*content)++;
        char *result;
        static char *seek_for4[] = {"[", "{", "\"", "d", ",", "]", "null", "true", "false", NULL};
        while ((result = (char *)rliza_seek_string(content, seek_for4)) != NULL && *result) {
            if (**content == ',') {
                (*content)++;

            } else if (**content == ']') {
                break;
            }
            rliza_t *obj = _rliza_loads(content);
            if (!obj) {
                rliza_free(rliza);
                return NULL;
            }
            rliza_push(rliza, obj);
            if (!**content) {
                rliza_free(rliza);
                return NULL;
            }
        }
        if (**content != ']') {
            rliza_free(rliza);
            return NULL;
        }
        (*content)++;
        return rliza;
    } else if (**content >= '0' && **content <= '9') {
        char *ptr = *content;
        bool is_decimal = false;

        while (**content) {
            if (**content == '.') {
                is_decimal = true;
            } else if (!isdigit(**content)) {
                break;
            }
            (*content)++;
        }
        if (*(*content - 1) == '.') {
            rliza_free(rliza);
            return NULL;
        }
        if (!**content) {
            rliza_free(rliza);
            return NULL;
        }
        if (is_decimal) {
            rliza->type = RLIZA_NUMBER;
            rliza->content.number = strtod(ptr, NULL);
        } else {
            rliza->type = RLIZA_INTEGER;
            rliza->content.integer = strtoll(ptr, NULL, 10);
        }
        return rliza;
    } else if (!strncmp(*content, "true", 4)) {
        rliza->type = RLIZA_BOOLEAN;
        rliza->content.boolean = true;
        *content += 4;
        if (!**content) {
            rliza_free(rliza);
            return NULL;
        }
        return rliza;
    } else if (!strncmp(*content, "false", 5)) {
        rliza->type = RLIZA_BOOLEAN;
        rliza->content.boolean = false;
        *content += 5;
        if (!**content) {
            rliza_free(rliza);
            return NULL;
        }
        return rliza;
    } else if (!strncmp(*content, "null", 4)) {
        rliza->type = RLIZA_NULL;
        *content += 4;
        if (!**content) {
            rliza_free(rliza);
            return NULL;
        }
        return rliza;
    }
    // Parsing error
    rliza_free(rliza);
    return NULL;
}
rliza_t *rliza_loads(char **content) {
    char *original_content = *content;
    rliza_t *result = _rliza_loads(content);
    if (!result) {
        *content = original_content;
    }
    return result;
}

char *rliza_dumps(rliza_t *rliza) {
    size_t size = 4096;
    char *content = (char *)calloc(1, size * sizeof(char));
    content[0] = 0;
    if (rliza->type == RLIZA_INTEGER) {
        if (rliza->key) {
            sprintf(content, "\"%s\":%lld", rliza->key, rliza->content.integer);
        } else {
            sprintf(content, "%lld", rliza->content.integer);
        }
    } else if (rliza->type == RLIZA_STRING) {

        char *escaped_string = (char *)calloc(1, strlen((char *)rliza->content.string) * 2 + 1024);
        rstrstripslashes((char *)rliza->content.string, escaped_string);
        size_t min_size = strlen((char *)escaped_string) + (rliza->key ? strlen(rliza->key) : 0) + 1024;
        if (size < min_size) {
            size = min_size;
            content = realloc(content, min_size);
        }
        if (rliza->key) {
            char *escaped_key = (char *)malloc(strlen((char *)rliza->key) * 2 + 20);
            rstrstripslashes((char *)rliza->key, escaped_key);
            sprintf(content, "\"%s\":\"%s\"", escaped_key, escaped_string);
            free(escaped_key);
            //  rliza->content.string);
        } else {

            sprintf(content, "\"%s\"",
                    escaped_string); // rliza->content.string);
        }
        free(escaped_string);
    } else if (rliza->type == RLIZA_NUMBER) {
        if (rliza->key) {
            sprintf(content, "\"%s\":%f", rliza->key, rliza->content.number);
        } else {
            sprintf(content, "%f", rliza->content.number);
        }
        int last_zero = 0;
        bool beyond_dot = false;
        for (size_t i = 0; i < strlen(content); i++) {
            if (content[i] == '.') {
                beyond_dot = true;
            } else if (beyond_dot == true) {
                if (content[i - 1] != '.') {
                    if (content[i] == '0') {
                        if (!last_zero)
                            last_zero = i;
                    } else {
                        last_zero = 0;
                    }
                }
            }
        }
        if (last_zero != 0) {
            content[last_zero] = 0;
        }
    } else if (rliza->type == RLIZA_BOOLEAN) {
        if (rliza->key) {
            sprintf(content, "\"%s\":%s", rliza->key, rliza->content.boolean ? "true" : "false");
        } else {
            sprintf(content, "%s", rliza->content.boolean ? "true" : "false");
        }
    } else if (rliza->type == RLIZA_OBJECT) {
        if (rliza->key) {
            sprintf(content, "\"%s\":", rliza->key);
        }
        strcat(content, "{");
        for (unsigned i = 0; i < rliza->count; i++) {
            char *content_chunk = rliza_dumps(rliza->content.map[i]);
            if (strlen(content_chunk) + strlen(content) > size) {
                size += strlen(content_chunk) + 1;
                content = realloc(content, size);
            }
            strcat(content, content_chunk);
            free(content_chunk);
            strcat(content, ",");
        }
        content[strlen(content) - 1] = 0;
        strcat(content, "}");
    } else if (rliza->type == RLIZA_ARRAY) {
        if (rliza->key) {
            char *escaped_key = (char *)malloc(strlen((char *)rliza->key) * 2 + 1);
            rstraddslashes((char *)rliza->key, escaped_key);

            sprintf(content, "\"%s\":[", escaped_key);
            free(escaped_key);
        } else
            strcpy(content, "[");

        for (unsigned i = 0; i < rliza->count; i++) {
            char *content_chunk = rliza_dumps(rliza->content.map[i]);
            if (strlen(content_chunk) + strlen(content) > size) {
                size += strlen(content_chunk) + 1;
                content = realloc(content, size);
            }
            strcat(content, content_chunk);
            free(content_chunk);
            strcat(content, ",");
        }
        if (content[strlen(content) - 1] != '[')
            content[strlen(content) - 1] = 0;
        strcat(content, "]");
    } else if (rliza->type == RLIZA_NULL) {

        if (rliza->key) {
            char *escaped_key = (char *)malloc(strlen((char *)rliza->key) * 2 + 1);
            rstraddslashes((char *)rliza->key, escaped_key);

            sprintf(content, "\"%s\":null", escaped_key);
            free(escaped_key);
        } else
            strcpy(content, "null");
    }
    return content;
}

void rliza_push(rliza_t *self, rliza_t *obj) {
    rliza_push_object(self, obj);
    // self->content.array =
    //     realloc(self->content.array, sizeof(rliza_t *) * (self->count + 1));
    // self->content.array[self->count] = obj;
    // self->count++;
}

int rliza_validate(char *json_content) {
    char *json_contentp = json_content;
    rliza_t *to_object = _rliza_loads(&json_contentp);
    if (to_object) {
        rliza_free(to_object);
        return json_contentp - json_content;
    }
    return false;
}

#endif
