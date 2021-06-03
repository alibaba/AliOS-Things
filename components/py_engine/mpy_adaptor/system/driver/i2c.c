#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"
#include "board_mgr.h"
#include "aos_hal_i2c.h"

extern const mp_obj_type_t driver_i2c_type;

#define I2C_TIMEOUT (0xFFFFFF)

// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    item_handle_t i2c_handle;
} mp_i2c_obj_t;

void i2c_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t i2c_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_i2c_obj_t* driver_obj = m_new_obj(mp_i2c_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &driver_i2c_type;
    driver_obj->ModuleName = "i2c";
    driver_obj->i2c_handle.handle     = NULL;

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    i2c_dev_t *i2c_device = NULL;

    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_i2c_obj_t* driver_obj = (mp_i2c_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    char *id = (char *)mp_obj_str_get_str(args[1]);
    LOG_D("%s: id =%s;\n", __func__, id);

    if (id == NULL)
    {
        LOG_E("%s:illegal par id =%s;\n", __func__, id);
        return mp_const_none;
    }

    ret = py_board_mgr_init();
    if (ret != 0)
    {
        LOG_E("%s:py_board_mgr_init failed\n", __func__);
        return mp_const_none;
    }

    LOG_D("%s: py_board_mgr_init ret = %d;\n", __func__, ret);
    ret = py_board_attach_item(MODULE_I2C, id, &(driver_obj->i2c_handle));
    if (ret != 0)
    {
        LOG_E("%s: py_board_attach_item failed ret = %d;\n", __func__, ret);
        goto out;
    }

    i2c_device = py_board_get_node_by_handle(MODULE_I2C, &(driver_obj->i2c_handle));
    if (NULL == i2c_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        goto out;
    }

    LOG_D("%s: port = %d;\n", __func__, i2c_device->port);
    LOG_D("%s: address_width = %d;\n", __func__, i2c_device->config.address_width);
    LOG_D("%s: freq = %d;\n", __func__, i2c_device->config.freq);
    LOG_D("%s: mode = %d;\n", __func__, i2c_device->config.mode);
    LOG_D("%s: dev_addr = %d;\n", __func__, i2c_device->config.dev_addr);
    ret = aos_hal_i2c_init(i2c_device);

out:
	if (0 != ret) {
        LOG_E("%s: aos_hal_i2c_init failed ret = %d;\n", __func__, ret);
		py_board_disattach_item(MODULE_I2C, &(driver_obj->i2c_handle));
	}

    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(i2c_obj_open, 2, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    i2c_dev_t *i2c_device = NULL;

    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_i2c_obj_t* driver_obj = (mp_i2c_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    i2c_device = py_board_get_node_by_handle(MODULE_I2C, &(driver_obj->i2c_handle));
    if (NULL == i2c_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    ret = aos_hal_i2c_finalize(i2c_device);
    if (ret != 0)
    {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    py_board_disattach_item(MODULE_I2C, &(driver_obj->i2c_handle));
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(i2c_obj_close, 1, obj_close);

STATIC mp_obj_t obj_read(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    i2c_dev_t *i2c_device = NULL;

    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_i2c_obj_t* driver_obj = (mp_i2c_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    i2c_device = py_board_get_node_by_handle(MODULE_I2C, &(driver_obj->i2c_handle));
    if (NULL == i2c_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
    memset(bufinfo.buf, 0, bufinfo.len);

    ret = aos_hal_i2c_master_recv(i2c_device, i2c_device->config.dev_addr,
                                  bufinfo.buf, bufinfo.len, I2C_TIMEOUT);
    if (ret !=0 )
    {
        LOG_E("aos_hal_i2c_master_recv failed\n");
    }
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(i2c_obj_read, 2, obj_read);

STATIC mp_obj_t obj_write(mp_obj_t self_in, mp_obj_t write_buf)
{
    LOG_D("entern  %s;\n", __func__);
    int ret = -1;
    i2c_dev_t *i2c_device = NULL;

    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(self_in);
    mp_i2c_obj_t* driver_obj = (mp_i2c_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    i2c_device = py_board_get_node_by_handle(MODULE_I2C, &(driver_obj->i2c_handle));
    if (NULL == i2c_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

	mp_buffer_info_t src;
    mp_get_buffer_raise(write_buf, &src, MP_BUFFER_READ);
    LOG_D("%s:src.buf = %p;src.len = %d;\n", __func__, src.buf, src.len);
    ret = aos_hal_i2c_master_send(i2c_device, i2c_device->config.dev_addr,
                                  src.buf, src.len, I2C_TIMEOUT);
    if (ret != 0)
	{
		LOG_E("%s: aos_hal_i2c_master_send failed;\n", __func__);
        //return mp_const_none;
	}
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(i2c_obj_write, obj_write);

STATIC mp_obj_t obj_readReg(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    i2c_dev_t *i2c_device = NULL;
    uint16_t mem_addr;

    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_i2c_obj_t* driver_obj = (mp_i2c_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    i2c_device = py_board_get_node_by_handle(MODULE_I2C, &(driver_obj->i2c_handle));
    if (NULL == i2c_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    mem_addr = (uint16_t)mp_obj_get_int(args[1]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    memset(bufinfo.buf, 0, bufinfo.len);

    ret = aos_hal_i2c_mem_read(i2c_device, i2c_device->config.dev_addr,
                               mem_addr, 1, bufinfo.buf, bufinfo.len, I2C_TIMEOUT);
    if (ret !=0 )
    {
        LOG_E("aos_hal_i2c_mem_read failed\n");
    }
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(i2c_obj_readReg, 3, obj_readReg);

STATIC mp_obj_t obj_writeReg(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    i2c_dev_t *i2c_device = NULL;
    uint16_t mem_addr;

    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_i2c_obj_t* driver_obj = (mp_i2c_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    i2c_device = py_board_get_node_by_handle(MODULE_I2C, &(driver_obj->i2c_handle));
    if (NULL == i2c_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    mem_addr = (uint16_t)mp_obj_get_int(args[1]);
	mp_buffer_info_t src;
    mp_get_buffer_raise(args[2], &src, MP_BUFFER_READ);
    LOG_D("%s:src.buf = %p;src.len = %d;\n", __func__, src.buf, src.len);
    ret = aos_hal_i2c_mem_write(i2c_device, i2c_device->config.dev_addr,
                               mem_addr, 1, src.buf, src.len, I2C_TIMEOUT);
    if (ret != 0)
	{
        LOG_E("%s:aos_hal_i2c_mem_write failed\n", __func__);
	}
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(i2c_obj_writeReg, 3, obj_writeReg);

STATIC const mp_rom_map_elem_t i2c_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_I2C)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&i2c_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&i2c_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&i2c_obj_read)},
    {MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&i2c_obj_write)},
    {MP_ROM_QSTR(MP_QSTR_readReg), MP_ROM_PTR(&i2c_obj_readReg)},
    {MP_ROM_QSTR(MP_QSTR_writeReg), MP_ROM_PTR(&i2c_obj_writeReg)},
};

STATIC MP_DEFINE_CONST_DICT(i2c_locals_dict, i2c_locals_dict_table);

const mp_obj_type_t driver_i2c_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_I2C,
    .print = i2c_obj_print,
    .make_new = i2c_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&i2c_locals_dict,
};

