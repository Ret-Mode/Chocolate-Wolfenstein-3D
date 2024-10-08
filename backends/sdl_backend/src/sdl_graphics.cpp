#include "sdl_graphics.h"
#include "wl_def.h"
#include "crt.h"

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

#ifdef _WIN32

FILE _iob[] = {*stdin, *stdout, *stderr};

extern "C" FILE * __cdecl __iob_func(void)
{
    return _iob;
}

#include "SDL.h"
#include "SDL_syswm.h"
#endif
#if !defined(_WIN32)
#include <SDL/SDL.h>
#endif

static SDL_Surface *latchpics[NUMLATCHPICS];
static SDL_Surface *screenBuffer;

static unsigned screenBits;
static unsigned curPitch;

static SDL_Surface *screen = NULL;
static SDL_Surface *curSurface;


#define CASSERT(x) extern int ASSERT_COMPILE[((x) != 0) * 2 - 1];
#define WOLF_RGB(r, g, b) {(r)*255/63, (g)*255/63, (b)*255/63, 0}

SDL_Color gamepal[]={
    #if defined (SPEAR) || defined (SPEARDEMO)
    #include "sodpal.inc"
    #else
    #include "wolfpal.inc"
    #endif
};

CASSERT(lengthof(gamepal) == 256)

static SDL_Color palette1[256];
static SDL_Color palette2[256];
static SDL_Color curpal[256];

#define NUMREDSHIFTS    6
#define REDSTEPS        8

#define NUMWHITESHIFTS  3
#define WHITESTEPS      20
#define WHITETICS       6

static SDL_Color redshifts[NUMREDSHIFTS][256];
static SDL_Color whiteshifts[NUMWHITESHIFTS][256];
static SDL_Joystick *Joystick;
static int JoyNumHats;

static int GrabInput = false;

byte        ASCIINames[] =      // Unshifted ASCII for scan codes       // TODO: keypad
{
//   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,8  ,9  ,0  ,0  ,0  ,13 ,0  ,0  ,    // 0
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,27 ,0  ,0  ,0  ,    // 1
    ' ',0  ,0  ,0  ,0  ,0  ,0  ,39 ,0  ,0  ,'*','+',',','-','.','/',    // 2
    '0','1','2','3','4','5','6','7','8','9',0  ,';',0  ,'=',0  ,0  ,    // 3
    '`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',    // 4
    'p','q','r','s','t','u','v','w','x','y','z','[',92 ,']',0  ,0  ,    // 5
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,    // 6
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0       // 7
};
byte ShiftNames[] =     // Shifted ASCII for scan codes
{
//   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,8  ,9  ,0  ,0  ,0  ,13 ,0  ,0  ,    // 0
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,27 ,0  ,0  ,0  ,    // 1
    ' ',0  ,0  ,0  ,0  ,0  ,0  ,34 ,0  ,0  ,'*','+','<','_','>','?',    // 2
    ')','!','@','#','$','%','^','&','*','(',0  ,':',0  ,'+',0  ,0  ,    // 3
    '~','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',    // 4
    'P','Q','R','S','T','U','V','W','X','Y','Z','{','|','}',0  ,0  ,    // 5
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,    // 6
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0       // 7
};
byte SpecialNames[] =   // ASCII for 0xe0 prefixed codes
{
//   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,    // 0
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,13 ,0  ,0  ,0  ,    // 1
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,    // 2
    0  ,0  ,0  ,0  ,0  ,'/',0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,    // 3
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,    // 4
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,    // 5
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,    // 6
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0       // 7
};


static int GetLatchPicWidth(int which) {
    return latchpics[which]->w;
}
static int GetLatchPicHeight(int which) {
    return latchpics[which]->h;
}

void DelayWolfTicks(int ticks) {
    SDL_Delay(ticks * 100 / 7);
}

void DelayMilliseconds(int milliseconds) {
    SDL_Delay(milliseconds);
}

void DelayVBL(int param) {
    SDL_Delay(param*8);
}

unsigned int GetWolfTicks(void) {
    return (SDL_GetTicks()*7)/100;
}

unsigned int GetMilliseconds(void) {
    return SDL_GetTicks();
}

static unsigned char *GraphicLockBytes(void *surface)
{
    SDL_Surface *src = (SDL_Surface*)surface;
    if(SDL_MUSTLOCK(src))
    {
        if(SDL_LockSurface(src) < 0)
            return NULL;
    }
    return (unsigned char *) src->pixels;
}

static void GraphicUnlockBytes(void *surface)
{
    SDL_Surface *src = (SDL_Surface*)surface;
    if(SDL_MUSTLOCK(src))
    {
        SDL_UnlockSurface(src);
    }
}

void *GetScreenBuffer(void) {
    return (void *)screenBuffer;
}

static void *GetScreen(void) {
    return (void *) screen;
}

static void GetCurrentPaletteColor(int color, int *red, int *green, int *blue) {
    SDL_Color *col = &curpal[color];
    *red = col->r;
    *green = col->g;
    *blue = col->b;
}

static unsigned GetScreenBits(void) {
    return screenBits;
}

static void *GetCurSurface(void) {
    return (void*) curSurface;
}

void SetWholePalette(void *palette, int forceupdate) {
    memcpy(curpal, (SDL_Color *)palette, sizeof(SDL_Color) * 256);

    if(GetScreenBits() == 8)
        SDL_SetPalette((SDL_Surface *)GetScreenBuffer(), SDL_PHYSPAL, (SDL_Color *)palette, 0, 256);
    else
    {
        SDL_SetPalette((SDL_Surface*)GetCurSurface(), SDL_LOGPAL, (SDL_Color *)palette, 0, 256);
        if(forceupdate)
        {
            SDL_BlitSurface((SDL_Surface *)GetScreenBuffer(), NULL, (SDL_Surface *)GetScreen(), NULL);
            CRT_DAC();
        }
    }
}

static void FillPalette(int red, int green, int blue) {
    int i;
    SDL_Color pal[256];

    for(i=0; i<256; i++)
    {
        pal[i].r = red;
        pal[i].g = green;
        pal[i].b = blue;
    }
    SetWholePalette((void*)pal, true);
}

static void GetWholePalette(void *palette) {
    memcpy(palette, curpal, sizeof(SDL_Color) * 256);
}

static unsigned char *GetCurSurfacePixels(void) {
    return (unsigned char*) curSurface->pixels;
}

void ClearCurrentSurface(unsigned int color) {
    SDL_FillRect(curSurface, NULL, color);
}

static unsigned char *GetSurfacePixels(void *surface) {
    return (unsigned char*)((SDL_Surface*)surface)->pixels;
}

static unsigned short GetSurfacePitch(void *surface) {
    return ((SDL_Surface*)surface)->pitch;
}

void *GetGamePal(void) {
    return (void*)gamepal;
}

void CenterWindow(void) {
    // JUST FOR WIN32
    #ifdef _WIN32
    struct SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);

    if(SDL_GetWMInfo(&wmInfo) != -1)
    {
        HWND hwndSDL = wmInfo.window;
        DWORD style = GetWindowLong(hwndSDL, GWL_STYLE) & ~WS_SYSMENU;
        SetWindowLong(hwndSDL, GWL_STYLE, style);
        SetWindowPos(hwndSDL, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
    #endif
}

void ConvertPaletteToRGB(unsigned char *pixelPointer, int width, int height) {
    for (int i=0; i < width*height; i++) {
        unsigned char paletteIndex;
        paletteIndex = ((uint8_t*)screenBuffer->pixels)[i];
        *pixelPointer++ = curpal[paletteIndex].r;
        *pixelPointer++ = curpal[paletteIndex].g;
        *pixelPointer++ = curpal[paletteIndex].b;
    }
}

static void ScreenToScreen (void *source, void *dest) {
    SDL_BlitSurface((SDL_Surface *)source, NULL, (SDL_Surface *)dest, NULL);
    CRT_DAC();
}

static void LatchToScreenScaledCoord(int which, int xsrc, int ysrc, int width, int height, int scxdest, int scydest) {
    void *source = latchpics[which];
    SDL_Rect srcrect = { xsrc, ysrc, width, height };
    SDL_Rect destrect = { scxdest, scydest, 0, 0 }; // width and height are ignored
    SDL_BlitSurface((SDL_Surface *)source, &srcrect, (SDL_Surface*)GetCurSurface(), &destrect);
}

void InitGraphics(void) {
        // initialize SDL
#if defined _WIN32
    putenv("SDL_VIDEODRIVER=directx");
#endif
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
    {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);
}

void ReadMouseState(int *btns, int *mx, int *my) {
    int mousex, mousey, buttons;
    buttons = SDL_GetMouseState(&mousex, &mousey);
    int middlePressed = buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE);
    int rightPressed = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
    buttons &= ~(SDL_BUTTON(SDL_BUTTON_MIDDLE) | SDL_BUTTON(SDL_BUTTON_RIGHT));
    if(middlePressed) buttons |= 1 << 2;
    if(rightPressed) buttons |= 1 << 1;
    
    *mx = mousex;
    *my = mousey;
    *btns = buttons;
}

void CenterMouse(int width, int height) {
    SDL_WarpMouse(width / 2, height / 2);
}

void InitRedShifts (void)
{
    SDL_Color *workptr, *baseptr;
    int i, j, delta;

    for (i = 1; i <= NUMREDSHIFTS; i++)
    {
        workptr = redshifts[i - 1];
        baseptr = gamepal;

        for (j = 0; j <= 255; j++)
        {
            delta = 256 - baseptr->r;
            workptr->r = baseptr->r + delta * i / REDSTEPS;
            delta = -baseptr->g;
            workptr->g = baseptr->g + delta * i / REDSTEPS;
            delta = -baseptr->b;
            workptr->b = baseptr->b + delta * i / REDSTEPS;
            baseptr++;
            workptr++;
        }
    }
}


void InitWhiteShifts (void)
{
    SDL_Color *workptr, *baseptr;
    int i, j, delta;
    for (i = 1; i <= NUMWHITESHIFTS; i++)
    {
        workptr = whiteshifts[i - 1];
        baseptr = gamepal;

        for (j = 0; j <= 255; j++)
        {
            delta = 256 - baseptr->r;
            workptr->r = baseptr->r + delta * i / WHITESTEPS;
            delta = 248 - baseptr->g;
            workptr->g = baseptr->g + delta * i / WHITESTEPS;
            delta = 0-baseptr->b;
            workptr->b = baseptr->b + delta * i / WHITESTEPS;
            baseptr++;
            workptr++;
        }
    }
}

int GetWhitePaletteShifts(void) {
    return NUMWHITESHIFTS;
}


int GetRedPaletteShifts(void) {
    return NUMREDSHIFTS;
}

int GetWhitePaletteSwapMs(void) {
    return WHITETICS;
}

void* GetRedPaletteShifted(int which) {
    return (void*)redshifts[which - 1];
}

void* GetWhitePaletteShifted(int which) {
    return (void*)whiteshifts[which - 1];
}

void PaletteFadeOut (int start, int end, int red, int green, int blue, int steps) {
    int         i,j,orig,delta;
    SDL_Color   *origptr, *newptr;

    red = red * 255 / 63;
    green = green * 255 / 63;
    blue = blue * 255 / 63;

    DelayMilliseconds(8);
    GetWholePalette(palette1);
    memcpy(palette2, palette1, sizeof(SDL_Color) * 256);

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
        SetWholePalette(palette2, true);
    }

    FillPalette(red,green,blue);
}

void PaletteFadeIn(int start, int end, void *platettePtr, int steps) {
    SDL_Color *palette = (SDL_Color *)platettePtr;
    int i,j,delta;

    DelayMilliseconds(8);
    GetWholePalette(palette1);
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
        SetWholePalette(palette2, true);
    }

//
// final color
//
    SetWholePalette(palette, true);
    screenfaded = false;
}

void SaveBitmap(char *filename) {
    SDL_SaveBMP((SDL_Surface *)GetCurSurface(), filename);
}

int GetMouseButtons(void) {
    int buttons = SDL_GetMouseState(NULL, NULL);
    int middlePressed = buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE);
    int rightPressed = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
    buttons &= ~(SDL_BUTTON(SDL_BUTTON_MIDDLE) | SDL_BUTTON(SDL_BUTTON_RIGHT));
    if(middlePressed) buttons |= 1 << 2;
    if(rightPressed) buttons |= 1 << 1;

    return buttons;
}

extern void SetScreenBufferPitch(unsigned bpitch);
void SetVGAMode(unsigned *scrWidth, unsigned *scrHeight, unsigned *sclFactor)  {

    SDL_WM_SetCaption("Wolfenstein 3D", NULL);
    const SDL_VideoInfo *vidInfo = SDL_GetVideoInfo();
    screenBits = vidInfo->vfmt->BitsPerPixel;

    //Fab's CRT Hack
    //Adjust height so the screen is 4:3 aspect ratio
    *scrHeight=*scrWidth * 3.0/4.0;
    
    screen = SDL_SetVideoMode(*scrWidth, *scrHeight, screenBits,
          (usedoublebuffering ? SDL_HWSURFACE | SDL_DOUBLEBUF : 0)
        | (screenBits == 8 ? SDL_HWPALETTE : 0)
        | (fullscreen ? SDL_FULLSCREEN : 0) | SDL_OPENGL | SDL_OPENGLBLIT);
    

    if(!screen)
    {
        printf("Unable to set %ix%ix%i video mode: %s\n", *scrWidth, *scrHeight, GetScreenBits(), SDL_GetError());
        exit(1);
    }
    if((screen->flags & SDL_DOUBLEBUF) != SDL_DOUBLEBUF)
        usedoublebuffering = false;
    SDL_ShowCursor(SDL_DISABLE);

    SDL_SetColors(screen, gamepal, 0, 256);
    memcpy(curpal, gamepal, sizeof(SDL_Color) * 256);

    //Fab's CRT Hack
    CRT_Init(*scrWidth);
    
    //Fab's CRT Hack
    *scrWidth=320;
    *scrHeight=200;

    screenBuffer = SDL_CreateRGBSurface(SDL_SWSURFACE, *scrWidth,
        *scrHeight, 8, 0, 0, 0, 0);
    if(!screenBuffer)
    {
        printf("Unable to create screen buffer surface: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_SetColors(screenBuffer, (SDL_Color*)gamepal, 0, 256);

    SetScreenBufferPitch(screenBuffer->pitch); 

    curSurface = screenBuffer;
    curPitch = screenBuffer->pitch;

    *sclFactor = *scrWidth/320;
    if(*scrHeight/200 < *sclFactor) *sclFactor = *scrHeight/200;

}

void LoadLatchMemory (void) {
    int i,width,height,start,end;
    byte *src;
    SDL_Surface *surf;

//
// tile 8s
//
    
    surf = SDL_CreateRGBSurface(SDL_HWSURFACE, 8*8,
        ((NUMTILE8 + 7) / 8) * 8, 8, 0, 0, 0, 0);
    if(surf == NULL)
    {
        Quit("Unable to create surface for tiles!");
    }
    SDL_SetColors(surf, gamepal, 0, 256);

    latchpics[0] = surf;
    CA_CacheGrChunk (STARTTILE8);
    src = (byte*)GetGrSegs(STARTTILE8);

    for (i=0;i<NUMTILE8;i++)
    {
        VL_MemToLatch (src, 8, 8, (void*)surf, (i & 7) * 8, (i >> 3) * 8);
        src += 64;
    }
    // FILE *fp = fopen("data3.raw", "wb");
    // GraphicLockBytes(surf);
    // fwrite(GetSurfacePixels(surf), (8*8) * (((NUMTILE8 + 7) / 8) * 8), 1, fp);
    // GraphicUnlockBytes(surf);
    //fclose(fp);
    ClearGrSegs (STARTTILE8);

//
// pics
//
    start = LATCHPICS_LUMP_START;
    end = LATCHPICS_LUMP_END;

    for (i=start;i<=end;i++)
    {
        width = pictable[i-STARTPICS].width;
        height = pictable[i-STARTPICS].height;
        surf = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, 8, 0, 0, 0, 0);
        if(surf == NULL)
        {
            Quit("Unable to create surface for picture!");
        }
        SDL_SetColors(surf, gamepal, 0, 256);

        latchpics[2+i-start] = surf;

        CA_CacheGrChunk (i);
        VL_MemToLatch ((byte*)GetGrSegs(i), width, height, (void*)surf, 0, 0);
        ClearGrSegs(i);
    }
}

int SubFizzleFade (int x1, int y1,
                       unsigned width, unsigned height, 
                       unsigned frames, int abortable,
                       int rndbits_y, int rndmask)
{
    SDL_Surface *source = (SDL_Surface *)GetScreenBuffer();
    unsigned x, y, frame, pixperframe;
    int32_t  rndval, lastrndval;
    int      first = 1;

    lastrndval = 0;
    pixperframe = width * height / frames;

    IN_StartAck ();

    frame = GetWolfTicks();

    //can't rely on screen as dest b/c crt.cpp writes over it with screenBuffer
    //can't rely on screenBuffer as source for same reason: every flip it has to be updated
    SDL_Surface *source_copy = SDL_ConvertSurface(source, source->format, source->flags);
    SDL_Surface *screen_copy = SDL_ConvertSurface(screen, screen->format, screen->flags);

    byte *srcptr = GraphicLockBytes((void*)source_copy);
    do
    {
        if(abortable && IN_CheckAck ())
        {
            GraphicUnlockBytes((void*)source_copy);
            SDL_BlitSurface(screen_copy, NULL, screenBuffer, NULL);
            SDL_BlitSurface(screenBuffer, NULL, screen, NULL);
            //SDL_Flip((SDL_Surface *)GetScreen());
            CRT_DAC();
            SDL_FreeSurface(source_copy);
            SDL_FreeSurface(screen_copy);
            return true;
        }

        byte *destptr = GraphicLockBytes((void*)screen_copy);

        rndval = lastrndval;

        // When using double buffering, we have to copy the pixels of the last AND the current frame.
        // Only for the first frame, there is no "last frame"
        for(int i = first; i < 2; i++)
        {
            for(unsigned p = 0; p < pixperframe; p++)
            {
                //
                // seperate random value into x/y pair
                //

                x = rndval >> rndbits_y;
                y = rndval & ((1 << rndbits_y) - 1);

                //
                // advance to next random element
                //

                rndval = (rndval >> 1) ^ (rndval & 1 ? 0 : rndmask);

                if(x >= width || y >= height)
                {
                    if(rndval == 0)     // entire sequence has been completed
                        goto finished;
                    p--;
                    continue;
                }

                //
                // copy one pixel
                //

                if(screenBits == 8)
                {
                    *(destptr + (y1 + y) * screen->pitch + x1 + x)
                        = *(srcptr + (y1 + y) * source->pitch + x1 + x);
                }
                else
                {
                    byte col = *(srcptr + (y1 + y) * source->pitch + x1 + x);
                    int red, green, blue;
                    GetCurrentPaletteColor(col, &red, &green, &blue);
                    uint32_t fullcol = SDL_MapRGB(screen->format, red, green, blue);
                    memcpy(destptr + (y1 + y) * screen->pitch + (x1 + x) * screen->format->BytesPerPixel,
                        &fullcol, screen->format->BytesPerPixel);
                }

                if(rndval == 0)     // entire sequence has been completed
                    goto finished;
            }

            if(!i || first) lastrndval = rndval;
        }

        // If there is no double buffering, we always use the "first frame" case
        if(usedoublebuffering) first = 0;

        GraphicUnlockBytes((void*)screen_copy);
        SDL_BlitSurface(screen_copy, NULL, screenBuffer, NULL);
        SDL_BlitSurface(screenBuffer, NULL, screen, NULL);
        //SDL_Flip(screen);
        CRT_DAC();

        frame++;
        unsigned int val = frame - GetWolfTicks();
        if(val>0) {
            DelayWolfTicks(val);
        }
    } while (1);

finished:
    GraphicUnlockBytes((void*)source_copy);
    GraphicUnlockBytes((void*)screen_copy);
    SDL_BlitSurface(screen_copy, NULL, screenBuffer, NULL);
    SDL_BlitSurface(screenBuffer, NULL, screen, NULL);
    //SDL_Flip(screen);
    CRT_DAC();
    SDL_FreeSurface(source_copy);
    SDL_FreeSurface(screen_copy);
    return false;
}

void GetJoystickDelta(int *dx,int *dy) {
    if(!Joystick)
    {
        *dx = *dy = 0;
        return;
    }

    SDL_JoystickUpdate();

    int x = SDL_JoystickGetAxis(Joystick, 0) >> 8;
    int y = SDL_JoystickGetAxis(Joystick, 1) >> 8;

    if(param_joystickhat != -1)
    {
        uint8_t hatState = SDL_JoystickGetHat(Joystick, param_joystickhat);
        if(hatState & SDL_HAT_RIGHT)
            x += 127;
        else if(hatState & SDL_HAT_LEFT)
            x -= 127;
        if(hatState & SDL_HAT_DOWN)
            y += 127;
        else if(hatState & SDL_HAT_UP)
            y -= 127;

        if(x < -128) x = -128;
        else if(x > 127) x = 127;

        if(y < -128) y = -128;
        else if(y > 127) y = 127;
    }

    *dx = x;
    *dy = y;
}

void GetJoystickFineDelta(int *dx, int *dy) {
    if(!Joystick)
    {
        *dx = 0;
        *dy = 0;
        return;
    }

    SDL_JoystickUpdate();
    int x = SDL_JoystickGetAxis(Joystick, 0);
    int y = SDL_JoystickGetAxis(Joystick, 1);

    if(x < -128) x = -128;
    else if(x > 127) x = 127;

    if(y < -128) y = -128;
    else if(y > 127) y = 127;

    *dx = x;
    *dy = y;
}

int GetJoystickButtons(void) {
    if(!Joystick) return 0;

    SDL_JoystickUpdate();

    int res = 0;
    for(int i = 0; i < JoyNumButtons && i < 32; i++)
        res |= SDL_JoystickGetButton(Joystick, i) << i;
    return res;
}

int IsJoystickPresent(void) {
    return Joystick != NULL;
}

static void processEvent(SDL_Event *event)
{
    switch (event->type)
    {
        // exit if the window is closed
        case SDL_QUIT:
            Quit(NULL);

        // check for keypresses
        case SDL_KEYDOWN:
        {
            if(event->key.keysym.sym==SDLK_SCROLLOCK || event->key.keysym.sym==SDLK_F12)
            {
                GrabInput = !GrabInput;
                SDL_WM_GrabInput(GrabInput ? SDL_GRAB_ON : SDL_GRAB_OFF);
                return;
            }

            LastScan = event->key.keysym.sym;
            SDLMod mod = SDL_GetModState();
            if(Keyboard[sc_Alt])
            {
                if(LastScan==SDLK_F4)
                    Quit(NULL);
            }

            if(LastScan == SDLK_KP_ENTER) LastScan = SDLK_RETURN;
            else if(LastScan == SDLK_RSHIFT) LastScan = SDLK_LSHIFT;
            else if(LastScan == SDLK_RALT) LastScan = SDLK_LALT;
            else if(LastScan == SDLK_RCTRL) LastScan = SDLK_LCTRL;
            else
            {
                if((mod & KMOD_NUM) == 0)
                {
                    switch(LastScan)
                    {
                        case SDLK_KP2: LastScan = SDLK_DOWN; break;
                        case SDLK_KP4: LastScan = SDLK_LEFT; break;
                        case SDLK_KP6: LastScan = SDLK_RIGHT; break;
                        case SDLK_KP8: LastScan = SDLK_UP; break;
                    }
                }
            }

            int sym = LastScan;
            if(sym >= 'a' && sym <= 'z')
                sym -= 32;  // convert to uppercase

            if(mod & (KMOD_SHIFT | KMOD_CAPS))
            {
                if(sym < lengthof(ShiftNames) && ShiftNames[sym])
                    LastASCII = ShiftNames[sym];
            }
            else
            {
                if(sym < lengthof(ASCIINames) && ASCIINames[sym])
                    LastASCII = ASCIINames[sym];
            }

			if (LastScan<SDLK_i){
			}

			if(LastScan<SDLK_LAST){
                Keyboard[LastScan] = 1;
			}
            if(LastScan == SDLK_PAUSE)
                Paused = true;
            break;
        }

        case SDL_KEYUP:
        {
            int key = event->key.keysym.sym;
            if(key == SDLK_KP_ENTER) key = SDLK_RETURN;
            else if(key == SDLK_RSHIFT) key = SDLK_LSHIFT;
            else if(key == SDLK_RALT) key = SDLK_LALT;
            else if(key == SDLK_RCTRL) key = SDLK_LCTRL;
            else
            {
                if((SDL_GetModState() & KMOD_NUM) == 0)
                {
                    switch(key)
                    {
                        case SDLK_KP2: key = SDLK_DOWN; break;
                        case SDLK_KP4: key = SDLK_LEFT; break;
                        case SDLK_KP6: key = SDLK_RIGHT; break;
                        case SDLK_KP8: key = SDLK_UP; break;
                    }
                }
            }

			if(key<SDLK_LAST){
                Keyboard[key] = 0;
			}
            break;
        }
    }
}

void WaitAndProcessEvents(void) {
    SDL_Event event;
    if(!SDL_WaitEvent(&event)) return;
    do
    {
        processEvent(&event);
    }
    while(SDL_PollEvent(&event));
}

void ProcessEvents(void ) {
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        processEvent(&event);
    }
}

int IsInputGrabbed(void) {
    return GrabInput;
}

void JoystickShutdown(void) {
    if(Joystick) {
        SDL_JoystickClose(Joystick);
    }
}

void JoystickStartup(void) {
    if(param_joystickindex >= 0 && param_joystickindex < SDL_NumJoysticks())
    {
        Joystick = SDL_JoystickOpen(param_joystickindex);
        if(Joystick)
        {
            JoyNumButtons = SDL_JoystickNumButtons(Joystick);
            if(JoyNumButtons > 32) JoyNumButtons = 32;      // only up to 32 buttons are supported
            JoyNumHats = SDL_JoystickNumHats(Joystick);
            if(param_joystickhat < -1 || param_joystickhat >= JoyNumHats)
                Quit("The joystickhat param must be between 0 and %i!", JoyNumHats - 1);
        }
    }

    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);

    if(fullscreen || forcegrabmouse)
    {
        GrabInput = true;
        SDL_WM_GrabInput(SDL_GRAB_ON);
    }
}

void CheckIsJoystickCorrect(void) {
    int numJoysticks = SDL_NumJoysticks();
    if(param_joystickindex && (param_joystickindex < -1 || param_joystickindex >= numJoysticks))
    {
        if(!numJoysticks)
            printf("No joysticks are available to SDL!\n");
        else
            printf("The joystick index must be between -1 and %i!\n", numJoysticks - 1);
        exit(1);
    }
}







/* Contents of id_vl file */

void VL_Plot (int x, int y, int color)
{
    assert(x >= 0 && (unsigned) x < screenWidth
            && y >= 0 && (unsigned) y < screenHeight
            && "VL_Plot: Pixel out of bounds!");

    GraphicLockBytes(GetCurSurface());
    ((byte *) GetCurSurfacePixels())[y * curPitch + x] = color;
    GraphicUnlockBytes(GetCurSurface());
}


byte VL_GetPixel (int x, int y)
{
    assert_ret(x >= 0 && (unsigned) x < screenWidth
            && y >= 0 && (unsigned) y < screenHeight
            && "VL_GetPixel: Pixel out of bounds!");

    GraphicLockBytes(GetCurSurface());
    byte col = ((byte *) GetCurSurfacePixels())[y * curPitch + x];
    GraphicUnlockBytes(GetCurSurface());
    return col;
}

void VL_Hlin (unsigned x, unsigned y, unsigned width, int color)
{
    assert(x >= 0 && x + width <= screenWidth
            && y >= 0 && y < screenHeight
            && "VL_Hlin: Destination rectangle out of bounds!");

    GraphicLockBytes(GetCurSurface());
    unsigned char *dest = ((byte *) GetCurSurfacePixels()) + y * curPitch + x;
    memset(dest, color, width);
    GraphicUnlockBytes(GetCurSurface());
}

void VL_Vlin (int x, int y, int height, int color)
{
    assert(x >= 0 && (unsigned) x < screenWidth
            && y >= 0 && (unsigned) y + height <= screenHeight
            && "VL_Vlin: Destination rectangle out of bounds!");

    GraphicLockBytes(GetCurSurface());
    unsigned char *dest = ((byte *) GetCurSurfacePixels()) + y * curPitch + x;

    while (height--)
    {
        *dest = color;
        dest += curPitch;
    }
    GraphicUnlockBytes(GetCurSurface());
}


void VL_BarScaledCoord (int scx, int scy, int scwidth, int scheight, int color)
{
    assert(scx >= 0 && (unsigned) scx + scwidth <= screenWidth
            && scy >= 0 && (unsigned) scy + scheight <= screenHeight
            && "VL_BarScaledCoord: Destination rectangle out of bounds!");

    GraphicLockBytes(GetCurSurface());
    unsigned char *dest = ((byte *) GetCurSurfacePixels()) + scy * curPitch + scx;

    while (scheight--)
    {
        memset(dest, color, scwidth);
        dest += curPitch;
    }
    GraphicUnlockBytes(GetCurSurface());
}

void VL_MemToScreenScaledCoord (byte *source, int width, int height, int destx, int desty)
{
    assert(destx >= 0 && destx + width * scaleFactor <= screenWidth
            && desty >= 0 && desty + height * scaleFactor <= screenHeight
            && "VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");

    GraphicLockBytes(GetCurSurface());
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
    GraphicUnlockBytes(GetCurSurface());
}

void VL_MemToLatch(byte *source, int width, int height,
    void *destSurface, int x, int y)
{
    assert(x >= 0 && (unsigned) x + width <= screenWidth
            && y >= 0 && (unsigned) y + height <= screenHeight
            && "VL_MemToLatch: Destination rectangle out of bounds!");

    GraphicLockBytes((void*)destSurface);
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
    GraphicUnlockBytes(destSurface);
}

void VL_MemToScreenScaledCoord (byte *source, int origwidth, int origheight, int srcx, int srcy,
                                int destx, int desty, int width, int height)
{
    assert(destx >= 0 && destx + width * scaleFactor <= screenWidth
            && desty >= 0 && desty + height * scaleFactor <= screenHeight
            && "VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");

    GraphicLockBytes(GetCurSurface());
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
    GraphicUnlockBytes(GetCurSurface());
}


void VL_LatchToScreenScaledCoord(int which, int xsrc, int ysrc,
    int width, int height, int scxdest, int scydest)
{
    void *source = latchpics[which];
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
            GraphicLockBytes(source);
            byte *src = (byte *) GetSurfacePixels(source);
            unsigned srcPitch = GetSurfacePitch(source);

            GraphicLockBytes(GetCurSurface());
            byte *vbuf = (byte *) GetCurSurfacePixels();
            for(int j=0,scj=0; j<height; j++, scj++)
            {
                for(int i=0,sci=0; i<width; i++, sci++)
                {
                    byte col = src[(ysrc + j)*srcPitch + xsrc + i];
                    vbuf[(scydest+scj)*curPitch+scxdest+sci] = col;
                }
            }
            GraphicUnlockBytes(GetCurSurface());
            GraphicUnlockBytes((void*)source);
        }
        else
        {
            LatchToScreenScaledCoord(which, xsrc, ysrc, width, height, scxdest, scydest);
        }
    }
    else
    {
        GraphicLockBytes(source);
        byte *src = (byte *) GetSurfacePixels(source);
        unsigned srcPitch =  GetSurfacePitch(source);

        GraphicLockBytes(GetCurSurface());
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
        GraphicUnlockBytes(GetCurSurface());
        GraphicUnlockBytes(source);
    }
}

void LatchDrawPic (unsigned x, unsigned y, unsigned picnum)
{
    int which = 2+picnum-LATCHPICS_LUMP_START;
    VL_LatchToScreenScaledCoord(which,0,0,GetLatchPicWidth(which),GetLatchPicHeight(which), scaleFactor*x*8,scaleFactor*y);
}

void LatchDrawPicScaledCoord (unsigned scx, unsigned scy, unsigned picnum)
{
    int which = 2+picnum-LATCHPICS_LUMP_START;
    VL_LatchToScreenScaledCoord(which,0,0,GetLatchPicWidth(which),GetLatchPicHeight(which),scx*8,scy);
}

byte *VL_LockSurface(void* surface)
{
    return GraphicLockBytes(surface);
}

void VL_UnlockSurface(void *surface)
{
    GraphicUnlockBytes(surface);
}

static void VL_ScreenToScreen (void *source, void *dest)
{
    ScreenToScreen (source, dest);
}

void VH_UpdateScreen()
{
    VL_ScreenToScreen(GetScreenBuffer(), GetScreen());
}

void VWB_DrawPropString(const char* string, fontData_t *fd)
{
    fontstruct  *font;
    int         width, step, height;
    byte        *source, *dest;
    byte        ch;

    byte *vbuf = VL_LockSurface(GetCurSurface());

    font = (fontstruct *) GetGrSegs(STARTFONT+fd->fontnumber);
    height = font->height;
    dest = vbuf + scaleFactor * (fd->py * curPitch + fd->px);

    while ((ch = (byte)*string++)!=0)
    {
        width = step = font->width[ch];
        source = ((byte *)font)+font->location[ch];
        while (width--)
        {
            for(int i=0;i<height;i++)
            {
                if(source[i*step])
                {
                    for(unsigned sy=0; sy<scaleFactor; sy++)
                        for(unsigned sx=0; sx<scaleFactor; sx++)
                            dest[(scaleFactor*i+sy)*curPitch+sx]=fontcolor;
                }
            }

            source++;
            (fd->px)++;
            dest+=scaleFactor;
        }
    }

    VL_UnlockSurface(GetCurSurface());
}

void BlitPictureToScreen(unsigned char *pic) {
    byte *vbuf = VL_LockSurface(GetCurSurface());
    for(int y = 0, scy = 0; y < 200; y++, scy += scaleFactor)
    {
        for(int x = 0, scx = 0; x < 320; x++, scx += scaleFactor)
        {
            byte col = pic[(y * 80 + (x >> 2)) + (x & 3) * 80 * 200];
            for(unsigned i = 0; i < scaleFactor; i++)
                for(unsigned j = 0; j < scaleFactor; j++)
                    vbuf[(scy + i) * curPitch + scx + j] = col;
        }
    } 
    VL_UnlockSurface(GetCurSurface());
}