#include "wl_def.h"
#include "sdl_graphics.h"

pictabletype    *pictable;

int     px,py;
byte    fontcolor,backcolor;
int     fontnumber;

//==========================================================================

// Moved to backend
// void VWB_DrawPropString(const char* string)
// {
//     fontstruct  *font;
//     int         width, step, height;
//     byte        *source, *dest;
//     byte        ch;

//     byte *vbuf = VL_LockSurface(GetCurSurface());

//     font = (fontstruct *) grsegs[STARTFONT+fontnumber];
//     height = font->height;
//     dest = vbuf + scaleFactor * (py * curPitch + px);

//     while ((ch = (byte)*string++)!=0)
//     {
//         width = step = font->width[ch];
//         source = ((byte *)font)+font->location[ch];
//         while (width--)
//         {
//             for(int i=0;i<height;i++)
//             {
//                 if(source[i*step])
//                 {
//                     for(unsigned sy=0; sy<scaleFactor; sy++)
//                         for(unsigned sx=0; sx<scaleFactor; sx++)
//                             dest[(scaleFactor*i+sy)*curPitch+sx]=fontcolor;
//                 }
//             }

//             source++;
//             px++;
//             dest+=scaleFactor;
//         }
//     }

//     VL_UnlockSurface(GetCurSurface());
// }

/*
=================
=
= VL_MungePic
=
=================
*/

void VL_MungePic (byte *source, unsigned width, unsigned height)
{
    unsigned x,y,plane,size,pwidth;
    byte *temp, *dest, *srcline;

    size = width*height;

    if (width&3)
        Quit ("VL_MungePic: Not divisable by 4!");

//
// copy the pic to a temp buffer
//
    temp=(byte *) malloc(size);
    CHECKMALLOCRESULT(temp);
    memcpy (temp,source,size);

//
// munge it back into the original buffer
//
    dest = source;
    pwidth = width/4;

    for (plane=0;plane<4;plane++)
    {
        srcline = temp;
        for (y=0;y<height;y++)
        {
            for (x=0;x<pwidth;x++)
                *dest++ = *(srcline+x*4+plane);
            srcline+=width;
        }
    }

    free(temp);
}

void VWL_MeasureString (const char *string, word *width, word *height, fontstruct *font)
{
    *height = font->height;
    for (*width = 0;*string;string++)
        *width += font->width[*((byte *)string)];   // proportional width
}

void VW_MeasurePropString (const char *string, word *width, word *height)
{
    VWL_MeasureString(string,width,height,(fontstruct *)grsegs[STARTFONT+fontnumber]);
}

/*
=============================================================================

                Double buffer management routines

=============================================================================
*/

/* moved to backed */
// void VH_UpdateScreen()
// {
//     VL_ScreenToScreen(GetScreenBuffer(), GetScreen());
//     //SDL_Flip((SDL_Surface *)GetScreen());
//     //CRT_DAC();
// }


void VWB_DrawTile8 (int x, int y, int tile)
{
    VL_LatchToScreenScaledCoord(0,((tile)&7)*8,((tile)>>3)*8*64,8,8,scaleFactor*x,scaleFactor*y);
}

void VWB_DrawTile8M (int x, int y, int tile)
{
    //VL_MemToScreen (((byte *)grsegs[STARTTILE8M])+tile*64,8,8,x,y);
    VL_MemToScreenScaledCoord(((byte *)grsegs[STARTTILE8M])+tile*64, 8, 8, scaleFactor*x, scaleFactor*y);
}

void VWB_DrawPic (int x, int y, int chunknum)
{
    int picnum = chunknum - STARTPICS;
    unsigned width,height;

    x &= ~7;

    width = pictable[picnum].width;
    height = pictable[picnum].height;

    //VL_MemToScreen (grsegs[chunknum],width,height,x,y);
    VL_MemToScreenScaledCoord(grsegs[chunknum], width, height, scaleFactor*x, scaleFactor*y);
}

void VWB_DrawPicScaledCoord (int scx, int scy, int chunknum)
{
    int picnum = chunknum - STARTPICS;
    unsigned width,height;

    width = pictable[picnum].width;
    height = pictable[picnum].height;

    VL_MemToScreenScaledCoord (grsegs[chunknum],width,height,scx,scy);
}


void VWB_Bar (int x, int y, int width, int height, int color)
{
    VW_Bar (x,y,width,height,color);
}

void VWB_Plot (int x, int y, int color)
{
    if(scaleFactor == 1)
        VW_Plot(x,y,color);
    else
        VW_Bar(x, y, 1, 1, color);
}

void VWB_Hlin (int x1, int x2, int y, int color)
{
    if(scaleFactor == 1)
        VW_Hlin(x1,x2,y,color);
    else
        VW_Bar(x1, y, x2-x1+1, 1, color);
}

void VWB_Vlin (int y1, int y2, int x, int color)
{
    if(scaleFactor == 1)
        VW_Vlin(y1,y2,x,color);
    else
        VW_Bar(x, y1, 1, y2-y1+1, color);
}


/*
=============================================================================

                        WOLFENSTEIN STUFF

=============================================================================
*/

/*
=====================
=
= LatchDrawPic
=
=====================
*/

/* moved to backend */
// void LatchDrawPic (unsigned x, unsigned y, unsigned picnum)
// {
//     int which = 2+picnum-LATCHPICS_LUMP_START;
//     VL_LatchToScreenScaledCoord(which,0,0,GetLatchPicWidth(which),GetLatchPicHeight(which), scaleFactor*x*8,scaleFactor*y);
// }

// void LatchDrawPicScaledCoord (unsigned scx, unsigned scy, unsigned picnum)
// {
//     int which = 2+picnum-LATCHPICS_LUMP_START;
//     VL_LatchToScreenScaledCoord(which,0,0,GetLatchPicWidth(which),GetLatchPicHeight(which),scx*8,scy);
// }


//==========================================================================

/*
===================
=
= LoadLatchMem
=
===================
*/

void LoadLatchMem (void)
{
    LoadLatchMemory();
}

//==========================================================================

/*
===================
=
= FizzleFade
=
= returns true if aborted
=
= It uses maximum-length Linear Feedback Shift Registers (LFSR) counters.
= You can find a list of them with lengths from 3 to 168 at:
= http://www.xilinx.com/support/documentation/application_notes/xapp052.pdf
= Many thanks to Xilinx for this list!!!
=
===================
*/

// XOR masks for the pseudo-random number sequence starting with n=17 bits
static const uint32_t rndmasks[] = {
                    // n    XNOR from (starting at 1, not 0 as usual)
    0x00012000,     // 17   17,14
    0x00020400,     // 18   18,11
    0x00040023,     // 19   19,6,2,1
    0x00090000,     // 20   20,17
    0x00140000,     // 21   21,19
    0x00300000,     // 22   22,21
    0x00420000,     // 23   23,18
    0x00e10000,     // 24   24,23,22,17
    0x01200000,     // 25   25,22      (this is enough for 8191x4095)
};

static unsigned int rndbits_y;
static unsigned int rndmask;

// Returns the number of bits needed to represent the given value
static int log2_ceil(uint32_t x)
{
    int n = 0;
    uint32_t v = 1;
    while(v < x)
    {
        n++;
        v <<= 1;
    }
    return n;
}

void VH_Startup()
{
    int rndbits_x = log2_ceil(screenWidth);
    rndbits_y = log2_ceil(screenHeight);

    int rndbits = rndbits_x + rndbits_y;
    if(rndbits < 17)
        rndbits = 17;       // no problem, just a bit slower
    else if(rndbits > 25)
        rndbits = 25;       // fizzle fade will not fill whole screen

    rndmask = rndmasks[rndbits - 17];
}

boolean FizzleFade (int x1, int y1,
    unsigned width, unsigned height, unsigned frames, boolean abortable)
{
    return SubFizzleFade(x1, y1, width, height, frames, abortable, rndbits_y, rndmask);
}
