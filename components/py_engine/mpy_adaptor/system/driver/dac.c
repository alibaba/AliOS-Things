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

extern const mp_obj_type_t driver_dac_type;

// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    item_handle_t dac_handle;
} mp_dac_obj_t;

void dac_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_dac_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t dac_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_dac_obj_t* driver_obj = m_new_obj(mp_dac_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &driver_dac_type;
    driver_obj->ModuleName = "dac";
    driver_obj->dac_handle.handle     = NULL;

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    dac_dev_t *dac_device = NULL;

    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_dac_obj_t* driver_obj = (mp_dac_obj_t *)self;
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
    ret = py_board_attach_item(MODULE_DAC, id, &(driver_obj->dac_handle));
    if (ret != 0)
    {
        LOG_E("%s: py_board_attach_item failed ret = %d;\n", __func__, ret);
        goto out;
    }

    dac_device = py_board_get_node_by_handle(MODULE_DAC, &(driver_obj->dac_handle));
    if (NULL == dac_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        goto out;
    }

    LOG_D("%s: port = %d;\n", __func__, dac_device->port);
#if 0
    ret = hal_dac_init(dac_device);
    if (ret != 0)
    {
        LOG_E("hal_dac_init failed\n");
        goto out;
    }

    ret = hal_dac_start(dac_device, dac_device->port);
    if (ret != 0)
    {
        LOG_E("hal_dac_start failed\n");
    }
#endif
out:
	if (0 != ret) {
		py_board_disattach_item(MODULE_DAC, &(driver_obj->dac_handle));
	}

    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(dac_obj_open, 2, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    dac_dev_t *dac_device = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_dac_obj_t* driver_obj = (mp_dac_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    dac_device = py_board_get_node_by_handle(MODULE_DAC, &(driver_obj->dac_handle));
    if (NULL == dac_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    //hal_dac_stop(dac_device, dac_device->port);
    //ret = hal_dac_finalize(dac_device);
    py_board_disattach_item(MODULE_DAC, &(driver_obj->dac_handle));
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(dac_obj_close, 1, obj_close);

STATIC mp_obj_t obj_getVol(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    dac_dev_t *dac_device = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_dac_obj_t* driver_obj = (mp_dac_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    dac_device = py_board_get_node_by_handle(MODULE_DAC, &(driver_obj->dac_handle));
    if (NULL == dac_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    //ret = (int)hal_dac_get_value(dac_device, dac_device->port);
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(dac_obj_getVol, 1, obj_getVol);

STATIC mp_obj_t obj_setVol(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    uint32_t voltage = 0;
    dac_dev_t *dac_device = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_dac_obj_t* driver_obj = (mp_dac_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    dac_device = py_board_get_node_by_handle(MODULE_DAC, &(driver_obj->dac_handle));
    if (NULL == dac_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    voltage = (uint32_t)mp_obj_get_int(args[1]);
    //ret     = hal_dac_set_value(dac_device, dac_device->port, voltage);
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(dac_obj_setVol, 2, obj_setVol);

STATIC const mp_rom_map_elem_t dac_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_DAC)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&dac_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&dac_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_getVol), MP_ROM_PTR(&dac_obj_getVol)},
    {MP_ROM_QSTR(MP_QSTR_setVol), MP_ROM_PTR(&dac_obj_setVol)},
};

STATIC MP_DEFINE_CONST_DICT(dac_locals_dict, dac_locals_dict_table);

const mp_obj_type_t driver_dac_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_DAC,
    .print = dac_obj_print,
    .make_new = dac_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&dac_locals_dict,
};

