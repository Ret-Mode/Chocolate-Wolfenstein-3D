#ifndef __SDL_GRAPHICS__
#define __SDL_GRAPHICS__

int GetLatchPicWidth(int which);
int GetLatchPicHeight(int which);
void DelayWolfTicks(int ticks);
void DelayMilliseconds(int milliseconds);
void DelayVBL(int param);
unsigned int GetMilliseconds(void);
unsigned int GetWolfTicks(void);
unsigned char *GraphicLockBytes(void *surface);
void GraphicUnlockBytes(void *surface);
void *GetScreenBuffer(void);
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
void ScreenToScreen (void *source, void *dest);
unsigned char *GetSurfacePixels(void *surface);
unsigned short GetSurfacePitch(void *surface);
void LatchToScreenScaledCoord(int which, int xsrc, int ysrc, int width, int height, int scxdest, int scydest);
void InitGraphics(void);
void ReadMouseState(int *btns, int *mx, int *my);
void CenterMouse(int width, int height);
void InitRedShifts (void);
void InitWhiteShifts (void);
int GetWhitePaletteShifts(void);
int GetRedPaletteShifts(void);
int GetWhitePaletteSwapMs(void);
void* GetRedPaletteShifted(int which);
void* GetWhitePaletteShifted(int which);
void PaletteFadeOut (int start, int end, int red, int green, int blue, int steps);
void PaletteFadeIn(int start, int end, void *platettePtr, int steps);
void SaveBitmap(char *filename);
int GetMouseButtons(void);
int GetNuberOfJoysticks(void);
void SetVGAMode(unsigned *scrWidth, unsigned *scrHeight, 
                unsigned *scrPitch, unsigned *bufPitch, 
                unsigned *currPitch, unsigned *sclFactor);
void LoadLatchMemory (void);
int SubFizzleFade (void *src, int x1, int y1,
                       unsigned width, unsigned height, 
                       unsigned frames, int abortable,
                       int rndbits_y, int rndmask);
void GetJoystickDelta(int *dx,int *dy);
void GetJoystickFineDelta(int *dx, int *dy);
int GetJoystickButtons(void);
int IsJoystickPresent(void);
void WaitAndProcessEvents(void);
void ProcessEvents(void );
int IsInputGrabbed(void);
void JoystickShutdown(void);
void JoystickStartup(void);
void CheckIsJoystickCorrect(void);

#endif