#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"

extern const mp_obj_type_t driver_und_type;

// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
} mp_und_obj_t;

void und_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_und_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t und_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_und_obj_t* driver_obj = m_new_obj(mp_und_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &driver_und_type;
    driver_obj->ModuleName = "und";

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
    mp_und_obj_t* driver_obj = (mp_und_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(und_obj_open, 1, obj_open);

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
    mp_und_obj_t* driver_obj = (mp_und_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(und_obj_close, 1, obj_close);

STATIC mp_obj_t obj_start(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_und_obj_t* driver_obj = (mp_und_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    //ret = und_init();
    if (ret != 0)
    {
        LOG_E("und_init failed\n");
    }
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(und_obj_start, 1, obj_start);

STATIC mp_obj_t obj_update(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int32_t cap_idx;
    int32_t reason_code;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_und_obj_t* driver_obj = (mp_und_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    cap_idx = (uint32_t)mp_obj_get_int(args[1]);
    reason_code = (uint32_t)mp_obj_get_int(args[2]);
    LOG_D("%s: cap_idx = %d;\n", __func__, cap_idx);
    LOG_D("%s: reason_code = %d;\n", __func__, reason_code);

    //ret = und_update_statis(cap_idx, reason_code);
    if (ret != 0)
    {
        LOG_E("und_update_statis failed\n");
    }
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(und_obj_update, 3, obj_update);

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
    mp_und_obj_t* driver_obj = (mp_und_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    //ret = und_deinit();
    if (ret != 0)
    {
        LOG_E("%s:und_deinit failed\n", __func__);
    }
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(und_obj_stop, 1, obj_stop);

STATIC const mp_rom_map_elem_t und_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_UND)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&und_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&und_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&und_obj_start)},
    {MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&und_obj_update)},
    {MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&und_obj_stop)},
};

STATIC MP_DEFINE_CONST_DICT(und_locals_dict, und_locals_dict_table);

const mp_obj_type_t driver_und_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_UND,
    .print = und_obj_print,
    .make_new = und_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&und_locals_dict,
};

