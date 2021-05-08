#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HaasLog.h"
#include "k_api.h"
#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "uvoice_types.h"
#include "uvoice_init.h"
#include "uvoice_player.h"

#define PLAYER_CHECK_PARAMS()                                                  \
    uvocie_player_obj_t *self = (uvocie_player_obj_t *)MP_OBJ_TO_PTR(self_in); \
    do                                                                         \
    {                                                                          \
        if (self == NULL || self->player_obj == NULL)                          \
        {                                                                      \
            mp_raise_OSError(EINVAL);                                          \
            return mp_const_none;                                              \
        }                                                                      \
    } while (0)

extern const mp_obj_type_t uvoice_player_type;

extern bool g_is_uvoice_inited;
extern const char *get_str_from_dict(mp_obj_t dict, const char *key);
extern int get_int_from_dict(mp_obj_t dict, const char *key);

// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t base;

    // a member created by us
    char *modName;
    uvoice_player_t *player_obj;
} uvocie_player_obj_t;

void uvoice_player_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    uvocie_player_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "modName(%s)", self->modName);
}

STATIC mp_obj_t uvoice_player_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    uvocie_player_obj_t *uvocie_player_com_obj = m_new_obj(uvocie_player_obj_t);
    if (!uvocie_player_com_obj) {
        mp_raise_OSError(ENOMEM);
        return mp_const_none;
    }
    memset(uvocie_player_com_obj, 0, sizeof(uvocie_player_obj_t));

    uvocie_player_com_obj->base.type = &uvoice_player_type;
    uvocie_player_com_obj->modName = "uvoice-player";

    return MP_OBJ_FROM_PTR(uvocie_player_com_obj);
}

STATIC mp_obj_t uvoice_create(mp_obj_t self_in)
{
    uvocie_player_obj_t *self = (uvocie_player_obj_t *)MP_OBJ_TO_PTR(self_in);
    if (self == NULL) {
        LOG_E("uvocie_player_obj_t NULL");
        return mp_const_none;
    }

    if (g_is_uvoice_inited == false) {
        mp_raise_OSError(ENXIO);
        return mp_const_none;
    }

    if (self->player_obj == NULL) {
        self->player_obj = uvoice_player_create();
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_create_obj, uvoice_create);

STATIC mp_obj_t uvoice_release(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t status = -1;
    if (self->player_obj != NULL)
    {
        status = uvoice_player_release(self->player_obj);
        self->player_obj = NULL;
    }
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_release_obj, uvoice_release);

STATIC mp_obj_t uvoice_start(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t status = self->player_obj->start();
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_start_obj, uvoice_start);

STATIC mp_obj_t uvoice_stop(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t status = self->player_obj->stop();
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_stop_obj, uvoice_stop);

STATIC mp_obj_t uvoice_pause(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t status = self->player_obj->pause();
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_pause_obj, uvoice_pause);

STATIC mp_obj_t uvoice_resume(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t status = self->player_obj->resume();
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_resume_obj, uvoice_resume);

STATIC mp_obj_t uvoice_complete(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t status = self->player_obj->complete();
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_complete_obj, uvoice_complete);

STATIC mp_obj_t uvoice_stop_async(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t status = self->player_obj->stop_async();
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_stop_async_obj, uvoice_stop_async);

STATIC mp_obj_t uvoice_pause_async(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t status = self->player_obj->pause_async();
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_pause_async_obj, uvoice_pause_async);

STATIC mp_obj_t uvoice_resume_async(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t status = self->player_obj->resume_async();
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_resume_async_obj, uvoice_resume_async);

STATIC mp_obj_t uvoice_set_source(mp_obj_t self_in, mp_obj_t source_in)
{
    PLAYER_CHECK_PARAMS();

    char *source = mp_obj_str_get_str(source_in);
    LOG_D("source = %s", source);

    mp_int_t status = self->player_obj->set_source(source);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_set_source_obj, uvoice_set_source);

STATIC mp_obj_t uvoice_clr_source(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t status = self->player_obj->clr_source();
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_clr_source_obj, uvoice_clr_source);

STATIC mp_obj_t uvoice_set_stream(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 4) {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    mp_obj_t self_in = args[0];
    PLAYER_CHECK_PARAMS();

    media_format_t format = (media_format_t)mp_obj_get_int(args[1]);
    int cache_enable = mp_obj_get_int(args[2]);
    int cache_size = mp_obj_get_int(args[3]);
    LOG_D("format=%d, cache_enable = %d, cache_size = %d", format, cache_enable, cache_size);

    mp_int_t status = self->player_obj->set_stream(format, cache_enable, cache_size);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(uvoice_set_stream_obj, 4, uvoice_set_stream);

STATIC mp_obj_t uvoice_put_stream(mp_obj_t self_in, mp_obj_t buffer_in, mp_obj_t nbytes_in)
{
    PLAYER_CHECK_PARAMS();

    int nbytes = mp_obj_get_int(nbytes_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer_in, &bufinfo, MP_BUFFER_READ);
    const uint8_t *data = (const uint8_t *)bufinfo.buf;

    LOG_D("bufinfo.buf= [%d, %d, %d, %d], bufinfo.len = %d, nbytes = %d", data[0], data[1], data[2], data[3],
          bufinfo.len, nbytes);

    mp_int_t status = self->player_obj->put_stream(data, bufinfo.len);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(uvoice_put_stream_obj, uvoice_put_stream);

STATIC mp_obj_t uvoice_clr_stream(mp_obj_t self_in, mp_obj_t immediate_in)
{
    PLAYER_CHECK_PARAMS();

    int immediate = mp_obj_get_int(immediate_in);
    LOG_D("immediate = %d", immediate);

    mp_int_t status = self->player_obj->clr_stream(immediate);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_clr_stream_obj, uvoice_clr_stream);

STATIC mp_obj_t uvoice_set_pcminfo(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 5) {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    mp_obj_t self_in = args[0];
    PLAYER_CHECK_PARAMS();

    int rate = mp_obj_get_int(args[1]);
    int channels = mp_obj_get_int(args[2]);
    int bits = mp_obj_get_int(args[3]);
    int frames = mp_obj_get_int(args[4]);

    LOG_D("rate=%d, channels=%d, bits=%d, frames=%d\n", rate, channels, bits, frames);

    mp_int_t status = self->player_obj->set_pcminfo(rate, channels, bits, frames);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(uvoice_set_pcminfo_obj, 5, uvoice_set_pcminfo);

STATIC mp_obj_t uvoice_get_duration(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    int duration = -1;
    self->player_obj->get_duration(&duration);

    return mp_obj_new_int(duration);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_get_duration_obj, uvoice_get_duration);

STATIC mp_obj_t uvoice_get_position(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    int position = -1;
    self->player_obj->get_position(&position);
    return mp_obj_new_int(position);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_get_position_obj, uvoice_get_position);

STATIC mp_obj_t uvoice_set_volume(mp_obj_t self_in, mp_obj_t volume_in)
{
    PLAYER_CHECK_PARAMS();

    int volume = mp_obj_get_int(volume_in);
    mp_int_t status = self->player_obj->set_volume(volume);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_set_volume_obj, uvoice_set_volume);

STATIC mp_obj_t uvoice_get_volume(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    int volume = -1;
    self->player_obj->get_volume(&volume);

    return mp_obj_new_int(volume);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_get_volume_obj, uvoice_get_volume);

STATIC mp_obj_t uvoice_volume_range(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    int max = 100;
    int min = 0;
    mp_int_t status = self->player_obj->volume_range(&max, &min);

    // dict solution
    mp_obj_t volume_dict = mp_obj_new_dict(2);
    mp_obj_dict_store(volume_dict, MP_OBJ_NEW_QSTR(qstr_from_str("vol_max")), mp_obj_new_int(max));
    mp_obj_dict_store(volume_dict, MP_OBJ_NEW_QSTR(qstr_from_str("vol_min")), mp_obj_new_int(min));
    return volume_dict;

    // mp_obj_t volume[2];
    // volume[0] = mp_obj_new_int(max);
    // volume[1] = mp_obj_new_int(min);
    // mp_obj_t volume_tuple = mp_obj_new_tuple(2, volume);
    // return volume_tuple;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_volume_range_obj, uvoice_volume_range);

STATIC mp_obj_t uvoice_seek(mp_obj_t self_in, mp_obj_t second_in)
{
    PLAYER_CHECK_PARAMS();

    int second = mp_obj_get_int(second_in);
    LOG_D("second = %d", second);

    mp_int_t status = self->player_obj->seek(second);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_seek_obj, uvoice_seek);

STATIC mp_obj_t uvoice_playback(mp_obj_t self_in, mp_obj_t source_in)
{
    PLAYER_CHECK_PARAMS();

    char *source = (char*)mp_obj_str_get_str(source_in);
    LOG_D("source = %s", source);

    mp_int_t status = self->player_obj->playback(source);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_playback_obj, uvoice_playback);

STATIC mp_obj_t uvoice_wait_complete(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t status = self->player_obj->wait_complete();
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_wait_complete_obj, uvoice_wait_complete);

STATIC mp_obj_t uvoice_download(mp_obj_t self_in, mp_obj_t name_in)
{
    PLAYER_CHECK_PARAMS();
    char *name = (char *)mp_obj_str_get_str(name_in);
    LOG_D("name = %s", name);

    mp_int_t status = self->player_obj->download(name);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_download_obj, uvoice_download);

STATIC mp_obj_t uvoice_download_abort(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t status = self->player_obj->download_abort();
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_download_abort_obj, uvoice_download_abort);

STATIC mp_obj_t uvoice_cache_config(mp_obj_t self_in, mp_obj_t config_dict)
{
    PLAYER_CHECK_PARAMS();

    cache_config_t config = {0};
    config.place = get_int_from_dict(config_dict, "place");
    config.mem_size = get_int_from_dict(config_dict, "mem_size");
    const char *file_path = get_str_from_dict(config_dict, "file_path");
    strncpy(config.file_path, file_path, strlen(file_path));

    LOG_D("place = %d", config.place);
    LOG_D("mem_size = %d", config.mem_size);
    LOG_D("file_path = %s", config.file_path);

    mp_int_t status = self->player_obj->cache_config(&config);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_cache_config_obj, uvoice_cache_config);

STATIC mp_obj_t uvoice_set_fade(mp_obj_t self_in, mp_obj_t out_period_in, mp_obj_t in_period_in)
{
    PLAYER_CHECK_PARAMS();

    int out_period = mp_obj_get_int(out_period_in);
    int in_period = mp_obj_get_int(in_period_in);

    LOG_D("fade out = %d, fade in = %d\n", out_period, in_period);

    mp_int_t status = self->player_obj->set_fade(out_period, in_period);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(uvoice_set_fade_obj, uvoice_set_fade);

STATIC mp_obj_t uvoice_set_format(mp_obj_t self_in, mp_obj_t format_in)
{
    PLAYER_CHECK_PARAMS();

    media_format_t format = (media_format_t)mp_obj_get_int(format_in);
    LOG_D("format = %d\n", format);

    mp_int_t status = self->player_obj->set_format(format);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_set_format_obj, uvoice_set_format);

STATIC mp_obj_t uvoice_set_standby(mp_obj_t self_in, mp_obj_t msec_in)
{
    PLAYER_CHECK_PARAMS();

    int msec = mp_obj_get_int(msec_in);
    LOG_D("msec = %d\n", msec);

    mp_int_t status = self->player_obj->set_standby(msec);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_set_standby_obj, uvoice_set_standby);

STATIC mp_obj_t uvoice_eq_enable(mp_obj_t self_in, mp_obj_t enable_in)
{
    PLAYER_CHECK_PARAMS();

    int enable = mp_obj_get_int(enable_in);
    LOG_D("enable = %d\n", enable);

    mp_int_t status = self->player_obj->eq_enable(enable);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_eq_enable_obj, uvoice_eq_enable);

STATIC mp_obj_t uvoice_state_dump(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    mp_int_t status = self->player_obj->state_dump();
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_state_dump_obj, uvoice_state_dump);

STATIC mp_obj_t uvoice_pcmdump_enable(mp_obj_t self_in, mp_obj_t enable_in)
{
    PLAYER_CHECK_PARAMS();

    int enable = mp_obj_get_int(enable_in);
    LOG_D("enable = %d\n", enable);

    mp_int_t status = self->player_obj->pcmdump_enable(enable);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_pcmdump_enable_obj, uvoice_pcmdump_enable);

STATIC mp_obj_t uvoice_get_state(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    player_state_t state;
    self->player_obj->get_state(&state);
    return mp_obj_new_int(state);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_get_state_obj, uvoice_get_state);

STATIC mp_obj_t uvoice_get_delay(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    int delay_ms;
    self->player_obj->get_delay(&delay_ms);
    return mp_obj_new_int(delay_ms);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_get_delay_obj, uvoice_get_delay);

STATIC mp_obj_t uvoice_get_mediainfo(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    media_info_t info = {0};
    mp_int_t status = self->player_obj->get_mediainfo(&info);
    if (status != 0) {
        LOG_E("failed to get mediainfo");
        return mp_const_none;
    }

    LOG_D(
        "name=%s, author=%s, album=%s, year=%s, valid=%d, type=%d, "
        "bitrate=%d, media_size=%d, duration=%d, len = %d\n\n",
        info.name, info.author, info.album, info.year, info.valid, info.type,
        info.bitrate, info.media_size, info.duration, strlen(info.name));

    mp_obj_t mediainfo_dict = mp_obj_new_dict(9);
    mp_obj_dict_store(mediainfo_dict, MP_OBJ_NEW_QSTR(qstr_from_str("name")), mp_obj_new_str(info.name, strlen(info.name)));
    mp_obj_dict_store(mediainfo_dict, MP_OBJ_NEW_QSTR(qstr_from_str("author")), mp_obj_new_str(info.author, strlen(info.author)));
    mp_obj_dict_store(mediainfo_dict, MP_OBJ_NEW_QSTR(qstr_from_str("album")), mp_obj_new_str(info.album, strlen(info.album)));
    mp_obj_dict_store(mediainfo_dict, MP_OBJ_NEW_QSTR(qstr_from_str("year")), mp_obj_new_str(info.year, strlen(info.year)));
    mp_obj_dict_store(mediainfo_dict, MP_OBJ_NEW_QSTR(qstr_from_str("valid")), mp_obj_new_int(info.valid));
    mp_obj_dict_store(mediainfo_dict, MP_OBJ_NEW_QSTR(qstr_from_str("type")), mp_obj_new_int(info.type));
    mp_obj_dict_store(mediainfo_dict, MP_OBJ_NEW_QSTR(qstr_from_str("bitrate")), mp_obj_new_int(info.bitrate));
    mp_obj_dict_store(mediainfo_dict, MP_OBJ_NEW_QSTR(qstr_from_str("media_size")), mp_obj_new_int(info.media_size));
    mp_obj_dict_store(mediainfo_dict, MP_OBJ_NEW_QSTR(qstr_from_str("duration")), mp_obj_new_int(info.duration));

    return mediainfo_dict;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_get_mediainfo_obj, uvoice_get_mediainfo);

STATIC mp_obj_t uvoice_get_cacheinfo(mp_obj_t self_in)
{
    PLAYER_CHECK_PARAMS();

    int cache_size = -1;
    self->player_obj->get_cacheinfo(&cache_size);
    return mp_obj_new_int(cache_size);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_get_cacheinfo_obj, uvoice_get_cacheinfo);

STATIC mp_obj_t uvoice_format_support(mp_obj_t self_in, mp_obj_t format_in)
{
    PLAYER_CHECK_PARAMS();

    media_format_t format = (media_format_t)mp_obj_get_int(format_in);
    LOG_D("format = %d\n", format);

    mp_int_t status = self->player_obj->format_support(format);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_format_support_obj, uvoice_format_support);

STATIC const mp_rom_map_elem_t uvoice_module_player_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&uvoice_release_obj)},
    {MP_ROM_QSTR(MP_QSTR_create), MP_ROM_PTR(&uvoice_create_obj)},
    {MP_ROM_QSTR(MP_QSTR_release), MP_ROM_PTR(&uvoice_release_obj)},
    {MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&uvoice_start_obj)},
    {MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&uvoice_stop_obj)},
    {MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&uvoice_pause_obj)},
    {MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&uvoice_resume_obj)},
    {MP_ROM_QSTR(MP_QSTR_complete), MP_ROM_PTR(&uvoice_complete_obj)},
    {MP_ROM_QSTR(MP_QSTR_stop_async), MP_ROM_PTR(&uvoice_stop_async_obj)},
    {MP_ROM_QSTR(MP_QSTR_pause_async), MP_ROM_PTR(&uvoice_pause_async_obj)},
    {MP_ROM_QSTR(MP_QSTR_resume_async), MP_ROM_PTR(&uvoice_resume_async_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_source), MP_ROM_PTR(&uvoice_set_source_obj)},
    {MP_ROM_QSTR(MP_QSTR_clr_source), MP_ROM_PTR(&uvoice_clr_source_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_stream), MP_ROM_PTR(&uvoice_set_stream_obj)},
    {MP_ROM_QSTR(MP_QSTR_put_stream), MP_ROM_PTR(&uvoice_put_stream_obj)},
    {MP_ROM_QSTR(MP_QSTR_clr_stream), MP_ROM_PTR(&uvoice_clr_stream_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_pcminfo), MP_ROM_PTR(&uvoice_set_pcminfo_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_duration), MP_ROM_PTR(&uvoice_get_duration_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_position), MP_ROM_PTR(&uvoice_get_position_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_volume), MP_ROM_PTR(&uvoice_set_volume_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_volume), MP_ROM_PTR(&uvoice_get_volume_obj)},
    {MP_ROM_QSTR(MP_QSTR_volume_range), MP_ROM_PTR(&uvoice_volume_range_obj)},
    {MP_ROM_QSTR(MP_QSTR_seek), MP_ROM_PTR(&uvoice_seek_obj)},
    {MP_ROM_QSTR(MP_QSTR_playback), MP_ROM_PTR(&uvoice_playback_obj)},
    {MP_ROM_QSTR(MP_QSTR_wait_complete), MP_ROM_PTR(&uvoice_wait_complete_obj)},
    {MP_ROM_QSTR(MP_QSTR_download), MP_ROM_PTR(&uvoice_download_obj)},
    {MP_ROM_QSTR(MP_QSTR_download_abort), MP_ROM_PTR(&uvoice_download_abort_obj)},
    {MP_ROM_QSTR(MP_QSTR_cache_config), MP_ROM_PTR(&uvoice_cache_config_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_fade), MP_ROM_PTR(&uvoice_set_fade_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_format), MP_ROM_PTR(&uvoice_set_format_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_standby), MP_ROM_PTR(&uvoice_set_standby_obj)},
    {MP_ROM_QSTR(MP_QSTR_eq_enable), MP_ROM_PTR(&uvoice_eq_enable_obj)},
    {MP_ROM_QSTR(MP_QSTR_state_dump), MP_ROM_PTR(&uvoice_state_dump_obj)},
    {MP_ROM_QSTR(MP_QSTR_pcmdump_enable), MP_ROM_PTR(&uvoice_pcmdump_enable_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_state), MP_ROM_PTR(&uvoice_get_state_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_delay), MP_ROM_PTR(&uvoice_get_delay_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_mediainfo), MP_ROM_PTR(&uvoice_get_mediainfo_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_cacheinfo), MP_ROM_PTR(&uvoice_get_cacheinfo_obj)},
    {MP_ROM_QSTR(MP_QSTR_format_support), MP_ROM_PTR(&uvoice_format_support_obj)},
};
STATIC MP_DEFINE_CONST_DICT(uvoice_module_player_globals, uvoice_module_player_globals_table);

const mp_obj_type_t uvoice_player_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_Player,
    .print = uvoice_player_print,
    .make_new = uvoice_player_new,
    .locals_dict = (mp_obj_dict_t *)&uvoice_module_player_globals,
};
