#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if MICROPY_PY_AUDIO

#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ulog/ulog.h"
#include "utility.h"
#include "uvoice_init.h"
#include "uvoice_player.h"
#include "uvoice_event.h"
#include "uvoice_types.h"

#define LOG_TAG "UVOICE_PLAYER"

#define PLAYER_CHECK_PARAMS()                                                  \
    uvoice_player_obj_t *self = (uvoice_player_obj_t *)MP_OBJ_TO_PTR(self_in); \
    do {                                                                       \
        if (self == NULL || self->player_obj == NULL) {                        \
            mp_raise_OSError(EINVAL);                                          \
            return mp_const_none;                                              \
        }                                                                      \
    } while (0)

#define PLAYER_CHECK_RESULT()                                                            \
    do {                                                                                 \
        if (ret != 0) {                                                                  \
            LOGE(LOG_TAG, "%s, %d, player failed with ret=%d", __func__, __LINE__, ret); \
            return MP_OBJ_NEW_SMALL_INT(ret);                                            \
        }                                                                                \
    } while (0)

extern const mp_obj_type_t uvoice_player_type;

// this is the actual C-structure for our new object
typedef struct {
    // base represents some basic information, like type
    mp_obj_base_t base;

    // a member created by us
    char *modName;
    uvoice_player_t *player_obj;
    int list_playing;
    int state;
    mp_obj_t callback;
} uvoice_player_obj_t;

enum {
    AUDIOPLAYER_STAT_STOP = 0,
    AUDIOPLAYER_STAT_PAUSED = 1,
    AUDIOPLAYER_STAT_PLAYING = 2,
    AUDIOPLAYER_STAT_ERROR = 5,
};

void uvoice_player_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    uvoice_player_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "modName(%s)", self->modName);
}

STATIC mp_obj_t uvoice_player_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    uvoice_player_obj_t *uvocie_player_com_obj = m_new_obj(uvoice_player_obj_t);
    if (!uvocie_player_com_obj) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Memory malloc failed"));
    }
    memset(uvocie_player_com_obj, 0, sizeof(uvoice_player_obj_t));

    uvocie_player_com_obj->base.type = &uvoice_player_type;
    uvocie_player_com_obj->modName = "uvoice-player";

    return MP_OBJ_FROM_PTR(uvocie_player_com_obj);
}

STATIC mp_obj_t uvoice_open(mp_obj_t self_in)
{
    uvoice_player_obj_t *self = (uvoice_player_obj_t *)MP_OBJ_TO_PTR(self_in);

    if (self == NULL) {
        mp_raise_OSError(EINVAL);
    }

    if (self->player_obj == NULL) {
        self->player_obj = uvoice_player_create();
        if (self->player_obj == NULL) {
            return MP_OBJ_NEW_SMALL_INT(-MP_ENODEV);
        }
    }
    return MP_OBJ_NEW_SMALL_INT(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_open_obj, uvoice_open);

STATIC mp_obj_t uvoice_close(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t ret = 0;

    if (self->player_obj != NULL) {
        self->player_obj->stop();
        ret = uvoice_player_release(self->player_obj);
        self->player_obj = NULL;
    }

    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_close_obj, uvoice_close);

STATIC mp_obj_t uvoice_release(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t ret = -1;
    if (self->player_obj != NULL) {
        self->player_obj->stop();
        ret = uvoice_player_release(self->player_obj);
        self->player_obj = NULL;
    }
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_release_obj, uvoice_release);

STATIC mp_obj_t uvoice_play(size_t n_args, const mp_obj_t *args)
{
    mp_int_t ret = 0;

    if (n_args < 2) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    mp_obj_t self_in = args[0];
    PLAYER_CHECK_PARAMS();

    if (get_uvoice_state() == false) {
        LOGE(LOG_TAG, "snd card not inited");
        return MP_OBJ_NEW_SMALL_INT(-MP_EPERM);
    }

    const char *source = mp_obj_str_get_str(args[1]);
    ret = self->player_obj->set_source(source);
    PLAYER_CHECK_RESULT();

    ret = self->player_obj->start();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(uvoice_play_obj, 2, uvoice_play);

STATIC mp_obj_t uvoice_stop(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t ret = self->player_obj->stop();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_stop_obj, uvoice_stop);

STATIC mp_obj_t uvoice_pause(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t ret = self->player_obj->pause();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_pause_obj, uvoice_pause);

STATIC mp_obj_t uvoice_resume(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t ret = self->player_obj->resume();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_resume_obj, uvoice_resume);

STATIC mp_obj_t uvoice_complete(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t ret = self->player_obj->complete();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_complete_obj, uvoice_complete);

STATIC mp_obj_t uvoice_stop_async(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t ret = self->player_obj->stop_async();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_stop_async_obj, uvoice_stop_async);

STATIC mp_obj_t uvoice_pause_async(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t ret = self->player_obj->pause_async();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_pause_async_obj, uvoice_pause_async);

STATIC mp_obj_t uvoice_resume_async(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t ret = self->player_obj->resume_async();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_resume_async_obj, uvoice_resume_async);

STATIC mp_obj_t uvoice_set_source(mp_obj_t self_in, mp_obj_t source_in)
{
    PLAYER_CHECK_PARAMS();

    char *source = mp_obj_str_get_str(source_in);
    mp_int_t ret = self->player_obj->set_source(source);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_set_source_obj, uvoice_set_source);

STATIC mp_obj_t uvoice_clr_source(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t ret = self->player_obj->clr_source();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_clr_source_obj, uvoice_clr_source);

STATIC mp_obj_t uvoice_set_stream(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 4) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    mp_obj_t self_in = args[0];
    PLAYER_CHECK_PARAMS();

    media_format_t format = (media_format_t)mp_obj_get_int(args[1]);
    int cache_enable = mp_obj_get_int(args[2]);
    int cache_size = mp_obj_get_int(args[3]);
    mp_int_t ret = self->player_obj->set_stream(format, cache_enable, cache_size);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(uvoice_set_stream_obj, 4, uvoice_set_stream);

STATIC mp_obj_t uvoice_put_stream(mp_obj_t self_in, mp_obj_t buffer_in, mp_obj_t nbytes_in)
{
    PLAYER_CHECK_PARAMS();

    int nbytes = mp_obj_get_int(nbytes_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer_in, &bufinfo, MP_BUFFER_READ);
    const uint8_t *data = (const uint8_t *)bufinfo.buf;
    mp_int_t ret = self->player_obj->put_stream(data, bufinfo.len);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(uvoice_put_stream_obj, uvoice_put_stream);

STATIC mp_obj_t uvoice_clr_stream(mp_obj_t self_in, mp_obj_t immediate_in)
{
    PLAYER_CHECK_PARAMS();

    int immediate = mp_obj_get_int(immediate_in);
    mp_int_t ret = self->player_obj->clr_stream(immediate);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_clr_stream_obj, uvoice_clr_stream);

STATIC mp_obj_t uvoice_set_pcminfo(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 5) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    mp_obj_t self_in = args[0];
    PLAYER_CHECK_PARAMS();

    int rate = mp_obj_get_int(args[1]);
    int channels = mp_obj_get_int(args[2]);
    int bits = mp_obj_get_int(args[3]);
    int frames = mp_obj_get_int(args[4]);

    mp_int_t ret = self->player_obj->set_pcminfo(rate, channels, bits, frames);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(uvoice_set_pcminfo_obj, 5, uvoice_set_pcminfo);

STATIC mp_obj_t uvoice_getDuration(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    int duration = -1;
    int ret = self->player_obj->get_duration(&duration);
    PLAYER_CHECK_RESULT();

    return MP_OBJ_NEW_SMALL_INT(duration);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_getDuration_obj, uvoice_getDuration);

STATIC mp_obj_t uvoice_getPosition(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    int position = -1;
    int ret = self->player_obj->get_position(&position);
    PLAYER_CHECK_RESULT();

    return MP_OBJ_NEW_SMALL_INT(position);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_getPosition_obj, uvoice_getPosition);

STATIC mp_obj_t uvoice_setVolume(mp_obj_t self_in, mp_obj_t volume_in)
{
    PLAYER_CHECK_PARAMS();

    int volume = mp_obj_get_int(volume_in);
    mp_int_t ret = self->player_obj->set_volume(volume);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_setVolume_obj, uvoice_setVolume);

STATIC mp_obj_t uvoice_getVolume(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    int volume = -1;
    int ret = self->player_obj->get_volume(&volume);
    PLAYER_CHECK_RESULT();

    return MP_OBJ_NEW_SMALL_INT(volume);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_getVolume_obj, uvoice_getVolume);

static void uvoice_player_state_notify(void *arg)
{
    uvoice_player_obj_t *audioplayer = (uvoice_player_obj_t *)arg;

    mp_obj_t state;
    state = mp_obj_new_dict(1);
    mp_obj_dict_store(MP_OBJ_FROM_PTR(state), mp_obj_new_str("state", 5),
                              mp_obj_new_int(audioplayer->state));

    if (audioplayer->callback != mp_const_none) {
        callback_to_python(audioplayer->callback, state);
    }
}

static void uvoice_player_state_cb(uvoice_event_t *event, void *data)
{
    uvoice_player_obj_t *audioplayer = (uvoice_player_obj_t *)data;
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
        default:
            return;
        }
    }

    if (audioplayer->state == audioplayer_state) {
        return;
    }
    audioplayer->state = audioplayer_state;

    py_task_schedule_call(uvoice_player_state_notify, audioplayer);
}

STATIC mp_obj_t uvoice_state_on(size_t n_args, const mp_obj_t *args)
{
    int ret = 0;

    if (n_args < 2) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    mp_obj_t self_in = args[0];
    PLAYER_CHECK_PARAMS();

    self->callback = args[1];

    ret = uvoice_event_register(UVOICE_EV_PLAYER, uvoice_player_state_cb, (void *)self);
    if (ret) {
        LOGE(LOG_TAG, "register event fail");
        return MP_OBJ_NEW_SMALL_INT(ret);
    }

    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(uvoice_state_on_obj, 2, uvoice_state_on);

STATIC mp_obj_t uvoice_volume_range(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    int max = 100;
    int min = 0;
    mp_int_t ret = self->player_obj->volume_range(&max, &min);

    // dict solution
    mp_obj_t volume_dict = mp_obj_new_dict(2);
    mp_obj_dict_store(volume_dict, MP_OBJ_NEW_QSTR(qstr_from_str("vol_max")), MP_OBJ_NEW_SMALL_INT(max));
    mp_obj_dict_store(volume_dict, MP_OBJ_NEW_QSTR(qstr_from_str("vol_min")), MP_OBJ_NEW_SMALL_INT(min));
    return volume_dict;

    // mp_obj_t volume[2];
    // volume[0] = MP_OBJ_NEW_SMALL_INT(max);
    // volume[1] = MP_OBJ_NEW_SMALL_INT(min);
    // mp_obj_t volume_tuple = mp_obj_new_tuple(2, volume);
    // return volume_tuple;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_volume_range_obj, uvoice_volume_range);

STATIC mp_obj_t uvoice_seekTo(mp_obj_t self_in, mp_obj_t second_in)
{
    PLAYER_CHECK_PARAMS();

    int second = mp_obj_get_int(second_in);
    mp_int_t ret = self->player_obj->seek(second);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_seekTo_obj, uvoice_seekTo);

STATIC mp_obj_t uvoice_playback(mp_obj_t self_in, mp_obj_t source_in)
{
    PLAYER_CHECK_PARAMS();

    char *source = (char *)mp_obj_str_get_str(source_in);
    mp_int_t ret = self->player_obj->playback(source);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_playback_obj, uvoice_playback);

STATIC mp_obj_t uvoice_wait_complete(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t ret = self->player_obj->wait_complete();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_wait_complete_obj, uvoice_wait_complete);

STATIC mp_obj_t uvoice_download(mp_obj_t self_in, mp_obj_t name_in)
{
    PLAYER_CHECK_PARAMS();
    char *name = (char *)mp_obj_str_get_str(name_in);
    mp_int_t ret = self->player_obj->download(name);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_download_obj, uvoice_download);

STATIC mp_obj_t uvoice_download_abort(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t ret = self->player_obj->download_abort();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_download_abort_obj, uvoice_download_abort);

STATIC mp_obj_t uvoice_cache_config(mp_obj_t self_in, mp_obj_t config_dict)
{
    PLAYER_CHECK_PARAMS();

    cache_config_t config = { 0 };
    config.place = get_int_from_dict(config_dict, "place");
    config.mem_size = get_int_from_dict(config_dict, "mem_size");
    const char *file_path = get_str_from_dict(config_dict, "file_path");
    strncpy(config.file_path, file_path, strlen(file_path));
    mp_int_t ret = self->player_obj->cache_config(&config);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_cache_config_obj, uvoice_cache_config);

STATIC mp_obj_t uvoice_set_fade(mp_obj_t self_in, mp_obj_t out_period_in, mp_obj_t in_period_in)
{
    PLAYER_CHECK_PARAMS();

    int out_period = mp_obj_get_int(out_period_in);
    int in_period = mp_obj_get_int(in_period_in);
    mp_int_t ret = self->player_obj->set_fade(out_period, in_period);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(uvoice_set_fade_obj, uvoice_set_fade);

STATIC mp_obj_t uvoice_set_format(mp_obj_t self_in, mp_obj_t format_in)
{
    PLAYER_CHECK_PARAMS();

    media_format_t format = (media_format_t)mp_obj_get_int(format_in);
    mp_int_t ret = self->player_obj->set_format(format);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_set_format_obj, uvoice_set_format);

STATIC mp_obj_t uvoice_set_standby(mp_obj_t self_in, mp_obj_t msec_in)
{
    PLAYER_CHECK_PARAMS();

    int msec = mp_obj_get_int(msec_in);
    mp_int_t ret = self->player_obj->set_standby(msec);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_set_standby_obj, uvoice_set_standby);

STATIC mp_obj_t uvoice_eq_enable(mp_obj_t self_in, mp_obj_t enable_in)
{
    PLAYER_CHECK_PARAMS();

    int enable = mp_obj_get_int(enable_in);
    mp_int_t ret = self->player_obj->eq_enable(enable);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_eq_enable_obj, uvoice_eq_enable);

STATIC mp_obj_t uvoice_state_dump(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t ret = self->player_obj->state_dump();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_state_dump_obj, uvoice_state_dump);

STATIC mp_obj_t uvoice_pcmdump_enable(mp_obj_t self_in, mp_obj_t enable_in)
{
    PLAYER_CHECK_PARAMS();

    int enable = mp_obj_get_int(enable_in);
    mp_int_t ret = self->player_obj->pcmdump_enable(enable);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_pcmdump_enable_obj, uvoice_pcmdump_enable);

STATIC mp_obj_t uvoice_getState(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    player_state_t state = -1;
    mp_int_t ret = self->player_obj->get_state(&state);
    if (ret == 0) {
        return MP_OBJ_NEW_SMALL_INT(state);
    } else {
        return MP_OBJ_NEW_SMALL_INT(ret);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_getState_obj, uvoice_getState);

STATIC mp_obj_t uvoice_get_delay(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    int delay_ms;
    self->player_obj->get_delay(&delay_ms);
    return MP_OBJ_NEW_SMALL_INT(delay_ms);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_get_delay_obj, uvoice_get_delay);

STATIC mp_obj_t uvoice_get_mediainfo(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    media_info_t info = { 0 };
    mp_int_t ret = self->player_obj->get_mediainfo(&info);
    if (ret != 0) {
        LOGE(LOG_TAG, "failed to get mediainfo");
        return mp_const_none;
    }

    // LOGD(LOG_TAG,
    //      "name=%s, author=%s, album=%s, year=%s, valid=%d, type=%d, "
    //      "bitrate=%d, media_size=%d, duration=%d, len = %d\n\n",
    //      info.name, info.author, info.album, info.year, info.valid,
    //      info.type, info.bitrate, info.media_size, info.duration,
    //      strlen(info.name));

    mp_obj_t mediainfo_dict = mp_obj_new_dict(9);
    mp_obj_dict_store(mediainfo_dict, MP_OBJ_NEW_QSTR(qstr_from_str("name")),
                      mp_obj_new_str(info.name, strlen(info.name)));
    mp_obj_dict_store(mediainfo_dict, MP_OBJ_NEW_QSTR(qstr_from_str("author")),
                      mp_obj_new_str(info.author, strlen(info.author)));
    mp_obj_dict_store(mediainfo_dict, MP_OBJ_NEW_QSTR(qstr_from_str("album")),
                      mp_obj_new_str(info.album, strlen(info.album)));
    mp_obj_dict_store(mediainfo_dict, MP_OBJ_NEW_QSTR(qstr_from_str("year")),
                      mp_obj_new_str(info.year, strlen(info.year)));
    mp_obj_dict_store(mediainfo_dict, MP_OBJ_NEW_QSTR(qstr_from_str("valid")), MP_OBJ_NEW_SMALL_INT(info.valid));
    mp_obj_dict_store(mediainfo_dict, MP_OBJ_NEW_QSTR(qstr_from_str("type")), MP_OBJ_NEW_SMALL_INT(info.type));
    mp_obj_dict_store(mediainfo_dict, MP_OBJ_NEW_QSTR(qstr_from_str("bitrate")), MP_OBJ_NEW_SMALL_INT(info.bitrate));
    mp_obj_dict_store(mediainfo_dict, MP_OBJ_NEW_QSTR(qstr_from_str("media_size")),
                      MP_OBJ_NEW_SMALL_INT(info.media_size));
    mp_obj_dict_store(mediainfo_dict, MP_OBJ_NEW_QSTR(qstr_from_str("duration")), MP_OBJ_NEW_SMALL_INT(info.duration));

    return mediainfo_dict;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_get_mediainfo_obj, uvoice_get_mediainfo);

STATIC mp_obj_t uvoice_get_cacheinfo(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    int cache_size = -1;
    self->player_obj->get_cacheinfo(&cache_size);
    return MP_OBJ_NEW_SMALL_INT(cache_size);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_get_cacheinfo_obj, uvoice_get_cacheinfo);

STATIC mp_obj_t uvoice_format_support(mp_obj_t self_in, mp_obj_t format_in)
{
    PLAYER_CHECK_PARAMS();

    media_format_t format = (media_format_t)mp_obj_get_int(format_in);
    mp_int_t ret = self->player_obj->format_support(format);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_format_support_obj, uvoice_format_support);

STATIC const mp_rom_map_elem_t uvoice_module_player_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&uvoice_release_obj) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&uvoice_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&uvoice_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&uvoice_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&uvoice_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&uvoice_resume_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&uvoice_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_setVolume), MP_ROM_PTR(&uvoice_setVolume_obj) },
    { MP_ROM_QSTR(MP_QSTR_getVolume), MP_ROM_PTR(&uvoice_getVolume_obj) },
    { MP_ROM_QSTR(MP_QSTR_seekTo), MP_ROM_PTR(&uvoice_seekTo_obj) },
    { MP_ROM_QSTR(MP_QSTR_getState), MP_ROM_PTR(&uvoice_getState_obj) },
    { MP_ROM_QSTR(MP_QSTR_getPosition), MP_ROM_PTR(&uvoice_getPosition_obj) },
    { MP_ROM_QSTR(MP_QSTR_getDuration), MP_ROM_PTR(&uvoice_getDuration_obj) },
    { MP_ROM_QSTR(MP_QSTR_waitComplete), MP_ROM_PTR(&uvoice_wait_complete_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&uvoice_state_on_obj) },
    /* audio player state */
    { MP_ROM_QSTR(MP_QSTR_AUDIOPLAYER_STAT_STOP), MP_ROM_INT(AUDIOPLAYER_STAT_STOP) },
    { MP_ROM_QSTR(MP_QSTR_AUDIOPLAYER_STAT_PAUSED), MP_ROM_INT(AUDIOPLAYER_STAT_PAUSED) },
    { MP_ROM_QSTR(MP_QSTR_AUDIOPLAYER_STAT_PLAYING), MP_ROM_INT(AUDIOPLAYER_STAT_PLAYING) },
    { MP_ROM_QSTR(MP_QSTR_AUDIOPLAYER_STAT_ERROR), MP_ROM_INT(AUDIOPLAYER_STAT_ERROR) },
};
STATIC MP_DEFINE_CONST_DICT(uvoice_module_player_globals, uvoice_module_player_globals_table);

const mp_obj_type_t uvoice_player_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_Player,
    .print = uvoice_player_print,
    .make_new = uvoice_player_new,
    .locals_dict = (mp_obj_dict_t *)&uvoice_module_player_globals,
};

#endif // MICROPY_PY_AUDIO
