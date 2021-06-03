/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdarg.h>

#include "aos_system.h"
#include "amp_defines.h"
#include "dev_model_api.h"
#include "be_inl.h"

#include "fibo_opencpu.h"


#define MOD_STR        "audioplayer"


#define PLAY_LIST_FILE_COUNT_MAX	32
#define PLAY_LIST_FILE_PATH_LEN_MAX	64
static char g_play_list[PLAY_LIST_FILE_COUNT_MAX][PLAY_LIST_FILE_PATH_LEN_MAX];

static int g_play_cb_js_ref;

enum {
    AUDIOPLAYER_STAT_STOP = 0,
    AUDIOPLAYER_STAT_PLAYING = 1
};

static void __audioplayer_complete_cb(void)
{
    duk_context *ctx;
    ctx = be_get_context();
    be_push_ref(ctx, g_play_cb_js_ref);
    if (duk_pcall(ctx, 0) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    duk_gc(ctx, 0);
}

static duk_ret_t native_audioplayer_play(duk_context *ctx)
{
    char *audiosource, *source = NULL;
    char root_dir[16];
    int ret = -1;

    if (!duk_is_string(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_error(MOD_STR, "parameter must be (string, function)");
        goto out;
    }

    if (fibo_get_audio_status()) {
        amp_error(MOD_STR, "player busy");
        goto out;
    }

    audiosource = (char *)duk_get_string(ctx, 0);
    if (!audiosource) {
        amp_error(MOD_STR, "audio source null");
        goto out;
    }

    duk_dup(ctx, 1);
    g_play_cb_js_ref = be_ref(ctx);

    memset(root_dir, 0, sizeof(root_dir));
    amp_get_user_dir(root_dir);
    if (!strncmp(audiosource, "http", strlen("http"))) {
        amp_error(MOD_STR, "http play not support");
        goto out;
    } else {
        source = aos_malloc(strlen(audiosource) + strlen(root_dir) + 1);
        if (!source) {
            amp_error(MOD_STR, "alloc source fail");
            goto out;
        }
        snprintf(source, strlen(audiosource) + strlen(root_dir) + 1, "%s%s", root_dir, audiosource);
    }

    if (fibo_audio_path_play(1, source, __audioplayer_complete_cb) < 0) {
        amp_error(MOD_STR, "play %s fail", source);
        goto out;
    }

    aos_free(source);
    ret = 0;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_stop(duk_context *ctx)
{
    int ret = -1;

    if (fibo_audio_stop() < 0) {
        amp_error(MOD_STR, "play stop fail");
        goto out;
    }

    ret = 0;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_pause(duk_context *ctx)
{
    int ret = -1;

    if (fibo_audio_pause() < 0) {
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
    int ret = -1;

    if (fibo_audio_resume() < 0) {
        amp_error(MOD_STR, "play resume fail");
        goto out;
    }

    ret = 0;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_state_get(duk_context *ctx)
{
    int ret;
    if (fibo_get_audio_status()) {
        ret = AUDIOPLAYER_STAT_PLAYING;
    } else {
        ret = AUDIOPLAYER_STAT_STOP;
    }
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_state_on(duk_context *ctx)
{
    int ret = -1;
    int js_cb_ref;

    if (!duk_is_function(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be (function)");
        goto out;
    }

    //duk_dup(ctx, 0);
    //js_cb_ref = be_ref(ctx);

    ret = 0;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_seekto(duk_context *ctx)
{
    int ret = -1;
    int seconds;

    if (!duk_is_number(ctx, 0)) {
        amp_error(MOD_STR, "parameter must be (number)");
        goto out;
    }

    seconds = duk_get_int(ctx, 0);
    if (seconds < 0) {
        amp_error(MOD_STR, "seconds %d invalid", seconds);
        goto out;
    }

    ret = 0;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_position_get(duk_context *ctx)
{
    int position = 0;
    int ret = -1;

    ret = position;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_duration_get(duk_context *ctx)
{
    int duration = 0;
    int ret = -1;

    ret = duration;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_volume_set(duk_context *ctx)
{
    int ret = -1;
    int volume;

    if (!duk_is_number(ctx, 0)) {
        amp_error(MOD_STR, "parameter must be (number)");
        goto out;
    }

    volume = duk_get_int(ctx, 0);
    if (volume < 0 || volume > 100) {
        amp_error(MOD_STR, "volume %d invalid", volume);
        goto out;
    }

    if (fibo_set_volume_external(AUDIO_PLAY_VOLUME, volume) < 0) {
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
    int ret;

    ret = fibo_get_volume_external(AUDIO_PLAY_VOLUME);
    if (ret < 0) {
        amp_error(MOD_STR, "get volume fail");
        ret = -1;
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_play_list(duk_context *ctx)
{
    char *source;
    char root_dir[16] = {0};
    int rootdir_len;
    int arr_idx;
    int arr_cnt;
    int i, j, len, ret = -1;

    if (!duk_is_array(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_error(MOD_STR, "parameter must be (array, function)");
        goto out;
    }

    arr_idx = duk_normalize_index(ctx, 0);
    arr_cnt = duk_get_length(ctx, arr_idx);
	if (arr_cnt > PLAY_LIST_FILE_COUNT_MAX) {
        amp_error(MOD_STR, "list count out of limit");
        goto out;
	}

    amp_get_user_dir(root_dir);
    rootdir_len = strlen(root_dir);

    for (i = 0; i < arr_cnt; i++) {
        duk_get_prop_index(ctx, arr_idx, i);
        if (!duk_is_string(ctx, -1)) {
            amp_error(MOD_STR, "array element %d is not string", i);
            duk_pop(ctx);
            goto out;
        }
        source = duk_get_string(ctx, -1);
        snprintf(g_play_list[i], PLAY_LIST_FILE_PATH_LEN_MAX, "%s%s", root_dir, source);
        duk_pop(ctx);
    }

    if (fibo_audio_list_play(g_play_list, arr_cnt) < 0) {
        amp_error(MOD_STR, "play list fail");
        ret = -1;
    } else {
        ret = 0;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audioplayer_play_list_stop(duk_context *ctx)
{
    int ret = -1;

    if (fibo_audio_stop() < 0) {
        amp_error(MOD_STR, "play stop fail");
        goto out;
    }

    ret = 0;
out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_audioplayer_register(void)
{
    duk_context *ctx = be_get_context();

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

