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
#include "uvoice_recorder.h"

extern const mp_obj_type_t uvoice_recorder_type;
extern bool g_is_uvoice_inited;

#define RECORDER_CHECK_PARAMS()                                                    \
    uvocie_recorder_obj_t *self = (uvocie_recorder_obj_t *)MP_OBJ_TO_PTR(self_in); \
    do                                                                             \
    {                                                                              \
        if (self == NULL || self->recorder_obj == NULL)                            \
        {                                                                          \
            mp_raise_OSError(EINVAL);                                              \
            return mp_const_none;                                                  \
        }                                                                          \
    } while (0)

// this is the actual C-structure for our new object
typedef struct
{
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

STATIC mp_obj_t uvoice_create(mp_obj_t self_in)
{
    uvocie_recorder_obj_t *self = (uvocie_recorder_obj_t *)MP_OBJ_TO_PTR(self_in);
    if (self == NULL) {
        LOG_E("uvocie_recorder_obj_t NULL");
        return mp_const_none;
    }

    if (g_is_uvoice_inited == false) {
        mp_raise_OSError(ENXIO);
        return mp_const_none;
    }

    if (self->recorder_obj == NULL) {
        self->recorder_obj = uvoice_recorder_create();
        if (self->recorder_obj == NULL) {
            LOG_E("create media recorder failed !\n");
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_create_obj, uvoice_create);

STATIC mp_obj_t uvoice_release(mp_obj_t self_in)
{
    RECORDER_CHECK_PARAMS();

    int status = -1;
    if (self->recorder_obj != NULL) {
        status = uvoice_recorder_release(self->recorder_obj);
        if (status != 0) {
            LOG_E("failed to release recorder!\n");
        } else {
            self->recorder_obj = NULL;
        }
    }
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_release_obj, uvoice_release);

STATIC mp_obj_t uvoice_set_sink(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 8) {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
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
    char *sink = mp_obj_is_type(args[7], &mp_type_NoneType) ? NULL: (char *) mp_obj_str_get_str(args[7]);

    LOG_D("format=%d, rate=%d, channels=%d, bits=%d, frames=%d, bitrate=%d, sink=%s\n",
          format, rate, channels, bits, frames, bitrate, sink);

    mp_int_t status = self->recorder_obj->set_sink(format, rate, channels, bits, frames, bitrate, sink);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(uvoice_set_sink_obj, 8, uvoice_set_sink);

STATIC mp_obj_t uvoice_clr_sink(mp_obj_t self_in)
{
    RECORDER_CHECK_PARAMS();

    mp_int_t status = self->recorder_obj->clr_sink();
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_clr_sink_obj, uvoice_clr_sink);

STATIC mp_obj_t uvoice_start(mp_obj_t self_in)
{
    RECORDER_CHECK_PARAMS();

    mp_int_t status = self->recorder_obj->start();
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_start_obj, uvoice_start);

STATIC mp_obj_t uvoice_stop(mp_obj_t self_in)
{
    RECORDER_CHECK_PARAMS();

    mp_int_t status = self->recorder_obj->stop();
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_stop_obj, uvoice_stop);

STATIC mp_obj_t uvoice_get_stream(mp_obj_t self_in, mp_obj_t buffer_in, mp_obj_t nbytes_in)
{
    RECORDER_CHECK_PARAMS();

    int nbytes = mp_obj_get_int(nbytes_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer_in, &bufinfo, MP_BUFFER_WRITE);
    memset(bufinfo.buf, 0, bufinfo.len);

    mp_int_t rsize = self->recorder_obj->get_stream((uint8_t*)bufinfo.buf, nbytes);
    return mp_obj_new_int(rsize);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(uvoice_get_stream_obj, uvoice_get_stream);

STATIC mp_obj_t uvoice_get_state(mp_obj_t self_in)
{
    RECORDER_CHECK_PARAMS();

    recorder_state_t state;
    self->recorder_obj->get_state(&state);
    return mp_obj_new_int(state);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_get_state_obj, uvoice_get_state);

STATIC mp_obj_t uvoice_get_position(mp_obj_t self_in)
{
    RECORDER_CHECK_PARAMS();

    int pos = -1;
    mp_int_t status = self->recorder_obj->get_position(&pos);
    return mp_obj_new_int(pos);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uvoice_get_position_obj, uvoice_get_position);

STATIC mp_obj_t uvoice_ns_enable(mp_obj_t self_in, mp_obj_t enable_in)
{
    RECORDER_CHECK_PARAMS();

    int enable = mp_obj_get_int(enable_in);
    mp_int_t status = self->recorder_obj->ns_enable(enable);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_ns_enable_obj, uvoice_ns_enable);

STATIC mp_obj_t uvoice_ec_enable(mp_obj_t self_in, mp_obj_t enable_in)
{
    RECORDER_CHECK_PARAMS();

    int enable = mp_obj_get_int(enable_in);
    mp_int_t status = self->recorder_obj->ec_enable(enable);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_ec_enable_obj, uvoice_ec_enable);

STATIC mp_obj_t uvoice_agc_enable(mp_obj_t self_in, mp_obj_t enable_in)
{
    RECORDER_CHECK_PARAMS();

    int enable = mp_obj_get_int(enable_in);
    mp_int_t status = self->recorder_obj->agc_enable(enable);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_agc_enable_obj, uvoice_agc_enable);

STATIC mp_obj_t uvoice_vad_enable(mp_obj_t self_in, mp_obj_t enable_in)
{
    RECORDER_CHECK_PARAMS();

    int enable = mp_obj_get_int(enable_in);
    mp_int_t status = self->recorder_obj->vad_enable(enable);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_vad_enable_obj, uvoice_vad_enable);

STATIC mp_obj_t uvoice_format_support(mp_obj_t self_in, mp_obj_t format_in)
{
    RECORDER_CHECK_PARAMS();

    int format = (media_format_t)mp_obj_get_int(format_in);
    mp_int_t status = self->recorder_obj->format_support(format);
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(uvoice_format_support_obj, uvoice_format_support);

STATIC const mp_rom_map_elem_t uvoice_module_recorder_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&uvoice_clr_sink_obj)},
    {MP_ROM_QSTR(MP_QSTR_create), MP_ROM_PTR(&uvoice_create_obj)},
    {MP_ROM_QSTR(MP_QSTR_release), MP_ROM_PTR(&uvoice_release_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_sink), MP_ROM_PTR(&uvoice_set_sink_obj)},
    {MP_ROM_QSTR(MP_QSTR_clr_sink), MP_ROM_PTR(&uvoice_clr_sink_obj)},
    {MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&uvoice_start_obj)},
    {MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&uvoice_stop_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_stream), MP_ROM_PTR(&uvoice_get_stream_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_state), MP_ROM_PTR(&uvoice_get_state_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_position), MP_ROM_PTR(&uvoice_get_position_obj)},
    {MP_ROM_QSTR(MP_QSTR_ns_enable), MP_ROM_PTR(&uvoice_ns_enable_obj)},
    {MP_ROM_QSTR(MP_QSTR_ec_enable), MP_ROM_PTR(&uvoice_ec_enable_obj)},
    {MP_ROM_QSTR(MP_QSTR_agc_enable), MP_ROM_PTR(&uvoice_agc_enable_obj)},
    {MP_ROM_QSTR(MP_QSTR_vad_enable), MP_ROM_PTR(&uvoice_vad_enable_obj)},
    {MP_ROM_QSTR(MP_QSTR_format_support), MP_ROM_PTR(&uvoice_format_support_obj)},

};
STATIC MP_DEFINE_CONST_DICT(uvoice_module_recorder_globals, uvoice_module_recorder_globals_table);

const mp_obj_type_t uvoice_recorder_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_Recorder,
    .print = uvoice_recorder_print,
    .make_new = uvoice_recorder_new,
    .locals_dict = (mp_obj_dict_t *)&uvoice_module_recorder_globals,
};
