#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"

#include "ulog/ulog.h"

#define LOG_TAG "MOD_SYSTEMINFO"

// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
} mp_system_obj_t;
STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(system_obj_open, 1, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(system_obj_close, 1, obj_close);

STATIC mp_obj_t obj_versions(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;

    char * version = NULL;
    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    //version = amp_get_system_version();
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return MP_ROM_QSTR(version);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(system_obj_versions, 1, obj_versions);

STATIC mp_obj_t obj_version(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;

    //char version[AMP_VERSION_LENGTH] = {0};
    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    //amp_version_get(version);
    LOGD(LOG_TAG, "%s:out\n", __func__);

    //return MP_ROM_QSTR(version);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(system_obj_version, 1, obj_version);

STATIC mp_obj_t obj_platform(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    char * type = NULL;

    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    //type = amp_get_platform_type();
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return MP_ROM_QSTR(type);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(system_obj_platform, 1, obj_platform);

STATIC mp_obj_t obj_uptime(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;

    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    //ret = amp_uptime();
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(system_obj_uptime, 1, obj_uptime);

STATIC mp_obj_t obj_memory_total(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;

    //amp_heap_info_t heap_info;
    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    //ret = amp_heap_memory_info(&heap_info);
    if (ret != 0)
    {
        LOGE(LOG_TAG, "%s:amp_heap_memory_info failed\n", __func__);
        return mp_const_none;
    }
    //LOGD(LOG_TAG, "%s:heap_info.heap_total = %d;\n", __func__, heap_info.heap_total);
    //LOGD(LOG_TAG, "%s:heap_info.heap_used = %d;\n", __func__, heap_info.heap_used);
    LOGD(LOG_TAG, "%s:out\n", __func__);

    //return MP_ROM_INT(heap_info.heap_total);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(system_obj_memory_total, 1, obj_memory_total);

STATIC mp_obj_t obj_memory_used(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;

    //amp_heap_info_t heap_info;
    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    //ret = amp_heap_memory_info(&heap_info);
    if (ret != 0)
    {
        LOGE(LOG_TAG, "%s:amp_heap_memory_info failed\n", __func__);
        return mp_const_none;
    }
    //LOGD(LOG_TAG, "%s:heap_info.heap_total = %d;\n", __func__, heap_info.heap_total);
    //LOGD(LOG_TAG, "%s:heap_info.heap_used = %d;\n", __func__, heap_info.heap_used);
    LOGD(LOG_TAG, "%s:out\n", __func__);

    //return MP_ROM_INT(heap_info.heap_used);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(system_obj_memory_used, 1, obj_memory_used);

STATIC mp_obj_t obj_gc(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(system_obj_gc, 1, obj_gc);

STATIC const mp_rom_map_elem_t systeminfo_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_SystemInfo)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&system_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&system_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_versions), MP_ROM_PTR(&system_obj_versions)},
    {MP_ROM_QSTR(MP_QSTR_version), MP_ROM_PTR(&system_obj_version)},
    {MP_ROM_QSTR(MP_QSTR_platform), MP_ROM_PTR(&system_obj_platform)},
    {MP_ROM_QSTR(MP_QSTR_uptime), MP_ROM_PTR(&system_obj_uptime)},
    {MP_ROM_QSTR(MP_QSTR_memory_total), MP_ROM_PTR(&system_obj_memory_total)},
    {MP_ROM_QSTR(MP_QSTR_memory_used), MP_ROM_PTR(&system_obj_memory_used)},
    {MP_ROM_QSTR(MP_QSTR_gc), MP_ROM_PTR(&system_obj_gc)},
};

STATIC MP_DEFINE_CONST_DICT(systeminfo_locals_dict, systeminfo_locals_dict_table);

const mp_obj_module_t system_info_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&systeminfo_locals_dict,
};
