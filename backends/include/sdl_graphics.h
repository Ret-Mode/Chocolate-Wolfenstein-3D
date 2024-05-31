#ifndef __SDL_GRAPHICS__
#define __SDL_GRAPHICS__

void DelayWolfTicks(int ticks);
void DelayMilliseconds(int milliseconds);
void DelayVBL(int param);
unsigned int GetMilliseconds(void);
unsigned int GetWolfTicks(void);
void ClearCurrentSurface(unsigned int color);
void BlitPictureToScreen(unsigned char *pic);
void *GetGamePal(void);
void CenterWindow(void);
void SetWholePalette(void *palette, int forceupdate);
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
void SetVGAMode(unsigned *scrWidth, unsigned *scrHeight, 
                unsigned *bufPitch, 
                unsigned *currPitch, unsigned *sclFactor);
void LoadLatchMemory (void);
int SubFizzleFade (int x1, int y1,
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