// ID_VL.C

#include <string.h>
#include "wl_def.h"
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
//unsigned screenPitch;

//SDL_Surface *screenBuffer = NULL;
unsigned bufferPitch;

//SDL_Surface *curSurface = NULL;
unsigned curPitch;

unsigned scaleFactor;

boolean  screenfaded;
unsigned bordercolor;






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
    SetVGAMode(&screenWidth, &screenHeight, 
                &bufferPitch, 
                &curPitch, &scaleFactor);
    SetPixelAngleArray(screenWidth);
    SetWallHeight(screenWidth);

}

/*
=============================================================================

                        PALETTE OPS

        To avoid snow, do a WaitVBL BEFORE calling these

=============================================================================
*/

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
= VL_FadeOut
=
= Fades the current palette to the given color in the given number of steps
=
=================
*/

void VL_FadeOut (int start, int end, int red, int green, int blue, int steps)
{
    PaletteFadeOut(start, end, red, green, blue, steps);
    screenfaded = true;
}


/*
=================
=
= VL_FadeIn
=
=================
*/

void VL_FadeIn (int start, int end, void *palettePtr, int steps)
{
    PaletteFadeIn(start, end, palettePtr, steps);
}

/*
=============================================================================

                            PIXEL OPS

=============================================================================
*/

// byte *VL_LockSurface(void* surface)
// {
//     return GraphicLockBytes(surface);
// }

// void VL_UnlockSurface(void *surface)
// {
//     GraphicUnlockBytes(surface);
// }

/*
=================
=
= VL_Plot
=
=================
*/
/* Moved to sdl part */

// void VL_Plot (int x, int y, int color)
// {
//     assert(x >= 0 && (unsigned) x < screenWidth
//             && y >= 0 && (unsigned) y < screenHeight
//             && "VL_Plot: Pixel out of bounds!");

//     VL_LockSurface(GetCurSurface());
//     ((byte *) GetCurSurfacePixels())[y * curPitch + x] = color;
//     VL_UnlockSurface(GetCurSurface());
// }

/*
=================
=
= VL_GetPixel
=
=================
*/
/* Moved to sdl part */

// byte VL_GetPixel (int x, int y)
// {
//     assert_ret(x >= 0 && (unsigned) x < screenWidth
//             && y >= 0 && (unsigned) y < screenHeight
//             && "VL_GetPixel: Pixel out of bounds!");

//     VL_LockSurface(GetCurSurface());
//     byte col = ((byte *) GetCurSurfacePixels())[y * curPitch + x];
//     VL_UnlockSurface(GetCurSurface());
//     return col;
// }


/*
=================
=
= VL_Hlin
=
=================
*/
/* Moved to sdl part */

// void VL_Hlin (unsigned x, unsigned y, unsigned width, int color)
// {
//     assert(x >= 0 && x + width <= screenWidth
//             && y >= 0 && y < screenHeight
//             && "VL_Hlin: Destination rectangle out of bounds!");

//     VL_LockSurface(GetCurSurface());
//     unsigned char *dest = ((byte *) GetCurSurfacePixels()) + y * curPitch + x;
//     memset(dest, color, width);
//     VL_UnlockSurface(GetCurSurface());
// }


/*
=================
=
= VL_Vlin
=
=================
*/
/* Moved to sdl part */

// void VL_Vlin (int x, int y, int height, int color)
// {
//     assert(x >= 0 && (unsigned) x < screenWidth
//             && y >= 0 && (unsigned) y + height <= screenHeight
//             && "VL_Vlin: Destination rectangle out of bounds!");

//     VL_LockSurface(GetCurSurface());
//     unsigned char *dest = ((byte *) GetCurSurfacePixels()) + y * curPitch + x;

//     while (height--)
//     {
//         *dest = color;
//         dest += curPitch;
//     }
//     VL_UnlockSurface(GetCurSurface());
// }


/*
=================
=
= VL_Bar
=
=================
*/
/* Moved to sdl part */

// void VL_BarScaledCoord (int scx, int scy, int scwidth, int scheight, int color)
// {
//     assert(scx >= 0 && (unsigned) scx + scwidth <= screenWidth
//             && scy >= 0 && (unsigned) scy + scheight <= screenHeight
//             && "VL_BarScaledCoord: Destination rectangle out of bounds!");

//     VL_LockSurface(GetCurSurface());
//     unsigned char *dest = ((byte *) GetCurSurfacePixels()) + scy * curPitch + scx;

//     while (scheight--)
//     {
//         memset(dest, color, scwidth);
//         dest += curPitch;
//     }
//     VL_UnlockSurface(GetCurSurface());
// }

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
/* Moved to sdl part */

// void VL_MemToLatch(byte *source, int width, int height,
//     void *destSurface, int x, int y)
// {
//     assert(x >= 0 && (unsigned) x + width <= screenWidth
//             && y >= 0 && (unsigned) y + height <= screenHeight
//             && "VL_MemToLatch: Destination rectangle out of bounds!");

//     GraphicLockBytes((void*)destSurface);
//     int pitch =  GetSurfacePitch(destSurface);
//     byte *dest = (byte *) GetSurfacePixels(destSurface) + y * pitch + x;
//     for(int ysrc = 0; ysrc < height; ysrc++)
//     {
//         for(int xsrc = 0; xsrc < width; xsrc++)
//         {
//             dest[ysrc * pitch + xsrc] = source[(ysrc * (width >> 2) + (xsrc >> 2))
//                 + (xsrc & 3) * (width >> 2) * height];
//         }
//     }
//     GraphicUnlockBytes(destSurface);
// }

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
/* Moved to sdl part */

// void VL_MemToScreenScaledCoord (byte *source, int width, int height, int destx, int desty)
// {
//     assert(destx >= 0 && destx + width * scaleFactor <= screenWidth
//             && desty >= 0 && desty + height * scaleFactor <= screenHeight
//             && "VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");

//     GraphicLockBytes(GetCurSurface());
//     byte *vbuf = (byte *) GetCurSurfacePixels();
//     for(int j=0,scj=0; j<height; j++, scj+=scaleFactor)
//     {
//         for(int i=0,sci=0; i<width; i++, sci+=scaleFactor)
//         {
//             byte col = source[(j*(width>>2)+(i>>2))+(i&3)*(width>>2)*height];
//             for(unsigned m=0; m<scaleFactor; m++)
//             {
//                 for(unsigned n=0; n<scaleFactor; n++)
//                 {
//                     vbuf[(scj+m+desty)*curPitch+sci+n+destx] = col;
//                 }
//             }
//         }
//     }
//     GraphicUnlockBytes(GetCurSurface());
// }

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
/* Moved to sdl part */

// void VL_MemToScreenScaledCoord (byte *source, int origwidth, int origheight, int srcx, int srcy,
//                                 int destx, int desty, int width, int height)
// {
//     assert(destx >= 0 && destx + width * scaleFactor <= screenWidth
//             && desty >= 0 && desty + height * scaleFactor <= screenHeight
//             && "VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");

//     VL_LockSurface(GetCurSurface());
//     byte *vbuf = (byte *) GetCurSurfacePixels();
//     for(int j=0,scj=0; j<height; j++, scj+=scaleFactor)
//     {
//         for(int i=0,sci=0; i<width; i++, sci+=scaleFactor)
//         {
//             byte col = source[((j+srcy)*(origwidth>>2)+((i+srcx)>>2))+((i+srcx)&3)*(origwidth>>2)*origheight];
//             for(unsigned m=0; m<scaleFactor; m++)
//             {
//                 for(unsigned n=0; n<scaleFactor; n++)
//                 {
//                     vbuf[(scj+m+desty)*curPitch+sci+n+destx] = col;
//                 }
//             }
//         }
//     }
//     VL_UnlockSurface(GetCurSurface());
// }

//==========================================================================

/*
=================
=
= VL_LatchToScreen
=
=================
*/
/* Moved to sdl part */

// void VL_LatchToScreenScaledCoord(int which, int xsrc, int ysrc,
//     int width, int height, int scxdest, int scydest)
// {
//     void *source = GetLatchPic(which);
//     assert(scxdest >= 0 && scxdest + width * scaleFactor <= screenWidth
//             && scydest >= 0 && scydest + height * scaleFactor <= screenHeight
//             && "VL_LatchToScreenScaledCoord: Destination rectangle out of bounds!");

//     if(scaleFactor == 1)
//     {
//         // HACK: If screenBits is not 8 and the screen is faded out, the
//         //       result will be black when using SDL_BlitSurface. The reason
//         //       is that the logical palette needed for the transformation
//         //       to the screen color depth is not equal to the logical
//         //       palette of the latch (the latch is not faded). Therefore,
//         //       SDL tries to map the colors...
//         //       The result: All colors are mapped to black.
//         //       So, we do the blit on our own...
//         if(GetScreenBits() != 8)
//         {
//             VL_LockSurface(source);
//             byte *src = (byte *) GetSurfacePixels(source);
//             unsigned srcPitch = GetSurfacePitch(source);

//             VL_LockSurface(GetCurSurface());
//             byte *vbuf = (byte *) GetCurSurfacePixels();
//             for(int j=0,scj=0; j<height; j++, scj++)
//             {
//                 for(int i=0,sci=0; i<width; i++, sci++)
//                 {
//                     byte col = src[(ysrc + j)*srcPitch + xsrc + i];
//                     vbuf[(scydest+scj)*curPitch+scxdest+sci] = col;
//                 }
//             }
//             VL_UnlockSurface(GetCurSurface());
//             VL_UnlockSurface((void*)source);
//         }
//         else
//         {
//             LatchToScreenScaledCoord(which, xsrc, ysrc, width, height, scxdest, scydest);
//         }
//     }
//     else
//     {
//         VL_LockSurface(source);
//         byte *src = (byte *) GetSurfacePixels(source);
//         unsigned srcPitch =  GetSurfacePitch(source);

//         VL_LockSurface(GetCurSurface());
//         byte *vbuf = (byte *) GetCurSurfacePixels();
//         for(int j=0,scj=0; j<height; j++, scj+=scaleFactor)
//         {
//             for(int i=0,sci=0; i<width; i++, sci+=scaleFactor)
//             {
//                 byte col = src[(ysrc + j)*srcPitch + xsrc + i];
//                 for(unsigned m=0; m<scaleFactor; m++)
//                 {
//                     for(unsigned n=0; n<scaleFactor; n++)
//                     {
//                         vbuf[(scydest+scj+m)*curPitch+scxdest+sci+n] = col;
//                     }
//                 }
//             }
//         }
//         VL_UnlockSurface(GetCurSurface());
//         VL_UnlockSurface(source);
//     }
// }

//===========================================================================

/*
=================
=
= VL_ScreenToScreen
=
=================
*/

/* moved to backed */
// void VL_ScreenToScreen (void *source, void *dest)
// {
//     ScreenToScreen (source, dest);
// }
