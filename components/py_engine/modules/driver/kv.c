#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"

#include "ulog/ulog.h"
#include "aos/kv.h"

#define LOG_TAG "DRIVER_KV"

extern const mp_obj_type_t driver_kv_type;
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
    LOGD(LOG_TAG, "entern %s;\n", __func__);
    mp_kv_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t kv_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s;\n", __func__);
    mp_kv_obj_t* driver_obj = m_new_obj(mp_kv_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(MP_EINVAL);
    }

    driver_obj->Base.type = &driver_kv_type;
    driver_obj->ModuleName = "kv";

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_setStorageSync(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    char *key;
    char *value;
    int32_t value_len = 0;

    if (n_args < 3)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_kv_obj_t* driver_obj = (mp_kv_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return mp_const_none;
    }

    key = (char *)mp_obj_str_get_str(args[1]);
    value = (char *)mp_obj_str_get_str(args[2]);
    value_len = strlen(value);

    ret = aos_kv_set(key, value, value_len, 1);
    if (ret != 0)
    {
        LOGE(LOG_TAG, "%s:aos_kv_set failed\n", __func__);
    }
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(kv_obj_setStorageSync, 3, obj_setStorageSync);

STATIC mp_obj_t obj_getStorageSync(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int32_t value_len = KV_BUFFER_MAX_LEN;
    char *key;
    char *value;

    if (n_args < 2)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_kv_obj_t* driver_obj = (mp_kv_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return mp_const_none;
    }

    key = (char *)mp_obj_str_get_str(args[1]);
    value = (char *)aos_malloc(KV_BUFFER_MAX_LEN);
    if (value == NULL) {
        LOGE(LOG_TAG, "allocate memory failed\n");
        return mp_const_none;
    }

    ret = aos_kv_get(key, value, &value_len);
    if (ret != 0) {
        LOGE(LOG_TAG, "aos_kv_get failed key = %s;\n", key);
        return mp_const_none;
    }

    mp_obj_t dict = mp_obj_new_dict(1);
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str(key, strlen(key)), mp_obj_new_str(value, strlen(value)));

    LOGD(LOG_TAG, "%s:key = %s;value = %s;\n", __func__, key, value);
    aos_free(value);
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return dict;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(kv_obj_getStorageSync, 2, obj_getStorageSync);

STATIC mp_obj_t obj_removeStorageSync(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    char * key;

    if (n_args < 2)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_kv_obj_t* driver_obj = (mp_kv_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return mp_const_none;
    }

    key = (char *)mp_obj_str_get_str(args[1]);
    LOGD(LOG_TAG, "%s:key = %s;\n", __func__, key);

    ret = aos_kv_del(key);
    if (ret != 0) {
        LOGE(LOG_TAG, "kv delete item failed\n");
        return mp_const_none;
    }
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(kv_obj_removeStorageSync, 2, obj_removeStorageSync);

STATIC const mp_rom_map_elem_t kv_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_KV)},
    {MP_ROM_QSTR(MP_QSTR_setStorageSync), MP_ROM_PTR(&kv_obj_setStorageSync)},
    {MP_ROM_QSTR(MP_QSTR_getStorageSync), MP_ROM_PTR(&kv_obj_getStorageSync)},
    {MP_ROM_QSTR(MP_QSTR_removeStorageSync), MP_ROM_PTR(&kv_obj_removeStorageSync)},
};

STATIC MP_DEFINE_CONST_DICT(kv_locals_dict, kv_locals_dict_table);

const mp_obj_type_t driver_kv_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_KV,
    .print = kv_obj_print,
    .make_new = kv_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&kv_locals_dict,
};

