#ifndef __SDL_MUSIC__
#define __SDL_MUSIC__

int SDL_Mus_GetChannelNumber(void);
int SDL_Mus_Mix_HaltChannel(int channel);
int SDL_Mus_Mix_GroupAvailable(int tag);
int SDL_Mus_Mix_GroupOldest(int tag);
int SDL_Mus_Mix_SetPanning(int channel, unsigned char left, unsigned char right);
void SDL_Mus_Mix_Load8bit7042(int which, unsigned char *origsamples, int size, int frequency);
void SDL_Mus_Mix_HookMusic(void *mf, void *arg);
void SDL_Mus_Mix_ChannelFinished(void (*channel_finished)(int channel));
void SDL_Mus_Mix_FreeAllChunks(void);
int SDL_Mus_Startup(int frequency, int chunksize);
int SDL_Mus_PlayChunk(int channel, int which);
void SetAmountOfSounds(int amount);

typedef struct
{
    int valid;
    int globalsoundx, globalsoundy;
} globalsoundpos;

extern globalsoundpos channelSoundPos[];


#endif // __SDL_MUSIC__