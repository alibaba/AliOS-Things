#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if PY_BUILD_AUDIO

#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ulog/ulog.h"
#include "uvoice_init.h"
#include "uvoice_recorder.h"
#include "uvoice_types.h"

#define LOG_TAG "UVOICE_RECORDER"

extern const mp_obj_type_t uvoice_recorder_type;
extern bool g_is_uvoice_inited;

#define RECORDER_CHECK_PARAMS()                                                    \
    uvocie_recorder_obj_t *self = (uvocie_recorder_obj_t *)MP_OBJ_TO_PTR(self_in); \
    do {                                                                           \
        if (self == NULL || self->recorder_obj == NULL) {                          \
            mp_raise_OSError(EINVAL);                                              \
        }                                                                          \
    } while (0)

// this is the actual C-structure for our new object
typedef struct {
    // base represents some basic information, like type
    mp_obj_base_t base;

    // a member created by us
    char *modName;
    uvoice_recorder_t *recorder_obj;
} uvocie_recorder_obj_t;

void uvoice_recorder_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    uvocie_recorder_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "modName(%s)", self->modName);
}

STATIC mp_obj_t uvoice_recorder_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    uvocie_recorder_obj_t *uvocie_recorder_obj = m_new_obj(uvocie_recorder_obj_t);
    if (!uvocie_recorder_obj) {
        mp_raise_OSError(ENOMEM);
        return mp_const_none;
    }
    memset(uvocie_recorder_obj, 0, sizeof(uvocie_recorder_obj_t));

    uvocie_recorder_obj->base.type = &uvoice_recorder_type;
    uvocie_recorder_obj->modName = "uvoice-recorder";
    uvocie_recorder_obj->recorder_obj = NULL;

    return MP_OBJ_FROM_PTR(uvocie_recorder_obj);
}

STATIC mp_obj_t uvoice_open(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 8) {
        LOGE(LOG_TAG, "%s: args list illegal, exepect 8, get = %d\n", __func__, n_args);
        return MP_OBJ_NEW_SMALL_INT(-MP_E2BIG);
    }

    if (get_uvoice_state() == false) {
        LOGE(LOG_TAG, "snd card not inited");
        return MP_OBJ_NEW_SMALL_INT(-MP_EPERM);
    }

    uvocie_recorder_obj_t *self = (uvocie_recorder_obj_t *)MP_OBJ_TO_PTR(args[0]);
    if (self == NULL) {
        LOGE(LOG_TAG, "uvocie_recorder_obj_t NULL");
        return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
    }

    if (self->recorder_obj == NULL) {
        self->recorder_obj = uvoice_recorder_create();
        if (self->recorder_obj == NULL) {
            LOGE(LOG_TAG, "open media recorder failed !\n");
            return MP_OBJ_NEW_SMALL_INT(-MP_ENODEV);
        }
    }

    media_format_t format = (media_format_t)mp_obj_get_int(args[1]);
    int rate = mp_obj_get_int(args[2]);
    int channels = mp_obj_get_int(args[3]);
    int bits = mp_obj_get_int(args[4]);
    int frames = mp_obj_get_int(args[5]);
    int bitrate = mp_obj_get_int(args[6]);
    const char *sink = mp_obj_is_str(args[7]) ? mp_obj_str_get_str(args[7]) : NULL;

    LOGD(LOG_TAG,
         "format=%d, rate=%d, channels=%d, bits=%d, frames=%d, bitrate=%d, "
         "sink=%s\n",
         format, rate, channels, bits, frames, bitrate, sink);

    mp_int_t ret = self->recorder_obj->set_sink(format, rate, channels, bits, frames, bitrate, sink);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(uvoice_open_obj, 8, uvoice_open);

STATIC mp_obj_t uvoice_close(mp_obj_t self_in)
{
    RECORDER_CHECK_PARAMS();

    int ret = -1;
    if (self->recorder_obj != NULL) {
        ret = self->recorder_obj->stop();
        ret = uvoice_recorder_release(self->recorder_obj);
        if (ret != 0) {
            LOGE(LOG_TAG, "failed to release recorder!\n");
        } else {
            self->recorder_obj = NULL;
        }
    }
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_close_obj, uvoice_close);

STATIC mp_obj_t uvoice_set_sink(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 8) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    mp_obj_t self_in = args[0];
    RECORDER_CHECK_PARAMS();

    media_format_t format = (media_format_t)mp_obj_get_int(args[1]);
    int rate = mp_obj_get_int(args[2]);
    int channels = mp_obj_get_int(args[3]);
    int bits = mp_obj_get_int(args[4]);
    int frames = mp_obj_get_int(args[5]);
    int bitrate = mp_obj_get_int(args[6]);
    char *sink = mp_obj_is_str(args[7]) ? (char *)mp_obj_str_get_str(args[7]) : NULL;

    LOGD(LOG_TAG,
         "format=%d, rate=%d, channels=%d, bits=%d, frames=%d, bitrate=%d, "
         "sink=%s\n",
         format, rate, channels, bits, frames, bitrate, sink);

    mp_int_t ret = self->recorder_obj->set_sink(format, rate, channels, bits, frames, bitrate, sink);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(uvoice_set_sink_obj, 8, uvoice_set_sink);

STATIC mp_obj_t uvoice_clr_sink(mp_obj_t self_in)
{
    RECORDER_CHECK_PARAMS();

    mp_int_t ret = self->recorder_obj->clr_sink();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_clr_sink_obj, uvoice_clr_sink);

STATIC mp_obj_t uvoice_start(mp_obj_t self_in)
{
    RECORDER_CHECK_PARAMS();

    mp_int_t ret = self->recorder_obj->start();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_start_obj, uvoice_start);

STATIC mp_obj_t uvoice_stop(mp_obj_t self_in)
{
    RECORDER_CHECK_PARAMS();

    mp_int_t ret = self->recorder_obj->stop();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_stop_obj, uvoice_stop);

STATIC mp_obj_t uvoice_get_stream(mp_obj_t self_in, mp_obj_t buffer_in, mp_obj_t nbytes_in)
{
    RECORDER_CHECK_PARAMS();

    int nbytes = mp_obj_get_int(nbytes_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer_in, &bufinfo, MP_BUFFER_WRITE);
    memset(bufinfo.buf, 0, bufinfo.len);

    mp_int_t rsize = self->recorder_obj->get_stream((uint8_t *)bufinfo.buf, nbytes);
    return MP_OBJ_NEW_SMALL_INT(rsize);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(uvoice_get_stream_obj, uvoice_get_stream);

STATIC mp_obj_t uvoice_get_state(mp_obj_t self_in)
{
    RECORDER_CHECK_PARAMS();

    recorder_state_t state;
    self->recorder_obj->get_state(&state);
    return MP_OBJ_NEW_SMALL_INT(state);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_get_state_obj, uvoice_get_state);

STATIC mp_obj_t uvoice_get_position(mp_obj_t self_in)
{
    RECORDER_CHECK_PARAMS();

    int pos = -1;
    mp_int_t ret = self->recorder_obj->get_position(&pos);
    return MP_OBJ_NEW_SMALL_INT(pos);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_get_position_obj, uvoice_get_position);

STATIC mp_obj_t uvoice_ns_enable(mp_obj_t self_in, mp_obj_t enable_in)
{
    RECORDER_CHECK_PARAMS();

    int enable = mp_obj_get_int(enable_in);
    mp_int_t ret = self->recorder_obj->ns_enable(enable);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_ns_enable_obj, uvoice_ns_enable);

STATIC mp_obj_t uvoice_ec_enable(mp_obj_t self_in, mp_obj_t enable_in)
{
    RECORDER_CHECK_PARAMS();

    int enable = mp_obj_get_int(enable_in);
    mp_int_t ret = self->recorder_obj->ec_enable(enable);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_ec_enable_obj, uvoice_ec_enable);

STATIC mp_obj_t uvoice_agc_enable(mp_obj_t self_in, mp_obj_t enable_in)
{
    RECORDER_CHECK_PARAMS();

    int enable = mp_obj_get_int(enable_in);
    mp_int_t ret = self->recorder_obj->agc_enable(enable);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_agc_enable_obj, uvoice_agc_enable);

STATIC mp_obj_t uvoice_vad_enable(mp_obj_t self_in, mp_obj_t enable_in)
{
    RECORDER_CHECK_PARAMS();

    int enable = mp_obj_get_int(enable_in);
    mp_int_t ret = self->recorder_obj->vad_enable(enable);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_vad_enable_obj, uvoice_vad_enable);

STATIC mp_obj_t uvoice_format_support(mp_obj_t self_in, mp_obj_t format_in)
{
    RECORDER_CHECK_PARAMS();

    int format = (media_format_t)mp_obj_get_int(format_in);
    mp_int_t ret = self->recorder_obj->format_support(format);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_format_support_obj, uvoice_format_support);

STATIC const mp_rom_map_elem_t uvoice_module_recorder_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&uvoice_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&uvoice_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&uvoice_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&uvoice_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_getStream), MP_ROM_PTR(&uvoice_get_stream_obj) },
    { MP_ROM_QSTR(MP_QSTR_getState), MP_ROM_PTR(&uvoice_get_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_enableNS), MP_ROM_PTR(&uvoice_ns_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_enableEC), MP_ROM_PTR(&uvoice_ec_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_enableAGC), MP_ROM_PTR(&uvoice_agc_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_enableVAD), MP_ROM_PTR(&uvoice_vad_enable_obj) },
};
STATIC MP_DEFINE_CONST_DICT(uvoice_module_recorder_globals, uvoice_module_recorder_globals_table);

const mp_obj_type_t uvoice_recorder_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_Recorder,
    .print = uvoice_recorder_print,
    .make_new = uvoice_recorder_new,
    .locals_dict = (mp_obj_dict_t *)&uvoice_module_recorder_globals,
};

#endif // PY_BUILD_AUDIO
