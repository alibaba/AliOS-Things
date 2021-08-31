/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ulog/ulog.h>
#include "uvoice_init.h"
#include "uvoice_types.h"
#include "uvoice_event.h"
#include "uvoice_player.h"
#include "uvoice_recorder.h"
#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "uvoice_os.h"
#include "player.h"

#define TAG "player"
static aos_task_t play_task;
static uvoice_player_t *uvocplayer;
static char text_source[256];
static player_cb_t play_done_cb = NULL;
static player_mp3_e g_file = PLAYER_MP3_MAX; 

extern int audio_install_codec_driver();

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

    if (uvocplayer->start()) {
        LOGE(TAG, "start failed !\n");
        uvocplayer->clr_source();
    }

    // uvocplayer->wait_complete();

    if (play_done_cb)
        play_done_cb(g_file);

    return NULL;
}


int32_t player_play(player_mp3_e file)
{
    int32_t random;

    random = rand() % 240;
    LOG("random: %d\n", random);
    memset(text_source, 0, sizeof(text_source));
    if (file == PLAYER_MP3_WELCOME) {
        strcpy(text_source, "fs:/data/mp3/welcome.mp3");
    } else if (file == PLAYER_MP3_WAKEUP) {
        // if (random < 100) {
            // strcpy(text_source, "fs:/data/mp3/haas_intro.mp3");
        // } else
        if (random > 0 && random < 150) {
            strcpy(text_source, "fs:/data/mp3/zhurenyoushenmkeyibangnin.mp3");
        } else if (random > 150 && random < 200) {
            strcpy(text_source, "fs:/data/mp3/zhurenwozai.mp3");
        } else {
            strcpy(text_source, "fs:/data/mp3/eiwozai.mp3");
        }
        g_file = PLAYER_MP3_WAKEUP;
    } else if (file == PLAYER_MP3_LIGHT_ON) {
        strcpy(text_source, "fs:/data/mp3/haodeyiweinindakai.mp3");
        g_file = PLAYER_MP3_LIGHT_ON;
    } else if (file == PLAYER_MP3_LIGHT_OFF) {
        strcpy(text_source, "fs:/data/mp3/haodeyiweininguanbi.mp3");
        g_file = PLAYER_MP3_LIGHT_OFF;
    }

#if 1
    /*set player enter idle state after a long time*/
    uvocplayer->set_standby(2147483646);

    play_music(NULL);
    uvocplayer->wait_complete();
    aos_msleep(500);
#else
    aos_task_new_ext(&play_task,
        "play music task", play_music,
        NULL, 8192, 0);
#endif
    return 0;
}

int32_t player_stop(void)
{
    player_state_t state;

    /*stop and clear current playing*/
    uvocplayer->set_fade(40, 40);
    uvocplayer->stop_async();
    uvocplayer->clr_source();

}


int32_t player_wait_complete(void)
{
    int32_t ret;

    ret = uvocplayer->wait_complete();
    if (ret < 0)
        aos_msleep(1000);
    aos_msleep(500);
}

int32_t player_init(player_cb_t cb)
{
    int32_t ret;

    /*Init uvoice to play mp3*/
    ret = uvoice_init();
    if (ret < 0) {
        LOGE(TAG, "uvoice_init failed !\n");
        return -1;
    }

    /*create uvoice player*/
    uvocplayer = uvoice_player_create();
    if (!uvocplayer) {
        LOGE(TAG, "create media player failed !\n");
        return -1;
    }
    /*set eq*/
    uvocplayer->eq_enable(0);

    /*set play volume*/
    uvocplayer->set_volume(10);

    play_done_cb = cb;
    return 0;
}
