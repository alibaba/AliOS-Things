/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos_hal_wdg.h"
#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ulog/ulog.h"
#include "board_mgr.h"

#define LOG_TAG "DRIVER_WDT"

#define WDT_CHECK_PARAMS(count)                                                       \
    if (n_args < count) {                                                             \
        LOGE(LOG_TAG, "%s: args count is illegal: n_args = %d;\n", __func__, n_args); \
        return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);                                       \
    }                                                                                 \
    mp_wdg_obj_t *driver_obj = (mp_wdg_obj_t *)MP_OBJ_TO_PTR(args[0]);                \
    do {                                                                              \
        if (driver_obj == NULL) {                                                     \
            LOGE(LOG_TAG, "driver_obj[WDT] is NULL\n");                               \
            return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);                                  \
        }                                                                             \
    } while (0)

#define WDT_NODE_GET()                                                                          \
    wdg_dev_t *wdt_device = py_board_get_node_by_handle(MODULE_WDT, &(driver_obj->wdg_dev)); \
    do {                                                                                        \
        if (NULL == wdt_device) {                                                               \
            LOGE(LOG_TAG, "%s: py_board_get_node_by_handle failed;\n", __func__);               \
            py_board_disattach_item(MODULE_WDT, &(driver_obj->wdg_dev));                     \
            return MP_OBJ_NEW_SMALL_INT(-MP_ENXIO);                                             \
        }                                                                                       \
    } while (0)

extern const mp_obj_type_t driver_wdt_type;

// this is the actual C-structure for our new object
typedef struct {
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    wdg_dev_t wdg_dev;
} mp_wdg_obj_t;

void wdg_obj_print(const mp_print_t *print, mp_obj_t self_in,
                   mp_print_kind_t kind)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
    mp_wdg_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t wdg_obj_make_new(const mp_obj_type_t *type, size_t n_args,
                                 size_t n_kw, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s;\n", __func__);
    mp_wdg_obj_t *driver_obj = m_new_obj(mp_wdg_obj_t);
    if (!driver_obj)
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Memory malloc failed"));

    driver_obj->Base.type = &driver_wdt_type;
    driver_obj->ModuleName = "wdt";

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    char *id = NULL;

    WDT_CHECK_PARAMS(2);

    if (mp_obj_is_str(args[1])) {
        id = (char *)mp_obj_str_get_str(args[1]);
    } else {
        LOGE(LOG_TAG, "%s: 2nd arg should be string\n", __func__);
        return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
    }

    ret = py_board_mgr_init();
    if (ret != 0) {
        LOGE(LOG_TAG, "%s:py_board_mgr_init failed\n", __func__);
        return MP_OBJ_NEW_SMALL_INT(-MP_ENOENT);
    }

    ret = py_board_attach_item(MODULE_WDT, id, &(driver_obj->wdg_dev));
    if (ret != 0) {
        LOGE(LOG_TAG, "%s: py_board_attach_item[%s] failed, ret = %d;\n", __func__, id, ret);
        goto out;
    }

    WDT_NODE_GET();
    LOGD(LOG_TAG, "%s: timeout = %d;\n", __func__, wdt_device->config.timeout);

    ret = aos_hal_wdg_init(wdt_device);

out:
    if (ret != 0) {
        LOGE(LOG_TAG, "%s: adc open failed ret = %d;\n", __func__, ret);
        py_board_disattach_item(MODULE_WDT, &(driver_obj->wdg_dev));
    }

    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(wdt_obj_open, 2, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;

    WDT_CHECK_PARAMS(1);
    WDT_NODE_GET();

    ret = aos_hal_wdg_finalize(wdt_device);
    py_board_disattach_item(MODULE_WDT, &(driver_obj->wdg_dev));

    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(wdt_obj_close, 1, obj_close);

STATIC mp_obj_t obj_feed(size_t n_args, const mp_obj_t *args)
{
    WDT_CHECK_PARAMS(1);
    WDT_NODE_GET();

    aos_hal_wdg_reload(wdt_device);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(wdt_obj_feed, 1, obj_feed);

STATIC const mp_rom_map_elem_t wdg_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_WDT) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&wdt_obj_open) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&wdt_obj_close) },
    { MP_ROM_QSTR(MP_QSTR_feed), MP_ROM_PTR(&wdt_obj_feed) },
};

STATIC MP_DEFINE_CONST_DICT(wdg_locals_dict, wdg_locals_dict_table);

const mp_obj_type_t driver_wdt_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_WDT,
    .print = wdg_obj_print,
    .make_new = wdg_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&wdg_locals_dict,
};
