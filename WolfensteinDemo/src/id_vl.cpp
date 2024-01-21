// ID_VL.C

#include <string.h>
#include "wl_def.h"
#include "crt.h"
#include "sdl_graphics.h"
#pragma hdrstop

// Uncomment the following line, if you get destination out of bounds
// assertion errors and want to ignore them during debugging
//#define IGNORE_BAD_DEST

#ifdef IGNORE_BAD_DEST
#undef assert
#define assert(x) if(!(x)) return
#define assert_ret(x) if(!(x)) return 0
#else
#define assert_ret(x) assert(x)
#endif

boolean fullscreen = false;


boolean usedoublebuffering = true;
unsigned screenWidth = 640;
unsigned screenHeight = 480;

//SDL_Surface *screen = NULL;
unsigned screenPitch;

//SDL_Surface *screenBuffer = NULL;
unsigned bufferPitch;

//SDL_Surface *curSurface = NULL;
unsigned curPitch;

unsigned scaleFactor;

boolean  screenfaded;
unsigned bordercolor;

SDL_Color palette1[256], palette2[256];





//===========================================================================


/*
=======================
=
= VL_Shutdown
=
=======================
*/

void    VL_Shutdown (void)
{
    //VL_SetTextMode ();
}


/*
=======================
=
= VL_SetVGAPlaneMode
=
=======================
*/

void    VL_SetVGAPlaneMode (void)
{
    SetWindowTitle("Wolfenstein 3D");

    SetScreenBits();

    //Fab's CRT Hack
    //Adjust height so the screen is 4:3 aspect ratio
    screenHeight=screenWidth * 3.0/4.0;
    
    void *screen = SDL_SetVideoMode(screenWidth, screenHeight, GetScreenBits(),
          (usedoublebuffering ? SDL_HWSURFACE | SDL_DOUBLEBUF : 0)
        | (GetScreenBits() == 8 ? SDL_HWPALETTE : 0)
        | (fullscreen ? SDL_FULLSCREEN : 0) | SDL_OPENGL | SDL_OPENGLBLIT);
    
    SetScreen(screen);

    if(!GetScreen())
    {
        printf("Unable to set %ix%ix%i video mode: %s\n", screenWidth, screenHeight, GetScreenBits(), SDL_GetError());
        exit(1);
    }
    if((GetScreenFlags() & SDL_DOUBLEBUF) != SDL_DOUBLEBUF)
        usedoublebuffering = false;
    SDL_ShowCursor(SDL_DISABLE);

    SetScreenPalette();

    //Fab's CRT Hack
    CRT_Init(screenWidth);
    
    //Fab's CRT Hack
    screenWidth=320;
    screenHeight=200;
    
    SDL_Surface *screenBuffer = (SDL_Surface *)CreateScreenBuffer(GetGamePal(), &bufferPitch, screenWidth, screenHeight);

    screenPitch = GetScreenPitch();

    SetCurSurface(screenBuffer);
    curPitch = bufferPitch;

    scaleFactor = screenWidth/320;
    if(screenHeight/200 < scaleFactor) scaleFactor = screenHeight/200;
    
    
    pixelangle = (short *) malloc(screenWidth * sizeof(short));
    CHECKMALLOCRESULT(pixelangle);
    wallheight = (int *) malloc(screenWidth * sizeof(int));
    CHECKMALLOCRESULT(wallheight);
    
    
}

/*
=============================================================================

                        PALETTE OPS

        To avoid snow, do a WaitVBL BEFORE calling these

=============================================================================
*/

/*
=================
=
= VL_ConvertPalette
=
=================
*/

void VL_ConvertPalette(byte *srcpal, void *dest, int numColors)
{
    ConvertPalette(srcpal, dest, numColors);
}

/*
=================
=
= VL_FillPalette
=
=================
*/

void VL_FillPalette (int red, int green, int blue)
{
    FillPalette(red, green, blue);
}

//===========================================================================

/*
=================
=
= VL_SetColor
=
=================
*/

void VL_SetColor    (int color, int red, int green, int blue)
{
    SetCurrentPaletteColor(color, red, green, blue, GetScreenBits());
}

//===========================================================================

/*
=================
=
= VL_GetColor
=
=================
*/

void VL_GetColor    (int color, int *red, int *green, int *blue)
{
    GetCurrentPaletteColor(color, red, green, blue);
}

//===========================================================================

/*
=================
=
= VL_SetPalette
=
=================
*/

void VL_SetPalette (void *palette, bool forceupdate)
{
    SetWholePalette(palette, (int)forceupdate);
}


//===========================================================================

/*
=================
=
= VL_GetPalette
=
=================
*/

void VL_GetPalette (void *palette)
{
    GetWholePalette(palette);
}


//===========================================================================

/*
=================
=
= VL_FadeOut
=
= Fades the current palette to the given color in the given number of steps
=
=================
*/

void VL_FadeOut (int start, int end, int red, int green, int blue, int steps)
{
    int         i,j,orig,delta;
    SDL_Color   *origptr, *newptr;

    red = red * 255 / 63;
    green = green * 255 / 63;
    blue = blue * 255 / 63;

    DelayMilliseconds(8);
    VL_GetPalette(palette1);
    memcpy(palette2, palette1, sizeof(SDL_Color) * 256);

//
// fade through intermediate frames
//
    for (i=0;i<steps;i++)
    {
        origptr = &palette1[start];
        newptr = &palette2[start];
        for (j=start;j<=end;j++)
        {
            orig = origptr->r;
            delta = red-orig;
            newptr->r = orig + delta * i / steps;
            orig = origptr->g;
            delta = green-orig;
            newptr->g = orig + delta * i / steps;
            orig = origptr->b;
            delta = blue-orig;
            newptr->b = orig + delta * i / steps;
            origptr++;
            newptr++;
        }

        if(!usedoublebuffering || GetScreenBits() == 8) DelayMilliseconds(8);
        VL_SetPalette (palette2, true);
    }

//
// final color
//
    VL_FillPalette (red,green,blue);

    screenfaded = true;
}


/*
=================
=
= VL_FadeIn
=
=================
*/

void VL_FadeIn (int start, int end, void *platettePtr, int steps)
{
    SDL_Color *palette = (SDL_Color *)platettePtr;
    int i,j,delta;

    DelayMilliseconds(8);
    VL_GetPalette(palette1);
    memcpy(palette2, palette1, sizeof(SDL_Color) * 256);

//
// fade through intermediate frames
//
    for (i=0;i<steps;i++)
    {
        for (j=start;j<=end;j++)
        {
            delta = palette[j].r-palette1[j].r;
            palette2[j].r = palette1[j].r + delta * i / steps;
            delta = palette[j].g-palette1[j].g;
            palette2[j].g = palette1[j].g + delta * i / steps;
            delta = palette[j].b-palette1[j].b;
            palette2[j].b = palette1[j].b + delta * i / steps;
        }

        if(!usedoublebuffering || GetScreenBits() == 8) DelayMilliseconds(8);
        VL_SetPalette(palette2, true);
    }

//
// final color
//
    VL_SetPalette (palette, true);
    screenfaded = false;
}

/*
=============================================================================

                            PIXEL OPS

=============================================================================
*/

byte *VL_LockSurface(void* surface)
{
    return GraphicLockBytes(surface);
}

void VL_UnlockSurface(void *surface)
{
    GraphicUnlockBytes(surface);
}

/*
=================
=
= VL_Plot
=
=================
*/

void VL_Plot (int x, int y, int color)
{
    assert(x >= 0 && (unsigned) x < screenWidth
            && y >= 0 && (unsigned) y < screenHeight
            && "VL_Plot: Pixel out of bounds!");

    VL_LockSurface(GetCurSurface());
    ((byte *) GetCurSurfacePixels())[y * curPitch + x] = color;
    VL_UnlockSurface(GetCurSurface());
}

/*
=================
=
= VL_GetPixel
=
=================
*/

byte VL_GetPixel (int x, int y)
{
    assert_ret(x >= 0 && (unsigned) x < screenWidth
            && y >= 0 && (unsigned) y < screenHeight
            && "VL_GetPixel: Pixel out of bounds!");

    VL_LockSurface(GetCurSurface());
    byte col = ((byte *) GetCurSurfacePixels())[y * curPitch + x];
    VL_UnlockSurface(GetCurSurface());
    return col;
}


/*
=================
=
= VL_Hlin
=
=================
*/

void VL_Hlin (unsigned x, unsigned y, unsigned width, int color)
{
    assert(x >= 0 && x + width <= screenWidth
            && y >= 0 && y < screenHeight
            && "VL_Hlin: Destination rectangle out of bounds!");

    VL_LockSurface(GetCurSurface());
    unsigned char *dest = ((byte *) GetCurSurfacePixels()) + y * curPitch + x;
    memset(dest, color, width);
    VL_UnlockSurface(GetCurSurface());
}


/*
=================
=
= VL_Vlin
=
=================
*/

void VL_Vlin (int x, int y, int height, int color)
{
    assert(x >= 0 && (unsigned) x < screenWidth
            && y >= 0 && (unsigned) y + height <= screenHeight
            && "VL_Vlin: Destination rectangle out of bounds!");

    VL_LockSurface(GetCurSurface());
    unsigned char *dest = ((byte *) GetCurSurfacePixels()) + y * curPitch + x;

    while (height--)
    {
        *dest = color;
        dest += curPitch;
    }
    VL_UnlockSurface(GetCurSurface());
}


/*
=================
=
= VL_Bar
=
=================
*/

void VL_BarScaledCoord (int scx, int scy, int scwidth, int scheight, int color)
{
    assert(scx >= 0 && (unsigned) scx + scwidth <= screenWidth
            && scy >= 0 && (unsigned) scy + scheight <= screenHeight
            && "VL_BarScaledCoord: Destination rectangle out of bounds!");

    VL_LockSurface(GetCurSurface());
    unsigned char *dest = ((byte *) GetCurSurfacePixels()) + scy * curPitch + scx;

    while (scheight--)
    {
        memset(dest, color, scwidth);
        dest += curPitch;
    }
    VL_UnlockSurface(GetCurSurface());
}

/*
============================================================================

                            MEMORY OPS

============================================================================
*/

/*
=================
=
= VL_MemToLatch
=
=================
*/

void VL_MemToLatch(byte *source, int width, int height,
    void *destSurface, int x, int y)
{
    assert(x >= 0 && (unsigned) x + width <= screenWidth
            && y >= 0 && (unsigned) y + height <= screenHeight
            && "VL_MemToLatch: Destination rectangle out of bounds!");

    VL_LockSurface((void*)destSurface);
    int pitch =  GetSurfacePitch(destSurface);
    byte *dest = (byte *) GetSurfacePixels(destSurface) + y * pitch + x;
    for(int ysrc = 0; ysrc < height; ysrc++)
    {
        for(int xsrc = 0; xsrc < width; xsrc++)
        {
            dest[ysrc * pitch + xsrc] = source[(ysrc * (width >> 2) + (xsrc >> 2))
                + (xsrc & 3) * (width >> 2) * height];
        }
    }
    VL_UnlockSurface(destSurface);
}

//===========================================================================


/*
=================
=
= VL_MemToScreenScaledCoord
=
= Draws a block of data to the screen with scaling according to scaleFactor.
=
=================
*/

void VL_MemToScreenScaledCoord (byte *source, int width, int height, int destx, int desty)
{
    assert(destx >= 0 && destx + width * scaleFactor <= screenWidth
            && desty >= 0 && desty + height * scaleFactor <= screenHeight
            && "VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");

    VL_LockSurface(GetCurSurface());
    byte *vbuf = (byte *) GetCurSurfacePixels();
    for(int j=0,scj=0; j<height; j++, scj+=scaleFactor)
    {
        for(int i=0,sci=0; i<width; i++, sci+=scaleFactor)
        {
            byte col = source[(j*(width>>2)+(i>>2))+(i&3)*(width>>2)*height];
            for(unsigned m=0; m<scaleFactor; m++)
            {
                for(unsigned n=0; n<scaleFactor; n++)
                {
                    vbuf[(scj+m+desty)*curPitch+sci+n+destx] = col;
                }
            }
        }
    }
    VL_UnlockSurface(GetCurSurface());
}

/*
=================
=
= VL_MemToScreenScaledCoord
=
= Draws a part of a block of data to the screen.
= The block has the size origwidth*origheight.
= The part at (srcx, srcy) has the size width*height
= and will be painted to (destx, desty) with scaling according to scaleFactor.
=
=================
*/

void VL_MemToScreenScaledCoord (byte *source, int origwidth, int origheight, int srcx, int srcy,
                                int destx, int desty, int width, int height)
{
    assert(destx >= 0 && destx + width * scaleFactor <= screenWidth
            && desty >= 0 && desty + height * scaleFactor <= screenHeight
            && "VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");

    VL_LockSurface(GetCurSurface());
    byte *vbuf = (byte *) GetCurSurfacePixels();
    for(int j=0,scj=0; j<height; j++, scj+=scaleFactor)
    {
        for(int i=0,sci=0; i<width; i++, sci+=scaleFactor)
        {
            byte col = source[((j+srcy)*(origwidth>>2)+((i+srcx)>>2))+((i+srcx)&3)*(origwidth>>2)*origheight];
            for(unsigned m=0; m<scaleFactor; m++)
            {
                for(unsigned n=0; n<scaleFactor; n++)
                {
                    vbuf[(scj+m+desty)*curPitch+sci+n+destx] = col;
                }
            }
        }
    }
    VL_UnlockSurface(GetCurSurface());
}

//==========================================================================

/*
=================
=
= VL_LatchToScreen
=
=================
*/

void VL_LatchToScreenScaledCoord(int which, int xsrc, int ysrc,
    int width, int height, int scxdest, int scydest)
{
    void *source = GetLatchPic(which);
    assert(scxdest >= 0 && scxdest + width * scaleFactor <= screenWidth
            && scydest >= 0 && scydest + height * scaleFactor <= screenHeight
            && "VL_LatchToScreenScaledCoord: Destination rectangle out of bounds!");

    if(scaleFactor == 1)
    {
        // HACK: If screenBits is not 8 and the screen is faded out, the
        //       result will be black when using SDL_BlitSurface. The reason
        //       is that the logical palette needed for the transformation
        //       to the screen color depth is not equal to the logical
        //       palette of the latch (the latch is not faded). Therefore,
        //       SDL tries to map the colors...
        //       The result: All colors are mapped to black.
        //       So, we do the blit on our own...
        if(GetScreenBits() != 8)
        {
            VL_LockSurface(source);
            byte *src = (byte *) GetSurfacePixels(source);
            unsigned srcPitch = GetSurfacePitch(source);

            VL_LockSurface(GetCurSurface());
            byte *vbuf = (byte *) GetCurSurfacePixels();
            for(int j=0,scj=0; j<height; j++, scj++)
            {
                for(int i=0,sci=0; i<width; i++, sci++)
                {
                    byte col = src[(ysrc + j)*srcPitch + xsrc + i];
                    vbuf[(scydest+scj)*curPitch+scxdest+sci] = col;
                }
            }
            VL_UnlockSurface(GetCurSurface());
            VL_UnlockSurface((void*)source);
        }
        else
        {
            LatchToScreenScaledCoord(which, xsrc, ysrc, width, height, scxdest, scydest);
        }
    }
    else
    {
        VL_LockSurface(source);
        byte *src = (byte *) GetSurfacePixels(source);
        unsigned srcPitch =  GetSurfacePitch(source);

        VL_LockSurface(GetCurSurface());
        byte *vbuf = (byte *) GetCurSurfacePixels();
        for(int j=0,scj=0; j<height; j++, scj+=scaleFactor)
        {
            for(int i=0,sci=0; i<width; i++, sci+=scaleFactor)
            {
                byte col = src[(ysrc + j)*srcPitch + xsrc + i];
                for(unsigned m=0; m<scaleFactor; m++)
                {
                    for(unsigned n=0; n<scaleFactor; n++)
                    {
                        vbuf[(scydest+scj+m)*curPitch+scxdest+sci+n] = col;
                    }
                }
            }
        }
        VL_UnlockSurface(GetCurSurface());
        VL_UnlockSurface(source);
    }
}

//===========================================================================

/*
=================
=
= VL_ScreenToScreen
=
=================
*/

void VL_ScreenToScreen (void *source, void *dest)
{
    ScreenToScreen (source, dest);
}
