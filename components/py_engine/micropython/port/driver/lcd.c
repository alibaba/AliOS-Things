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
#include "fontlib.h"

extern const mp_obj_type_t driver_lcd_type;

#define LCD_JSON_CONFIG_PATH "/data/python/config/lcd.json"

// this is the actual C-structure for our new object
typedef struct lcd_handle {
    uint32_t width;
    uint32_t height;
} lcd_handle_t;

static lcd_handle_t *lcd_handle = NULL;

static int8_t lcd_init(uint32_t width, uint32_t height)
{
    if (NULL != lcd_handle) {
        return -1;
    }
    lcd_handle_t *new_handle = amp_calloc(1, sizeof(*new_handle));
    if (NULL == new_handle) {
        return -1;
    }
    //amp_hal_lcd_init(NULL);
    new_handle->height = height;
    new_handle->width  = width;
    lcd_handle         = new_handle;
    return 0;
}

static int8_t lcd_deinit(void)
{
    if (NULL != lcd_handle) {
        amp_free(lcd_handle);
        lcd_handle = NULL;
    }
    return 0;
}

static int8_t lcd_draw_char(uint16_t x, uint16_t y, uint8_t num, uint8_t size,
                            uint32_t fcolor)
{
    uint8_t i        = 0;
    uint8_t j        = 0;
    uint8_t temp     = 0;
    uint16_t xtemp   = x;
    uint16_t ytemp   = y;
    uint8_t char_len = 0;
    uint8_t *pos     = NULL;
    num              = num - ' ';
    if (12 == size) {
        pos      = (uint8_t *)&g_asc2_1206[num];
        char_len = 12;
    } else if (16 == size) {
        pos      = (uint8_t *)&g_asc2_1608[num];
        char_len = 16;
    } else if (24 == size) {
        pos      = (uint8_t *)&g_asc2_2412[num];
        char_len = 36;
    } else if (36 == size) {
        pos      = (uint8_t *)&g_asc2_3636[num];
        char_len = 90;
    } else {
        return -1;
    }
    for (i = 0; i < char_len; ++i) {
        temp = *pos++;
        for (j = 0; j < 8; ++j) {
            if (temp & 0x80) {
                //amp_hal_lcd_draw_point(xtemp, ytemp, fcolor);
            }
            temp <<= 1;
            ytemp += 1;
            if (size == (ytemp - y)) {
                ytemp = y;
                xtemp += 1;
                break;
            }
        }
    }
    return 0;
}

static int8_t lcd_draw_str(uint16_t x, uint16_t y, uint8_t *str, uint8_t size,
                           uint32_t color, int8_t new_line)
{
    uint16_t xstart = x;
    uint16_t ystart = y;
    uint16_t xres   = 0;
    uint16_t yres   = 0;
    if (NULL == lcd_handle) {
        return -1;
    }
    xres = lcd_handle->width;
    yres = lcd_handle->height;
    while ((*str <= '~') && (*str >= ' ')) {
        if (new_line) {
            if (xstart > xres) {
                xstart = x;
                ystart += size;
            }
            if (ystart > yres) {
                break;
            }
        }
        lcd_draw_char(xstart, ystart, *str, size, color);
        xstart += size / 2;
        str += 1;
    }
    return 0;
}

static void lcd_draw_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                          uint32_t color, uint8_t need_fill)
{
    int32_t i = 0;
    if (need_fill) {
        //for (i = y1; i <= y2; i++) amp_hal_lcd_draw_line(x1, i, color, x2 - x1 + 1);
    } else {
        for (i = x1; i <= x2; ++i) {
            //amp_hal_lcd_draw_point(i, y1, color);
            //amp_hal_lcd_draw_point(i, y2, color);
        }
        for (i = y1; i <= y2; ++i) {
            //amp_hal_lcd_draw_point(x1, i, color);
            //amp_hal_lcd_draw_point(x2, i, color);
        }
    }
}

typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
} mp_lcd_obj_t;

void lcd_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_lcd_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t lcd_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_lcd_obj_t* driver_obj = m_new_obj(mp_lcd_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &driver_lcd_type;
    driver_obj->ModuleName = "lcd";

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    uint32_t width = 0;
    uint32_t height = 0;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_lcd_obj_t* driver_obj = (mp_lcd_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    width = (uint32_t)mp_obj_get_int(args[1]);
    height = (uint32_t)mp_obj_get_int(args[2]);
    LOG_D("%s:width = %d;height = %d;\n", __func__, width, height);
    ret = lcd_init(width, height);
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(lcd_obj_open, 3, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_lcd_obj_t* driver_obj = (mp_lcd_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    lcd_deinit();
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(lcd_obj_close, 1, obj_close);

STATIC mp_obj_t obj_show(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 7)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_lcd_obj_t* driver_obj = (mp_lcd_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    int x_pos = (int)mp_obj_get_int(args[1]);
    int y_pos = (int)mp_obj_get_int(args[2]);
    int color = (int)mp_obj_get_int(args[3]);
    int is_newline = (int)mp_obj_get_int(args[4]);
    char *str = (char *)mp_obj_str_get_str(args[5]);
    int front_size = (int)mp_obj_get_int(args[6]);

    LOG_D("%s:x_pos = %d;\n", __func__, x_pos);
    LOG_D("%s:y_pos = %d;\n", __func__, y_pos);
    LOG_D("%s:color = %d;\n", __func__, color);
    LOG_D("%s:is_newline = %d;\n", __func__, is_newline);
    LOG_D("%s:str = %s;\n", __func__, str);
    LOG_D("%s:front_size = %d;\n", __func__, front_size);

    lcd_draw_str(x_pos, y_pos, (uint8_t *)str, front_size, color, is_newline);
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(lcd_obj_show, 7, obj_show);

STATIC mp_obj_t obj_fill(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 7)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_lcd_obj_t* driver_obj = (mp_lcd_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    int sx = (int)mp_obj_get_int(args[1]);
    int sy = (int)mp_obj_get_int(args[2]);
    int ex = (int)mp_obj_get_int(args[3]);
    int ey = (int)mp_obj_get_int(args[4]);
    int color = (int)mp_obj_get_int(args[5]);
    int is_fill = (int)mp_obj_get_int(args[6]);

    LOG_D("%s:sx = %d;\n", __func__, sx);
    LOG_D("%s:sy = %d;\n", __func__, sy);
    LOG_D("%s:ex = %d;\n", __func__, ex);
    LOG_D("%s:ey = %d;\n", __func__, ey);
    LOG_D("%s:color = %s;\n", __func__, color);
    LOG_D("%s:is_fill = %d;\n", __func__, is_fill);

    lcd_draw_rect(sx, sy, ex, ey, color, is_fill);
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(lcd_obj_fill, 7, obj_fill);

STATIC const mp_rom_map_elem_t lcd_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_LCD)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&lcd_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&lcd_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&lcd_obj_show)},
    {MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&lcd_obj_fill)},
};

STATIC MP_DEFINE_CONST_DICT(lcd_locals_dict, lcd_locals_dict_table);

const mp_obj_type_t driver_lcd_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_LCD,
    .print = lcd_obj_print,
    .make_new = lcd_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&lcd_locals_dict,
};

