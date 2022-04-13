/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdarg.h>

#include "amp_system.h"
#include "amp_defines.h"
#include "dev_model_api.h"
#include "be_inl.h"
#include "amp_audio.h"


#define MOD_STR        "TTS"

static duk_ret_t native_tts_play(duk_context *ctx)
{
    int ret = -1;
    int encode_type;
    char *text;

    if (!duk_is_string(ctx, 0)||  !duk_is_number(ctx, 1)) {
        amp_error(MOD_STR, "parameter must be (string, num)");
        goto out;
    }

    text = (char *)duk_get_string(ctx, 0);
    if (!text) {
        amp_error(MOD_STR, "text null");
        goto out;
    }

    encode_type = duk_get_number(ctx, 1);
    if (encode_type < 0) {
        amp_error(MOD_STR, "encode type invalid");
        goto out;
    }

    if (amp_hal_tts_is_playing()) {
        amp_warn(MOD_STR, "tts is playing, ignore");
        goto out;
    }

    ret = amp_hal_tts_play(text, encode_type);
    if (ret != 0) {
        amp_error(MOD_STR, "tts play failed");
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_tts_stop(duk_context *ctx)
{
    int ret = -1;

    ret = amp_hal_tts_stop();
    if (ret != 0) {
        amp_error(MOD_STR, "tts stop failed");
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_tts_volume_set(duk_context *ctx)
{
    int ret = -1;
    int volume;

    if (!duk_is_number(ctx, 0)) {
        amp_error(MOD_STR, "parameter must be (number)");
        goto out;
    }

    volume = duk_get_int(ctx, 0);
    if (volume < 0) {
        amp_error(MOD_STR, "volume invalid");
        goto out;
    }

    ret = amp_hal_tts_volume_set(volume);
    if (ret != 0) {
        amp_error(MOD_STR, "set tts volume failed");
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_tts_pitch_set(duk_context *ctx)
{
    int ret = -1;
    int pitch;

    if (!duk_is_number(ctx, 0)) {
        amp_error(MOD_STR, "parameter must be (number)");
        goto out;
    }

    pitch = duk_get_int(ctx, 0);
    if (pitch < 0) {
        amp_error(MOD_STR, "pitch invalid");
        goto out;
    }

    ret = amp_hal_tts_pitch_set(pitch);
    if (ret != 0) {
        amp_error(MOD_STR, "set tts pitch failed");
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_tts_speed_set(duk_context *ctx)
{
    int ret = -1;
    int speed;

    if (!duk_is_number(ctx, 0)) {
        amp_error(MOD_STR, "parameter must be (number)\n");
        goto out;
    }

    speed = duk_get_int(ctx, 0);
    if (speed < 0) {
        amp_error(MOD_STR, "speed invalid");
        goto out;
    }

    ret = amp_hal_tts_speed_set(speed);
    if (ret != 0) {
        amp_error(MOD_STR, "set tts speed failed");
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_tts_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_tts_play, 2);
    duk_put_prop_string(ctx, -2, "play");

    duk_push_c_function(ctx, native_tts_stop, 0);
    duk_put_prop_string(ctx, -2, "stop");

    duk_push_c_function(ctx, native_tts_volume_set, 1);
    duk_put_prop_string(ctx, -2, "volume");

    duk_push_c_function(ctx, native_tts_pitch_set, 1);
    duk_put_prop_string(ctx, -2, "pitch");

    duk_push_c_function(ctx, native_tts_speed_set, 1);
    duk_put_prop_string(ctx, -2, "speed");

    duk_put_prop_string(ctx, -2, "TTS");
}

