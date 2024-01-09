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

unsigned int GetWolfTicks(void) {
    return (SDL_GetTicks()*7)/100;
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