/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * OMV Python module.
 */
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "py/obj.h"
#include "usbdbg.h"
#include "framebuffer.h"
#include "omv_boardconfig.h"

static mp_obj_t py_omv_version_string()
{
    char str[12];
    snprintf(str, 12, "%d.%d.%d",
             FIRMWARE_VERSION_MAJOR,
             FIRMWARE_VERSION_MINOR,
             FIRMWARE_VERSION_PATCH);
    return mp_obj_new_str(str, strlen(str));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_omv_version_string_obj, py_omv_version_string);

static mp_obj_t py_omv_arch()
{
    char *str = OMV_ARCH_STR;
    return mp_obj_new_str(str, strlen(str));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_omv_arch_obj, py_omv_arch);

static mp_obj_t py_omv_board_type()
{
    char *str = OMV_BOARD_TYPE;
    return mp_obj_new_str(str, strlen(str));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_omv_board_type_obj, py_omv_board_type);

static mp_obj_t py_omv_board_id()
{
    char str[25];
    snprintf(str, 25, "%08X%08X%08X",
             *((unsigned int *) (OMV_UNIQUE_ID_ADDR + 8)),
             *((unsigned int *) (OMV_UNIQUE_ID_ADDR + 4)),
             *((unsigned int *) (OMV_UNIQUE_ID_ADDR + 0)));
    return mp_obj_new_str(str, strlen(str));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_omv_board_id_obj, py_omv_board_id);

static mp_obj_t py_omv_disable_fb(uint n_args, const mp_obj_t *args)
{
    if (!n_args) {
        return mp_obj_new_bool(!fb_get_streaming_enabled());
    }
    fb_set_streaming_enabled(!mp_obj_get_int(args[0]));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(py_omv_disable_fb_obj, 0, 1, py_omv_disable_fb);

static const mp_rom_map_elem_t globals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),        MP_OBJ_NEW_QSTR(MP_QSTR_omv) },
    { MP_ROM_QSTR(MP_QSTR_version_major),   MP_ROM_INT(FIRMWARE_VERSION_MAJOR) },
    { MP_ROM_QSTR(MP_QSTR_version_minor),   MP_ROM_INT(FIRMWARE_VERSION_MINOR) },
    { MP_ROM_QSTR(MP_QSTR_version_patch),   MP_ROM_INT(FIRMWARE_VERSION_PATCH) },
    { MP_ROM_QSTR(MP_QSTR_version_string),  MP_ROM_PTR(&py_omv_version_string_obj) },
    { MP_ROM_QSTR(MP_QSTR_arch),            MP_ROM_PTR(&py_omv_arch_obj) },
    { MP_ROM_QSTR(MP_QSTR_board_type),      MP_ROM_PTR(&py_omv_board_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_board_id),        MP_ROM_PTR(&py_omv_board_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_fb),      MP_ROM_PTR(&py_omv_disable_fb_obj) }
};

STATIC MP_DEFINE_CONST_DICT(globals_dict, globals_dict_table);

const mp_obj_module_t omv_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_t) &globals_dict,
};
