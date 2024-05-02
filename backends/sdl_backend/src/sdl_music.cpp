//#include "wl_def.h"
#ifdef _WIN32
#include "SDL_mixer.h"
#elif __linux__
#include <SDL/SDL_mixer.h>
#else
#include <SDL/SDL_mixer.h>
#endif

//#include "id_sd.h"

static int soundsAmount;

typedef struct
{
    char RIFF[4];
    longword filelenminus8;
    char WAVE[4];
    char fmt_[4];
    longword formatlen;
    word val0x0001;
    word channels;
    longword samplerate;
    longword bytespersec;
    word bytespersample;
    word bitspersample;
} headchunk;

typedef struct
{
    char chunkid[4];
    longword chunklength;
} wavechunk;

static Mix_Chunk **SoundChunks;
static byte      **SoundBuffers;
globalsoundpos channelSoundPos[MIX_CHANNELS];

void SetAmountOfSounds(int amount) {
    soundsAmount = amount;
    SoundChunks = (Mix_Chunk **)malloc(sizeof(Mix_Chunk *) * amount);
    SoundBuffers = (byte **)malloc(sizeof(byte *) * amount);
}

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

static signed short GetSample(float csample, byte *samples, int size)
{
    float s0=0, s1=0, s2=0;
    int cursample = (int) csample;
    float sf = csample - (float) cursample;

    if(cursample-1 >= 0) s0 = (float) (samples[cursample-1] - 128);
    s1 = (float) (samples[cursample] - 128);
    if(cursample+1 < size) s2 = (float) (samples[cursample+1] - 128);

    float val = s0*sf*(sf-1)/2 - s1*(sf*sf-1) + s2*(sf+1)*sf/2;
    int32_t intval = (int32_t) (val * 256);
    if(intval < -32768) intval = -32768;
    else if(intval > 32767) intval = 32767;
    return (signed short) intval;
}

void SDL_Mus_Mix_Load8bit7042(int which, unsigned char *origsamples, int size, int frequency) {
    if(origsamples + size >= PM_GetEnd())
        Quit("SD_PrepareSound(%i): Sound reaches out of page file!\n", which);

    int destsamples = (int) ((float) size * (float) param_samplerate
        / (float) frequency);

    byte *wavebuffer = (byte *) malloc(sizeof(headchunk) + sizeof(wavechunk)
        + destsamples * 2);     // dest are 16-bit samples
    if(wavebuffer == NULL)
        Quit("Unable to allocate wave buffer for sound %i!\n", which);

    headchunk head = {{'R','I','F','F'}, 0, {'W','A','V','E'},
        {'f','m','t',' '}, 0x10, 0x0001, 1, (unsigned int)param_samplerate, (unsigned int)param_samplerate*2, 2, 16};
    wavechunk dhead = {{'d', 'a', 't', 'a'}, (unsigned int)destsamples*2};
    head.filelenminus8 = sizeof(head) + destsamples*2;  // (sizeof(dhead)-8 = 0)
    memcpy(wavebuffer, &head, sizeof(head));
    memcpy(wavebuffer+sizeof(head), &dhead, sizeof(dhead));

    // alignment is correct, as wavebuffer comes from malloc
    // and sizeof(headchunk) % 4 == 0 and sizeof(wavechunk) % 4 == 0
    signed short *newsamples = (signed short *)(void *) (wavebuffer + sizeof(headchunk)
        + sizeof(wavechunk));
    float cursample = 0.F;
    float samplestep = (float) frequency / (float) param_samplerate;
    for(int i=0; i<destsamples; i++, cursample+=samplestep)
    {
        newsamples[i] = GetSample((float)size * (float)i / (float)destsamples,
            origsamples, size);
    }
    SoundBuffers[which] = wavebuffer;
    SoundChunks[which] = Mix_LoadWAV_RW(SDL_RWFromMem(wavebuffer, sizeof(headchunk) + sizeof(wavechunk) + destsamples * 2), 1);
}

static int SDL_Mus_Mix_OpenAudio(int frequency, unsigned short format, int channels, int chunksize){
    return Mix_OpenAudio(frequency, format, channels, chunksize);
}

void SDL_Mus_Mix_HookMusic(void *mf, void *arg){
    void (*mix_func) (void *udata, unsigned char *stream, int len) = (void (*) (void *udata, unsigned char *stream, int len))mf;
    Mix_HookMusic(mix_func, arg);
}

void SDL_Mus_Mix_ChannelFinished(void (*channel_finished)(int channel)) {
    Mix_ChannelFinished(channel_finished);
}

void SDL_Mus_Mix_FreeAllChunks(void) {
    int i;
    for (i = 0; i < soundsAmount; ++i) {
        if(SoundChunks[i]) {
            Mix_FreeChunk(SoundChunks[i]);
        }
    }

    for(int i = 0; i < soundsAmount; i++)
    {
        if(SoundBuffers[i]) free(SoundBuffers[i]);
    }

    free(SoundChunks);
    free(SoundBuffers);
}

int SDL_Mus_Startup(int frequency, int chunksize) {
    if(SDL_Mus_Mix_OpenAudio(frequency, AUDIO_S16, 2, chunksize))
    {
        printf("Unable to open audio: %s\n", Mix_GetError());
        return 0;
    }

    Mix_ReserveChannels(2);  // reserve player and boss weapon channels
    Mix_GroupChannels(2, MIX_CHANNELS-1, 1); // group remaining channels

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
        printf("Unable to play sound: %s\n", Mix_GetError());
        return 0;
    }
    return channel;
}
