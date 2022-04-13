#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"
#include "amp_system.h"
#include "amp_defines.h"
#include "dev_model_api.h"
//#include "amp_tts.h"

extern const mp_obj_type_t driver_tts_type;

#define TTS_JSON_CONFIG_PATH "/data/python/config/tts.json"

// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
} mp_tts_obj_t;

void tts_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_tts_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t tts_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_tts_obj_t* driver_obj = m_new_obj(mp_tts_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &driver_tts_type;
    driver_obj->ModuleName = "tts";

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_tts_obj_t* driver_obj = (mp_tts_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    //amp_tts_init();
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(tts_obj_open, 1, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_tts_obj_t* driver_obj = (mp_tts_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(tts_obj_close, 1, obj_close);

STATIC mp_obj_t obj_play(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int encode_type;
    char *text;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_tts_obj_t* driver_obj = (mp_tts_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    text = (char *)mp_obj_str_get_str(args[1]);
    encode_type = (int)mp_obj_get_int(args[2]);
    LOG_D("%s: text = %s;encode_type = %d;\n", __func__, text, encode_type);
#if 0
    if (amp_tts_is_playing())
    {
        LOG_E("%s:amp_tts_is_playing failed\n", __func__);
        return mp_const_none;
    }

    ret = amp_tts_play(text, encode_type);
    if (ret != 0)
    {
        LOG_E("%s:amp_tts_play failed\n", __func__);
    }
#endif
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(tts_obj_play, 3, obj_play);

STATIC mp_obj_t obj_stop(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_tts_obj_t* driver_obj = (mp_tts_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

#if 0
    ret = amp_tts_stop();
    if (ret != 0)
    {
        LOG_E("amp_tts_stop failed\n");
    }
#endif
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(tts_obj_stop, 1, obj_stop);

STATIC mp_obj_t obj_getState(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int state = -1;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_tts_obj_t* driver_obj = (mp_tts_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    //amp_tts_state_get(&state);
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(state);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(tts_obj_getState, 1, obj_getState);

STATIC mp_obj_t obj_setVolume(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int volume;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_tts_obj_t* driver_obj = (mp_tts_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    volume = (int)mp_obj_get_int(args[1]);
    LOG_D("%s:volume = %d;\n", __func__, volume);
#if 0
    ret = amp_tts_volume_set(volume);
    if (ret != 0)
    {
        LOG_E("amp_tts_volume_set failed\n");
    }
#endif
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(tts_obj_setVolume, 2, obj_setVolume);

STATIC mp_obj_t obj_getVolume(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int volume = -1;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_tts_obj_t* driver_obj = (mp_tts_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    //amp_tts_volume_get(&volume);
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(volume);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(tts_obj_getVolume, 1, obj_getVolume);

STATIC mp_obj_t obj_setPitch(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int pitch;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_tts_obj_t* driver_obj = (mp_tts_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    pitch = (int)mp_obj_get_int(args[1]);
    LOG_D("%s:pitch = %d;\n", __func__, pitch);
#if 0
    ret = amp_tts_pitch_set(pitch);
    if (ret != 0)
    {
        LOG_E("amp_tts_pitch_set failed\n");
    }
#endif
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(tts_obj_setPitch, 2, obj_setPitch);

STATIC mp_obj_t obj_setSpeed(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int speed;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_tts_obj_t* driver_obj = (mp_tts_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    speed = (int)mp_obj_get_int(args[1]);
    LOG_D("%s:speed = %d;\n", __func__, speed);
#if 0
    ret = amp_tts_speed_set(speed);
    if (ret != 0)
    {
        LOG_E("amp_tts_speed_set failed\n");
    }
#endif

    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(tts_obj_setSpeed, 2, obj_setSpeed);

STATIC mp_obj_t obj_getSpeed(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int speed = -1;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_tts_obj_t* driver_obj = (mp_tts_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    //amp_tts_speed_get(&speed);
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(speed);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(tts_obj_getSpeed, 1, obj_getSpeed);

STATIC const mp_rom_map_elem_t tts_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_TTS)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&tts_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&tts_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&tts_obj_play)},
    {MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&tts_obj_stop)},
    {MP_ROM_QSTR(MP_QSTR_getState), MP_ROM_PTR(&tts_obj_getState)},
    {MP_ROM_QSTR(MP_QSTR_setVolume), MP_ROM_PTR(&tts_obj_setVolume)},
    {MP_ROM_QSTR(MP_QSTR_getVolume), MP_ROM_PTR(&tts_obj_getVolume)},
    {MP_ROM_QSTR(MP_QSTR_setPitch), MP_ROM_PTR(&tts_obj_setPitch)},
    {MP_ROM_QSTR(MP_QSTR_setSpeed), MP_ROM_PTR(&tts_obj_setSpeed)},
    {MP_ROM_QSTR(MP_QSTR_getSpeed), MP_ROM_PTR(&tts_obj_getSpeed)},
};

STATIC MP_DEFINE_CONST_DICT(tts_locals_dict, tts_locals_dict_table);

const mp_obj_type_t driver_tts_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_TTS,
    .print = tts_obj_print,
    .make_new = tts_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&tts_locals_dict,
};

