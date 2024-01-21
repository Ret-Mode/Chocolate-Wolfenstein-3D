#ifndef __SDL_GRAPHICS__
#define __SDL_GRAPHICS__

void *GetLatchPic(int which);
int GetLatchPicWidth(int which);
int GetLatchPicHeight(int which);
void SetLatchPic(int which, void *data);
void DelayWolfTicks(int ticks);
void DelayMilliseconds(int milliseconds);
unsigned int GetWolfTicks(void);
unsigned char *GraphicLockBytes(void *surface);
void GraphicUnlockBytes(void *surface);
void *CreateScreenBuffer(void *gamepal, unsigned int *bufferPitch, unsigned int screenWidth, unsigned int screenHeight);
void *GetScreenBuffer(void);
unsigned char GetScreenBufferPixel(int offset);
void GetCurrentPaletteColor(int color, int *red, int *green, int *blue);
void SetCurrentPaletteColor(int color, int red, int green, int blue, unsigned int screenBits);
void SetWindowTitle(const char *title);
void SetScreenBits(void);
unsigned GetScreenBits(void);
void SetScreen(void *screenPtr);
void *GetScreen(void);
short GetScreenFlags(void);
unsigned short GetScreenPitch(void);
void *GetScreenFormat(void);
unsigned char GetScreenBytesPerPixel(void);
void *GetCurSurface(void);
void SetCurSurface(void *current);
unsigned char *GetCurSurfacePixels(void);
void ClearCurrentSurface(unsigned int color);
void *GetGamePal(void);
void CenterWindow(void);
void ConvertPaletteToRGB(unsigned char *pixelPointer, int width, int height);
void SetWholePalette(void *palette, int forceupdate);
void GetWholePalette(void *palette);
void SetScreenPalette(void);
void ConvertPalette(unsigned char *srcpal, void *dest, int numColors);
void FillPalette(int red, int green, int blue);
void ScreenToScreen (void *source, void *dest);
unsigned char *GetSurfacePixels(void *surface);
unsigned short GetSurfacePitch(void *surface);
void LatchToScreenScaledCoord(int which, int xsrc, int ysrc, int width, int height, int scxdest, int scydest);
#endif