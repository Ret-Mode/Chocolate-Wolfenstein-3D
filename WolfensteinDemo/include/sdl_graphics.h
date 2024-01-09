#ifndef __SDL_GRAPHICS__
#define __SDL_GRAPHICS__

void *GetLatchPic(int which);
int GetLatchPicWidth(int which);
int GetLatchPicHeight(int which);
void SetLatchPic(int which, void *data);
void DelayWolfTicks(int ticks);
unsigned int GetWolfTicks(void);
unsigned char *GraphicLockBytes(void *surface);
void GraphicUnlockBytes(void *surface);
void *CreateScreenBuffer(void *gamepal, unsigned int *bufferPitch, unsigned int screenWidth, unsigned int screenHeight);
void *GetScreenBuffer(void);
#endif