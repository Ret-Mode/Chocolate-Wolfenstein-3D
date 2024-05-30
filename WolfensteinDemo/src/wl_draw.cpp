// WL_DRAW.C
// ALL contents of this file were moved to backend

#include "wl_def.h"
#include "sdl_graphics.h"
#pragma hdrstop

int32_t finetangent[FINEANGLES/4];
fixed sintable[ANGLES+ANGLES/4];
fixed *costable = sintable+(ANGLES/4);

int32_t    lasttimecount;
int32_t    frameon;
boolean    fpscounter;

short   midangle,angle;
fixed   viewx,viewy;                    // the focal point
short   viewangle;
fixed   viewsin,viewcos;
short   focaltx,focalty,viewtx,viewty;
word horizwall[MAXWALLTILES],vertwall[MAXWALLTILES];

short *pixelangle;
int *wallheight;

void SetPixelAngleArray(int screenWidth) {
    void *newPixelAngle = (short *) malloc(screenWidth * sizeof(short));
    if (pixelangle) {
        void *oldPixelAngle = pixelangle;
        pixelangle = (short *)newPixelAngle;
        free(oldPixelAngle);
    } else {
        pixelangle = (short *)newPixelAngle;
    }
    if (pixelangle) {
        memset(pixelangle, 0x0, screenWidth * sizeof(short));
    }
    CHECKMALLOCRESULT(pixelangle);
}

void SetWallHeight(int screenWidth) {
    void *newWallHeight = (int *) malloc(screenWidth * sizeof(int));
    if (wallheight) {
        void *oldWallHeight = wallheight;
        wallheight = (int *)newWallHeight;
        free(oldWallHeight);
    } else {
        wallheight = (int *)newWallHeight;
    }
    if (wallheight) {
        memset(wallheight, 0x0, screenWidth * sizeof(int));
    }
    CHECKMALLOCRESULT(wallheight);
}

void CalcViewVariables(void)
{
    viewangle = player->angle;
    //printf("\nvieangle=%d\n",viewangle);
    midangle = viewangle*(FINEANGLES/ANGLES);
    viewsin = sintable[viewangle];
    viewcos = costable[viewangle];
    //printf("%d\n",viewcos);
    viewx = player->x - FixedMul(focallength,viewcos);
    viewy = player->y + FixedMul(focallength,viewsin);

    focaltx = (short)(viewx>>TILESHIFT);
    focalty = (short)(viewy>>TILESHIFT);

    viewtx = (short)(player->x >> TILESHIFT);
    viewty = (short)(player->y >> TILESHIFT);
}

static byte vgaCeiling[]=
{

 0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0xbf,
 0x4e,0x4e,0x4e,0x1d,0x8d,0x4e,0x1d,0x2d,0x1d,0x8d,
 0x1d,0x1d,0x1d,0x1d,0x1d,0x2d,0xdd,0x1d,0x1d,0x98,

 0x1d,0x9d,0x2d,0xdd,0xdd,0x9d,0x2d,0x4d,0x1d,0xdd,
 0x7d,0x1d,0x2d,0x2d,0xdd,0xd7,0x1d,0x1d,0x1d,0x2d,
 0x1d,0x1d,0x1d,0x1d,0xdd,0xdd,0x7d,0xdd,0xdd,0xdd

};

unsigned char *GetVgaCeiling(void) {
    return vgaCeiling;
}

/*
=====================
=
= CalcTics
=
=====================
*/

void CalcTics (void)
{
//
// calculate tics since last refresh for adaptive timing
//
    if (lasttimecount > (int32_t) GetTimeCount())
        lasttimecount = GetTimeCount();    // if the game was paused a LONG time

    uint32_t curtime = GetMilliseconds();
    tics = (curtime * 7) / 100 - lasttimecount;
    if(!tics)
    {
        // wait until end of current tic
        DelayMilliseconds(((lasttimecount + 1) * 100) / 7 - curtime);
        tics = 1;
    }

    lasttimecount += tics;

    if (tics>MAXTICS)
        tics = MAXTICS;
}
