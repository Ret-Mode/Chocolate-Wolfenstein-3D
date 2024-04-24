#include "sdl_graphics.h"
#include "wl_def.h"

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

static SDL_Surface *screen = NULL;
static SDL_Surface *curSurface;
// static SDL_Color gamepal[256];

#define CASSERT(x) extern int ASSERT_COMPILE[((x) != 0) * 2 - 1];
#define WOLF_RGB(r, g, b) {(r)*255/63, (g)*255/63, (b)*255/63, 0}

SDL_Color gamepal[]={
    #include "wolfpal.inc"
};

CASSERT(lengthof(gamepal) == 256)

// static SDL_Color palette1[256];
// static SDL_Color palette2[256];
static SDL_Color curpal[256];

#define NUMREDSHIFTS    6
#define REDSTEPS        8

#define NUMWHITESHIFTS  3
#define WHITESTEPS      20
#define WHITETICS       6

static SDL_Color redshifts[NUMREDSHIFTS][256];
static SDL_Color whiteshifts[NUMWHITESHIFTS][256];

void *GetLatchPic(int which) {
    return (void*) latchpics[which];
}

void SetLatchPic(int which, void *data) {
    latchpics[which] = (SDL_Surface *)data;
}

int GetLatchPicWidth(int which) {
    return latchpics[which]->w;
}
int GetLatchPicHeight(int which) {
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

unsigned char *GraphicLockBytes(void *surface)
{
    SDL_Surface *src = (SDL_Surface*)surface;
    if(SDL_MUSTLOCK(src))
    {
        if(SDL_LockSurface(src) < 0)
            return NULL;
    }
    return (unsigned char *) src->pixels;
}

void GraphicUnlockBytes(void *surface)
{
    SDL_Surface *src = (SDL_Surface*)surface;
    if(SDL_MUSTLOCK(src))
    {
        SDL_UnlockSurface(src);
    }
}

void *CreateScreenBuffer(void *gamepal, unsigned int *bufferPitch, unsigned int screenWidth, unsigned int screenHeight) {
    screenBuffer = SDL_CreateRGBSurface(SDL_SWSURFACE, screenWidth,
        screenHeight, 8, 0, 0, 0, 0);
    if(!screenBuffer)
    {
        printf("Unable to create screen buffer surface: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_SetColors(screenBuffer, (SDL_Color*)gamepal, 0, 256);

    *bufferPitch = screenBuffer->pitch;

    return (void *)screenBuffer;
}

void *GetScreenBuffer(void) {
    return (void *)screenBuffer;
}

unsigned char GetScreenBufferPixel(int offset) {
    return ((uint8_t*)screenBuffer->pixels)[offset];
}

void GetCurrentPaletteColor(int color, int *red, int *green, int *blue) {
    SDL_Color *col = &curpal[color];
    *red = col->r;
    *green = col->g;
    *blue = col->b;
}

void SetCurrentPaletteColor(int color, int red, int green, int blue, unsigned int screenBits) {
    SDL_Color col = { red, green, blue };
    curpal[color] = col;

    if(screenBits == 8)
        SDL_SetPalette(screen, SDL_PHYSPAL, &col, color, 1);
    else
    {
        SDL_SetPalette(curSurface, SDL_LOGPAL, &col, color, 1);
        SDL_BlitSurface((SDL_Surface *)GetScreenBuffer(), NULL, screen, NULL);
        SDL_Flip(screen);
    }
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
            SDL_Flip((SDL_Surface *)GetScreen());
        }
    }
}

void ConvertPalette(unsigned char *srcpal, void *dest, int numColors) {
    SDL_Color *destpal = (SDL_Color *)dest;
    for(int i=0; i<numColors; i++)
    {
        destpal[i].r = *srcpal++ * 255 / 63;
        destpal[i].g = *srcpal++ * 255 / 63;
        destpal[i].b = *srcpal++ * 255 / 63;
    }
}

void FillPalette(int red, int green, int blue) {
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

void GetWholePalette(void *palette) {
    memcpy(palette, curpal, sizeof(SDL_Color) * 256);
}

void SetScreenPalette(void) {
    SDL_SetColors((SDL_Surface*)GetScreen(), (SDL_Color*)GetGamePal(), 0, 256);
    memcpy(curpal, GetGamePal(), sizeof(SDL_Color) * 256);
}

void SetWindowTitle(const char *title) {
    SDL_WM_SetCaption(title, NULL);
}

void SetScreenBits(void) {
    const SDL_VideoInfo *vidInfo = SDL_GetVideoInfo();
    screenBits = vidInfo->vfmt->BitsPerPixel;
}

unsigned GetScreenBits(void) {
    return screenBits;
}

void SetScreen(void *screenPtr) {
    screen = (SDL_Surface *)screenPtr;
}

void *GetScreen(void) {
    return (void *) screen;
}

short GetScreenFlags(void) {
    return screen->flags;
}

unsigned short GetScreenPitch(void) {
    return screen->pitch;
}

void *GetScreenFormat(void) {
    return (void *)screen->format;
}

unsigned char GetScreenBytesPerPixel(void) {
    return screen->format->BytesPerPixel;
}

void *GetCurSurface(void) {
    return (void*) curSurface;
}

void SetCurSurface(void *current) {
    curSurface = (SDL_Surface *) current;
}

unsigned char *GetCurSurfacePixels(void) {
    return (unsigned char*) curSurface->pixels;
}

void ClearCurrentSurface(unsigned int color) {
    SDL_FillRect(curSurface, NULL, color);
}

unsigned char *GetSurfacePixels(void *surface) {
    return (unsigned char*)((SDL_Surface*)surface)->pixels;
}

unsigned short GetSurfacePitch(void *surface) {
    return ((SDL_Surface*)surface)->pitch;
}

void *GetGamePal(void) {
    return (void*)gamepal;
}

void CenterWindow(void) {
    // JUST FOR WIN32
    struct SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);

    if(SDL_GetWMInfo(&wmInfo) != -1)
    {
        HWND hwndSDL = wmInfo.window;
        DWORD style = GetWindowLong(hwndSDL, GWL_STYLE) & ~WS_SYSMENU;
        SetWindowLong(hwndSDL, GWL_STYLE, style);
        SetWindowPos(hwndSDL, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
}

void ConvertPaletteToRGB(unsigned char *pixelPointer, int width, int height) {
    for (int i=0; i < width*height; i++) {
        unsigned char paletteIndex;
        paletteIndex = GetScreenBufferPixel(i);
        *pixelPointer++ = curpal[paletteIndex].r;
        *pixelPointer++ = curpal[paletteIndex].g;
        *pixelPointer++ = curpal[paletteIndex].b;
    }
}

void ScreenToScreen (void *source, void *dest) {
    SDL_BlitSurface((SDL_Surface *)source, NULL, (SDL_Surface *)dest, NULL);
}

void LatchToScreenScaledCoord(int which, int xsrc, int ysrc, int width, int height, int scxdest, int scydest) {
    void *source = GetLatchPic(which);
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


//
// fade through intermediate frames
//
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

int GetNuberOfJoysticks(void) {
    return SDL_NumJoysticks();
}