#include "py/runtime.h"
#include "lvgl_driver.h"

STATIC mp_obj_t init() {
	lvgl_init(NULL);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(init_obj, init);

STATIC const mp_rom_map_elem_t ZDD_module_globals_table[] = {
	{ MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ZDD) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&init_obj) },
};
STATIC MP_DEFINE_CONST_DICT(ZDD_module_globals, ZDD_module_globals_table);

const mp_obj_module_t ZDD_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&ZDD_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_ZDD, ZDD_user_cmodule, 1);
