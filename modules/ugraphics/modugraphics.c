#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if MICROPY_PY_UGRAPHICS

#include "py/builtin.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "ugraphics.h"
#include "ulog/ulog.h"

#define LOG_TAG "ugraphics"

STATIC mp_obj_t mp_ugraphics_init(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 2) {
        mp_raise_OSError(EINVAL);
        return mp_const_none;
    }

    int32_t width = 0;
    int32_t height = 0;

    width = mp_obj_get_int(args[0]);
    height = mp_obj_get_int(args[1]);

    mp_int_t status = ugraphics_init(width, height);

    return MP_OBJ_NEW_SMALL_INT(-status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_ugraphics_init_obj, 2, mp_ugraphics_init);

STATIC mp_obj_t mp_ugraphics_deinit(size_t n_args, const mp_obj_t *args)
{
    ugraphics_quit();

    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_ugraphics_deinit_obj, 0, mp_ugraphics_deinit);

STATIC mp_obj_t mp_ugraphics_loadFont(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 2) {
        mp_raise_OSError(EINVAL);
        return mp_const_none;
    }

    int32_t size = 0;

    char *path = (char *)mp_obj_str_get_str(args[0]);
    size = mp_obj_get_int(args[1]);

    mp_int_t status = ugraphics_load_font(path, size);

    return MP_OBJ_NEW_SMALL_INT(-status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_ugraphics_loadFont_obj, 2, mp_ugraphics_loadFont);

STATIC mp_obj_t mp_ugraphics_setFontSytle(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 1) {
        mp_raise_OSError(EINVAL);
        return mp_const_none;
    }

    int32_t style = 0;

    style = mp_obj_get_int(args[0]);

    ugraphics_set_font_style(style);

    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_ugraphics_setFontSytle_obj, 1, mp_ugraphics_setFontSytle);

STATIC mp_obj_t mp_ugraphics_ugraphicsFlip(size_t n_args, const mp_obj_t *args)
{
    ugraphics_flip();

    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_ugraphics_ugraphicsFlip_obj, 0, mp_ugraphics_ugraphicsFlip);

STATIC mp_obj_t mp_ugraphics_ugraphicsClear(size_t n_args, const mp_obj_t *args)
{
    mp_int_t status = ugraphics_clear();

    return MP_OBJ_NEW_SMALL_INT(-status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_ugraphics_ugraphicsClear_obj, 0, mp_ugraphics_ugraphicsClear);

STATIC mp_obj_t mp_ugraphics_setColour(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 1) {
        mp_raise_OSError(EINVAL);
        return mp_const_none;
    }

    int32_t colour = 0;

    colour = mp_obj_get_int(args[0]);

    mp_int_t status = ugraphics_set_color(colour);

    return MP_OBJ_NEW_SMALL_INT(-status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_ugraphics_setColour_obj, 1, mp_ugraphics_setColour);

STATIC mp_obj_t mp_ugraphics_drawRect(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 4) {
        mp_raise_OSError(EINVAL);
        return mp_const_none;
    }

    int32_t x = 0;
    int32_t y = 0;
    int32_t w = 0;
    int32_t h = 0;

    x = mp_obj_get_int(args[0]);
    y = mp_obj_get_int(args[1]);
    w = mp_obj_get_int(args[2]);
    h = mp_obj_get_int(args[3]);

    mp_int_t status = ugraphics_draw_rect(x, y, w, h);

    return MP_OBJ_NEW_SMALL_INT(-status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_ugraphics_drawRect_obj, 4, mp_ugraphics_drawRect);

STATIC mp_obj_t mp_ugraphics_fillRect(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 4) {
        mp_raise_OSError(EINVAL);
        return mp_const_none;
    }

    int32_t x = 0;
    int32_t y = 0;
    int32_t w = 0;
    int32_t h = 0;

    x = mp_obj_get_int(args[0]);
    y = mp_obj_get_int(args[1]);
    w = mp_obj_get_int(args[2]);
    h = mp_obj_get_int(args[3]);

    mp_int_t status = ugraphics_fill_rect(x, y, w, h);

    return MP_OBJ_NEW_SMALL_INT(-status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_ugraphics_fillRect_obj, 4, mp_ugraphics_fillRect);

STATIC mp_obj_t mp_ugraphics_drawLine(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 4) {
        mp_raise_OSError(EINVAL);
        return mp_const_none;
    }

    int32_t x1 = 0;
    int32_t y1 = 0;
    int32_t x2 = 0;
    int32_t y2 = 0;

    x1 = mp_obj_get_int(args[0]);
    y1 = mp_obj_get_int(args[1]);
    x2 = mp_obj_get_int(args[2]);
    y2 = mp_obj_get_int(args[3]);

    mp_int_t status = ugraphics_draw_line(x1, y1, x2, y2);

    return MP_OBJ_NEW_SMALL_INT(-status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_ugraphics_drawLine_obj, 4, mp_ugraphics_drawLine);

STATIC mp_obj_t mp_ugraphics_drawString(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 3) {
        mp_raise_OSError(EINVAL);
        return mp_const_none;
    }

    int32_t x = 0;
    int32_t y = 0;

    char *string = (char *)mp_obj_str_get_str(args[0]);

    x = mp_obj_get_int(args[1]);
    y = mp_obj_get_int(args[2]);

    mp_int_t status = ugraphics_draw_string(string, x, y);

    return MP_OBJ_NEW_SMALL_INT(-status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_ugraphics_drawString_obj, 3, mp_ugraphics_drawString);

STATIC mp_obj_t mp_ugraphics_stringWidth(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 1) {
        mp_raise_OSError(EINVAL);
        return mp_const_none;
    }
    char *string = (char *)mp_obj_str_get_str(args[0]);

    mp_int_t ret = ugraphics_string_width(string);

    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_ugraphics_stringWidth_obj, 1, mp_ugraphics_stringWidth);

STATIC mp_obj_t mp_ugraphics_saveImage(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 3) {
        mp_raise_OSError(EINVAL);
        return mp_const_none;
    }

    int32_t len = 0;

    len = mp_obj_get_int(args[1]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_WRITE);
    if (bufinfo.len < len * sizeof(uint16_t)) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("bytearray size should not smaller than "
                                                        "twice of reg_quantity"));
        return mp_const_none;
    }

    char *path = (char *)mp_obj_str_get_str(args[2]);

    mp_int_t status = ugraphics_save_image(bufinfo.buf, len, path);

    return MP_OBJ_NEW_SMALL_INT(-status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_ugraphics_saveImage_obj, 3, mp_ugraphics_saveImage);

STATIC mp_obj_t mp_ugraphics_drawImage(size_t n_args, const mp_obj_t *args)
{
    if (n_args < 3) {
        mp_raise_OSError(EINVAL);
        return mp_const_none;
    }

    int32_t x = 0;
    int32_t y = 0;

    char *path = (char *)mp_obj_str_get_str(args[0]);
    x = mp_obj_get_int(args[1]);
    y = mp_obj_get_int(args[2]);

    mp_int_t status = ugraphics_draw_image(path, x, y);

    return MP_OBJ_NEW_SMALL_INT(-status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_ugraphics_drawImage_obj, 3, mp_ugraphics_drawImage);

STATIC const mp_rom_map_elem_t ugraphics_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ugraphics) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_ugraphics_init_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&mp_ugraphics_deinit_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_loadFont), MP_ROM_PTR(&mp_ugraphics_loadFont_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_setFontStyle), MP_ROM_PTR(&mp_ugraphics_setFontSytle_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ugraphicsFlip), MP_ROM_PTR(&mp_ugraphics_ugraphicsFlip_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ugraphicsClear), MP_ROM_PTR(&mp_ugraphics_ugraphicsClear_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_setColour), MP_ROM_PTR(&mp_ugraphics_setColour_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_drawRect), MP_ROM_PTR(&mp_ugraphics_drawRect_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_fillRect), MP_ROM_PTR(&mp_ugraphics_fillRect_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_drawLine), MP_ROM_PTR(&mp_ugraphics_drawLine_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_drawString), MP_ROM_PTR(&mp_ugraphics_drawString_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stringWidth), MP_ROM_PTR(&mp_ugraphics_stringWidth_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_saveImage), MP_ROM_PTR(&mp_ugraphics_saveImage_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_drawImage), MP_ROM_PTR(&mp_ugraphics_drawImage_obj) },
};

STATIC MP_DEFINE_CONST_DICT(ugraphics_module_globals, ugraphics_module_globals_table);

const mp_obj_module_t ugraphics_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&ugraphics_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_ugraphics, ugraphics_module, MICROPY_PY_UGRAPHICS);

#endif  // MICROPY_PY_UGRAPHICS
