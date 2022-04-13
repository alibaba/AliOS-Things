#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"
#if 0
#include "amp_system.h"
#include "amp_defines.h"
#include "amp_utils.h"
#endif
extern const mp_obj_type_t system_info_type;
// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
} mp_system_obj_t;

void system_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_system_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t system_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_system_obj_t* driver_obj = m_new_obj(mp_system_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &system_info_type;
    driver_obj->ModuleName = "system";

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
    mp_system_obj_t* driver_obj = (mp_system_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(system_obj_open, 1, obj_open);

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
    mp_system_obj_t* driver_obj = (mp_system_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(system_obj_close, 1, obj_close);

STATIC mp_obj_t obj_versions(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
#if 0
    char * version = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_system_obj_t* driver_obj = (mp_system_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    //version = amp_get_system_version();
    LOG_D("%s:out\n", __func__);

    return MP_ROM_QSTR(version);
#else
    return mp_const_none;
#endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(system_obj_versions, 1, obj_versions);

STATIC mp_obj_t obj_version(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
#if 0
    char version[AMP_VERSION_LENGTH] = {0};
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_system_obj_t* driver_obj = (mp_system_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    amp_version_get(version);
    LOG_D("%s:out\n", __func__);

    return MP_ROM_QSTR(version);
#else
    return mp_const_none;
#endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(system_obj_version, 1, obj_version);

STATIC mp_obj_t obj_platform(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    char * type = NULL;
#if 0
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_system_obj_t* driver_obj = (mp_system_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    //type = amp_get_platform_type();
    LOG_D("%s:out\n", __func__);

    return MP_ROM_QSTR(type);
#else
    return mp_const_none;
#endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(system_obj_platform, 1, obj_platform);

STATIC mp_obj_t obj_uptime(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
#if 0
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_system_obj_t* driver_obj = (mp_system_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    //ret = amp_uptime();
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
#else
    return mp_const_none;
#endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(system_obj_uptime, 1, obj_uptime);

STATIC mp_obj_t obj_memory_total(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
#if 0
    amp_heap_info_t heap_info;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_system_obj_t* driver_obj = (mp_system_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    //ret = amp_heap_memory_info(&heap_info);
    if (ret != 0)
    {
        LOG_E("%s:amp_heap_memory_info failed\n", __func__);
        return mp_const_none;
    }
    LOG_D("%s:heap_info.heap_total = %d;\n", __func__, heap_info.heap_total);
    LOG_D("%s:heap_info.heap_used = %d;\n", __func__, heap_info.heap_used);
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(heap_info.heap_total);
#else
    return mp_const_none;
#endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(system_obj_memory_total, 1, obj_memory_total);

STATIC mp_obj_t obj_memory_used(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
#if 0
    amp_heap_info_t heap_info;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_system_obj_t* driver_obj = (mp_system_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    //ret = amp_heap_memory_info(&heap_info);
    if (ret != 0)
    {
        LOG_E("%s:amp_heap_memory_info failed\n", __func__);
        return mp_const_none;
    }
    LOG_D("%s:heap_info.heap_total = %d;\n", __func__, heap_info.heap_total);
    LOG_D("%s:heap_info.heap_used = %d;\n", __func__, heap_info.heap_used);
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(heap_info.heap_used);
#else
    return mp_const_none;
#endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(system_obj_memory_used, 1, obj_memory_used);

STATIC mp_obj_t obj_gc(size_t n_args, const mp_obj_t *args)
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
    mp_system_obj_t* driver_obj = (mp_system_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

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

const mp_obj_type_t system_info_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_SystemInfo,
    .print = system_obj_print,
    .make_new = system_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&systeminfo_locals_dict,
};

