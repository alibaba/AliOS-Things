/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <string.h>

#if MICROPY_PY_AUDIO
#include "py/objstr.h"
#include "py/runtime.h"

#include "audio_hal.h"
#include "audio_pipeline.h"
#include "board.h"
#include "filter_resample.h"

#include "i2s_stream.h"
#include "raw_stream.h"
#include "vfs_stream.h"

#include "amrnb_encoder.h"
#include "wav_encoder.h"

enum {
    PCM,
    AMR,
    WAV,
    MP3
};

typedef struct _audio_recorder_obj_t {
    mp_obj_base_t base;

    audio_pipeline_handle_t pipeline;
    audio_element_handle_t i2s_stream;
    audio_element_handle_t filter;
    audio_element_handle_t encoder;
    audio_element_handle_t out_stream;

    esp_timer_handle_t timer;
    mp_obj_t end_cb;
} audio_recorder_obj_t;

STATIC mp_obj_t audio_recorder_stop(mp_obj_t self_in);

STATIC mp_obj_t audio_recorder_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    audio_recorder_obj_t *self = m_new_obj_with_finaliser(audio_recorder_obj_t);
    self->base.type = type;
    return MP_OBJ_FROM_PTR(self);
}

STATIC audio_element_handle_t audio_recorder_create_filter(int encoder_type)
{
    rsp_filter_cfg_t rsp_cfg = DEFAULT_RESAMPLE_FILTER_CONFIG();
    rsp_cfg.src_rate = 48000;
    rsp_cfg.src_ch = 2;
    rsp_cfg.dest_ch = 1;
    rsp_cfg.task_core = 1;

    switch (encoder_type) {
        case PCM: {
            rsp_cfg.dest_rate = 16000;
            break;
        }
        case AMR: {
            rsp_cfg.dest_rate = 8000;
            break;
        }
        case WAV: {
            rsp_cfg.dest_rate = 16000;
            break;
        }
        default:
            break;
    }
    return rsp_filter_init(&rsp_cfg);
}

STATIC audio_element_handle_t audio_recorder_create_encoder(int encoder_type)
{
    audio_element_handle_t encoder = NULL;

    switch (encoder_type) {
        case AMR: {
            amrnb_encoder_cfg_t amr_enc_cfg = DEFAULT_AMRNB_ENCODER_CONFIG();
            amr_enc_cfg.task_core = 1;
            encoder = amrnb_encoder_init(&amr_enc_cfg);
            break;
        }
        case WAV: {
            wav_encoder_cfg_t wav_cfg = DEFAULT_WAV_ENCODER_CONFIG();
            wav_cfg.task_core = 1;
            encoder = wav_encoder_init(&wav_cfg);
            break;
        }
        default:
            break;
    }

    return encoder;
}

STATIC audio_element_handle_t audio_recorder_create_outstream(const char *uri)
{
    audio_element_handle_t out_stream = NULL;
    if (strstr(uri, "/sdcard/") != NULL) {
        vfs_stream_cfg_t vfs_cfg = VFS_STREAM_CFG_DEFAULT();
        vfs_cfg.type = AUDIO_STREAM_WRITER;
        vfs_cfg.task_core = 1;
        out_stream = vfs_stream_init(&vfs_cfg);
    } else if (strstr(uri, "/spiffs/") != NULL) {
        // TODO: spiffs
    } else {
        raw_stream_cfg_t raw_cfg = RAW_STREAM_CFG_DEFAULT();
        raw_cfg.type = AUDIO_STREAM_WRITER;
        out_stream = raw_stream_init(&raw_cfg);
    }
    audio_element_set_uri(out_stream, uri);

    return out_stream;
}

STATIC void audio_recorder_create(audio_recorder_obj_t *self, const char *uri, int format)
{
    // init audio board
    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_BOTH, AUDIO_HAL_CTRL_START);

    // pipeline
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    self->pipeline = audio_pipeline_init(&pipeline_cfg);
    // I2S
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_READER;
    i2s_cfg.uninstall_drv = false;
    i2s_cfg.i2s_config.sample_rate = 48000;
    i2s_cfg.task_core = 1;
    self->i2s_stream = i2s_stream_init(&i2s_cfg);
    // filter
    self->filter = audio_recorder_create_filter(format);
    // encoder
    self->encoder = audio_recorder_create_encoder(format);
    // out stream
    self->out_stream = audio_recorder_create_outstream(uri);
    // register to pipeline
    audio_pipeline_register(self->pipeline, self->i2s_stream, "i2s");
    if (self->filter) {
        audio_pipeline_register(self->pipeline, self->filter, "filter");
    }
    if (self->encoder) {
        audio_pipeline_register(self->pipeline, self->encoder, "encoder");
    }
    audio_pipeline_register(self->pipeline, self->out_stream, "out");
    if (format == WAV) {
        audio_element_info_t out_stream_info;
        audio_element_getinfo(self->out_stream, &out_stream_info);
        out_stream_info.sample_rates = 16000;
        out_stream_info.channels = 1;
        audio_element_setinfo(self->out_stream, &out_stream_info);
    }
    // link
    if (self->filter && self->encoder) {
        const char *link_tag[4] = {"i2s", "filter", "encoder", "out"};
        audio_pipeline_link(self->pipeline, &link_tag[0], 4);
    } else if (self->filter) {
        const char *link_tag[3] = {"i2s", "filter", "out"};
        audio_pipeline_link(self->pipeline, &link_tag[0], 3);
    } else if (self->encoder) {
        const char *link_tag[3] = {"i2s", "encoder", "out"};
        audio_pipeline_link(self->pipeline, &link_tag[0], 3);
    } else {
        const char *link_tag[2] = {"i2s", "out"};
        audio_pipeline_link(self->pipeline, &link_tag[0], 2);
    }
}

static void audio_recorder_maxtime_cb(void *arg)
{
    audio_recorder_stop(arg);
    audio_recorder_obj_t *self = (audio_recorder_obj_t *)arg;
    if (self->end_cb != mp_const_none) {
        mp_sched_schedule(self->end_cb, self);
    }
}

STATIC mp_obj_t audio_recorder_start(mp_uint_t n_args, const mp_obj_t *args_in, mp_map_t *kw_args)
{
    enum {
        ARG_uri,
        ARG_format,
        ARG_maxtime,
        ARG_endcb
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_uri, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_format, MP_ARG_INT, { .u_int = PCM } },
        { MP_QSTR_maxtime, MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_endcb, MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    audio_recorder_obj_t *self = args_in[0];
    mp_arg_parse_all(n_args - 1, args_in + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (self->pipeline != NULL) {
        return mp_obj_new_bool(false);
    }

    audio_recorder_create(self, mp_obj_str_get_str(args[ARG_uri].u_obj), args[ARG_format].u_int);
    if (audio_pipeline_run(self->pipeline) == ESP_OK) {
        if (args[ARG_maxtime].u_int > 0) {
            esp_timer_create_args_t timer_conf = {
                .callback = &audio_recorder_maxtime_cb,
                .name = "maxtime",
                .arg = self,
            };
            esp_timer_create(&timer_conf, &self->timer);
            esp_timer_start_once(self->timer, args[ARG_maxtime].u_int * 1000000);
            self->end_cb = args[ARG_endcb].u_obj;
        }
        return mp_obj_new_bool(true);
    } else {
        return mp_obj_new_bool(false);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(audio_recorder_start_obj, 1, audio_recorder_start);

STATIC mp_obj_t audio_recorder_stop(mp_obj_t self_in)
{
    audio_recorder_obj_t *self = self_in;

    if (self->timer) {
        esp_timer_stop(self->timer);
        esp_timer_delete(self->timer);
        self->timer = NULL;
    }
    if (self->pipeline != NULL) {
        audio_pipeline_stop(self->pipeline);
        audio_pipeline_wait_for_stop(self->pipeline);
        audio_pipeline_deinit(self->pipeline);
    } else {
        return mp_obj_new_bool(false);
    }

    self->i2s_stream = NULL;
    self->filter = NULL;
    self->encoder = NULL;
    self->out_stream = NULL;
    self->pipeline = NULL;

    return mp_obj_new_bool(true);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_recorder_stop_obj, audio_recorder_stop);

STATIC mp_obj_t audio_recorder_is_running(mp_obj_t self_in)
{
    audio_recorder_obj_t *self = self_in;
    return mp_obj_new_bool(self->pipeline != NULL);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_recorder_is_running_obj, audio_recorder_is_running);

STATIC const mp_rom_map_elem_t recorder_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&audio_recorder_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&audio_recorder_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_running), MP_ROM_PTR(&audio_recorder_is_running_obj) },
    { MP_ROM_QSTR(MP_QSTR_PCM), MP_ROM_INT(PCM) },
    { MP_ROM_QSTR(MP_QSTR_AMR), MP_ROM_INT(AMR) },
    { MP_ROM_QSTR(MP_QSTR_WAV), MP_ROM_INT(WAV) },
    { MP_ROM_QSTR(MP_QSTR_MP3), MP_ROM_INT(MP3) },
};

STATIC MP_DEFINE_CONST_DICT(recorder_locals_dict, recorder_locals_dict_table);

const mp_obj_type_t audio_recorder_type = {
    { &mp_type_type },
    .name = MP_QSTR_Recorder,
    .make_new = audio_recorder_make_new,
    .locals_dict = (mp_obj_dict_t *)&recorder_locals_dict,
};

#endif  // MICROPY_PY_AUDIO