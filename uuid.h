#ifndef UUID_H
#define UUID_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "rtemp.h"

typedef struct {
    unsigned char bytes[16];
} UUID;

void generate_random_bytes(unsigned char *bytes, size_t len) {
    for (size_t i = 0; i < len; i++) {
        bytes[i] = rand() % 256;
    }
}

UUID generate_uuid4(void) {
    UUID uuid;

    generate_random_bytes(uuid.bytes, 16);

    uuid.bytes[6] &= 0x0f;
    uuid.bytes[6] |= 0x40;

    uuid.bytes[8] &= 0x3f;
    uuid.bytes[8] |= 0x80;

    return uuid;
}

void uuid_to_string(UUID uuid, char *str) {
    sprintf(str, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x", uuid.bytes[0], uuid.bytes[1], uuid.bytes[2],
            uuid.bytes[3], uuid.bytes[4], uuid.bytes[5], uuid.bytes[6], uuid.bytes[7], uuid.bytes[8], uuid.bytes[9], uuid.bytes[10],
            uuid.bytes[11], uuid.bytes[12], uuid.bytes[13], uuid.bytes[14], uuid.bytes[15]);
}

char *uuid4() {
    srand(time(NULL));
    UUID uuid = generate_uuid4();
    char str[37];
    uuid_to_string(uuid, str);
    return sbuf(str);
}
#endif