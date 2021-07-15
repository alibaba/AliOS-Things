/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "board_config.h"
#include "py_defines.h"
#include "aos_system.h"


#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/stackctrl.h"
#include  "ulog/ulog.h"

#define MOD_STR "LOG"
#define SCRIPT  "PYLOG"
#define LOG_LEVEL_DEBUG   "debug"
#define LOG_LEVEL_INFO    "info"
#define LOG_LEVEL_WARN    "warning"
#define LOG_LEVEL_ERROR   "error"
#define LOG_LEVEL_FATAL   "fatal"
#define LOG_LEVEL_NONE    "none"




static mp_obj_t set_stdlog_level(mp_obj_t loglevel)
{
    int ret = 0;
    aos_log_level_t  log_level;
    log_level = mp_obj_get_int(loglevel);
    ret = aos_set_log_level(log_level);
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_1(native_set_stdlog_level, set_stdlog_level);


static mp_obj_t set_popcloud_log_level(mp_obj_t loglevel)
{
    int ret = 0;
    aos_log_level_t  log_level;
    log_level = mp_obj_get_int(loglevel);
    ret = aos_set_popcloud_log_level(log_level);
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_1(native_set_popcloud_log_level, set_popcloud_log_level);


static mp_obj_t set_popfs_log_level(mp_obj_t loglevel)
{
    int ret = 0;
    aos_log_level_t  log_level;
    log_level = mp_obj_get_int(loglevel);
    ret = aos_set_popfs_log_level(log_level);
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_1(native_set_popfs_log_level, set_popfs_log_level);

static mp_obj_t set_log_file_path(mp_obj_t logpath)
{
    int ret = 0;

    const char *path = mp_obj_str_get_str(logpath);
    if (NULL == path) {
        amp_warn(MOD_STR, "invalid parameter\n");
        return mp_obj_new_int(-1);
    }

    ret = ulog_fs_log_file_path(path);
    if (ret) {
        amp_warn(MOD_STR, "fail to set log file path %s\n", path);
        return mp_obj_new_int(-1);
    }
    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_1(native_set_log_file_path, set_log_file_path);


static mp_obj_t set_log_file_size(mp_obj_t logsize)
{
    int ret = -1;
    unsigned int filesize = 0;

    filesize = mp_obj_get_int(logsize);

    ret = ulog_fs_log_file_size(filesize);
    if (ret) {
        amp_warn(MOD_STR, "fail to set log file size %d\n", filesize);
        return mp_obj_new_int(-1);
    }

    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_1(native_set_log_file_size, set_log_file_size);



static mp_obj_t debug_log_out(mp_obj_t log_str,mp_obj_t log)
{
    const char *msg = mp_obj_str_get_str(log);
    const char *tag = mp_obj_str_get_str(log_str);

    if (NULL == msg) {
        amp_error(MOD_STR, "ulog fail to get output format msg");
        return mp_obj_new_int(-1);
    }

    ulog(AOS_LL_DEBUG, tag, NULL, 0, msg);

    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_2(native_debug_log_out, debug_log_out);


static mp_obj_t info_log_out(mp_obj_t log_str,mp_obj_t log)
{
    const char *msg = mp_obj_str_get_str(log);
    const char *tag = mp_obj_str_get_str(log_str);

    if (NULL == msg) {
        amp_error(MOD_STR, "ulog fail to get output format msg");
        return mp_obj_new_int(-1);
    }

    ulog(AOS_LL_INFO, tag, NULL, 0, msg);

    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_2(native_info_log_out, info_log_out);


static mp_obj_t warn_log_out(mp_obj_t log_str,mp_obj_t log)
{
    const char *msg = mp_obj_str_get_str(log);
    const char *tag = mp_obj_str_get_str(log_str);

    if (NULL == msg) {
        amp_error(MOD_STR, "ulog fail to get output format msg");
        return mp_obj_new_int(-1);
    }

    ulog(AOS_LL_WARN, tag, NULL, 0, msg);

    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_2(native_warn_log_out, warn_log_out);


static mp_obj_t error_log_out(mp_obj_t log_str,mp_obj_t log)
{
    const char *msg = mp_obj_str_get_str(log);
    const char *tag = mp_obj_str_get_str(log_str);

    if (NULL == msg) {
        amp_error(MOD_STR, "ulog fail to get output format msg");
        return mp_obj_new_int(-1);
    }

    ulog(AOS_LL_ERROR, tag, NULL, 0, msg);

    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_2(native_error_log_out, error_log_out);


static mp_obj_t fatal_log_out(mp_obj_t log_str,mp_obj_t log)
{
    const char *msg = mp_obj_str_get_str(log);
    const char *tag = mp_obj_str_get_str(log_str);


    if (NULL == msg) {
        amp_error(MOD_STR, "ulog fail to get output format msg");
        return mp_obj_new_int(-1);
    }

    ulog(AOS_LL_FATAL, tag, NULL, 0, msg);

    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_2(native_fatal_log_out, fatal_log_out);


STATIC const mp_rom_map_elem_t ulog_module_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ulog)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_debug), MP_ROM_PTR(&native_debug_log_out)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_info), MP_ROM_PTR(&native_info_log_out)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_warn), MP_ROM_PTR(&native_warn_log_out)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_error), MP_ROM_PTR(&native_error_log_out)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_fatal), MP_ROM_PTR(&native_fatal_log_out)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_stdloglevel), MP_ROM_PTR(&native_set_stdlog_level)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_cloudloglevel), MP_ROM_PTR(&native_set_popcloud_log_level)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_fsloglevel), MP_ROM_PTR(&native_set_popfs_log_level)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_setlogfilepath), MP_ROM_PTR(&native_set_log_file_path)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_setlogfilesize), MP_ROM_PTR(&native_set_log_file_size)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_LOG_DEBUG), MP_ROM_INT(AOS_LL_DEBUG)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_LOG_INFO), MP_ROM_INT(AOS_LL_INFO)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_LOG_WARN), MP_ROM_INT(AOS_LL_WARN)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_LOG_ERROR), MP_ROM_INT(AOS_LL_ERROR)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_LOG_FATAL), MP_ROM_INT(AOS_LL_FATAL)},

};

STATIC MP_DEFINE_CONST_DICT(ulog_module_globals, ulog_module_globals_table);

const mp_obj_module_t ulog_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&ulog_module_globals,
};
