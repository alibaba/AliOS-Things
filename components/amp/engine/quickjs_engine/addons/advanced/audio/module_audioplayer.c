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

#include "uvoice_types.h"
#include "uvoice_event.h"
#include "uvoice_player.h"
#include "uvoice_comb.h"
#include "aos_pcm.h"
#include "aos_tts.h"

#include "quickjs.h"
#include "quickjs_addon_common.h"


#define MOD_STR        "audioplayer"


typedef struct {
    uvoice_player_t *player;
    JSValue js_cb_ref;
    int list_playing;
    int state;
} audio_player_t;

typedef struct {
    void *userdata;
    JSValue js_cb_ref;
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

enum {
    JS_ATOM_NULL,
#define DEF(name, str) JS_ATOM_##name,
#include "quickjs-atom.h"
#undef DEF
    JS_ATOM_END,
};

static audio_player_t *g_audioplayer;
static JSClassID js_audio_class_id;

static int js_get_length32(JSContext* ctx, uint32_t* pres,
                                       JSValueConst obj)
{
    JSValue len_val;
    len_val = JS_GetProperty(ctx, obj, JS_ATOM_length);
    if (JS_IsException(len_val)) {
        *pres = 0;
        return -1;
    }
    return JS_ToInt32(ctx, pres, len_val);
}

static void audioplayer_state_notify(void *arg)
{
    audio_player_t *audioplayer = (audio_player_t *)arg;
    JSValue state;

    JSContext *ctx = js_get_context();
    state = JS_NewInt32(ctx, audioplayer->state);

    JSValue val = JS_Call(ctx, audioplayer->js_cb_ref, JS_UNDEFINED, 1, &state);
    JS_FreeValue(ctx, val);
    JS_FreeValue(ctx, state);
}

static void audioplayer_state_cb(uvoice_event_t *event, void *data)
{
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
    JSContext *ctx = js_get_context();

    JSValue val = JS_Call(ctx, param->js_cb_ref, JS_UNDEFINED, 0, NULL);
    JS_FreeValue(ctx, val);
    JS_FreeValue(ctx, param->js_cb_ref);

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

static JSValue native_audioplayer_play(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    audio_player_t *audioplayer = g_audioplayer;
    audio_player_param_t *param = NULL;
    uvoice_player_t *player;
    char *audiosource = NULL;
    char *source = NULL;
    int len;
    int is_http = 0;
    int ret = -1;

    if (!audioplayer)
        goto out;

    player = audioplayer->player;

    if (!JS_IsString(argv[0]) || !JS_IsFunction(ctx, argv[1])) {
        amp_error(MOD_STR, "parameter must be (string, function)");
        goto out;
    }

    param = aos_malloc(sizeof(audio_player_param_t));
    if (!param) {
        amp_error(MOD_STR, "alloc param fail");
        goto out;
    }

    audiosource = (char *)JS_ToCString(ctx, argv[0]);
    if (!audiosource) {
        amp_error(MOD_STR, "audio source null");
        goto out;
    }

    if (!strncmp(audiosource, "http", strlen("http"))) {
        is_http = 1;
        source = audiosource;
    } else {
        source = aos_malloc(strlen(audiosource) + 4);
        if (!source) {
            amp_error(MOD_STR, "alloc source fail");
            goto out;
        }
        snprintf(source, strlen(audiosource) + 4, "fs:%s", audiosource);
    }

    if (!JS_IsFunction(ctx, argv[1])) {
        JS_ThrowTypeError(ctx, "not a function");
        goto out;
    }

    param->js_cb_ref = JS_DupValue(ctx, argv[1]);
    param->timestamp = (unsigned long)aos_now_ms();

    if (comb_add_http_source(source, 0)) {
        amp_error(MOD_STR, "add source fail");
        JS_FreeValue(ctx, param->js_cb_ref);
        goto out;
    }

    if (uvoice_event_register(UVOICE_EV_PLAYER,
        audioplayer_play_cplt_cb, param)) {
        amp_error(MOD_STR, "register event fail");
        JS_FreeValue(ctx, param->js_cb_ref);
        goto out;
    }

    ret = 0;

out:
    if (!is_http && source) {
        aos_free(source);
    }

    if(param != NULL) {
        aos_free(param);
    }

    if(audiosource != NULL) {
        JS_FreeCString(ctx, audiosource);
    }

    return JS_NewInt32(ctx, ret);
}

static JSValue native_audioplayer_stop(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
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
    return JS_NewInt32(ctx, ret);
}

static JSValue native_audioplayer_pause(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
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
    return JS_NewInt32(ctx, ret);
}

static JSValue native_audioplayer_resume(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
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
    return JS_NewInt32(ctx, ret);
}

static JSValue native_audioplayer_seekto(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    audio_player_t *audioplayer = g_audioplayer;
    uvoice_player_t *player;
    int ret = -1;
    int seconds;

    if (!audioplayer)
        goto out;
    player = audioplayer->player;

    if (!JS_IsNumber(argv[0])) {
        amp_error(MOD_STR, "parameter must be (number)");
        goto out;
    }

    JS_ToInt32(ctx, &seconds, argv[0]);
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
    return JS_NewInt32(ctx, ret);
}

static JSValue native_audioplayer_position_get(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
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
    return JS_NewInt32(ctx, ret);
}

static JSValue native_audioplayer_duration_get(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
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
    return JS_NewInt32(ctx, ret);
}

static JSValue native_audioplayer_state_get(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
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
    return JS_NewInt32(ctx, ret);
}

static JSValue native_audioplayer_state_on(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    audio_player_t *audioplayer = g_audioplayer;
    int ret = -1;

    if (!JS_IsFunction(ctx, argv[0])) {
        amp_warn(MOD_STR, "parameter must be (function)");
        goto out;
    }

    JSValue state_cb = argv[0];
    if (!JS_IsFunction(ctx, state_cb)) {
        return JS_ThrowTypeError(ctx, "not a function");
    }

    audioplayer->js_cb_ref = JS_DupValue(ctx, state_cb);

    if (uvoice_event_register(UVOICE_EV_PLAYER,
        audioplayer_state_cb, audioplayer)) {
        amp_error(MOD_STR, "register event fail");
        goto out;
    }

    ret = 0;
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_audioplayer_volume_set(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    audio_player_t *audioplayer = g_audioplayer;
    uvoice_player_t *player;
    int ret = -1;
    int volume;

    if (!audioplayer)
        goto out;
    player = audioplayer->player;

    if (!JS_IsNumber(argv[0])) {
        amp_error(MOD_STR, "parameter must be (number)");
        goto out;
    }

    JS_ToInt32(ctx, &volume, argv[0]);
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
    return JS_NewInt32(ctx, ret);
}

static JSValue native_audioplayer_volume_get(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
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
    return JS_NewInt32(ctx, ret);
}

static JSValue native_audioplayer_play_list(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    audio_player_param_t *param;
    char *source;
    int arr_idx;
    int arr_cnt;
    int i, ret = -1;
    int error = 0;
    struct aos_stat stat;
    comb_source_info_t *info;

    if (!JS_IsArray(ctx, argv[0]) || !JS_IsFunction(ctx, argv[1])) {
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

    js_get_length32(ctx, &arr_cnt, argv[0]);

    if (arr_cnt > COMB_SOURCE_LIST_MAX) {
        amp_error(MOD_STR, "list count out of limit");
        aos_free(param);
        aos_free(info);
        goto out;
    }

    JSValue play_cb = argv[1];
    if (!JS_IsFunction(ctx, play_cb)) {
        return JS_ThrowTypeError(ctx, "not a function");
    }
    param->js_cb_ref = JS_DupValue(ctx, play_cb);

    for (i = 0; i < arr_cnt; i++) {
        JSValue str_val = JS_GetPropertyUint32(ctx, argv[0], i);
        if (!JS_IsString(str_val)) {
            amp_error(MOD_STR, "array element %d is not string", i);
            JS_FreeValue(ctx, param->js_cb_ref);
            aos_free(param);
            aos_free(info);
            goto out;
        }
        source = JS_ToCString(ctx, str_val);

        if (strlen(source) >= COMB_SOURCE_PATH_LEN) {
            amp_error(MOD_STR, "source %s path too long", source);
            JS_FreeValue(ctx, param->js_cb_ref);
            aos_free(param);
            aos_free(info);
            JS_FreeCString(ctx, source);
            goto out;
        }

        memset(&stat, 0, sizeof(aos_stat));
        ret = aos_stat(source, &stat);
        if (ret < 0 || !S_ISREG(stat.st_mode)) {
            amp_error(MOD_STR, "source %s not exist", source);
            JS_FreeValue(ctx, param->js_cb_ref);
            aos_free(param);
            aos_free(info);
            ret = -1;
            JS_FreeCString(ctx, source);
            goto out;
        }

        info->count++;
        snprintf(info->sources[i], COMB_SOURCE_PATH_LEN, "%s", source);
        JS_FreeCString(ctx, source);
    }

    info->callback = audioplayer_listplay_cplt_cb;
    info->userdata = param;
    if (comb_add_file_source_list(info)) {
        error = -1;
    }
    aos_free(info);
    ret = error;
out:

    return JS_NewInt32(ctx, ret);
}

static JSValue native_audioplayer_play_list_stop(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    comb_play_stop();
    comb_clr_http_source();

    return JS_NewInt32(ctx, 0);
}

static int native_audioplayer_config_parse(uvoice_player_t *player)
{
    item_handle_t item_handle;
    item_handle.handle = NULL;
    aos_audio_dev_t *audio_dev;
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
            player->set_out_device(audio_dev->out_device);
#ifdef JSE_ADVANCED_ADDON_TTS
            aos_tts_set_out_device(audio_dev->out_device);
#endif
        } else {
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

static JSClassDef js_audio_class = {
    "audio",
};

static const JSCFunctionListEntry js_audio_funcs[] = {
    JS_CFUNC_DEF("listPlay",    2, native_audioplayer_play_list),
    JS_CFUNC_DEF("listPlayStop",0, native_audioplayer_play_list_stop),
    JS_CFUNC_DEF("play",        2, native_audioplayer_play),
    JS_CFUNC_DEF("stop",        0, native_audioplayer_stop),
    JS_CFUNC_DEF("pause",       0, native_audioplayer_pause),
    JS_CFUNC_DEF("resume",      0, native_audioplayer_resume),
    JS_CFUNC_DEF("seekto",      1, native_audioplayer_seekto),
    JS_CFUNC_DEF("getPosition", 0, native_audioplayer_position_get),
    JS_CFUNC_DEF("getDuration", 0, native_audioplayer_duration_get),
    JS_CFUNC_DEF("getState",    0, native_audioplayer_state_get),
    JS_CFUNC_DEF("onState",     1, native_audioplayer_state_on),
    JS_CFUNC_DEF("setVolume",   1, native_audioplayer_volume_set),
    JS_CFUNC_DEF("getVolume",   0, native_audioplayer_volume_get)
};

static int js_audio_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_audio_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_audio_class_id, &js_audio_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_audio_funcs, countof(js_audio_funcs));
    JS_SetClassProto(ctx, js_audio_class_id, proto);
    return JS_SetModuleExportList(ctx, m, js_audio_funcs, countof(js_audio_funcs));
}

JSModuleDef *js_init_module_audio(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_audio_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_audio_funcs, countof(js_audio_funcs));
    return m;
}

void module_audio_register(void)
{
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

    amp_debug(MOD_STR, "module_audio_register");
    JSContext *ctx = js_get_context();
    js_init_module_audio(ctx, "AUDIOPLAYER");
}

