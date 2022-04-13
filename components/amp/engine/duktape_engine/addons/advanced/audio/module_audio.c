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


#define MOD_STR        "Audio"

static duk_ret_t native_audio_play(duk_context *ctx)
{
    int ret = -1;
    char *source;

    if (!duk_is_string(ctx, 0)) {
        amp_error(MOD_STR, "parameter must be (string)");
        goto out;
    }

    source = (char *)duk_get_string(ctx, 0);
    if (!source) {
        amp_error(MOD_STR, "source null");
        goto out;
    }

    ret = amp_hal_audio_play(source);
    if (ret != 0) {
        amp_error(MOD_STR, "auido play failed");
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audio_resume(duk_context *ctx)
{
    int ret = -1;

    ret = amp_hal_audio_resume();
    if (ret != 0) {
        amp_error(MOD_STR, "audio resume failed");
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audio_pause(duk_context *ctx)
{
    int ret = -1;

    ret = amp_hal_audio_pause();
    if (ret != 0) {
        amp_error(MOD_STR, "audio pause failed");
        goto out;
    }

    ret = 0;    
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audio_stop(duk_context *ctx)
{
    int ret = -1;
   
    if (amp_hal_audio_stop()) {
        amp_error(MOD_STR, "audio stop failed");
        goto out;
    }

    ret = 0;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audio_volume_set(duk_context *ctx)
{
    int ret = -1;
    int device;
    int volume;

    if (!duk_is_number(ctx, 0) || !duk_is_number(ctx, 1)) {
        amp_error(MOD_STR, "parameter must be (number, number)");
        goto out;
    }

    device = duk_get_int(ctx, 0);
    if (device < 0) {
        amp_error(MOD_STR, "device %d invalid", device);
        goto out;
    }

    volume = duk_get_int(ctx, 1);
    if (volume < 0) {
        amp_error(MOD_STR, "volume %d invalid", volume);
        goto out;
    }

    ret = amp_hal_audio_volume_set(device, volume);
    if (ret < 0) {
        amp_error(MOD_STR, "set audio volume failed");
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audio_mic_gain_set(duk_context *ctx)
{
    int ret = -1;
    int gain;

    if (!duk_is_number(ctx, 0)) {
        amp_error(MOD_STR, "parameter must be (number)");
        goto out;
    }

    gain = duk_get_int(ctx, 0);
    ret = amp_hal_mic_gain_set(gain);
    if (ret != 0) {
        amp_error(MOD_STR, "set mic gain %d failed", gain);
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}


static duk_ret_t native_audio_mic_gain_get(duk_context *ctx)
{
    int ret = -1;
    int gain = 0;

    ret = amp_hal_mic_gain_get(&gain);
    if (ret != 0) {
        amp_error(MOD_STR, "get mic gain failed");
        goto out;
    }

    duk_push_int(ctx, gain);
    return 1;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_audio_mute(duk_context *ctx)
{
    int ret = -1;
    int state;

    if (!duk_is_number(ctx, 0)) {
        amp_error(MOD_STR, "parameter must be (number)");
        goto out;
    }

    state = duk_get_int(ctx, 0);
    ret = amp_hal_audio_mute(state);
    if (ret != 0) {
        amp_error(MOD_STR, "audio mute %d failed", state);
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}


void module_audio_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_audio_play, 1);
    duk_put_prop_string(ctx, -2, "play");

    duk_push_c_function(ctx, native_audio_stop, 0);
    duk_put_prop_string(ctx, -2, "stop");

    duk_push_c_function(ctx, native_audio_pause, 0);
    duk_put_prop_string(ctx, -2, "pause");

    duk_push_c_function(ctx, native_audio_resume, 0);
    duk_put_prop_string(ctx, -2, "resume");

    duk_push_c_function(ctx, native_audio_volume_set, 2);
    duk_put_prop_string(ctx, -2, "volume");
  
    duk_push_c_function(ctx, native_audio_mic_gain_set, 1);
    duk_put_prop_string(ctx, -2, "setGain");

    duk_push_c_function(ctx, native_audio_mic_gain_get, 0);
    duk_put_prop_string(ctx, -2, "getGain");

    duk_push_c_function(ctx, native_audio_mute, 1);
    duk_put_prop_string(ctx, -2, "mute");

    duk_put_prop_string(ctx, -2, "Audio");
}

