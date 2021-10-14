#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos_hal_adc.h"
#include "board_mgr.h"
#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

#define LOG_TAG "DRIVER_ADC"

extern const mp_obj_type_t driver_adc_type;

// this is the actual C-structure for our new object
typedef struct {
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    item_handle_t adc_handle;
} mp_adc_obj_t;

void adc_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t adc_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_adc_obj_t *driver_obj = m_new_obj(mp_adc_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }
    memset(driver_obj, 0, sizeof(mp_adc_obj_t));

    driver_obj->Base.type = &driver_adc_type;
    driver_obj->ModuleName = "adc";
    driver_obj->adc_handle.handle = NULL;

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t adc_open(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;
    if (n_args < 2) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return MP_OBJ_NEW_SMALL_INT (-MP_E2BIG);
    }

    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_adc_obj_t *driver_obj = (mp_adc_obj_t *)self;
    if (driver_obj == NULL) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return MP_OBJ_NEW_SMALL_INT (-MP_EINVAL);
    }

    char *id = (char *)mp_obj_str_get_str(args[1]);
    if (id == NULL) {
        LOGE(LOG_TAG, "%s:illegal par id =%s;\n", __func__, id);
        return MP_OBJ_NEW_SMALL_INT (-MP_EINVAL);
    }

    ret = py_board_mgr_init();
    if (ret != 0) {
        LOGE(LOG_TAG, "%s:py_board_mgr_init failed\n", __func__);
        return MP_OBJ_NEW_SMALL_INT (-MP_ENOENT);
    }

    ret = py_board_attach_item(MODULE_ADC, id, &(driver_obj->adc_handle));
    if (ret != 0) {
        LOGE(LOG_TAG, "%s: py_board_attach_item failed ret = %d;\n", __func__, ret);
        goto out;
    }

    adc_dev_t *adc_device = py_board_get_node_by_handle(MODULE_ADC, &(driver_obj->adc_handle));
    if (NULL == adc_device) {
        LOGE(LOG_TAG, "%s: py_board_get_node_by_handle failed;\n", __func__);
        goto out;
    }

    ret = aos_hal_adc_init(adc_device);

out:
    if (0 != ret) {
        LOGE(LOG_TAG, "%s: adc open failed ret = %d;\n", __func__, ret);
        py_board_disattach_item(MODULE_ADC, &(driver_obj->adc_handle));
    }

    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(adc_open_obj, 2, adc_open);

STATIC mp_obj_t adc_close(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;
    adc_dev_t *adc_device = NULL;
    if (n_args < 1) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return MP_OBJ_NEW_SMALL_INT (-MP_E2BIG);
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_adc_obj_t *driver_obj = (mp_adc_obj_t *)self;
    if (driver_obj == NULL) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return MP_OBJ_NEW_SMALL_INT (-MP_EINVAL);
    }

    adc_device = py_board_get_node_by_handle(MODULE_ADC, &(driver_obj->adc_handle));
    if (NULL == adc_device) {
        LOGE(LOG_TAG, "%s: py_board_get_node_by_handle failed;\n", __func__);
        return MP_OBJ_NEW_SMALL_INT (-MP_ENODEV);
    }
    ret = aos_hal_adc_finalize(adc_device);
    py_board_disattach_item(MODULE_ADC, &(driver_obj->adc_handle));

    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(adc_close_obj, 1, adc_close);

STATIC mp_obj_t adc_read(size_t n_args, const mp_obj_t *args)
{
    adc_dev_t *adc_device = NULL;
    if (n_args < 1) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return MP_OBJ_NEW_SMALL_INT(-MP_E2BIG);
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_adc_obj_t *driver_obj = (mp_adc_obj_t *)self;
    if (driver_obj == NULL) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return MP_OBJ_NEW_SMALL_INT (-MP_EINVAL);
    }
    adc_device = py_board_get_node_by_handle(MODULE_ADC, &(driver_obj->adc_handle));
    if (NULL == adc_device) {
        LOGE(LOG_TAG, "%s: py_board_get_node_by_handle failed;\n", __func__);
        return MP_OBJ_NEW_SMALL_INT (-MP_ENODEV);
    }

    int32_t adc_value = -1;
    int32_t ret = aos_hal_adc_value_get(adc_device, &adc_value, 0);
    if(ret != 0) {
        return MP_OBJ_NEW_SMALL_INT(ret);
    } else {
        return MP_OBJ_NEW_SMALL_INT(adc_value);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(adc_read_obj, 1, adc_read);

STATIC const mp_rom_map_elem_t adc_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ADC) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&adc_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&adc_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&adc_read_obj) },
};

STATIC MP_DEFINE_CONST_DICT(adc_locals_dict, adc_locals_dict_table);

const mp_obj_type_t driver_adc_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = adc_obj_print,
    .make_new = adc_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&adc_locals_dict,
};
