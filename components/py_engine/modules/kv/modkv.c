#if (MICROPY_PY_KV)

#include "aos/kv.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

#define LOG_TAG "DRIVER_KV"

#define KV_BUFFER_MAX_LEN 256

STATIC mp_obj_t obj_setStorageSync(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    char *key;
    char *value;
    int32_t value_len = 0;

    if (n_args < 2) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return MP_ROM_INT(-EINVAL);
    }

    key = (char *)mp_obj_str_get_str(args[0]);
    value = (char *)mp_obj_str_get_str(args[1]);
    value_len = strlen(value);

    ret = aos_kv_set(key, value, value_len, 1);
    if (ret != 0) {
        LOGE(LOG_TAG, "%s:aos_kv_set failed\n", __func__);
    }
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(kv_obj_setStorageSync, 2, obj_setStorageSync);

STATIC mp_obj_t obj_getStorageSync(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int32_t value_len = KV_BUFFER_MAX_LEN;
    char *key;
    char *value;

    if (n_args < 1) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return MP_ROM_INT(-EINVAL);
    }

    key = (char *)mp_obj_str_get_str(args[0]);
    value = (char *)aos_malloc(KV_BUFFER_MAX_LEN);
    if (value == NULL) {
        LOGE(LOG_TAG, "allocate memory failed\n");
        return MP_ROM_INT(-ENOMEM);
    }
    memset(value, 0, KV_BUFFER_MAX_LEN);

    ret = aos_kv_get(key, value, &value_len);
    if (ret != 0) {
        LOGE(LOG_TAG, "aos_kv_get failed key = %s;\n", key);
        return MP_ROM_INT(ret);
    }

    mp_obj_t val_str = mp_obj_new_str(value, strlen(value));

    LOGD(LOG_TAG, "%s:key = %s;value = %s;\n", __func__, key, value);
    aos_free(value);
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return val_str;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(kv_obj_getStorageSync, 1, obj_getStorageSync);

STATIC mp_obj_t obj_removeStorageSync(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    char *key;

    if (n_args < 1) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return MP_ROM_INT(-EINVAL);
    }

    key = (char *)mp_obj_str_get_str(args[0]);
    LOGD(LOG_TAG, "%s:key = %s;\n", __func__, key);

    ret = aos_kv_del(key);
    if (ret != 0) {
        LOGE(LOG_TAG, "kv delete item failed\n");
        return MP_ROM_INT(ret);
    }
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(kv_obj_removeStorageSync, 1, obj_removeStorageSync);

STATIC const mp_rom_map_elem_t kv_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_kv) },
    { MP_ROM_QSTR(MP_QSTR_setStorageSync), MP_ROM_PTR(&kv_obj_setStorageSync) },
    { MP_ROM_QSTR(MP_QSTR_getStorageSync), MP_ROM_PTR(&kv_obj_getStorageSync) },
    { MP_ROM_QSTR(MP_QSTR_removeStorageSync), MP_ROM_PTR(&kv_obj_removeStorageSync) },
};

STATIC MP_DEFINE_CONST_DICT(kv_module_globals, kv_module_globals_table);
const mp_obj_module_t mp_module_kv = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&kv_module_globals,
};

#endif // MICROPY_PY_KV
