#include "kws.h"
#include "ulog/ulog.h"
#include "aiagent_service.h"
#include "aiagent_common.h"
#if (BOARD_HAAS100 == 1)
#include "led.h"
#endif
#include "uvoice_init.h"
#include "uvoice_types.h"
#include "uvoice_event.h"
#include "uvoice_player.h"
#include "uvoice_recorder.h"

#include "uvoice_os.h"

#define TAG "kws"

static os_task_t play_task;
static uvoice_player_t *uvocplayer;
static char text_source[256];
static int source_sample_rate;
static int source_channels;

static int get_format_from_name(char *name, media_format_t *format)
{
    if (!name || !format) {
        LOGE(TAG, "arg null !\n");
        return -1;
    }

    if (strstr(name, ".mp3") || strstr(name, ".MP3"))
        *format = MEDIA_FMT_MP3;
    else if (strstr(name, ".wav") || strstr(name, ".WAV"))
        *format = MEDIA_FMT_WAV;
    else if (strstr(name, ".aac") || strstr(name, ".AAC"))
        *format = MEDIA_FMT_AAC;
    else if (strstr(name, ".m4a") || strstr(name, ".M4A"))
        *format = MEDIA_FMT_M4A;
    else if (strstr(name, ".pcm") || strstr(name, ".PCM"))
        *format = MEDIA_FMT_PCM;
    else if (strstr(name, ".spx") || strstr(name, ".SPX"))
        *format = MEDIA_FMT_SPX;
    else if (strstr(name, ".ogg") || strstr(name, ".OGG"))
        *format = MEDIA_FMT_OGG;
    else if (strstr(name, ".amrwb") || strstr(name, ".AMRWB"))
        *format = MEDIA_FMT_AMRWB;
    else if (strstr(name, ".amr") || strstr(name, ".AMR"))
        *format = MEDIA_FMT_AMR;
    else if (strstr(name, ".opus") || strstr(name, ".OPUS"))
        *format = MEDIA_FMT_OPS;
    else if (strstr(name, ".flac") || strstr(name, ".FLAC"))
        *format = MEDIA_FMT_FLAC;

    return 0;
}

static void *play_music(void *arg)
{
    media_format_t format = MEDIA_FMT_UNKNOWN;
    get_format_from_name(text_source, &format);

    if (uvocplayer->set_source(text_source)) {
        LOGE(TAG, "set source failed !\n");
        return NULL;
    }

    if (format == MEDIA_FMT_OPS || format == MEDIA_FMT_SPX)
        uvocplayer->set_pcminfo(source_sample_rate, source_channels, 16, 1280);

    if (uvocplayer->start()) {
        LOGE(TAG, "start failed !\n");
        uvocplayer->clr_source();
    }

    return NULL;
}

static int32_t play_local_mp3(void)
{
    int32_t random;

    random = rand() % 240;
    LOG("random: %d\n", random);
    memset(text_source, 0, sizeof(text_source));
    if (random < 100) {
        strcpy(text_source, "fs:/data/mp3/haas_intro.mp3");
    } else if (random > 100 && random < 150) {
        strcpy(text_source, "fs:/data/mp3/haasxiaozhushou.mp3");
    } else if (random > 150 && random < 200) {
        strcpy(text_source, "fs:/data/mp3/zhurenwozai.mp3");
    } else {
        strcpy(text_source, "fs:/data/mp3/eiwozai.mp3");
    }
    os_task_create(&play_task,
        "play music task", play_music,
        NULL, 8192, 0);
    return 0;
}

static int32_t kws_callback(ai_result_t *result)
{
    int32_t kws_ret = (int32_t)*result;
    player_state_t player_state = -1;

    if (kws_ret) {
        led_switch(1, LED_ON);
        aos_msleep(200);
        led_switch(2, LED_ON);
        aos_msleep(200);
        led_switch(3, LED_ON);
        aos_msleep(200);
        led_switch(4, LED_ON);
        aos_msleep(200);
        led_switch(5, LED_ON);
        aos_msleep(200);
        led_switch(6, LED_ON);

        /*play local tts*/
        play_local_mp3();

        uvocplayer->wait_complete();

        led_switch(1, LED_OFF);
        led_switch(2, LED_OFF);
        led_switch(3, LED_OFF);
        led_switch(4, LED_OFF);
        led_switch(5, LED_OFF);
        led_switch(6, LED_OFF);
    }

    return 0;
}

int32_t kws_init(void)
{
    int32_t ret;
    ai_config_t config;

    ret = aiagent_service_init("kws", AI_MODEL_KWS);
    if (ret < 0) {
        LOGE(TAG, "aiagent_service_init failed");
        return -1;
    }

    config.channel = 2;
    aiagent_service_config(&config);

    /*Init sound driver*/
    audio_install_codec_driver();

    /*Init uvoice to play mp3*/
    uvoice_init();

    /*create uvoice player*/
    uvocplayer = uvoice_player_create();
    if (!uvocplayer) {
        LOGE(TAG, "create media player failed !\n");
        return;
    }
    uvocplayer->eq_enable(0);

    /*set play volume*/
    uvocplayer->set_volume(10);

    return aiagent_service_model_infer(NULL, NULL, kws_callback);
}

int32_t kws_uninit(void)
{
    aiagent_service_uninit();
    return 0;
}
