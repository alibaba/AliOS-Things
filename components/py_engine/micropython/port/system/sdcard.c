#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "py/smallint.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "HaasLog.h"
#include "amp_fs.h"
#include "amp_defines.h"
#include "amp_system.h"

#if MICROPY_PY_SDCARD

extern const mp_obj_type_t system_sdcard_type;

typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
} mp_sdcard_obj_t;

void sdcard_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t sdcard_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_sdcard_obj_t* driver_obj = m_new_obj(mp_sdcard_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &system_sdcard_type;
    driver_obj->ModuleName = "sdcard";

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t sdcard_open(size_t n_args, const mp_obj_t * args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_sdcard_obj_t* driver_obj = (mp_sdcard_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    const char *p_path = mp_obj_str_get_str(args[1]);
    mp_int_t i_oflag = mp_obj_get_int(args[2]);
    LOG_D("%s:p_path = %s;\n", __func__, p_path);
    LOG_D("%s:i_oflag = %d;\n", __func__, i_oflag);

    int pfd = open(p_path, i_oflag);
    if (pfd == -1)
    {
        printf("[%s]: Failed to open file: %s\n", __func__, p_path);
        return mp_const_false;
    }

    return mp_obj_new_int(pfd);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_sdcard_open_obj, 3, sdcard_open);

STATIC mp_obj_t sdcard_close(size_t n_args, const mp_obj_t * args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_sdcard_obj_t* driver_obj = (mp_sdcard_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    int status = close(mp_obj_get_int(args[1]));
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_sdcard_close_obj, 2, sdcard_close);

// ssize_t read(int fd, void *buf, size_t count);
STATIC mp_obj_t sdcard_read(size_t n_args, const mp_obj_t * args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_sdcard_obj_t* driver_obj = (mp_sdcard_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    mp_int_t i_fd = mp_obj_get_int(args[1]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    memset(bufinfo.buf, 0, bufinfo.len);
    uint8_t *data = (uint8_t *)bufinfo.buf;

    LOG_D("%s:i_fd = %d;\n", __func__, i_fd);
    int size = read(i_fd, bufinfo.buf, bufinfo.len);
    for (int i = 0; i < bufinfo.len; i++)
    {
        LOG_D("%s:data[%d] = %d;\n", __func__, i, data[i]);
    }
    LOG_D("%s:size = %d;\n", __func__, size);
    return mp_obj_new_int(size);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_sdcard_read_obj, 3, sdcard_read);

// ssize_t write(int fd, const void *buf, size_t count);
STATIC mp_obj_t sdcard_write(size_t n_args, const mp_obj_t * args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_sdcard_obj_t* driver_obj = (mp_sdcard_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    mp_int_t i_fd = mp_obj_get_int(args[1]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);
    uint8_t *data = (uint8_t *)bufinfo.buf;
    for (int i = 0; i < bufinfo.len; i++)
    {
        LOG_D("%s:data[%d] = %d;\n", __func__, i, data[i]);
    }

    LOG_D("%s:i_fd = %d;\n", __func__, i_fd);
    int size = write(i_fd, bufinfo.buf, bufinfo.len);
    LOG_D("%s:size = %d;\n", __func__, size);

    return mp_obj_new_int(size);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_sdcard_write_obj, 3, sdcard_write);

STATIC const mp_rom_map_elem_t sdcard_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_Sdcard)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_sdcard_open_obj)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_sdcard_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_sdcard_read_obj)},
    {MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_sdcard_write_obj)},
};

STATIC MP_DEFINE_CONST_DICT(sdcard_locals_dict, sdcard_locals_dict_table);

const mp_obj_type_t system_sdcard_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_Sdcard,
    .print = sdcard_obj_print,
    .make_new = sdcard_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&sdcard_locals_dict,
};
#endif
