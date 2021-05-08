#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"
#include "aos/kv.h"

extern const mp_obj_type_t system_kv_type;
#define KV_BUFFER_MAX_LEN   256
// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
} mp_kv_obj_t;

void kv_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_kv_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t kv_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_kv_obj_t* driver_obj = m_new_obj(mp_kv_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &system_kv_type;
    driver_obj->ModuleName = "kv";

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_kv_obj_t* driver_obj = (mp_kv_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(kv_obj_open, 1, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_kv_obj_t* driver_obj = (mp_kv_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(kv_obj_close, 1, obj_close);

STATIC mp_obj_t obj_setStorageSync(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    char *key;
    char *value;
    int32_t value_len = 0;

    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_kv_obj_t* driver_obj = (mp_kv_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    key = (char *)mp_obj_str_get_str(args[1]);
    value = (char *)mp_obj_str_get_str(args[2]);
    value_len = strlen(value);

    ret = aos_kv_set(key, value, value_len, 1);
    if (ret != 0)
    {
        LOG_E("%s:aos_kv_set failed\n", __func__);
    }
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(kv_obj_setStorageSync, 3, obj_setStorageSync);

STATIC mp_obj_t obj_getStorageSync(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int32_t value_len = KV_BUFFER_MAX_LEN;
    char *key;
    char *value;

    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_kv_obj_t* driver_obj = (mp_kv_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    key = (char *)mp_obj_str_get_str(args[1]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    memset(bufinfo.buf, 0, bufinfo.len);

    value = (char *)aos_malloc(KV_BUFFER_MAX_LEN);
    if (value == NULL) {
        LOG_E("allocate memory failed\n");
        return mp_const_none;
    }

    ret = aos_kv_get(key, value, &value_len);
    if (ret != 0) {
        LOG_E("kv get storage failed\n");
        aos_free(value);
        return mp_const_none;
    }

    if (bufinfo.len < value_len)
    {
        LOG_E("buffer size is not enough\n");
        aos_free(value);
        return mp_const_none;
    }

    memcpy(bufinfo.buf, value, value_len);

    aos_free(value);
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(kv_obj_getStorageSync, 3, obj_getStorageSync);

STATIC mp_obj_t obj_removeStorageSync(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    char * key;

    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_kv_obj_t* driver_obj = (mp_kv_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    key = (char *)mp_obj_str_get_str(args[1]);
    LOG_D("%s:key = %s;\n", __func__, key);

    ret = aos_kv_del(key);
    if (ret != 0) {
        LOG_E("kv delete item failed\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(kv_obj_removeStorageSync, 2, obj_removeStorageSync);

STATIC const mp_rom_map_elem_t kv_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_KV)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&kv_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&kv_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_setStorageSync), MP_ROM_PTR(&kv_obj_setStorageSync)},
    {MP_ROM_QSTR(MP_QSTR_getStorageSync), MP_ROM_PTR(&kv_obj_getStorageSync)},
    {MP_ROM_QSTR(MP_QSTR_removeStorageSync), MP_ROM_PTR(&kv_obj_removeStorageSync)},
};

STATIC MP_DEFINE_CONST_DICT(kv_locals_dict, kv_locals_dict_table);

const mp_obj_type_t system_kv_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_KV,
    .print = kv_obj_print,
    .make_new = kv_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&kv_locals_dict,
};

