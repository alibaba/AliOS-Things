/* LVGL Example project
 *
 * Basic project to test LVGL on ESP32 based projects.
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */
#include <stdio.h>
#include <string.h>

#if MICROPY_PY_DISPLAY

#include "py/objstr.h"
#include "py/runtime.h"

extern const mp_obj_type_t display_tft_type;
extern const mp_obj_type_t display_lvgl_type;

STATIC const mp_rom_map_elem_t display_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_display) },
#if MICROPY_PY_DISPLAY_TFT
    { MP_OBJ_NEW_QSTR(MP_QSTR_TFT), MP_ROM_PTR(&display_tft_type) },
#endif
#if MICROPY_PY_DISPLAY_LVGL
    { MP_OBJ_NEW_QSTR(MP_QSTR_LVGL), MP_ROM_PTR(&display_lvgl_type) },
#endif
};
STATIC MP_DEFINE_CONST_DICT(mp_module_display_globals, display_globals_table);

const mp_obj_module_t mp_module_display = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_display_globals 
};
MP_REGISTER_MODULE(MP_QSTR_display, mp_module_display, MICROPY_PY_DISPLAY);

#endif  // MICROPY_PY_DISPLAY
