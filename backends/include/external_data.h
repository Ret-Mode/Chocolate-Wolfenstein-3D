#ifndef __EXTERNAL_DATA__
#define __EXTERNAL_DATA__

#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
// Win32
#ifdef _WIN32
#   include <wtypes.h>
#endif
#if !defined(_WIN32)
#   include <stdint.h>
#   include <string.h>
#   include <stdarg.h>
#endif

typedef uint8_t byte;
typedef uint16_t word;
typedef int32_t fixed;
typedef uint32_t longword;
// Win32
#ifndef _WIN32
typedef int8_t boolean;
#endif
typedef void * memptr;

void Quit(const char *errorStr, ...);

typedef struct
{
    int valid;
    int globalsoundx, globalsoundy;
} globalsoundpos;

extern globalsoundpos channelSoundPos[];

#endif