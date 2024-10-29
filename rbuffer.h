#ifndef RBUFFER_H
#define RBUFFER_H
#include "rmalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
typedef struct rbuffer_t {
    unsigned char *data;
    unsigned char *_data;
    size_t size;
    size_t pos;
    bool eof;
} rbuffer_t;

rbuffer_t *rbuffer_new(unsigned char *data, size_t size);
void rbuffer_free(rbuffer_t *rfb);
void rbuffer_reset(rbuffer_t *rfb);
void rbuffer_write(rbuffer_t *rfb, const unsigned char *data, size_t size);
size_t rbuffer_push(rbuffer_t *rfb, unsigned char);
unsigned char rbuffer_pop(rbuffer_t *rfb);
unsigned char *rbuffer_expect(rbuffer_t *rfb, char *options, char *ignore);
void rbuffer_set(rbuffer_t *rfb, const unsigned char *data, size_t size);

void rbuffer_set(rbuffer_t *rfb, const unsigned char *data, size_t size) {
    if (rfb->_data) {
        free(rfb->_data);
        rfb->_data = NULL;
        rfb->data = NULL;
        rfb->eof = true;
    }
    if (size) {
        rfb->_data = (unsigned char *)malloc(size);
        memcpy(rfb->_data, data, size);
        rfb->data = rfb->_data;
        rfb->eof = false;
    }
    rfb->size = size;
    rfb->pos = 0;
}

rbuffer_t *rbuffer_new(unsigned char *data, size_t size) {
    rbuffer_t *rfb = (rbuffer_t *)malloc(sizeof(rbuffer_t));
    if (size) {
        rfb->_data = (unsigned char *)malloc(size);
        memcpy(rfb->_data, data, size);
        rfb->eof = false;
    } else {
        rfb->_data = NULL;
        rfb->eof = true;
    }
    rfb->size = size;
    rfb->pos = 0;
    rfb->data = rfb->_data;
    return rfb;
}
void rbuffer_free(rbuffer_t *rfb) {
    if (rfb->_data)
        free(rfb->_data);
    free(rfb);
}

size_t rbuffer_push(rbuffer_t *rfb, unsigned char c) {
    if (rfb->pos < rfb->size) {
        rfb->_data[rfb->pos++] = c;
        return 1;
    }
    rfb->_data = realloc(rfb->_data, rfb->size + 1);
    rfb->_data[rfb->pos++] = c;
    rfb->size++;
    return rfb->pos;
}
void rbuffer_write(rbuffer_t *rfb, const unsigned char *data, size_t size) {
    unsigned char *data_ptr = (unsigned char *)data;
    for (size_t i = 0; i < size; i++) {
        rbuffer_push(rfb, data_ptr[i]);
    }
}

unsigned char rbuffer_peek(rbuffer_t *rfb) {
    unsigned char result = EOF;
    if (rfb->pos != rfb->size) {
        result = rfb->_data[rfb->pos];
        return result;
    }
    rfb->eof = true;
    return EOF;
}
unsigned char rbuffer_pop(rbuffer_t *rfb) {
    unsigned char result = EOF;
    if (rfb->pos <= rfb->size) {
        result = rfb->_data[rfb->pos];
        rfb->pos++;
        rfb->data++;
        if (rfb->pos == rfb->size) {
            rfb->eof = true;
        }
        return result;
    }
    rfb->eof = true;
    return result;
}
void rbuffer_reset(rbuffer_t *rfb) {
    rfb->data = rfb->_data;
    rfb->pos = 0;
}

unsigned char ustrncmp(const unsigned char *s1, const unsigned char *s2,
                       size_t n) {
    return strncmp((char *)s1, (char *)s2, n);
    while (n && *s1 == *s2) {
        n--;
        s1++;
        s2++;
    }
    return *s1 != *s2;
}
size_t ustrlen(const unsigned char *s) { return strlen((char *)s); }

unsigned char *rbuffer_to_string(rbuffer_t *rfb) {
    unsigned char *result = rfb->_data;
    rfb->_data = NULL;
    rfb->data = NULL;
    rbuffer_free(rfb);
    return result;
}

unsigned char *rbuffer_match_option(rbuffer_t *rfb, char *options) {
    char *option = NULL;
    char options_cpy[1024] = {0};
    strcpy(options_cpy, options);
    char *memory = options_cpy;
    while ((option = strtok_r(option == NULL ? memory : NULL, "|", &memory)) !=
           NULL) {

        size_t option_length = strlen(option);
        if (option_length > rfb->size - rfb->pos) {
            continue;
        }
        if (!strcmp(option, "\\d") && *rfb->data >= '0' && *rfb->data <= '9') {
            return rfb->data;
        }
        if (rfb->size - rfb->pos >= 5 && !strcmp(option, "\\b") &&
            ((!ustrncmp(rfb->data, (unsigned char *)"true", 4) ||
              !ustrncmp(rfb->data, (unsigned char *)"false", 5)))) {
            return rfb->data;
        }
        if (!ustrncmp(rfb->data, (unsigned char *)option, option_length)) {
            return rfb->data;
        }
    }
    return NULL;
}

unsigned char *rbuffer_expect(rbuffer_t *rfb, char *options, char *ignore) {
    while (rfb->pos < rfb->size) {
        if (rbuffer_match_option(rfb, options) != NULL) {
            return rfb->data;
        }
        if (rbuffer_match_option(rfb, ignore)) {
            printf("SKIP:%s\n", rfb->data);
            rbuffer_pop(rfb);
            continue;
        }
        break;
    }
    return NULL;
}
unsigned char *rbuffer_consume(rbuffer_t *rfb, char *options, char *ignore) {
    unsigned char *result = NULL;
    if ((result = rbuffer_expect(rfb, options, ignore)) != NULL) {
        rbuffer_pop(rfb);
    }
    return result;
}
#endif