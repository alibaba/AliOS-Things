
/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <string.h>

#if MICROPY_PY_CHANNEL_ENABLE
#include "board_config.h"
#include "py/builtin.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "py_defines.h"

#define MOD_STR "ONLINE_UPGRADE"

extern int check_channel_enable(void);

static mp_obj_t online_upgradde_trigger = MP_OBJ_NULL;

int on_get_url(char *url)
{
    callback_to_python_LoBo(online_upgradde_trigger, mp_obj_new_str(url, strlen(url)), NULL);
    return 0 ;
}

STATIC mp_obj_t online_channel_register_cb(mp_obj_t func)
{
    // if (check_channel_enable() == 0)
    //     amp_otaput_init(on_get_url);
    online_upgradde_trigger = func;
    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_1(native_channel_register_cb, online_channel_register_cb);

STATIC const mp_rom_map_elem_t online_upgrade_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_online_upgrade) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_on), MP_ROM_PTR(&native_channel_register_cb) },

};

STATIC MP_DEFINE_CONST_DICT(online_upgrade_module_globals, online_upgrade_module_globals_table);

const mp_obj_module_t mp_module_online_upgrade = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&online_upgrade_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_online_upgrade, mp_module_online_upgrade, MICROPY_PY_CHANNEL_ENABLE);
#endif
