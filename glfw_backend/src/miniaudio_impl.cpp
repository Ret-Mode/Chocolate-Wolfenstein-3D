#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "sdl_music.h"
#include "wl_def.h"

static ma_device device;
static ma_engine engine;

static void (*mix_func) (void *udata, unsigned char *stream, int len);
static void (*channel_finished)(int channel);


struct SoundBuffer_t {
    ma_uint64 size;
    unsigned char *data;
};

static struct SoundBuffer_t SoundBuffer[STARTMUSIC - STARTDIGISOUNDS];



ma_result musicFileVtable_on_read(ma_data_source* pDataSource, void* pFramesOut, ma_uint64 frameCount, ma_uint64* pFramesRead) {
    mix_func(NULL, (unsigned char *)pFramesOut, (int)frameCount*4);
    *pFramesRead = frameCount;
    return MA_SUCCESS;
}

ma_result musicFileVtable_on_seek(ma_data_source* pDataSource, ma_uint64 frameIndex) {
    return MA_NOT_IMPLEMENTED;
}

ma_result musicFileVtable_on_get_data_format(ma_data_source* pDataSource, ma_format* pFormat, ma_uint32* pChannels, ma_uint32* pSampleRate, ma_channel* pChannelMap, size_t channelMapCap) {
    *pFormat = ma_format_s16;
    *pChannels = 2;
    *pSampleRate = 44100;
    //pChannelMap[0] = MA_CHANNEL_LEFT;
    //pChannelMap[0] = MA_CHANNEL_RIGHT;
    return MA_SUCCESS;
}

ma_result musicFileVtable_on_get_cursor(ma_data_source* pDataSource, ma_uint64* pCursor) {
    *pCursor = 0;
    return MA_NOT_IMPLEMENTED;
}

ma_result musicFileVtable_on_get_length(ma_data_source* pDataSource, ma_uint64* pLength) {
    *pLength = 0;
    return MA_NOT_IMPLEMENTED; 
}

ma_result musicFileVtable_on_set_looping(ma_data_source* pDataSource, ma_bool32 isLooping) {
    return MA_SUCCESS;
}

static ma_data_source_vtable musicFileVtable =
{
    musicFileVtable_on_read,
    musicFileVtable_on_seek,  /* No-op for noise. */
    musicFileVtable_on_get_data_format,
    musicFileVtable_on_get_cursor,   /* onGetCursor. No notion of a cursor for noise. */
    musicFileVtable_on_get_length,   /* onGetLength. No notion of a length for noise. */
    NULL,   /* onSetLooping */
    0
};

struct musicFile
{
    ma_data_source_base base;
    
};

ma_result musicFile_init(musicFile* pMyDataSource)
{
    ma_result result;
    ma_data_source_config baseConfig;

    baseConfig = ma_data_source_config_init();
    baseConfig.vtable = &musicFileVtable;

    result = ma_data_source_init(&baseConfig, &pMyDataSource->base);
    if (result != MA_SUCCESS) {
        return result;
    }

    return MA_SUCCESS;
}

void musicFile_uninit(musicFile* pMyDataSource)
{
    ma_data_source_uninit(&pMyDataSource->base);
}






struct soundFile
{
    ma_data_source_base base;
    ma_uint64 position;
    ma_uint64 size;
    unsigned char *data;
    
};

ma_result soundFileVtable_on_read(ma_data_source* pDataSource, void* pFramesOut, ma_uint64 frameCount, ma_uint64* pFramesRead) {
    soundFile *f = (soundFile *)pDataSource;
    if (f->data == NULL) 
    {
        *pFramesRead = 0;
        //memset(pFramesOut, 0x80, frameCount);
        return MA_SUCCESS;
    }
    ma_uint64 toRead = f->size - f->position;
    if (toRead > frameCount) {
        toRead = frameCount;
        memcpy(pFramesOut, f->data + f->position, toRead);
        *pFramesRead = toRead;
        f->position += toRead;
        //return MA_AT_END;
        return MA_SUCCESS;
    }
    memcpy(pFramesOut, f->data + f->position, toRead);
    f->position += toRead;
    *pFramesRead = toRead;
    return MA_SUCCESS;
}

ma_result soundFileVtable_on_seek(ma_data_source* pDataSource, ma_uint64 frameIndex) {
    soundFile *f = (soundFile *)pDataSource;
    if (f->size > frameIndex) {
        f->position = frameIndex;
    } else {
        f->position = f->size;
    }
    return MA_SUCCESS;
}

ma_result soundFileVtable_on_get_data_format(ma_data_source* pDataSource, ma_format* pFormat, ma_uint32* pChannels, ma_uint32* pSampleRate, ma_channel* pChannelMap, size_t channelMapCap) {
    *pFormat = ma_format_u8;
    *pChannels = 1;
    *pSampleRate = 7042;
    return MA_SUCCESS;
}

ma_result soundFileVtable_on_get_cursor(ma_data_source* pDataSource, ma_uint64* pCursor) {
    *pCursor = ((soundFile *)pDataSource)->position;
    return MA_SUCCESS;
}

ma_result soundFileVtable_on_get_length(ma_data_source* pDataSource, ma_uint64* pLength) {
    *pLength = ((soundFile *)pDataSource)->size;
    return MA_SUCCESS; 
}

static ma_data_source_vtable soundFileVtable =
{
    soundFileVtable_on_read,
    soundFileVtable_on_seek,  /* No-op for noise. */
    soundFileVtable_on_get_data_format,
    soundFileVtable_on_get_cursor,   /* onGetCursor. No notion of a cursor for noise. */
    soundFileVtable_on_get_length,   /* onGetLength. No notion of a length for noise. */
    NULL,   /* onSetLooping */
    0
};


void soundFile_end_callback(void* pUserData, ma_sound* pSound)
{
    printf("Sound ended");
}


ma_result soundFile_init(soundFile* pMyDataSource)
{
    ma_result result;
    ma_data_source_config baseConfig;

    baseConfig = ma_data_source_config_init();
    baseConfig.vtable = &soundFileVtable;

    result = ma_data_source_init(&baseConfig, &pMyDataSource->base);
    if (result != MA_SUCCESS) {
        return result;
    }
    pMyDataSource->position = 0;
    pMyDataSource->size = 0;
    pMyDataSource->data = NULL;
    return MA_SUCCESS;
}

void soundFile_uninit(soundFile* pMyDataSource)
{
    ma_data_source_uninit(&pMyDataSource->base);
}




static musicFile music;
static ma_sound musicObject;


static soundFile soundFiles[10];
static ma_sound sounds[10];

struct soundFx_t {
    struct soundFx_t *next;
    soundFile sf;
    ma_sound snd;
};

static struct soundFXs {
    struct soundFx_t *playing;
    struct soundFx_t *stopped;
    struct soundFx_t arr[10];
} sndFx;


static void initSndFx(ma_engine *engine) {
    ma_result result;
    sndFx.playing = NULL;
    sndFx.stopped = sndFx.arr;
    struct soundFx_t *arrVal = sndFx.arr + sizeof(sndFx.arr)/sizeof(*sndFx.arr) - 1;
    arrVal->next = NULL;
    while (arrVal != sndFx.arr) {
        arrVal -= 1;
        arrVal->next = arrVal + 1;

        result = soundFile_init(&arrVal->sf);
        if (result != MA_SUCCESS) {
           return;  // Failed to initialize the engine.
        }

        result = ma_sound_init_from_data_source(engine, &arrVal->sf, 0, NULL, &arrVal->snd);
        if (result != MA_SUCCESS) {
            return;  // Failed to initialize the engine.
        }
        arrVal->snd.endCallback = soundFile_end_callback;
    }
}



int SDL_Mus_Startup(int frequency, int chunksize) {
    ma_result result;

    ma_engine_config e_config = ma_engine_config_init();

    e_config.noAutoStart = MA_TRUE;

    result = ma_engine_init(&e_config, &engine);
    if (result != MA_SUCCESS) {
        return 0;  // Failed to initialize the engine.
    }

    result = ma_engine_start(&engine);
    if (result != MA_SUCCESS) {
        return 0;  // Failed to initialize the engine.
    }

    return !0;
}

void SDL_Mus_Mix_HookMusic(void *mf, void *arg){
    mix_func = (void (*) (void *udata, unsigned char *stream, int len))mf;
    //ma_device_start(&device);
    ma_result result = musicFile_init(&music);
    if (result != MA_SUCCESS) {
        return;  // Failed to initialize the engine.
    }

    result = ma_sound_init_from_data_source(&engine, &music, MA_SOUND_FLAG_STREAM, NULL, &musicObject);
    if (result != MA_SUCCESS) {
        return;  // Failed to initialize the engine.
    }
    result = ma_sound_set_end_callback(&musicObject, soundFile_end_callback, NULL);
    if (result != MA_SUCCESS) {
        return;  // Failed to initialize the engine.
    }
    result = ma_sound_start(&musicObject);
    if (result != MA_SUCCESS) {
        return;  // Failed to initialize the engine.
    }

    initSndFx(&engine);

}

int SDL_Mus_Mix_HaltChannel(int channel) {
    ma_result result;

    while (sndFx.playing) {
        struct soundFx_t *arrVal = sndFx.playing; 
        result = ma_sound_stop(&arrVal->snd);
        sndFx.playing = arrVal->next;
        arrVal->next = sndFx.stopped;
        sndFx.stopped = arrVal;
    }

    return 0;
}

void SDL_Mus_Mix_ChannelFinished(void (*cf)(int channel)) {
    
}

void SDL_Mus_Mix_FreeAllChunks(void) {
    ma_result result;
    result = ma_sound_stop(&musicObject);
    if (result != MA_SUCCESS) {
        return;  // Failed to initialize the engine.
    }
    ma_sound_uninit(&musicObject);
    result = ma_engine_stop(&engine);
    if (result != MA_SUCCESS) {
        return;  // Failed to initialize the engine.
    }
    ma_engine_uninit(&engine);

    for (int i = 0; i < STARTMUSIC - STARTDIGISOUNDS; ++i) {
        free(SoundBuffer[i].data);
        SoundBuffer[i].data = NULL;
        SoundBuffer[i].size = 0;
    }
}

void SDL_Mus_Mix_Load8bit7042(int which, unsigned char *origsamples, int size, int frequency)
{
    assert (which < STARTMUSIC - STARTDIGISOUNDS);
    SoundBuffer[which].size = size;
    SoundBuffer[which].data = (unsigned char *)malloc(size);
    memcpy(SoundBuffer[which].data, origsamples, size);
}

int SDL_Mus_PlayChunk(int channel, int which) {
    soundFx_t *empty = sndFx.stopped;
    soundFx_t *playing = sndFx.playing;
    if (empty != NULL) {
        sndFx.stopped = empty->next;
        empty->next = NULL;
    } else {
        assert (playing != NULL);
        empty = playing;
        sndFx.playing = empty->next;
        empty->next = NULL;
        ma_sound_stop(&empty->snd);
    }

    empty->sf.data = SoundBuffer[which].data;
    empty->sf.position = 0;
    empty->sf.size = SoundBuffer[which].size;
    
    if (sndFx.playing == NULL) {
        sndFx.playing = empty;
    } else {
        playing = sndFx.playing;
        while (playing->next != NULL) {
            playing = playing->next;
        }
        playing->next = empty;
    }
    ma_sound_start(&empty->snd);
    return 0;
}