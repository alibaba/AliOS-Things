#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"
#include "amp_defines.h"
#include "board_mgr.h"
#include "amp_hal_timer.h"

extern const mp_obj_type_t driver_timer_type;

// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    item_handle_t timer_handle;
} mp_timer_obj_t;

void timer_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t timer_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_timer_obj_t* driver_obj = m_new_obj(mp_timer_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &driver_timer_type;
    driver_obj->ModuleName = "timer";
    driver_obj->timer_handle.handle     = NULL;

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    timer_dev_t *timer_device = NULL;

    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_timer_obj_t* driver_obj = (mp_timer_obj_t *)self;
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

    ret = board_mgr_init();
    if (ret != 0)
    {
        LOG_E("%s:board_mgr_init failed\n", __func__);
        return mp_const_none;
    }

    LOG_D("%s: board_mgr_init ret = %d;\n", __func__, ret);
    ret = board_attach_item(MODULE_TIMER, id, &(driver_obj->timer_handle));
    if (ret != 0)
    {
        LOG_E("%s: board_attach_item failed ret = %d;\n", __func__, ret);
        goto out;
    }

    timer_device = board_get_node_by_handle(MODULE_TIMER, &(driver_obj->timer_handle));
    if (NULL == timer_device) {
		LOG_E("%s: board_get_node_by_handle failed;\n", __func__);
        goto out;
    }

    LOG_D("%s: port = %d;\n", __func__, timer_device->port);
    ret = amp_hal_timer_init(timer_device);

out:
	if (0 != ret) {
        LOG_E("%s: amp_hal_timer_init failed ret = %d;\n", __func__, ret);
		board_disattach_item(MODULE_TIMER, &(driver_obj->timer_handle));
	}

    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(timer_obj_open, 2, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    timer_dev_t *timer_device = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_timer_obj_t* driver_obj = (mp_timer_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    timer_device = board_get_node_by_handle(MODULE_TIMER, &(driver_obj->timer_handle));
    if (NULL == timer_device) {
		LOG_E("%s: board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    amp_hal_timer_stop(timer_device);
    amp_hal_timer_finalize(timer_device);
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(timer_obj_close, 1, obj_close);

STATIC mp_obj_t obj_setTimeout(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    timer_dev_t *timer_device = NULL;
    uint32_t timeout = 0;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_timer_obj_t* driver_obj = (mp_timer_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    timer_device = board_get_node_by_handle(MODULE_TIMER, &(driver_obj->timer_handle));
    if (NULL == timer_device) {
		LOG_E("%s: board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    timeout = (uint32_t)mp_obj_get_int(args[1]);
    LOG_D("%s:timeout = %d;\n", __func__, timeout);
    timer_device->config.reload_mode = TIMER_RELOAD_MANU;
    timer_device->config.period = timeout;
    timer_device->config.cb = NULL;//TODO
    timer_device->config.arg = 0;//TODO

    amp_hal_timer_stop(timer_device);
    ret = amp_hal_timer_start(timer_device);
    if (ret != 0) {
        LOG_E("%s:amp_hal_timer_start failed\n", __func__);
        return mp_const_none;
    }

    //TODO
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(timer_obj_setTimeout, 2, obj_setTimeout);

STATIC mp_obj_t obj_clearTimeout(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    timer_dev_t *timer_device = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_timer_obj_t* driver_obj = (mp_timer_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    timer_device = board_get_node_by_handle(MODULE_TIMER, &(driver_obj->timer_handle));
    if (NULL == timer_device) {
		LOG_E("%s: board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    timer_device->config.reload_mode = TIMER_RELOAD_MANU;
    amp_hal_timer_stop(timer_device);

    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(timer_obj_clearTimeout, 1, obj_clearTimeout);

STATIC mp_obj_t obj_setInterval(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    timer_dev_t *timer_device = NULL;
    uint32_t timeout = 0;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_timer_obj_t* driver_obj = (mp_timer_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    timer_device = board_get_node_by_handle(MODULE_TIMER, &(driver_obj->timer_handle));
    if (NULL == timer_device) {
		LOG_E("%s: board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    timeout = (uint32_t)mp_obj_get_int(args[1]);
    LOG_D("%s:timeout = %d;\n", __func__, timeout);
    timer_device->config.reload_mode = TIMER_RELOAD_AUTO;
    timer_device->config.period = timeout;
    timer_device->config.cb = NULL;//TODO
    timer_device->config.arg = 0;//TODO

    ret = amp_hal_timer_start(timer_device);
    if (ret != 0) {
        LOG_E("%s:amp_hal_timer_start failed\n", __func__);
        return mp_const_none;
    }

    //TODO
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(timer_obj_setInterval, 2, obj_setInterval);

STATIC mp_obj_t obj_clearInterval(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    timer_dev_t *timer_device = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_timer_obj_t* driver_obj = (mp_timer_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    timer_device = board_get_node_by_handle(MODULE_TIMER, &(driver_obj->timer_handle));
    if (NULL == timer_device) {
		LOG_E("%s: board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    timer_device->config.reload_mode = TIMER_RELOAD_AUTO;
    amp_hal_timer_stop(timer_device);

    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(timer_obj_clearInterval, 1, obj_clearInterval);

STATIC const mp_rom_map_elem_t timer_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_TIMER)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&timer_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&timer_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_setTimeout), MP_ROM_PTR(&timer_obj_setTimeout)},
    {MP_ROM_QSTR(MP_QSTR_clearTimeout), MP_ROM_PTR(&timer_obj_clearTimeout)},
    {MP_ROM_QSTR(MP_QSTR_setInterval), MP_ROM_PTR(&timer_obj_setInterval)},
    {MP_ROM_QSTR(MP_QSTR_clearInterval), MP_ROM_PTR(&timer_obj_clearInterval)},
};

STATIC MP_DEFINE_CONST_DICT(timer_locals_dict, timer_locals_dict_table);

const mp_obj_type_t driver_timer_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_TIMER,
    .print = timer_obj_print,
    .make_new = timer_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&timer_locals_dict,
};

