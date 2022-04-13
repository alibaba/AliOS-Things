/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdarg.h>

#include "amp_system.h"
#include "amp_defines.h"
#include "amp_audio.h"
#include "dev_model_api.h"
#include "be_inl.h"

#include "uvoice_types.h"
#include "uvoice_event.h"
#include "uvoice_player.h"
#include "uvoice_comb.h"


#define MOD_STR        "audioplayer"


typedef struct {
    uvoice_player_t *player;
    int js_cb_ref;
    int list_playing;
    int state;
} audio_player_t;

static audio_player_t *g_audioplayer;

enum {
    AUDIOPLAYER_STAT_STOP = 0,
    AUDIOPLAYER_STAT_PAUSED = 1,
    AUDIOPLAYER_STAT_PLAYING = 2,
    AUDIOPLAYER_STAT_LIST_PLAY_BEGIN = 3,
    AUDIOPLAYER_STAT_LIST_PLAY_END = 4,
    AUDIOPLAYER_STAT_ERROR = 5,
};

static int get_format_from_name(char *name, media_format_t *format)
{
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

static void audioplayer_state_cb(uvoice_event_t *event, void *data)
{
    duk_context *ctx;
    audio_player_t *audioplayer = (audio_player_t *)data;
    player_state_t state;
    int audioplayer_state;
    int ret;

    if (!audioplayer)
        return;

    if (event->type != UVOICE_EV_PLAYER)
        return;

    if (event->code == UVOICE_CODE_PLAYER_STATE) {
        state = event->value;
        switch (state) {
        case PLAYER_STAT_RUNNING:
        case PLAYER_STAT_COMPLETE:
        case PLAYER_STAT_RESUME:
            audioplayer_state = AUDIOPLAYER_STAT_PLAYING;
            break;
        case PLAYER_STAT_PAUSED:
            audioplayer_state = AUDIOPLAYER_STAT_PAUSED;
            break;
        case PLAYER_STAT_STOP:
        case PLAYER_STAT_IDLE:
        case PLAYER_STAT_READY:
            audioplayer_state = AUDIOPLAYER_STAT_STOP;
            break;
        case PLAYER_STAT_ERROR:
            audioplayer_state = AUDIOPLAYER_STAT_ERROR;
            break;
        case PLAYER_STAT_LIST_PLAY_START:
            audioplayer_state = AUDIOPLAYER_STAT_LIST_PLAY_BEGIN;
            audioplayer->list_playing = 1;
            break;
        case PLAYER_STAT_LIST_PLAY_STOP:
            audioplayer_state = AUDIOPLAYER_STAT_LIST_PLAY_END;
            audioplayer->list_playing = 0;
            break;
        default:
            return;
        }
    }

    if (audioplayer->state == audioplayer_state) {
        return;
    }
    audioplayer->state = audioplayer_state;

    ctx = be_get_context();
    be_push_ref(ctx, audioplayer->js_cb_ref);
    duk_push_int(ctx, audioplayer_state);
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    duk_gc(ctx, 0);
}

static duk_ret_t native_audioplayer_play(duk_context *ctx)
{
    audio_player_t *audioplayer = g_audioplayer;
    uvoice_player_t *player;
    char *audiosource, *source = NULL;
    int len;
    int is_http = 0;
    int ret = -1;

    if (!audioplayer)
        goto out;

    player = audioplayer->player;

    if (!duk_is_string(ctx, 0)) {
        amp_error(MOD_STR, "parameter must be (string)");
        goto out;
    }

    audiosource = (char *)duk_get_string(ctx, 0);
    if (!audiosource) {
        amp_error(MOD_STR, "audio source null");
        goto out;
    }

    if (!strncmp(audiosource, "http", strlen("http"))) {
        is_http = 1;
        source = audiosource;
    } else {
        source = amp_malloc(strlen(audiosource) + 4);
        if (!source) {
            amp_error(MOD_STR, "alloc source fail");
            goto out;
        }
        snprintf(source, strlen(audiosource) + 4, "fs:%s", audiosource);
    }

#if 0
    if (player->set_source(source)) {
        amp_error(MOD_STR, "set source fail");
        goto out;
    }

    if (player->start()) {
        amp_error(MOD_STR, "play start fail");
        goto out;
    }
#else
    if (comb_add_http_source(source, 0)) {
        amp_error(MOD_STR, "play fail");
        goto out;
    }
#endif

    ret = 0;

out:
    if (!is_http && source) {
        amp_free(source);
    }
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_stop(duk_context *ctx)
{
    audio_player_t *audioplayer = g_audioplayer;
    uvoice_player_t *player;
    int ret = -1;

    if (!audioplayer)
        goto out;
    player = audioplayer->player;

    if (player->stop()) {
        amp_error(MOD_STR, "play stop fail");
        goto out;
    }

    player->clr_source();

    ret = 0;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_pause(duk_context *ctx)
{
    audio_player_t *audioplayer = g_audioplayer;
    uvoice_player_t *player;
    int ret = -1;

    if (!audioplayer)
        goto out;
    player = audioplayer->player;

    if (player->pause()) {
        amp_error(MOD_STR, "play pause fail");
        goto out;
    }

    ret = 0;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_resume(duk_context *ctx)
{
    audio_player_t *audioplayer = g_audioplayer;
    uvoice_player_t *player;
    int ret = -1;

    if (!audioplayer)
        goto out;
    player = audioplayer->player;

    if (player->resume()) {
        amp_error(MOD_STR, "play resume fail");
        goto out;
    }
    ret = 0;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_seekto(duk_context *ctx)
{
    audio_player_t *audioplayer = g_audioplayer;
    uvoice_player_t *player;
    int ret = -1;
    int seconds;

    if (!audioplayer)
        goto out;
    player = audioplayer->player;

    if (!duk_is_number(ctx, 0)) {
        amp_error(MOD_STR, "parameter must be (number)");
        goto out;
    }

    seconds = duk_get_int(ctx, 0);
    if (seconds < 0) {
        amp_error(MOD_STR, "seconds %d invalid", seconds);
        goto out;
    }

    if (player->seek(seconds)) {
        amp_error(MOD_STR, "seek to %d fail", seconds);
        goto out;
    }
    ret = 0;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_position_get(duk_context *ctx)
{
    audio_player_t *audioplayer = g_audioplayer;
    uvoice_player_t *player;
    int position = 0;
    int ret = -1;

    if (!audioplayer)
        goto out;
    player = audioplayer->player;

    if (player->get_position(&position)) {
        amp_error(MOD_STR, "get position fail");
        goto out;
    }

    ret = position;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_duration_get(duk_context *ctx)
{
    audio_player_t *audioplayer = g_audioplayer;
    uvoice_player_t *player;
    int duration = 0;
    int ret = -1;

    if (!audioplayer)
        goto out;
    player = audioplayer->player;

    if (player->get_duration(&duration)) {
        amp_error(MOD_STR, "get duration fail");
        goto out;
    }

    ret = duration;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_state_get(duk_context *ctx)
{
    audio_player_t *audioplayer = g_audioplayer;
    uvoice_player_t *player;
    player_state_t state = PLAYER_STAT_IDLE;
    int audioplayer_state;
    int ret = -1;

    if (!audioplayer)
        goto out;
    player = audioplayer->player;

    if (player->get_state(&state)) {
        amp_error(MOD_STR, "get player state fail");
        goto out;
    }

    switch (state) {
    case PLAYER_STAT_RUNNING:
    case PLAYER_STAT_COMPLETE:
    case PLAYER_STAT_RESUME:
        audioplayer_state = AUDIOPLAYER_STAT_PLAYING;
        break;
    case PLAYER_STAT_PAUSED:
        audioplayer_state = AUDIOPLAYER_STAT_PAUSED;
        break;
    case PLAYER_STAT_STOP:
    case PLAYER_STAT_IDLE:
    case PLAYER_STAT_READY:
        audioplayer_state = AUDIOPLAYER_STAT_STOP;
        break;
    case PLAYER_STAT_ERROR:
        audioplayer_state = AUDIOPLAYER_STAT_ERROR;
        break;
    case PLAYER_STAT_LIST_PLAY_START:
        audioplayer_state = AUDIOPLAYER_STAT_LIST_PLAY_BEGIN;
        break;
    case PLAYER_STAT_LIST_PLAY_STOP:
        audioplayer_state = AUDIOPLAYER_STAT_LIST_PLAY_END;
        break;
    default:
        audioplayer_state = AUDIOPLAYER_STAT_STOP;
        break;
    }
    ret = audioplayer_state;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_state_on(duk_context *ctx)
{
    audio_player_t *audioplayer = g_audioplayer;
    int ret = -1;

    if (!duk_is_function(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be (function)");
        goto out;
    }

    duk_dup(ctx, 0);
    audioplayer->js_cb_ref = be_ref(ctx);

    if (uvoice_event_register(UVOICE_EV_PLAYER,
        audioplayer_state_cb, audioplayer)) {
        amp_error(MOD_STR, "register event fail");
        goto out;
    }

    ret = 0;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_volume_set(duk_context *ctx)
{
    audio_player_t *audioplayer = g_audioplayer;
    uvoice_player_t *player;
    int ret = -1;
    int volume;

    if (!audioplayer)
        goto out;
    player = audioplayer->player;

    if (!duk_is_number(ctx, 0)) {
        amp_error(MOD_STR, "parameter must be (number)");
        goto out;
    }

    volume = duk_get_int(ctx, 0);
    if (volume < 0) {
        amp_error(MOD_STR, "volume %d invalid", volume);
        goto out;
    }

    if (player->set_volume(volume)) {
        amp_error(MOD_STR, "set volume fail");
        goto out;
    }
    ret = 0;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_volume_get(duk_context *ctx)
{
    audio_player_t *audioplayer = g_audioplayer;
    uvoice_player_t *player;
    int ret = -1;
    int volume = -1;

    if (!audioplayer)
        goto out;
    player = audioplayer->player;

    if (player->get_volume(&volume)) {
        amp_error(MOD_STR, "get volume fail");
        goto out;
    }

    ret = volume;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_play_list(duk_context *ctx)
{
    char *source;
    int arr_idx;
    int arr_cnt;
    int i, ret = -1;

    if (!duk_is_array(ctx, 0)) {
        amp_error(MOD_STR, "parameter must be (array)");
        goto out;
    }

    arr_idx = duk_normalize_index(ctx, 0);
    arr_cnt = duk_get_length(ctx, arr_idx);

    for (i = 0; i < arr_cnt; i++) {
        duk_get_prop_index(ctx, arr_idx, i);
        if (!duk_is_string(ctx, -1)) {
            amp_error(MOD_STR, "array element %d is not string", i);
            duk_pop(ctx);
            goto out;
        }
        source = duk_get_string(ctx, -1);
        comb_add_file_source(source, 0);
        duk_pop(ctx);
    }

    ret = 0;
out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_audioplayer_register(void)
{
    duk_context *ctx = be_get_context();
    audio_player_t *audioplayer = g_audioplayer;

    uvoice_init();
    comb_init();

    if (!audioplayer) {
        audioplayer = HAL_Malloc(sizeof(audio_player_t));
        if (!audioplayer) {
            amp_error(MOD_STR, "alloc audio player fail");
            return;
        }
        audioplayer->player = uvoice_player_create();
        if (!audioplayer->player) {
            amp_error(MOD_STR, "create uvoice player fail");
            HAL_Free(audioplayer);
            return;
        }
        audioplayer->state = -1;
        g_audioplayer = audioplayer;
    }

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_audioplayer_play_list, 1);
    duk_put_prop_string(ctx, -2, "listPlay");

    duk_push_c_function(ctx, native_audioplayer_play, 1);
    duk_put_prop_string(ctx, -2, "play");

    duk_push_c_function(ctx, native_audioplayer_stop, 0);
    duk_put_prop_string(ctx, -2, "stop");

    duk_push_c_function(ctx, native_audioplayer_pause, 0);
    duk_put_prop_string(ctx, -2, "pause");

    duk_push_c_function(ctx, native_audioplayer_resume, 0);
    duk_put_prop_string(ctx, -2, "resume");

    duk_push_c_function(ctx, native_audioplayer_seekto, 1);
    duk_put_prop_string(ctx, -2, "seekto");

    duk_push_c_function(ctx, native_audioplayer_position_get, 0);
    duk_put_prop_string(ctx, -2, "getPosition");

    duk_push_c_function(ctx, native_audioplayer_duration_get, 0);
    duk_put_prop_string(ctx, -2, "getDuration");

    duk_push_c_function(ctx, native_audioplayer_state_get, 0);
    duk_put_prop_string(ctx, -2, "getState");

    duk_push_c_function(ctx, native_audioplayer_state_on, 1);
    duk_put_prop_string(ctx, -2, "onState");

    duk_push_c_function(ctx, native_audioplayer_volume_set, 1);
    duk_put_prop_string(ctx, -2, "setVolume");

    duk_push_c_function(ctx, native_audioplayer_volume_get, 0);
    duk_put_prop_string(ctx, -2, "getVolume");

    duk_put_prop_string(ctx, -2, "audioplayer");
}

