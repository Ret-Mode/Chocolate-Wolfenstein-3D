// ID_VL.H

// wolf compatability

// Win32
//#ifndef ID_VL.H 
//#define ID_VL.H
#ifndef ID_VL_H 
#define ID_VL_H

#include "wl_def.h"
#include "sdl_graphics.h"
void Quit (const char *error,...);

//===========================================================================

#define CHARWIDTH       2
#define TILEWIDTH       4

//===========================================================================

//extern SDL_Surface *curSurface;
//extern SDL_Color gamepal[256];

extern  boolean  fullscreen, usedoublebuffering;
extern  unsigned screenWidth, bufferPitch, screenHeight, curPitch;
extern  unsigned scaleFactor;

extern  boolean  screenfaded;
extern  unsigned bordercolor;



//===========================================================================

//
// VGA hardware routines
//

#define VL_WaitVBL(a) DelayVBL(a)//SDL_Delay((a)*8)

void VL_SetVGAPlaneMode (void);
void VL_SetTextMode (void);
void VL_Shutdown (void);

void VL_GetColor    (int color, int *red, int *green, int *blue);
void VL_SetPalette  (void *palette, bool forceupdate);
void VL_FadeOut     (int start, int end, int red, int green, int blue, int steps);
void VL_FadeIn      (int start, int end, void *palette, int steps);

byte *VL_LockSurface(void *surface);
void VL_UnlockSurface(void *surface);

byte VL_GetPixel        (int x, int y);
void VL_Plot            (int x, int y, int color);
void VL_Hlin            (unsigned x, unsigned y, unsigned width, int color);
void VL_Vlin            (int x, int y, int height, int color);
void VL_BarScaledCoord  (int scx, int scy, int scwidth, int scheight, int color);
void inline VL_Bar      (int x, int y, int width, int height, int color)
{
    VL_BarScaledCoord(scaleFactor*x, scaleFactor*y,
        scaleFactor*width, scaleFactor*height, color);
}
void inline VL_ClearScreen(int color)
{
    ClearCurrentSurface(color);
}

void VL_MungePic                (byte *source, unsigned width, unsigned height);
void VL_DrawPicBare             (int x, int y, byte *pic, int width, int height);
void VL_MemToLatch              (byte *source, int width, int height,
                                    void *destSurface, int x, int y);
void VL_MemToScreenScaledCoord  (byte *source, int width, int height, int scx, int scy);
void VL_MemToScreenScaledCoord  (byte *source, int origwidth, int origheight, int srcx, int srcy,
                                    int destx, int desty, int width, int height);

// void inline VL_MemToScreen (byte *source, int width, int height, int x, int y)
// {
//     VL_MemToScreenScaledCoord(source, width, height, scaleFactor*x, scaleFactor*y);
// }

void VL_MaskedToScreen (byte *source, int width, int height, int x, int y);

void VL_LatchToScreenScaledCoord (int which, int xsrc, int ysrc,
    int width, int height, int scxdest, int scydest);

#endif
