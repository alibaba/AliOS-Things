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
#include "aos_hal_spi.h"

extern const mp_obj_type_t driver_spi_type;

#define SPI_TIMEOUT (0xFFFFFF)
// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    item_handle_t spi_handle;
} mp_spi_obj_t;

void spi_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t spi_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_spi_obj_t* driver_obj = m_new_obj(mp_spi_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &driver_spi_type;
    driver_obj->ModuleName = "spi";
    driver_obj->spi_handle.handle     = NULL;

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    spi_dev_t *spi_device = NULL;

    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_spi_obj_t* driver_obj = (mp_spi_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    char *id = (char *)mp_obj_str_get_str(args[1]);
    LOG_D("%s:id =%s;\n", __func__, id);

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
    ret = py_board_attach_item(MODULE_SPI, id, &(driver_obj->spi_handle));
    if (ret != 0)
    {
        LOG_E("%s: py_board_attach_item failed ret = %d;\n", __func__, ret);
        goto out;
    }

    spi_device = py_board_get_node_by_handle(MODULE_SPI, &(driver_obj->spi_handle));
    if (NULL == spi_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        goto out;
    }

    LOG_D("%s: port = %d;\n", __func__, spi_device->port);
    LOG_D("%s: spi_device = %p;\n", __func__, spi_device);
    LOG_D("%s: role = %d;\n", __func__, spi_device->config.role);
    LOG_D("%s: firstbit = %d;\n", __func__, spi_device->config.firstbit);
    LOG_D("%s: mode = %d;\n", __func__, spi_device->config.mode);
    LOG_D("%s: t_mode = %d;\n", __func__, spi_device->config.t_mode);
    LOG_D("%s: freq = %d;\n", __func__, spi_device->config.freq);
    LOG_D("%s: serial_len = %d;\n", __func__, spi_device->config.serial_len);
    LOG_D("%s: data_size = %d;\n", __func__, spi_device->config.data_size);
    LOG_D("%s: cs = %d;\n", __func__, spi_device->config.cs);
    ret = aos_hal_spi_init(spi_device);

out:
	if (0 != ret) {
        LOG_E("%s: aos_hal_spi_init failed ret = %d;\n", __func__, ret);
		py_board_disattach_item(MODULE_SPI, &(driver_obj->spi_handle));
	}

    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(spi_obj_open, 2, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    spi_dev_t *spi_device = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_spi_obj_t* driver_obj = (mp_spi_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    spi_device = py_board_get_node_by_handle(MODULE_SPI, &(driver_obj->spi_handle));
    if (NULL == spi_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    ret = aos_hal_spi_finalize(spi_device);
    if (ret != 0) {
		LOG_E("%s: aos_hal_spi_finalize failed;\n", __func__);
    }

    py_board_disattach_item(MODULE_SPI, &(driver_obj->spi_handle));
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(spi_obj_close, 1, obj_close);

STATIC mp_obj_t obj_read(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    spi_dev_t *spi_device = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_spi_obj_t* driver_obj = (mp_spi_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    spi_device = py_board_get_node_by_handle(MODULE_SPI, &(driver_obj->spi_handle));
    if (NULL == spi_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
    memset(bufinfo.buf, 0, bufinfo.len);

    ret = aos_hal_spi_recv(spi_device, bufinfo.buf, bufinfo.len, SPI_TIMEOUT);
    if (ret == -1)
    {
		LOG_E("%s: aos_hal_spi_recv failed;\n", __func__);
    }
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(spi_obj_read, 2, obj_read);

STATIC mp_obj_t obj_write(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    spi_dev_t *spi_device = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_spi_obj_t* driver_obj = (mp_spi_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    spi_device = py_board_get_node_by_handle(MODULE_SPI, &(driver_obj->spi_handle));
    if (NULL == spi_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_READ);
    // memset(bufinfo.buf, 0, bufinfo.len);

    ret = aos_hal_spi_send(spi_device, bufinfo.buf, bufinfo.len, SPI_TIMEOUT);
    if (ret == -1)
    {
		LOG_E("%s: aos_hal_spi_send failed;\n", __func__);
    }
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(spi_obj_write, 2, obj_write);

STATIC mp_obj_t obj_sendRecv(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    spi_dev_t *spi_device = NULL;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_spi_obj_t* driver_obj = (mp_spi_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    spi_device = py_board_get_node_by_handle(MODULE_SPI, &(driver_obj->spi_handle));
    if (NULL == spi_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    mp_buffer_info_t write_bufinfo;
    mp_get_buffer_raise(args[1], &write_bufinfo, MP_BUFFER_READ);

    mp_buffer_info_t read_bufinfo;
    mp_get_buffer_raise(args[2], &read_bufinfo, MP_BUFFER_WRITE);
    memset(read_bufinfo.buf, 0, read_bufinfo.len);

    ret = aos_hal_spi_send_recv(spi_device, write_bufinfo.buf,
           read_bufinfo.buf, read_bufinfo.len, SPI_TIMEOUT);
    if (ret == -1)
    {
        LOG_E("aos_hal_spi_send_recv failed\n");
    }
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(spi_obj_sendRecv, 3, obj_sendRecv);

STATIC const mp_rom_map_elem_t spi_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_SPI)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&spi_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&spi_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&spi_obj_read)},
    {MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&spi_obj_write)},
    {MP_ROM_QSTR(MP_QSTR_sendRecv), MP_ROM_PTR(&spi_obj_sendRecv)},
};

STATIC MP_DEFINE_CONST_DICT(spi_locals_dict, spi_locals_dict_table);

const mp_obj_type_t driver_spi_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_SPI,
    .print = spi_obj_print,
    .make_new = spi_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&spi_locals_dict,
};

