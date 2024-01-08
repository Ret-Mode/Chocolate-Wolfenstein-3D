#ifndef __SDL_GRAPHICS__
#define __SDL_GRAPHICS__



void *GetLatchPic(int which);
int GetLatchPicWidth(int which);
int GetLatchPicHeight(int which);
void SetLatchPic(int which, void *data);

#endif