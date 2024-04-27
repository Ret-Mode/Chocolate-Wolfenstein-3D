#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "sdl_music.h"

static ma_device_config config;
static ma_device device;

static void (*mix_func) (void *udata, unsigned char *stream, int len);
static void (*channel_finished)(int channel);

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    mix_func(NULL, (unsigned char *)pOutput, frameCount*4);
}

int SDL_Mus_Startup(int frequency, int chunksize) {
    config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = ma_format_s16;   // Set to ma_format_unknown to use the device's native format.
    config.playback.channels = 2;               // Set to 0 to use the device's native channel count.
    config.sampleRate        = 44100;           // Set to 0 to use the device's native sample rate.
    config.dataCallback      = data_callback;   // This function will be called when miniaudio needs more data.
    config.pUserData         = NULL;            // Can be accessed from the device object (device.pUserData).

    
    if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
        return 0;  // Failed to initialize the device.
    }

    //ma_device_uninit(&device);
    return !0;
}

void SDL_Mus_Mix_HookMusic(void *mf, void *arg){
    mix_func = (void (*) (void *udata, unsigned char *stream, int len))mf;
    ma_device_start(&device);
}

void SDL_Mus_Mix_ChannelFinished(void (*cf)(int channel)) {
    channel_finished = cf;
}