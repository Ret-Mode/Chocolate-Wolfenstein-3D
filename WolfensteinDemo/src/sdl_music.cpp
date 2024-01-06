#include "wl_def.h"
#ifdef _WIN32
#include "SDL_mixer.h"
#elif __linux__
#include <SDL/SDL_mixer.h>
#else
#include <SDL/SDL_mixer.h>
#endif

#include "id_sd.h"

static Mix_Chunk *SoundChunks[ STARTMUSIC - STARTDIGISOUNDS];
globalsoundpos channelSoundPos[MIX_CHANNELS];

int SDL_Mus_GetChannelNumber(void) {
    return MIX_CHANNELS;
}

int SDL_Mus_Mix_HaltChannel(int channel) {
    return Mix_HaltChannel(channel);
}

int SDL_Mus_Mix_GroupAvailable(int tag) {
    return Mix_GroupAvailable(tag);
}

int SDL_Mus_Mix_GroupOldest(int tag){
    return Mix_GroupOldest(tag);
}

int SDL_Mus_Mix_SetPanning(int channel, unsigned char left, unsigned char right) {
    return Mix_SetPanning(channel, left, right);
}

void SDL_Mus_Mix_LoadWAV_RW(int which, void *mem, int size, int freeSrc) {
    SoundChunks[which] = Mix_LoadWAV_RW(SDL_RWFromMem(mem, size), freeSrc);
}

int SDL_Mus_Mix_OpenAudio(int frequency, unsigned short format, int channels, int chunksize){
    return Mix_OpenAudio(frequency, format, channels, chunksize);
}

char * SDL_Mus_Mix_GetError(void) {
    return Mix_GetError();
}

int SDL_Mus_Mix_ReserveChannels(int num) {
    return Mix_ReserveChannels(num);
}

int SDL_Mus_Mix_GroupChannels(int from, int to, int tag) {
    return Mix_GroupChannels(from, to, tag);
}

void SDL_Mus_Mix_HookMusic(void (*mix_func) (void *udata, unsigned char *stream, int len), void *arg){
    Mix_HookMusic(mix_func, arg);
}

void SDL_Mus_Mix_ChannelFinished(void (*channel_finished)(int channel)) {
    Mix_ChannelFinished(channel_finished);
}

void SDL_Mus_Mix_FreeAllChunks(void) {
    int i;
    for (i = 0; i < STARTMUSIC - STARTDIGISOUNDS; ++i) {
        if(SoundChunks[i]) {
            Mix_FreeChunk(SoundChunks[i]);
        }
    }
}

int SDL_Mus_Startup(int frequency, int chunksize) {
    if(SDL_Mus_Mix_OpenAudio(frequency, AUDIO_S16, 2, chunksize))
    {
        printf("Unable to open audio: %s\n", SDL_Mus_Mix_GetError());
        return 0;
    }

    SDL_Mus_Mix_ReserveChannels(2);  // reserve player and boss weapon channels
    SDL_Mus_Mix_GroupChannels(2, MIX_CHANNELS-1, 1); // group remaining channels

    return !0;
}

int SDL_Mus_PlayChunk(int channel, int which) {
    Mix_Chunk *sample = SoundChunks[which];
    if(sample == NULL)
    {
        printf("SoundChunks[%i] is NULL!\n", which);
        return 0;
    }

    if(Mix_PlayChannel(channel, sample, 0) == -1)
    {
        printf("Unable to play sound: %s\n", SDL_Mus_Mix_GetError());
        return 0;
    }
    return channel;
}