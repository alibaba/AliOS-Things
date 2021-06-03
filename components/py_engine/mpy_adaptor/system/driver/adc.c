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
#include "aos_hal_adc.h"

extern const mp_obj_type_t driver_adc_type;

// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    item_handle_t adc_handle;
} mp_adc_obj_t;

void adc_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t adc_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_adc_obj_t* driver_obj = m_new_obj(mp_adc_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &driver_adc_type;
    driver_obj->ModuleName = "adc";
    driver_obj->adc_handle.handle     = NULL;

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_adc_obj_t* driver_obj = (mp_adc_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    adc_dev_t *adc_device = NULL;

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
    ret = py_board_attach_item(MODULE_ADC, id, &(driver_obj->adc_handle));
    if (ret != 0)
    {
        LOG_E("%s: py_board_attach_item failed ret = %d;\n", __func__, ret);
        goto out;
    }

    adc_device = py_board_get_node_by_handle(MODULE_ADC, &(driver_obj->adc_handle));
    if (NULL == adc_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        goto out;
    }

    LOG_D("%s: port = %d;sampling_cycle = %d;\n", __func__,
            adc_device->port, adc_device->config.sampling_cycle);
    ret = aos_hal_adc_init(adc_device);

out:
	if (0 != ret) {
        LOG_E("%s: aos_hal_adc_init failed ret = %d;\n", __func__, ret);
		py_board_disattach_item(MODULE_ADC, &(driver_obj->adc_handle));
	}

    LOG_D("%s:out\n", __func__);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(adc_obj_open, 2, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    adc_dev_t *adc_device = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_adc_obj_t* driver_obj = (mp_adc_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    adc_device = py_board_get_node_by_handle(MODULE_ADC, &(driver_obj->adc_handle));
    if (NULL == adc_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }
    ret = aos_hal_adc_finalize(adc_device);
    py_board_disattach_item(MODULE_ADC, &(driver_obj->adc_handle));

    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(adc_obj_close, 1, obj_close);

STATIC mp_obj_t obj_read(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    adc_dev_t *adc_device = NULL;
    int32_t adc_value = -1;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_adc_obj_t* driver_obj = (mp_adc_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    adc_device = py_board_get_node_by_handle(MODULE_ADC, &(driver_obj->adc_handle));
    if (NULL == adc_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    (void)aos_hal_adc_value_get(adc_device, (void *)&adc_value, 0);
    LOG_D("%s:out adc_value = %d;\n", __func__, adc_value);

    return MP_ROM_INT(adc_value);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(adc_obj_read, 1, obj_read);

STATIC const mp_rom_map_elem_t adc_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ADC)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&adc_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&adc_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&adc_obj_read)},
};

STATIC MP_DEFINE_CONST_DICT(adc_locals_dict, adc_locals_dict_table);

const mp_obj_type_t driver_adc_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_ADC,
    .print = adc_obj_print,
    .make_new = adc_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&adc_locals_dict,
};

