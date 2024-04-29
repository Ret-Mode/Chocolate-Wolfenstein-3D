#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "sdl_music.h"

static ma_device device;
static ma_engine engine;

static void (*mix_func) (void *udata, unsigned char *stream, int len);
static void (*channel_finished)(int channel);

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




void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    //mix_func(NULL, (unsigned char *)pOutput, frameCount*4);
    ma_uint64 readed;
    ma_engine_read_pcm_frames(&engine, pOutput, frameCount, &readed);

}



static musicFile music;
static ma_sound musicObject;


int SDL_Mus_Startup(int frequency, int chunksize) {
    ma_result result;

    ma_device_config config;
    config = ma_device_config_init(ma_device_type_playback);
    //config.playback.format   = ma_format_f32;   // Set to ma_format_unknown to use the device's native format.
    config.playback.channels = 2;               // Set to 0 to use the device's native channel count.
    // config.sampleRate        = 44100;           // Set to 0 to use the device's native sample rate.
    config.dataCallback      = data_callback;   // This function will be called when miniaudio needs more data.
    config.pUserData         = NULL;            // Can be accessed from the device object (device.pUserData).

    if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
        return 0;  // Failed to initialize the device.
    }

    ma_engine_config e_config = ma_engine_config_init();
    e_config.pDevice = &device;
    e_config.noAutoStart = MA_TRUE;

    result = ma_engine_init(&e_config, &engine);
    if (result != MA_SUCCESS) {
        return 0;  // Failed to initialize the engine.
    }

    result = ma_engine_start(&engine);
    if (result != MA_SUCCESS) {
        return 0;  // Failed to initialize the engine.
    }

    //ma_device_uninit(&device);
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
    result = ma_sound_start(&musicObject);
    if (result != MA_SUCCESS) {
        return;  // Failed to initialize the engine.
    }

}

void SDL_Mus_Mix_ChannelFinished(void (*cf)(int channel)) {
    channel_finished = cf;
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
}