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
#include "amr_decoder.h"
#include "audio_hal.h"
#include "board.h"
#include "esp_audio.h"
#include "esp_log.h"
#include "http_stream.h"
#include "i2s_stream.h"
#include "mp3_decoder.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "utility.h"
#include "vfs_stream.h"
#include "wav_decoder.h"

static const char *TAG = "AUDIO_MOD";

typedef struct _audio_player_obj_t {
    mp_obj_base_t base;
    mp_obj_t callback;

    esp_audio_handle_t player;
    audio_board_handle_t board;
    esp_audio_state_t state;
} audio_player_obj_t;

STATIC const qstr player_info_fields[] = { MP_QSTR_input, MP_QSTR_codec };

STATIC const MP_DEFINE_STR_OBJ(player_info_input_obj, "http|file stream");
STATIC const MP_DEFINE_STR_OBJ(player_info_codec_obj, "mp3|amr");

STATIC MP_DEFINE_ATTRTUPLE(player_info_obj, player_info_fields, 2, (mp_obj_t)&player_info_input_obj,
                           (mp_obj_t)&player_info_codec_obj);

STATIC mp_obj_t player_info(void)
{
    return (mp_obj_t)&player_info_obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(audio_player_info_obj, player_info);

STATIC void audio_state_cb(esp_audio_state_t *state, void *ctx)
{
    audio_player_obj_t *self = (audio_player_obj_t *)ctx;
    memcpy(&self->state, state, sizeof(esp_audio_state_t));
    if (self->callback != mp_const_none) {
        mp_obj_dict_t *dict = mp_obj_new_dict(1);

        mp_obj_dict_store(dict, MP_ROM_QSTR(MP_QSTR_state), MP_OBJ_TO_PTR(mp_obj_new_int(state->status)));
        // mp_obj_dict_store(dict, MP_ROM_QSTR(MP_QSTR_err_msg), MP_OBJ_TO_PTR(mp_obj_new_int(state->err_msg)));
        // mp_obj_dict_store(dict, MP_ROM_QSTR(MP_QSTR_media_src), MP_OBJ_TO_PTR(mp_obj_new_int(state->media_src)));

        callback_to_python(self->callback, dict);
    }
}

STATIC int _http_stream_event_handle(http_stream_event_msg_t *msg)
{
    if (msg->event_id == HTTP_STREAM_RESOLVE_ALL_TRACKS) {
        return ESP_OK;
    }

    if (msg->event_id == HTTP_STREAM_FINISH_TRACK) {
        return http_stream_next_track(msg->el);
    }
    if (msg->event_id == HTTP_STREAM_FINISH_PLAYLIST) {
        return http_stream_restart(msg->el);
    }
    return ESP_OK;
}

STATIC esp_audio_handle_t audio_player_create(audio_player_obj_t *self)
{
    // init audio board
    audio_board_handle_t board = audio_board_init();
    audio_hal_ctrl_codec(board->audio_hal, AUDIO_HAL_CODEC_MODE_BOTH, AUDIO_HAL_CTRL_START);
    self->board = board;

    // init player
    esp_audio_cfg_t cfg = DEFAULT_ESP_AUDIO_CONFIG();
    cfg.vol_handle = board->audio_hal;
    cfg.vol_set = (audio_volume_set)audio_hal_set_volume;
    cfg.vol_get = (audio_volume_get)audio_hal_get_volume;
    cfg.resample_rate = 44100;
    cfg.prefer_type = ESP_AUDIO_PREFER_MEM;
    esp_audio_handle_t player = esp_audio_create(&cfg);

    // add input stream
    // fatfs stream
    vfs_stream_cfg_t fs_reader = VFS_STREAM_CFG_DEFAULT();
    fs_reader.type = AUDIO_STREAM_READER;
    esp_audio_input_stream_add(player, vfs_stream_init(&fs_reader));
    // http stream
    http_stream_cfg_t http_cfg = HTTP_STREAM_CFG_DEFAULT();
    http_cfg.event_handle = _http_stream_event_handle;
    http_cfg.type = AUDIO_STREAM_READER;
    http_cfg.enable_playlist_parser = true;
    audio_element_handle_t http_stream_reader = http_stream_init(&http_cfg);
    esp_audio_input_stream_add(player, http_stream_reader);

    // add decoder
    // mp3
    mp3_decoder_cfg_t mp3_dec_cfg = DEFAULT_MP3_DECODER_CONFIG();
    esp_audio_codec_lib_add(player, AUDIO_CODEC_TYPE_DECODER, mp3_decoder_init(&mp3_dec_cfg));
    // amr
    // amr_decoder_cfg_t amr_dec_cfg = DEFAULT_AMR_DECODER_CONFIG();
    // esp_audio_codec_lib_add(player, AUDIO_CODEC_TYPE_DECODER, amr_decoder_init(&amr_dec_cfg));
    // wav
    wav_decoder_cfg_t wav_dec_cfg = DEFAULT_WAV_DECODER_CONFIG();
    esp_audio_codec_lib_add(player, AUDIO_CODEC_TYPE_DECODER, wav_decoder_init(&wav_dec_cfg));

    // Create writers and add to esp_audio
    i2s_stream_cfg_t i2s_writer = I2S_STREAM_CFG_DEFAULT();
    i2s_writer.type = AUDIO_STREAM_WRITER;
    i2s_writer.i2s_config.sample_rate = 44100;
    esp_audio_output_stream_add(player, i2s_stream_init(&i2s_writer));

    return player;
}

STATIC int audio_player_destroy(audio_player_obj_t *self)
{
    esp_err_t err = ESP_FAIL;
    if (self == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = audio_board_deinit(self->board);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "audio_board_deinit Failed");
        return err;
    } else {
        self->board = NULL;
    }

    err = esp_audio_destroy(self->player);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_audio_destroy Failed");
        return err;
    } else {
        self->player = NULL;
    }

    return err;
}

STATIC mp_obj_t audio_player_open(mp_obj_t self_in)
{
    audio_player_obj_t *self = (audio_player_obj_t *)self_in;

    if (self->player == NULL) {
        self->player = audio_player_create(self);
    }

    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_player_open_obj, audio_player_open);

STATIC mp_obj_t audio_player_close(mp_obj_t self_in)
{
    audio_player_obj_t *self = (audio_player_obj_t *)self_in;

    int err = ESP_ERR_AUDIO_FAIL;

    if (self != NULL) {
        err = audio_player_destroy(self);
        self->player = NULL;
    }

    return mp_obj_new_int(err);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_player_close_obj, audio_player_close);

STATIC mp_obj_t audio_player_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    audio_player_obj_t *self = m_new_obj_with_finaliser(audio_player_obj_t);
    memset(self, 0, sizeof(audio_player_obj_t));
    self->base.type = type;
    self->callback = mp_const_none;

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t audio_player_play_helper(audio_player_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args,
                                         mp_map_t *kw_args)
{
    enum {
        ARG_uri,
        ARG_pos,
        ARG_sync,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_uri, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_pos, MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_sync, MP_ARG_BOOL, { .u_bool = false } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_uri].u_obj != mp_const_none) {
        const char *uri = mp_obj_str_get_str(args[ARG_uri].u_obj);
        int pos = args[ARG_pos].u_int;

        esp_audio_state_t state = { 0 };
        esp_audio_state_get(self->player, &state);
        if (state.status == AUDIO_STATUS_RUNNING || state.status == AUDIO_STATUS_PAUSED) {
            esp_audio_stop(self->player, TERMINATION_TYPE_NOW);
            int wait = 20;
            esp_audio_state_get(self->player, &state);
            while (wait-- && (state.status == AUDIO_STATUS_RUNNING || state.status == AUDIO_STATUS_PAUSED)) {
                vTaskDelay(pdMS_TO_TICKS(100));
                esp_audio_state_get(self->player, &state);
            }
        }

        esp_audio_callback_set(self->player, audio_state_cb, self);

        if (args[ARG_sync].u_bool == false) {
            self->state.status = AUDIO_STATUS_RUNNING;
            self->state.err_msg = ESP_ERR_AUDIO_NO_ERROR;
            return mp_obj_new_int(esp_audio_play(self->player, AUDIO_CODEC_TYPE_DECODER, uri, pos));
        } else {
            return mp_obj_new_int(esp_audio_sync_play(self->player, uri, pos));
        }
    } else {
        return mp_obj_new_int(ESP_ERR_AUDIO_INVALID_PARAMETER);
    }
}

STATIC mp_obj_t audio_player_play(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    return audio_player_play_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(audio_player_play_obj, 1, audio_player_play);

STATIC mp_obj_t audio_player_stop_helper(audio_player_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args,
                                         mp_map_t *kw_args)
{
    enum {
        ARG_termination,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_termination, MP_ARG_INT, { .u_int = TERMINATION_TYPE_NOW } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    return mp_obj_new_int(esp_audio_stop(self->player, args[ARG_termination].u_int));
}

STATIC mp_obj_t audio_player_stop(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    return audio_player_stop_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(audio_player_stop_obj, 1, audio_player_stop);

STATIC mp_obj_t audio_player_pause(mp_obj_t self_in)
{
    audio_player_obj_t *self = self_in;
    return mp_obj_new_int(esp_audio_pause(self->player));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_player_pause_obj, audio_player_pause);

STATIC mp_obj_t audio_player_resume(mp_obj_t self_in)
{
    audio_player_obj_t *self = self_in;
    return mp_obj_new_int(esp_audio_resume(self->player));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_player_resume_obj, audio_player_resume);

STATIC mp_obj_t audio_player_vol_helper(audio_player_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args,
                                        mp_map_t *kw_args)
{
    enum {
        ARG_vol,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_vol, MP_ARG_INT, { .u_int = 0xffff } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_vol].u_int == 0xffff) {
        int vol = 0;
        esp_audio_vol_get(self->player, &vol);
        return mp_obj_new_int(vol);
    } else {
        if (args[ARG_vol].u_int >= 0 && args[ARG_vol].u_int <= 100) {
            return mp_obj_new_int(esp_audio_vol_set(self->player, args[ARG_vol].u_int));
        } else {
            return mp_obj_new_int(ESP_ERR_AUDIO_INVALID_PARAMETER);
        }
    }
}

STATIC mp_obj_t audio_player_vol(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    return audio_player_vol_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(audio_player_vol_obj, 1, audio_player_vol);

STATIC mp_obj_t audio_player_get_vol(mp_obj_t self_in)
{
    audio_player_obj_t *self = self_in;
    int vol = 0;
    esp_audio_vol_get(self->player, &vol);
    return mp_obj_new_int(vol);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_player_get_vol_obj, audio_player_get_vol);

STATIC mp_obj_t audio_player_set_vol(mp_obj_t self_in, mp_obj_t vol)
{
    audio_player_obj_t *self = self_in;
    int volume = mp_obj_get_int(vol);
    return mp_obj_new_int(esp_audio_vol_set(self->player, volume));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(audio_player_set_vol_obj, audio_player_set_vol);

STATIC mp_obj_t audio_player_state(mp_obj_t self_in)
{
    audio_player_obj_t *self = self_in;
    mp_obj_dict_t *dict = mp_obj_new_dict(3);

    return mp_obj_new_int(self->state.status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_player_state_obj, audio_player_state);

STATIC mp_obj_t audio_player_time(mp_obj_t self_in)
{
    audio_player_obj_t *self = self_in;
    int time = -1;
    int err = esp_audio_time_get(self->player, &time);
    if (err == ESP_ERR_AUDIO_NO_ERROR) {
        return mp_obj_new_int(time);
    } else {
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_player_time_obj, audio_player_time);

STATIC mp_obj_t audio_player_duration(mp_obj_t self_in)
{
    audio_player_obj_t *self = self_in;
    int duration = 0;
    int err = esp_audio_duration_get(self->player, &duration);
    if (err == ESP_ERR_AUDIO_NO_ERROR) {
        return mp_obj_new_int(duration);
    } else {
        return mp_obj_new_int(-err);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_player_duration_obj, audio_player_duration);

STATIC mp_obj_t audio_player_on(mp_obj_t self_in, mp_obj_t callback_in)
{
    audio_player_obj_t *self = self_in;
    if (self == NULL) {
        return mp_obj_new_int(-ESP_ERR_AUDIO_NOT_READY);
    } else {
        self->callback = callback_in;
        return mp_obj_new_int(ESP_ERR_AUDIO_NO_ERROR);
    };
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(audio_player_on_obj, audio_player_on);

STATIC const mp_rom_map_elem_t player_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&audio_player_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&audio_player_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&audio_player_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&audio_player_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&audio_player_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&audio_player_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&audio_player_resume_obj) },
    { MP_ROM_QSTR(MP_QSTR_vol), MP_ROM_PTR(&audio_player_vol_obj) },
    { MP_ROM_QSTR(MP_QSTR_getVolume), MP_ROM_PTR(&audio_player_get_vol_obj) },
    { MP_ROM_QSTR(MP_QSTR_setVolume), MP_ROM_PTR(&audio_player_set_vol_obj) },
    { MP_ROM_QSTR(MP_QSTR_getState), MP_ROM_PTR(&audio_player_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_getPosition), MP_ROM_PTR(&audio_player_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_getDuration), MP_ROM_PTR(&audio_player_duration_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&audio_player_on_obj) },

    // esp_audio_status_t
    { MP_ROM_QSTR(MP_QSTR_STATUS_UNKNOWN), MP_ROM_INT(AUDIO_STATUS_UNKNOWN) },
    { MP_ROM_QSTR(MP_QSTR_STATUS_RUNNING), MP_ROM_INT(AUDIO_STATUS_RUNNING) },
    { MP_ROM_QSTR(MP_QSTR_STATUS_PAUSED), MP_ROM_INT(AUDIO_STATUS_PAUSED) },
    { MP_ROM_QSTR(MP_QSTR_STATUS_STOPPED), MP_ROM_INT(AUDIO_STATUS_STOPPED) },
    { MP_ROM_QSTR(MP_QSTR_STATUS_FINISHED), MP_ROM_INT(AUDIO_STATUS_FINISHED) },
    { MP_ROM_QSTR(MP_QSTR_STATUS_ERROR), MP_ROM_INT(AUDIO_STATUS_ERROR) },

    // audio_termination_type
    { MP_ROM_QSTR(MP_QSTR_TERMINATION_NOW), MP_ROM_INT(TERMINATION_TYPE_NOW) },
    { MP_ROM_QSTR(MP_QSTR_TERMINATION_DONE), MP_ROM_INT(TERMINATION_TYPE_DONE) },
};

STATIC MP_DEFINE_CONST_DICT(player_locals_dict, player_locals_dict_table);

const mp_obj_type_t audio_player_type = {
    { &mp_type_type },
    .name = MP_QSTR_Player,
    .make_new = audio_player_make_new,
    .locals_dict = (mp_obj_dict_t *)&player_locals_dict,
};

#endif  // MICROPY_PY_AUDIO