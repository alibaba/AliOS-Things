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
#include "aos/vfs.h"
#include "uvoice_types.h"
#include "uvoice_event.h"
#include "uvoice_recorder.h"
#include "aos_pcm.h"
#include "aos_tts.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR        "audiorecorder"

typedef struct {
    uvoice_recorder_t *recorder;
    int state;
} audio_recorder_t;

enum {
    AUDIORECORDER_STAT_STOP = 0,
    AUDIORECORDER_STAT_PAUSED = 1,
    AUDIORECORDER_STAT_RECORDING = 2,
    AUDIORECORDER_STAT_ERROR = 5,
};

static audio_recorder_t *g_audiorecorder;
static JSClassID js_audiorecorder_class_id;

static JSValue native_audiorecorder_record(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    audio_recorder_t *audiorecorder = g_audiorecorder;
    uvoice_recorder_t *recorder;
    media_format_t format = MEDIA_FMT_UNKNOWN;
    const char *sink;
    char *sink_path = NULL;
    int samplerate, channels, sbits, frames;
    int ret = -1;

    if (!audiorecorder)
        goto out;

    recorder = audiorecorder->recorder;

    if (!JS_IsNumber(argv[0]) || !JS_IsNumber(argv[1]) ||
        !JS_IsNumber(argv[2]) || !JS_IsNumber(argv[3]) ||
        !JS_IsString(argv[4])) {
        amp_error(MOD_STR, "parameter must be (number, number, number, number, string)");
        goto out;
    }

    JS_ToInt32(ctx, &samplerate, argv[0]);
    JS_ToInt32(ctx, &channels, argv[1]);
    JS_ToInt32(ctx, &sbits, argv[2]);
    JS_ToInt32(ctx, &frames, argv[3]);

    sink = (char *)JS_ToCString(ctx, argv[4]);
    if (!sink) {
        amp_error(MOD_STR, "audio sink null");
        goto out;
    }

    sink_path = aos_malloc(strlen(sink) + 4);
    if (!sink_path) {
        amp_error(MOD_STR, "alloc sink buffer fail");
        goto out;
    }
    snprintf(sink_path, strlen(sink) + 4, "fs:%s", sink);

    ret = recorder->set_sink(MEDIA_FMT_UNKNOWN, samplerate, channels, sbits, frames, 0, sink_path);
    if (ret) {
        amp_error(MOD_STR, "set sink %s fail", sink_path);
        ret = -1;
        goto out;
    }

    ret = recorder->start();
    if (ret) {
        amp_error(MOD_STR, "recorder start fail");
        recorder->clr_sink();
        ret = -1;
        goto out;
    }

    ret = 0;

out:
    if (sink_path) {
        aos_free(sink_path);
    }

    return JS_NewInt32(ctx, ret);
}

static JSValue native_audiorecorder_stop(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    audio_recorder_t *audiorecorder = g_audiorecorder;
    uvoice_recorder_t *recorder;
    int ret = -1;

    if (!audiorecorder)
        goto out;
    recorder = audiorecorder->recorder;

    if (recorder->stop()) {
        amp_error(MOD_STR, "recorder stop fail");
        goto out;
    }

    recorder->clr_sink();
    ret = 0;

out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_audiorecorder_pause(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    audio_recorder_t *audiorecorder = g_audiorecorder;
    uvoice_recorder_t *recorder;
    int ret = -1;

    if (!audiorecorder)
        goto out;

    recorder = audiorecorder->recorder;

    if (recorder->stop()) {
        amp_error(MOD_STR, "recorder pause fail");
        goto out;
    }

    ret = 0;

out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_audiorecorder_resume(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    audio_recorder_t *audiorecorder = g_audiorecorder;
    uvoice_recorder_t *recorder;
    int ret = -1;

    if (!audiorecorder)
        goto out;

    recorder = audiorecorder->recorder;

    if (recorder->start()) {
        amp_error(MOD_STR, "recorder resume fail");
        goto out;
    }
    ret = 0;
out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_audiorecorder_position_get(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    audio_recorder_t *audiorecorder = g_audiorecorder;
    uvoice_recorder_t *recorder;
    int position = 0;
    int ret = -1;

    if (!audiorecorder)
        goto out;
    recorder = audiorecorder->recorder;

    if (recorder->get_position(&position)) {
        amp_error(MOD_STR, "get position fail");
        goto out;
    }

    ret = position;

out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_audiorecorder_state_get(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    audio_recorder_t *audiorecorder = g_audiorecorder;
    uvoice_recorder_t *recorder;
    recorder_state_t state = RECORDER_STAT_IDLE;
    int audiorecorder_state;
    int ret = -1;

    if (!audiorecorder)
        goto out;
    recorder = audiorecorder->recorder;

    if (recorder->get_state(&state)) {
        amp_error(MOD_STR, "get recorder state fail");
        goto out;
    }

    switch (state) {
    case RECORDER_STAT_RUNNING:
        audiorecorder_state = AUDIORECORDER_STAT_RECORDING;
        break;
    case RECORDER_STAT_STOP:
        audiorecorder_state = AUDIORECORDER_STAT_PAUSED;
        break;
    case RECORDER_STAT_IDLE:
    case RECORDER_STAT_READY:
        audiorecorder_state = AUDIORECORDER_STAT_STOP;
        break;
    case RECORDER_STAT_ERROR:
        audiorecorder_state = AUDIORECORDER_STAT_ERROR;
        break;
    default:
        audiorecorder_state = AUDIORECORDER_STAT_STOP;
        break;
    }
    ret = audiorecorder_state;

out:
    return JS_NewInt32(ctx, ret);
}

static void module_audiorecorder_source_clean(void)
{
    audio_recorder_t *audiorecorder = g_audiorecorder;
    uvoice_recorder_t *recorder;

    if (!audiorecorder)
        return;

    recorder = audiorecorder->recorder;
    if (!recorder) {
        return;
    }

    recorder->stop();
    recorder->clr_sink();

    uvoice_recorder_release(recorder);
    amp_free(audiorecorder);
    g_audiorecorder = NULL;
    uvoice_free();
}

static JSClassDef js_audiorecorder_class = {
    "AUDIORECORDER",
};

static const JSCFunctionListEntry js_audiorecorder_funcs[] = {
    JS_CFUNC_DEF("record",      5, native_audiorecorder_record),
    JS_CFUNC_DEF("stop",        0, native_audiorecorder_stop),
    JS_CFUNC_DEF("pause",       0, native_audiorecorder_pause),
    JS_CFUNC_DEF("resume",      0, native_audiorecorder_resume),
    JS_CFUNC_DEF("getPosition", 0, native_audiorecorder_position_get),
    JS_CFUNC_DEF("getState",    0, native_audiorecorder_state_get)
};

static int js_audiorecorder_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_audiorecorder_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_audiorecorder_class_id, &js_audiorecorder_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_audiorecorder_funcs, countof(js_audiorecorder_funcs));
    JS_SetClassProto(ctx, js_audiorecorder_class_id, proto);
    return JS_SetModuleExportList(ctx, m, js_audiorecorder_funcs, countof(js_audiorecorder_funcs));
}

JSModuleDef *js_init_module_audiorecorder(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_audiorecorder_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_audiorecorder_funcs, countof(js_audiorecorder_funcs));
    return m;
}

void module_audiorecorder_register(void)
{
    audio_recorder_t *audiorecorder;

    uvoice_init();

    audiorecorder = aos_malloc(sizeof(audio_recorder_t));
    if (!audiorecorder) {
        amp_error(MOD_STR, "alloc audio recorder fail");
        return;
    }
    audiorecorder->recorder = uvoice_recorder_create();
    if (!audiorecorder->recorder) {
        amp_error(MOD_STR, "create uvoice recorder fail");
        aos_free(audiorecorder);
        return;
    }
    audiorecorder->state = -1;
    g_audiorecorder = audiorecorder;

    amp_module_free_register(module_audiorecorder_source_clean);

    amp_debug(MOD_STR, "module_audiorecorder_register");
    JSContext *ctx = js_get_context();
    js_init_module_audiorecorder(ctx, "AUDIORECORDER");
}

