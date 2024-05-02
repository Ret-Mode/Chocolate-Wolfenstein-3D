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
#   include <stdint.h>
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

// typedef struct
// {
//     int16_t width,height;
// } pictabletype;

void Quit(const char *errorStr, ...);

typedef struct
{
    int valid;
    int globalsoundx, globalsoundy;
} globalsoundpos;

extern globalsoundpos channelSoundPos[];

extern  boolean  fullscreen, usedoublebuffering;

extern boolean param_debugmode;
extern boolean param_nowait;
extern int     param_difficulty;           // default is "normal"
extern int     param_tedlevel;            // default is not to start a level
extern int     param_joystickindex;
extern int     param_joystickhat;
extern int     param_samplerate;
extern int     param_audiobuffer;

extern int     param_mission;
extern boolean param_goodtimes;
extern boolean param_ignorenumchunks;
extern boolean MousePresent;
extern boolean forcegrabmouse;
extern int JoyNumButtons;
extern boolean  screenfaded;

extern volatile boolean    Keyboard[];
extern volatile boolean    Paused;
extern volatile char       LastASCII;

// extern byte    **grsegs;
// extern pictabletype    *pictable;
void CA_CacheGrChunk (int chunk);
void VL_MemToLatch(byte *source, int width, int height,
    void *destSurface, int x, int y);
void    IN_StartAck(void);
boolean IN_CheckAck (void);

#define UNCACHEGRCHUNK(chunk) {if(grsegs[chunk]) {free(grsegs[chunk]); grsegs[chunk]=NULL;}}

#endif