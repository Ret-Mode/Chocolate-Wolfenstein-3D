#ifndef __SDL_MUSIC__
#define __SDL_MUSIC__

int SDL_Mus_GetChannelNumber(void);

int SDL_Mus_Mix_HaltChannel(int channel);

int SDL_Mus_Mix_GroupAvailable(int tag);

int SDL_Mus_Mix_GroupOldest(int tag);

int SDL_Mus_Mix_SetPanning(int channel, unsigned char left, unsigned char right);

void SDL_Mus_Mix_LoadWAV_RW(int which, void *mem, int size, int freeSrc);

int SDL_Mus_Mix_OpenAudio(int frequency, unsigned short format, int channels, int chunksize);

char SDL_Mus_Mix_GetError(void);

int SDL_Mus_Mix_ReserveChannels(int num);

int SDL_Mus_Mix_GroupChannels(int from, int to, int tag);

void SDL_Mus_Mix_HookMusic(void *mf, void *arg);

void SDL_Mus_Mix_ChannelFinished(void (*channel_finished)(int channel));

void SDL_Mus_Mix_FreeAllChunks(void);

int SDL_Mus_Startup(int frequency, int chunksize);

int SDL_Mus_PlayChunk(int channel, int which);

#endif // __SDL_MUSIC__