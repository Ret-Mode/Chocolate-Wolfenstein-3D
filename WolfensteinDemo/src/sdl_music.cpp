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

void *SDL_Mus_Mix_LoadWAV_RW(void *mem, int size, int freeSrc) {
    return Mix_LoadWAV_RW(SDL_RWFromMem(mem, size), freeSrc);
}

Mix_PlayChannel
Mix_OpenAudio
Mix_GetError
Mix_ReserveChannels
Mix_GroupChannels
Mix_HookMusic
Mix_ChannelFinished
Mix_FreeChunk