
#ifndef RTYPES_H
#define RTYPES_H
#include <stdbool.h>
#include <stdint.h>    // uint
#include <sys/types.h> // ulong
#include <string.h>
#define ulonglong unsigned long long
#ifndef uint
typedef unsigned int uint;
#endif
#ifndef byte
typedef unsigned char byte;
#endif
#endif
