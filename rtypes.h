
#ifndef RTYPES_H
#define RTYPES_H
#ifdef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE_TEMP _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif
#ifndef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif
#include <stdbool.h>
#include <stdint.h>    // uint
#include <sys/types.h> // ulong
#include <string.h>
#ifndef ulonglong
#define ulonglong unsigned long long
#endif
#ifndef uint
typedef unsigned int uint;
#endif
#ifndef byte
typedef unsigned char byte;
#endif
#ifdef _POSIX_C_SOURCE_TEMP
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE _POSIX_C_SOURCE_TEMP
#undef _POSIX_C_SOURCE_TEMP
#else
#undef _POSIX_C_SOURCE
#endif
#endif
