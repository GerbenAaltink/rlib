#ifndef RLIZA_H
#define RLIZA_H
#include <assert.h>
#include "rmalloc.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rbuffer.h"

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
        struct rliza_t *object;
        struct rliza_t **array;
        struct rliza_t **map;
        long long integer;
    } content;
    unsigned int count;
    unsigned char *(*get_string)(struct rliza_t *, char *);
    long long (*get_integer)(struct rliza_t *, char *);
    double (*get_number)(struct rliza_t *, char *);
    bool (*get_boolean)(struct rliza_t *, char *);
    struct rliza_t **(*get_array)(struct rliza_t *, char *);
    struct rliza_t *(*get_object)(struct rliza_t *, char *);
    unsigned char *(*set_string)(struct rliza_t *, char *, char *);
    long long (*set_integer)(struct rliza_t *, char *, long long);
    double (*set_number)(struct rliza_t *, char *, double);
    double (*set_boolean)(struct rliza_t *, char *, bool);
} rliza_t;

void rliza_free(rliza_t *rliza) {
    if (rliza->key) {
        free(rliza->key);
        rliza->key = NULL;
    }
    if (rliza->value)
        rliza_free(rliza->value);
    if (rliza->type == RLIZA_STRING) {
        if (rliza->content.string) {
            free(rliza->content.string);
        } else if (rliza->type == RLIZA_NUMBER) {
        }
    } else if (rliza->type == RLIZA_OBJECT) {
        if (rliza->content.map) {

            for (unsigned int i = 0; i < rliza->count; i++) {
                rliza_free(rliza->content.map[i]);
            }
            free(rliza->content.map);
        }
    } else if (rliza->type == RLIZA_ARRAY) {

        if (rliza->content.array && rliza->count) {
            for (unsigned int i = 0; i < rliza->count; i++) {
                rliza_free(rliza->content.array[i]);
                // printf("Removed array item\n");
            }

            // free(rliza->content.array);
        }
    }
}

rliza_t *rliza_new(rliza_type_t type);
rliza_t *rliza_new_string(char *string);
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
rliza_t **rliza_get_array(rliza_t *self, char *key);
rliza_t *rliza_get_object(rliza_t *self, char *key);

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

rliza_t **rliza_get_array(rliza_t *self, char *key) {
    for (unsigned int i = 0; i < self->count; i++) {
        if (self->content.map[i]->key != NULL &&
            strcmp(self->content.map[i]->key, key) == 0) {
            if (self->content.map[i]->type == RLIZA_ARRAY ||
                self->content.map[i]->type == RLIZA_NULL) {
                return self->content.array;
            }
        }
    }
    return NULL;
}

rliza_t *rliza_new_string(char *string) {
    rliza_t *rliza = rliza_new(RLIZA_STRING);
    if (string == NULL) {
        rliza->type = RLIZA_NULL;
        rliza->content.string = NULL;
        return rliza;
    } else {
        rliza->content.string = (unsigned char *)strdup(string);
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

void rliza_set_string(rliza_t *self, char *key, char *string) {
    rliza_t *obj = rliza_get_object(self, key);
    if (!obj) {
        obj = rliza_new_string(string);
        obj->key = strdup(key);
        obj->type = RLIZA_STRING;
        rliza_set_object(self, obj);
    } else {
        if (obj->content.string)
            free(obj->content.string);
        obj->content.string = (unsigned char *)strdup(string);
    }
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
    rliza_t *rliza = malloc(sizeof(rliza_t));
    memset(rliza, 0, sizeof(rliza_t));
    rliza->type = type;
    rliza->key = NULL;
    rliza->count = 0;
    rliza->get_boolean = rliza_get_boolean;
    rliza->get_integer = rliza_get_integer;
    rliza->get_number = rliza_get_number;
    rliza->get_string = rliza_get_string;
    rliza->get_array = rliza_get_array;
    rliza->get_object = rliza_get_object;
    rliza->content.map = NULL;
    rliza->content.array = NULL;

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
            **content == '\t') {
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
    bool escaping = false;
    while (true) {
        (*content)++;
        if (**content == '\\') {
            escaping = true;
            (*content)++;
        } else if (**content == '"') {
            if (escaping) {
                (*content)++;
                escaping = false;
                continue;
            }
            break;
        }
        rbuffer_push(buffer, **content);
    }
    assert(**content == '"');
    (*content)++;
    rbuffer_push(buffer, 0);
    unsigned char *data = buffer->data;
    buffer->data = NULL;
    rbuffer_free(buffer);
    return data;
}
unsigned char *rliza_object_to_string(rliza_t *rliza);
rliza_t *rliza_object_from_string(char **content) {
    char *token = (char *)rliza_seek_string(content, "[|{|\"|\\d|\\b|root");
    if (!token)
        return NULL;
    rliza_t *rliza = rliza_new(RLIZA_NULL);
    if (**content == '{') {
        rliza->type = RLIZA_OBJECT;
        (*content)++;
        char *result = NULL;
        while ((result = (char *)rliza_seek_string(content, "\"|,|}|object")) !=
                   NULL &&
               *result) {
            if (**content == ',') {
                (*content)++;
                continue;
            }
            unsigned char *key = NULL;
            if (**content == '"') {
                key = rliza_extract_quotes((char **)content);
                assert(rliza_seek_string(content, ":|keystr"));
                (*content)++;
                rliza_t *value = rliza_object_from_string(content);
                value->key = (char *)key;
                rliza_set_object(rliza, value);
            } else if (**content == '}') {
                (*content)++;
            } else {
                assert(false && "Parse error.");
            }
        };
    } else if (**content == '[') {
        rliza->type = RLIZA_ARRAY;
        (*content)++;
        char *result;
        while ((result = (char *)rliza_seek_string(
                    content, "{|[|\"|\\d|\\b|,|]|array")) != NULL &&
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
    } else if (**content >= '0' && **content <= '9') {
        rliza->type = RLIZA_INTEGER;
        rliza->content.integer = strtoll(*content, NULL, 10);
        // printf("Parsed number: %lld\n", rliza->content.integer);
        while (**content >= '0' && **content <= '9') {
            (*content)++;
        }
    } else if (!strncmp(*content, "true", 4)) {
        rliza->type = RLIZA_BOOLEAN;
        rliza->content.boolean = true;
        *content += 4;
    } else if (!strncmp(*content, "false", 5)) {
        rliza->type = RLIZA_BOOLEAN;
        rliza->content.boolean = false;
        *content += 5;
    } else if (**content == '"') {
        unsigned char *extracted = rliza_extract_quotes((char **)content);
        bool is_number = true;
        unsigned char *ptr = extracted;
        while (*ptr) {
            if ((*ptr < '0' || *ptr > '9') && *ptr != '.') {
                is_number = false;
            }
            ptr++;
        }
        if (is_number) {
            rliza->type = RLIZA_NUMBER;
            rliza->content.number = strtod((char *)extracted, NULL);
            free(extracted);
        } else {
            rliza->type = RLIZA_STRING;
            rliza->content.string = extracted;
        }
    }
    return rliza;
}
unsigned char *rliza_object_to_string(rliza_t *rliza) {
    unsigned char *content = (unsigned char *)malloc(1024 * 1024);
    content[0] = 0;
    if (rliza->type == RLIZA_INTEGER) {
        if (rliza->key) {
            sprintf((char *)content, "\"%s\":%lld", rliza->key,
                    rliza->content.integer);
        } else {
            sprintf((char *)content, "%lld", rliza->content.integer);
        }
    } else if (rliza->type == RLIZA_STRING) {
        if (rliza->key) {
            sprintf((char *)content, "\"%s\":\"%s\"", rliza->key,
                    rliza->content.string);
        } else {
            sprintf((char *)content, "\"%s\"", rliza->content.string);
        }
    } else if (rliza->type == RLIZA_NUMBER) {
        if (rliza->key) {
            sprintf((char *)content, "\"%s\":\"%f\"", rliza->key,
                    rliza->content.number);
        } else {
            sprintf((char *)content, "\"%f\"", rliza->content.number);
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
        strcat((char *)content, "{");
        for (unsigned i = 0; i < rliza->count; i++) {
            strcat((char *)content,
                   (char *)rliza_object_to_string(rliza->content.map[i]));
            strcat((char *)content, ",");
        }
        content[strlen((char *)content) - 1] = 0;
        strcat((char *)content, "}");
    } else if (rliza->type == RLIZA_ARRAY) {
        sprintf((char *)content, "\"%s\":[", rliza->key);
        for (unsigned i = 0; i < rliza->count; i++) {
            strcat((char *)content,
                   (char *)rliza_object_to_string(rliza->content.map[i]));
            strcat((char *)content, ",");
        }
        content[strlen((char *)content) - 1] = 0;
        strcat((char *)content, "]");
    }
    return content;
}

void rliza_push(rliza_t *self, rliza_t *obj) {
    self->content.array =
        realloc(self->content.array, sizeof(rliza_t *) * (self->count + 1));
    self->content.array[self->count] = obj;
    self->count++;
}

#endif
