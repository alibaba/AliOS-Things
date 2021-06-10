/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "aos_fs.h"
#include "board_mgr.h"
#include "be_inl.h"

#include "uvoice_types.h"
#include "uvoice_event.h"
#include "uvoice_player.h"
#include "uvoice_comb.h"
#include "aos_pcm.h"
#include "aos_tts.h"


#define MOD_STR        "audioplayer"

typedef struct {
    uvoice_player_t *player;
    int js_cb_ref;
    int list_playing;
    int state;
} audio_player_t;

typedef struct {
    void *userdata;
    int js_cb_ref;
    int event;
    unsigned long timestamp;
} audio_player_param_t;

enum {
    AUDIOPLAYER_STAT_STOP = 0,
    AUDIOPLAYER_STAT_PAUSED = 1,
    AUDIOPLAYER_STAT_PLAYING = 2,
    AUDIOPLAYER_STAT_LIST_PLAY_BEGIN = 3,
    AUDIOPLAYER_STAT_LIST_PLAY_END = 4,
    AUDIOPLAYER_STAT_ERROR = 5,
};

static audio_player_t *g_audioplayer;

static void audioplayer_state_notify(void *arg)
{
    audio_player_t *audioplayer = (audio_player_t *)arg;
    duk_context *ctx;
    ctx = be_get_context();
    be_push_ref(ctx, audioplayer->js_cb_ref);
    duk_push_int(ctx, audioplayer->state);
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    duk_gc(ctx, 0);
}

static void audioplayer_state_cb(uvoice_event_t *event, void *data)
{
    duk_context *ctx;
    audio_player_t *audioplayer = (audio_player_t *)data;
    player_state_t state;
    int audioplayer_state = PLAYER_STAT_IDLE;
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

    amp_task_schedule_call(audioplayer_state_notify, audioplayer);
}

static void audioplayer_play_cplt_notify(void *arg)
{
    audio_player_param_t *param = (audio_player_param_t *)arg;
    duk_context *ctx;
    ctx = be_get_context();
    be_push_ref(ctx, param->js_cb_ref);
    if (duk_pcall(ctx, 0) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    be_unref(ctx, param->js_cb_ref);
    duk_gc(ctx, 0);
    aos_free(param);
}

static void audioplayer_play_cplt_cb(uvoice_event_t *event, void *data)
{
    audio_player_param_t *param = (audio_player_param_t *)data;

    if (!param)
        return;

    if (event->type != UVOICE_EV_PLAYER)
        return;

    if (event->code == UVOICE_CODE_PLAYER_STATE &&
        (event->value == PLAYER_STAT_COMPLETE ||
        event->value == PLAYER_STAT_STOP) &&
        (unsigned long)aos_now_ms() >= param->timestamp) {

        amp_task_schedule_call(audioplayer_play_cplt_notify, param);
        uvoice_event_unregister(UVOICE_EV_PLAYER, audioplayer_play_cplt_cb, param);
    }
}

static void audioplayer_listplay_cplt_cb(void *data, int event)
{
    audio_player_param_t *param = (audio_player_param_t *)data;
    param->event = event;
    amp_task_schedule_call(audioplayer_play_cplt_notify, param);
}

static duk_ret_t native_audioplayer_play(duk_context *ctx)
{
    audio_player_t *audioplayer = g_audioplayer;
    audio_player_param_t *param;
    uvoice_player_t *player;
    char *audiosource, *source = NULL;
    int len;
    int is_http = 0;
    int ret = -1;

    if (!audioplayer)
        goto out;

    player = audioplayer->player;

    if (!duk_is_string(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_error(MOD_STR, "parameter must be (string, function)");
        goto out;
    }

    param = aos_malloc(sizeof(audio_player_param_t));
    if (!param) {
        amp_error(MOD_STR, "alloc param fail");
        goto out;
    }

    audiosource = (char *)duk_get_string(ctx, 0);
    if (!audiosource) {
        amp_error(MOD_STR, "audio source null");
        aos_free(param);
        goto out;
    }

    if (!strncmp(audiosource, "http", strlen("http"))) {
        is_http = 1;
        source = audiosource;
    } else {
        source = aos_malloc(strlen(audiosource) + 4);
        if (!source) {
            amp_error(MOD_STR, "alloc source fail");
            aos_free(param);
            goto out;
        }
        snprintf(source, strlen(audiosource) + 4, "fs:%s", audiosource);
    }

    duk_dup(ctx, 1);
    param->js_cb_ref = be_ref(ctx);
    param->timestamp = (unsigned long)aos_now_ms();

    if (comb_add_http_source(source, 0)) {
        amp_error(MOD_STR, "add source fail");
        be_unref(ctx, param->js_cb_ref);
        aos_free(param);
        goto out;
    }

    if (uvoice_event_register(UVOICE_EV_PLAYER,
        audioplayer_play_cplt_cb, param)) {
        amp_error(MOD_STR, "register event fail");
        be_unref(ctx, param->js_cb_ref);
        aos_free(param);
        goto out;
    }

    ret = 0;

out:
    if (!is_http && source) {
        aos_free(source);
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
        comb_clr_http_source();
        goto out;
    }

    player->clr_source();
    comb_clr_http_source();
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
    audio_player_param_t *param;
    char *source;
    int arr_idx;
    int arr_cnt;
    int i, ret = -1;
    int error = 0;
    struct aos_stat stat;
    comb_source_info_t *info;

    if (!duk_is_array(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_error(MOD_STR, "parameter must be (array, function)");
        goto out;
    }

    param = aos_malloc(sizeof(audio_player_param_t));
    if (!param) {
        amp_error(MOD_STR, "alloc param fail");
        goto out;
    }

    info = aos_malloc(sizeof(comb_source_info_t));
    if (!info) {
        amp_error(MOD_STR, "alloc info fail");
        aos_free(param);
        goto out;
    }
    memset(info, 0, sizeof(comb_source_info_t));

    arr_idx = duk_normalize_index(ctx, 0);
    arr_cnt = duk_get_length(ctx, arr_idx);

    if (arr_cnt > COMB_SOURCE_LIST_MAX) {
        amp_error(MOD_STR, "list count out of limit");
        aos_free(param);
        aos_free(info);
        goto out;
    }

    duk_dup(ctx, 1);
    param->js_cb_ref = be_ref(ctx);

    for (i = 0; i < arr_cnt; i++) {
        duk_get_prop_index(ctx, arr_idx, i);
        if (!duk_is_string(ctx, -1)) {
            amp_error(MOD_STR, "array element %d is not string", i);
            duk_pop(ctx);
            be_unref(ctx, param->js_cb_ref);
            aos_free(param);
            aos_free(info);
            goto out;
        }
        source = duk_get_string(ctx, -1);

        if (strlen(source) >= COMB_SOURCE_PATH_LEN) {
            amp_error(MOD_STR, "source %s path too long", source);
            duk_pop(ctx);
            be_unref(ctx, param->js_cb_ref);
            aos_free(param);
            aos_free(info);
            goto out;
        }

        memset(&stat, 0, sizeof(aos_stat));
        ret = aos_stat(source, &stat);
        if (ret < 0 || !S_ISREG(stat.st_mode)) {
            amp_error(MOD_STR, "source %s not exist", source);
            duk_pop(ctx);
            be_unref(ctx, param->js_cb_ref);
            aos_free(param);
            aos_free(info);
            ret = -1;
            goto out;
        }

        info->count++;
        snprintf(info->sources[i], COMB_SOURCE_PATH_LEN, "%s", source);
        duk_pop(ctx);
    }

    info->callback = audioplayer_listplay_cplt_cb;
    info->userdata = param;
    if (comb_add_file_source_list(info)) {
        error = -1;
    }
    aos_free(info);
    ret = error;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_play_list_stop(duk_context *ctx)
{
    comb_play_stop();
    comb_clr_http_source();
    duk_push_int(ctx, 0);
    return 1;
}

static int native_audioplayer_config_parse(uvoice_player_t *player)
{
    item_handle_t item_handle;
    item_handle.handle = NULL;
    aos_audio_dev_t *audio_dev = NULL;
    audio_extpa_info_t pa_info;

    if (board_attach_item(MODULE_AUDIO, "audio", &item_handle)) {
        player->set_out_device(AOS_SND_DEVICE_OUT_HEADPHONE);
#ifdef JSE_ADVANCED_ADDON_TTS
        aos_tts_set_out_device(AOS_SND_DEVICE_OUT_HEADPHONE);
#endif
    } else {
        audio_dev = board_get_node_by_handle(MODULE_AUDIO, &item_handle);
        if (!audio_dev) {
            amp_warn(MOD_STR, "get audio module fail");
            return -1;
        } else {
            amp_info(MOD_STR, "out device %d, ext pa %s, pin %d, delay %d, active %d",
                audio_dev->out_device, audio_dev->external_pa ? "enable" : "disable",
                audio_dev->external_pa_pin, audio_dev->external_pa_delay_ms,
                audio_dev->external_pa_active_high);
        }

        if (audio_dev->out_device >= AOS_SND_DEVICE_OUT_SPEAKER &&
            audio_dev->out_device < AOS_SND_DEVICE_MAX) {
                if (player->set_out_device)
                    player->set_out_device(audio_dev->out_device);
#ifdef JSE_ADVANCED_ADDON_TTS
            aos_tts_set_out_device(audio_dev->out_device);
#endif
        } else {
            if (player->set_out_device)
                player->set_out_device(AOS_SND_DEVICE_OUT_HEADPHONE);
#ifdef JSE_ADVANCED_ADDON_TTS
            aos_tts_set_out_device(AOS_SND_DEVICE_OUT_HEADPHONE);
#endif
        }

        if (audio_dev->external_pa) {
            pa_info.used = audio_dev->external_pa;
            pa_info.active_high = audio_dev->external_pa_active_high;
            pa_info.pin = audio_dev->external_pa_pin;
            pa_info.delay_ms = audio_dev->external_pa_delay_ms;
            if (player->set_external_pa)
                player->set_external_pa(&pa_info);
#ifdef JSE_ADVANCED_ADDON_TTS
            aos_tts_set_external_pa(audio_dev->external_pa_pin,
                audio_dev->external_pa_active_high, audio_dev->external_pa_delay_ms);
#endif
        }
    }

    return 0;
}

static void module_audioplayer_source_clean(void)
{
    audio_player_t *audioplayer = g_audioplayer;
    uvoice_player_t *player;

    if (!audioplayer)
        return;

    player = audioplayer->player;
    if (!player) {
        return;
    }

    comb_play_stop();
    comb_clr_http_source();
    player->stop();
    player->clr_source();

    uvoice_player_release(player);
    aos_free(audioplayer);
    g_audioplayer = NULL;
    comb_deinit();
    uvoice_free();
}

void module_audioplayer_register(void)
{
    duk_context *ctx = be_get_context();
    audio_player_t *audioplayer;

    uvoice_init();
    comb_init();

    audioplayer = aos_malloc(sizeof(audio_player_t));
    if (!audioplayer) {
        amp_error(MOD_STR, "alloc audio player fail");
        return;
    }
    audioplayer->player = uvoice_player_create();
    if (!audioplayer->player) {
        amp_error(MOD_STR, "create uvoice player fail");
        aos_free(audioplayer);
        return;
    }
    native_audioplayer_config_parse(audioplayer->player);
    audioplayer->state = -1;
    g_audioplayer = audioplayer;

    amp_module_free_register(module_audioplayer_source_clean);

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("listPlay",    native_audioplayer_play_list, 2);
    AMP_ADD_FUNCTION("listPlayStop",native_audioplayer_play_list_stop, 0);
    AMP_ADD_FUNCTION("play",        native_audioplayer_play, 2);
    AMP_ADD_FUNCTION("stop",        native_audioplayer_stop, 0);
    AMP_ADD_FUNCTION("pause",       native_audioplayer_pause, 0);
    AMP_ADD_FUNCTION("resume",      native_audioplayer_resume, 0);
    AMP_ADD_FUNCTION("seekto",      native_audioplayer_seekto, 1);
    AMP_ADD_FUNCTION("getPosition", native_audioplayer_position_get, 0);
    AMP_ADD_FUNCTION("getDuration", native_audioplayer_duration_get, 0);
    AMP_ADD_FUNCTION("getState",    native_audioplayer_state_get, 0);
    AMP_ADD_FUNCTION("onState",     native_audioplayer_state_on, 1);
    AMP_ADD_FUNCTION("setVolume",   native_audioplayer_volume_set, 1);
    AMP_ADD_FUNCTION("getVolume",   native_audioplayer_volume_get, 0);

    duk_put_prop_string(ctx, -2, "audioplayer");
}

