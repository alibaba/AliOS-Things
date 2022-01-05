/*
 * This file is part of the MicroPython ESP32 project, https://github.com/loboris/MicroPython_ESP32_psRAM_LoBo
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 LoBo (https://github.com/loboris)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "sdkconfig.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "modmachine.h"

#include "tft/tftspi.h"
#include "tft/tft.h"

#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

typedef struct _display_tft_obj_t {
    mp_obj_base_t base;
    display_config_t dconfig;
    exspi_device_handle_t *disp_spi;
    mp_obj_t expwm_handler;
} display_tft_obj_t;

display_tft_obj_t *tft_save = NULL;

const mp_obj_type_t display_tft_type;

extern uint8_t disp_used_spi_host;

extern exspi_device_handle_t *disp_spi;

// constructor(id, ...)
//-----------------------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    display_tft_obj_t *self = m_new_obj(display_tft_obj_t);
    uint8_t lcd_init_state = 0;
    if(disp_spi == NULL) {
        disp_spi = (exspi_device_handle_t *)malloc(sizeof(exspi_device_handle_t));
    } else {
        lcd_init_state = 1;
    }

    self->base.type = &display_tft_type;
    self->disp_spi = disp_spi;
    self->dconfig.speed = 40000000;
    self->dconfig.rdspeed = 2000000;
    self->dconfig.type = DISP_TYPE_M5STACK;
    self->dconfig.host = HSPI_HOST;
    self->dconfig.mosi = 23;
    self->dconfig.miso = 38;
    self->dconfig.sck = 18;
    self->dconfig.cs = 5;
    self->dconfig.dc = 15;
    self->dconfig.tcs = -1;
    self->dconfig.rst = -1;
    self->dconfig.bckl = -1;
    self->dconfig.bckl_on = 1;
    self->dconfig.color_bits = 16;
    self->dconfig.gamma = 0;
    self->dconfig.width = 320;
    self->dconfig.height = 240;
    self->dconfig.invrot = 3;
    self->dconfig.bgr = 8;
    self->dconfig.touch = TOUCH_TYPE_NONE;

    if (lcd_init_state == 0) {
        TFT_display_init(&self->dconfig);
        spi_set_speed(disp_spi, self->dconfig.speed);
    }
    
    font_rotate = 0;
    text_wrap = 1;
    font_transparent = 1;
    font_forceFixed = 0;
    gray_scale = 0;
    TFT_resetclipwin();
    TFT_setFont(DEFAULT_FONT, NULL);
    TFT_fillScreen(TFT_BLACK);
    return MP_OBJ_FROM_PTR(self);
}

//-----------------------------------------------------------------------------------------------
STATIC void display_tft_printinfo(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_printf(print, "TFT   (Color mode: 16-bit, Clk=40000000 Hz)\n");
    mp_printf(print, "Pins  (miso=38, mosi=23, clk=18, cs=5, dc=15, reset by axp192)");
}

//--------------------------------------
STATIC color_t intToColor(uint32_t cint)
{
    color_t cl = {0,0,0};
    cl.r = (cint >> 16) & 0xFF;
    cl.g = (cint >> 8) & 0xFF;
    cl.b = cint & 0xFF;
    return cl;
}

// STATIC uint16_t* StrToUnicode(const byte* str, size_t str_len, size_t* utf_len) {
//     uint16_t len;
//     len = utf8_charlen(str, str_len);
//     if(len == 0) {
//         return NULL;
//     }
//     uint16_t* code = calloc(len, sizeof(uint16_t));
//     if (code == NULL) {
//         return NULL;
//     }
//     for (uint16_t i = 0; i < len; i++) {
//         code[i] = utf8_get_char(str);
//         str = utf8_next_char(str);
//     }
//     *utf_len = len;
//     return code;
// }

//-------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_drawPixel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_color,               MP_ARG_INT, { .u_int = -1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t color = _fg;
    mp_int_t x = args[0].u_int;
    mp_int_t y = args[1].u_int;
    if (args[2].u_int >= 0) {
        color = intToColor(args[2].u_int);
    }
    TFT_drawPixel(x, y, color, 1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_drawPixel_obj, 2, display_tft_drawPixel);

//------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_drawLine(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,     MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,     MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_x1,    MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y1,    MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_color,                   MP_ARG_INT, { .u_int = -1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t color = _fg;
    mp_int_t x0 = args[0].u_int;
    mp_int_t y0 = args[1].u_int;
    mp_int_t x1 = args[2].u_int;
    mp_int_t y1 = args[3].u_int;
    if (args[4].u_int >= 0) {
        color = intToColor(args[4].u_int);
    }
    TFT_drawLine(x0, y0, x1, y1, color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_drawLine_obj, 4, display_tft_drawLine);
#if 0
//-------------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_drawLineByAngle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_start,  MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_length, MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_angle,  MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_color,                    MP_ARG_INT, { .u_int = -1 } },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t color = _fg;
    mp_int_t x = args[0].u_int;
    mp_int_t y = args[1].u_int;
    mp_int_t start = args[2].u_int;
    mp_int_t len = args[3].u_int;
    mp_int_t angle = args[4].u_int;
    if (args[5].u_int >= 0) {
        color = intToColor(args[5].u_int);
    }
    TFT_drawLineByAngle(x, y, start, len, angle, color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_drawLineByAngle_obj, 5, display_tft_drawLineByAngle);

//----------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_drawTriangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_x1,     MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y1,     MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_x2,     MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y2,     MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_color,                    MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_fillcolor,                MP_ARG_INT, { .u_int = -1 } },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t color = _fg;
    mp_int_t x0 = args[0].u_int;
    mp_int_t y0 = args[1].u_int;
    mp_int_t x1 = args[2].u_int;
    mp_int_t y1 = args[3].u_int;
    mp_int_t x2 = args[4].u_int;
    mp_int_t y2 = args[5].u_int;
    if (args[6].u_int >= 0) {
        color = intToColor(args[6].u_int);
    }
    if (args[7].u_int >= 0) {
        TFT_fillTriangle(x0, y0, x1, y1, x2, y2, intToColor(args[7].u_int));
    }
    TFT_drawTriangle(x0, y0, x1, y1, x2, y2, color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_drawTriangle_obj, 6, display_tft_drawTriangle);

//----------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_fillTriangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_x1,     MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y1,     MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_x2,     MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y2,     MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_color,                    MP_ARG_INT, { .u_int = -1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t color = _fg;
    mp_int_t x0 = args[0].u_int;
    mp_int_t y0 = args[1].u_int;
    mp_int_t x1 = args[2].u_int;
    mp_int_t y1 = args[3].u_int;
    mp_int_t x2 = args[4].u_int;
    mp_int_t y2 = args[5].u_int;
    if (args[6].u_int >= 0) {
        color = intToColor(args[6].u_int);
    }
    TFT_fillTriangle(x0, y0, x1, y1, x2, y2, color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_fillTriangle_obj, 6, display_tft_fillTriangle);

//--------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_drawCircle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_r,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_color,                    MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_fillcolor,                MP_ARG_INT, { .u_int = -1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t color = _fg;
    mp_int_t x = args[0].u_int;
    mp_int_t y = args[1].u_int;
    mp_int_t radius = args[2].u_int;
    if (args[3].u_int >= 0) {
        color = intToColor(args[3].u_int);
    }
    if (args[4].u_int >= 0) {
        TFT_fillCircle(x, y, radius, intToColor(args[4].u_int));
        if (args[3].u_int != args[4].u_int) TFT_drawCircle(x, y, radius, color);
    }
    else TFT_drawCircle(x, y, radius, color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_drawCircle_obj, 3, display_tft_drawCircle);

//--------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_fillCircle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_r,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_color,                    MP_ARG_INT, { .u_int = -1 } },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t color = _fg;
    mp_int_t x = args[0].u_int;
    mp_int_t y = args[1].u_int;
    mp_int_t radius = args[2].u_int;
    if (args[3].u_int >= 0) {
        color = intToColor(args[3].u_int);
    }
    TFT_fillCircle(x, y, radius, color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_fillCircle_obj, 3, display_tft_fillCircle);

//--------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_drawSwitchBtn(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_w,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_h,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_color,                    MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_switch_color,             MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_state,                    MP_ARG_INT, { .u_int = 0 } },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t color = _fg;
    color_t switch_color = _fg;
    mp_int_t x = args[0].u_int;
    mp_int_t y = args[1].u_int;
    mp_int_t w = args[2].u_int;
    mp_int_t h = args[3].u_int;
    mp_int_t state = args[6].u_int;

    if (args[4].u_int >= 0) {
        color = intToColor(args[4].u_int);
    }

    if (args[5].u_int >= 0) {
        switch_color = intToColor(args[5].u_int);
    } else {
        switch_color = color;
    }

    TFT_drawSwitchBtn(x, y, w, h, color, switch_color, state);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_drawSwitchBtn_obj, 4, display_tft_drawSwitchBtn);

//---------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_drawEllipse(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_rx,     MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_ry,     MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_opt,                      MP_ARG_INT, { .u_int = 15 } },
        { MP_QSTR_color,                    MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_fillcolor,                MP_ARG_INT, { .u_int = -1 } },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t color = _fg;
    mp_int_t x = args[0].u_int;
    mp_int_t y = args[1].u_int;
    mp_int_t rx = args[2].u_int;
    mp_int_t ry = args[3].u_int;
    mp_int_t opt = args[4].u_int & 0x0F;
    if (args[5].u_int >= 0) {
        color = intToColor(args[5].u_int);
    }
    if (args[6].u_int >= 0) {
        TFT_fillEllipse(x, y, rx, ry, intToColor(args[6].u_int), opt);
    }
    TFT_drawEllipse(x, y, rx, ry, color, opt);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_drawEllipse_obj, 4, display_tft_drawEllipse);

//-----------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_drawArc(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_r,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_thick,  MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_start,  MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_end,    MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 15 } },
        { MP_QSTR_color,                    MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_fillcolor,                MP_ARG_INT, { .u_int = -1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t color = _fg;
    color_t fill_color = _fg;
    mp_int_t x = args[0].u_int;
    mp_int_t y = args[1].u_int;
    mp_int_t r = args[2].u_int;
    mp_int_t th = args[3].u_int;
    mp_int_t start = args[4].u_int;
    mp_int_t end = args[5].u_int;
    if (args[6].u_int >= 0) {
        color = intToColor(args[6].u_int);
    }
    if (args[7].u_int >= 0) {
        fill_color = intToColor(args[7].u_int);
    }
    TFT_drawArc(x, y, r, th, start, end, color, fill_color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_drawArc_obj, 6, display_tft_drawArc);

//------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_drawPoly(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_r,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_sides,  MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_thick,  MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 1 } },
        { MP_QSTR_color,                    MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_fillcolor,                MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_rotate,                   MP_ARG_INT, { .u_int = 0 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t color = _fg;
    color_t fill_color = _fg;
    mp_int_t x = args[0].u_int;
    mp_int_t y = args[1].u_int;
    mp_int_t r = args[2].u_int;
    mp_int_t sides = args[3].u_int;
    mp_int_t th = args[4].u_int;
    if (args[5].u_int >= 0) {
        color = intToColor(args[5].u_int);
    }
    if (args[6].u_int >= 0) {
        fill_color = intToColor(args[6].u_int);
    }
    TFT_drawPolygon(x, y, sides, r, color, fill_color, args[7].u_int, th);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_drawPoly_obj, 5, display_tft_drawPoly);
#endif
//------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_drawRect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_width,  MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_color,                    MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_fillcolor,                MP_ARG_INT, { .u_int = -1 } },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t color = _fg;
    mp_int_t x = args[0].u_int;
    mp_int_t y = args[1].u_int;
    mp_int_t w = args[2].u_int;
    mp_int_t h = args[3].u_int;
    if (args[4].u_int >= 0) {
        color = intToColor(args[4].u_int);
    }
    if (args[5].u_int >= 0) {
        TFT_fillRect(x, y, w, h, intToColor(args[5].u_int));
    }
    TFT_drawRect(x, y, w, h, color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_drawRect_obj, 4, display_tft_drawRect);

//------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_fillRect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_width,  MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_color,                    MP_ARG_INT, { .u_int = -1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t x = args[0].u_int;
    mp_int_t y = args[1].u_int;
    mp_int_t w = args[2].u_int;
    mp_int_t h = args[3].u_int;
    color_t color = _fg;
    if (args[4].u_int >= 0) {
        color = intToColor(args[4].u_int);
    }
     TFT_fillRect(x, y, w, h, color);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_fillRect_obj, 4, display_tft_fillRect);
#if 0
//------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_qrcode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_text,     MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_x,        MP_ARG_INT, { .u_int = 50 } },
        { MP_QSTR_y,        MP_ARG_INT, { .u_int = 10 } },
        { MP_QSTR_width,    MP_ARG_INT, { .u_int = 220 } },
        { MP_QSTR_version,  MP_ARG_INT, { .u_int = 6 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    char *text = (char *)mp_obj_str_get_str(args[0].u_obj);
    mp_int_t x = args[1].u_int;
    mp_int_t y = args[2].u_int;
    mp_int_t width = args[3].u_int;
    mp_int_t version = args[4].u_int;

    // Create the QR code
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(version)];
    qrcode_initText(&qrcode, qrcodeData, version, 0, text);
    
    // Top quiet zone
    uint8_t thickness = width / qrcode.size;
    uint16_t lineLength = qrcode.size * thickness;
    uint8_t xOffset = x + (width-lineLength)/2;
    uint8_t yOffset = y + (width-lineLength)/2;
    TFT_fillRect(x, y, width, width, TFT_WHITE);

    for (uint8_t y = 0; y < qrcode.size; y++) {
        for (uint8_t x = 0; x < qrcode.size; x++) {
            uint8_t q = qrcode_getModule(&qrcode, x, y);
            if (q) {
                TFT_fillRect(x * thickness + xOffset, y * thickness + yOffset, thickness, thickness, TFT_BLACK);
            }
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_qrcode_obj, 1, display_tft_qrcode);

//-----------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_drawRoundRect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_width,  MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_r,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_color,                    MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_fillcolor,                MP_ARG_INT, { .u_int = -1 } },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t color = _fg;
    mp_int_t x = args[0].u_int;
    mp_int_t y = args[1].u_int;
    mp_int_t w = args[2].u_int;
    mp_int_t h = args[3].u_int;
    mp_int_t r = args[4].u_int;
    if (args[5].u_int >= 0) {
        color = intToColor(args[5].u_int);
    }
    if (args[6].u_int >= 0) {
        TFT_fillRoundRect(x, y, w, h, r, intToColor(args[6].u_int));
    }
    TFT_drawRoundRect(x, y, w, h, r, color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_drawRoundRect_obj, 5, display_tft_drawRoundRect);

//-----------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_fillRoundRect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_width,  MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_r,      MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_color,                    MP_ARG_INT, { .u_int = -1 } },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t color = _fg;
    mp_int_t x = args[0].u_int;
    mp_int_t y = args[1].u_int;
    mp_int_t w = args[2].u_int;
    mp_int_t h = args[3].u_int;
    mp_int_t r = args[4].u_int;
    if (args[5].u_int >= 0) {
        color = intToColor(args[5].u_int);
    }
    TFT_fillRoundRect(x, y, w, h, r, color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_fillRoundRect_obj, 5, display_tft_fillRoundRect);
#endif

//--------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_fillScreen(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_color,                    MP_ARG_INT, { .u_int = -1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t color = _bg;
    if (args[0].u_int >= 0) {
        color = intToColor(args[0].u_int);
    }
    TFT_fillScreen(color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_fillScreen_obj, 0, display_tft_fillScreen);

//-----------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_fillWin(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_color,                    MP_ARG_INT, { .u_int = -1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t color = _bg;
    if (args[0].u_int >= 0) {
        color = intToColor(args[0].u_int);
    }
    TFT_fillWindow(color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_fillWin_obj, 0, display_tft_fillWin);

//--------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_7segAttrib(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_dist,    MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_width,   MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_outline, MP_ARG_REQUIRED | MP_ARG_BOOL, { .u_bool = false } },
        { MP_QSTR_color,   MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = -1 } },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    set_7seg_font_atrib(args[0].u_int, args[1].u_int, (int)args[2].u_bool, intToColor(args[3].u_int));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_7segAttrib_obj, 4, display_tft_7segAttrib);

//-----------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_setFont(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_font,         MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_rotate,       MP_ARG_KW_ONLY  | MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_transparent,  MP_ARG_KW_ONLY  | MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_fixedwidth,   MP_ARG_KW_ONLY  | MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_dist,         MP_ARG_KW_ONLY  | MP_ARG_INT, { .u_int = 8 } },
        { MP_QSTR_width,        MP_ARG_KW_ONLY  | MP_ARG_INT, { .u_int = 2 } },
        { MP_QSTR_outline,      MP_ARG_KW_ONLY  | MP_ARG_BOOL, { .u_bool = false } },
        { MP_QSTR_color,        MP_ARG_KW_ONLY  | MP_ARG_INT, { .u_int = 0 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const char *font_file = NULL;
    // char fullname[128] = {'\0'};
    mp_int_t font = DEFAULT_FONT;

    if (MP_OBJ_IS_STR(args[0].u_obj)) {
        font_file = mp_obj_str_get_str(args[0].u_obj);

        // if (physicalPath(font_file, fullname) == 0) {
        font = USER_FONT;
        //     font_file = fullname;
        // }
    }
    else {
        font = mp_obj_get_int(args[0].u_obj);
    }
    TFT_setFont(font, font_file);

    if (args[1].u_int >= 0) font_rotate = args[1].u_int;
    if (args[2].u_int >= 0) font_transparent = args[2].u_int & 1;
    if (args[3].u_int >= 0) font_forceFixed = args[3].u_int & 1;

    if (font == FONT_7SEG) {
        set_7seg_font_atrib(args[4].u_int, args[5].u_int, (int)args[6].u_bool, intToColor(args[7].u_int));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_setFont_obj, 1, display_tft_setFont);

//-------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_getFontSize(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    int width, height;
    TFT_getfontsize(&width, &height);

    mp_obj_t tuple[2];

    tuple[0] = mp_obj_new_int(width);
    tuple[1] = mp_obj_new_int(height);

    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_getFontSize_obj, 0, display_tft_getFontSize);

//-----------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_setRot(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_rot, MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = PORTRAIT } },
    };
    display_tft_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

      mp_int_t rot = args[0].u_int;
      if ((rot < 0) || (rot > 3)) rot = 0;

      TFT_setRotation(rot);
      self->dconfig.width = _width;
      self->dconfig.height = _height;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_setRot_obj, 1, display_tft_setRot);

//---------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,            MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,            MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_text,         MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_color,                          MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_rotate,       MP_ARG_KW_ONLY  | MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_transparent,  MP_ARG_KW_ONLY  | MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_fixedwidth,   MP_ARG_KW_ONLY  | MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_wrap,         MP_ARG_KW_ONLY  | MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_bgcolor,      MP_ARG_KW_ONLY  | MP_ARG_INT, { .u_int = -1 } },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t old_fg = _fg;
    color_t old_bg = _bg;
    int old_rot = font_rotate;
    int old_transp = font_transparent;
    int old_fixed = font_forceFixed;
    int old_wrap = text_wrap;

    mp_int_t x = args[0].u_int;
    mp_int_t y = args[1].u_int;

    if (args[3].u_int >= 0) _fg = intToColor(args[3].u_int);
    if (args[4].u_int >= 0) font_rotate = args[4].u_int;
    if (args[5].u_int >= 0) font_transparent = args[5].u_int & 1;
    if (args[6].u_int >= 0) font_forceFixed = args[6].u_int & 1;
    if (args[7].u_int >= 0) text_wrap = args[7].u_int & 1;
    if (args[8].u_int >= 0) _bg = intToColor(args[8].u_int);
    // if(font_now == UNICODE_FONT) {
    //     size_t len;
    //     const byte *str = (const byte *)mp_obj_str_get_data(args[2].u_obj, &len);
    //     uint16_t* code = StrToUnicode(str, len, &len);
    //     if(code == NULL) {
    //         return mp_const_none;
    //     }
    //     TFT_print_unicode(code, len, x, y);
    //     free(code);
    // } else {
    char *st = (char *)mp_obj_str_get_str(args[2].u_obj);
    TFT_print(st, x, y);
    // }

    _fg = old_fg;
    _bg = old_bg;
    font_rotate = old_rot;
    font_transparent = old_transp;
    font_forceFixed = old_fixed;
    text_wrap = old_wrap;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_text_obj, 3, display_tft_text);

//---------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_print(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_text,         MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_x,                              MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,                              MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_color,                          MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_rotate,       MP_ARG_KW_ONLY  | MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_transparent,  MP_ARG_KW_ONLY  | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_fixedwidth,   MP_ARG_KW_ONLY  | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_wrap,         MP_ARG_KW_ONLY  | MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t old_fg = _fg;
    mp_int_t x;
    mp_int_t y;
    if (n_args > 2) {
        x = args[1].u_int;
        y = args[2].u_int;
    } else {
        x = TFT_X;
        y = TFT_Y;
    }
    if (args[3].u_int >= 0) {
        _fg = intToColor(args[3].u_int);
    }
    if (args[4].u_int >= 0) font_rotate = args[4].u_int;
    if (mp_obj_is_integer(args[5].u_obj)) font_transparent = args[5].u_int;
    if (mp_obj_is_integer(args[6].u_obj)) font_forceFixed = args[6].u_int;
    if (mp_obj_is_integer(args[7].u_obj)) text_wrap = args[7].u_int;

    char* st;
    size_t len;

    if (MP_OBJ_IS_STR(args[0].u_obj)) {
        st = (char *)mp_obj_str_get_data(args[0].u_obj, &len);
    } else {
        char str[32];
        if (mp_obj_is_integer(args[0].u_obj)) {
            int num = mp_obj_get_int(args[0].u_obj);
            sprintf(str, "%d", num);
        } else if (mp_obj_is_float(args[0].u_obj)) {
            float f = mp_obj_float_get(args[0].u_obj);
            sprintf(str, "%f", f);
        }
        st = str;
        len = strlen(st);
    }
    
    // if(font_now == UNICODE_FONT) {
    //     uint16_t* code = StrToUnicode((byte *)st, len, &len);
    //     if(code == NULL) {
    //         return mp_const_none;
    //     }
    //     TFT_print_unicode(code, len, x, y);
    //     free(code);
    // } else {
    TFT_print(st, x, y);
    // }

    _fg = old_fg;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_print_obj, 1, display_tft_print);

//---------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_println(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    display_tft_print(n_args, pos_args, kw_args);
    TFT_print("\r\n", TFT_X, TFT_Y);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_println_obj, 1, display_tft_println);

//---------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_stringWidth(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_text,  MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    mp_int_t w = 0;
    // if(font_now == UNICODE_FONT) {
        // size_t len = 0;
        // const byte *st = (const byte *)mp_obj_str_get_data(args[0].u_obj, &len);
        // uint16_t* code = StrToUnicode(st, len, &len);
        // if(code == NULL) {
            // w = -1;
        // } else {
            // for (uint16_t i = 0; i < len; i++) {
                // w += GetUnicodeWidth(code[i]) + 1;
            // }
            // free(code);
        // }
    // } else {
    char *st = (char *)mp_obj_str_get_str(args[0].u_obj);
    w = TFT_getStringWidth(st);
    // }

    return mp_obj_new_int(w);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_stringWidth_obj, 1, display_tft_stringWidth);

//-------------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_clearStringRect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,       MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,       MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_text,    MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_color,                     MP_ARG_INT, { .u_int = -1 } },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    color_t old_bg = _bg;
    mp_int_t x = args[0].u_int;
    mp_int_t y = args[1].u_int;
    char *st = (char *)mp_obj_str_get_str(args[2].u_obj);

    if (args[3].u_int >= 0) _bg = intToColor(args[3].u_int);

    TFT_clearStringRect(x, y, st);

    _bg = old_bg;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_clearStringRect_obj, 3, display_tft_clearStringRect);

STATIC mp_obj_t display_tft_Image_buff(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) 
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,     MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,     MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_file,  MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_scale,                   MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_type,                    MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_DEBUG, MP_ARG_KW_ONLY  | MP_ARG_INT, { .u_int = 0 } },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    image_debug = (uint8_t)args[5].u_bool;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2].u_obj, &bufinfo, MP_BUFFER_READ);
    
    if (bufinfo.len < 20) {
        return mp_const_none;
    }

    uint8_t* buf_ptr = (uint8_t *)bufinfo.buf;
    if (buf_ptr[0] == 0xff && buf_ptr[1] == 0xd8 && buf_ptr[6] == 'J' && buf_ptr[7] == 'F') {
        TFT_jpg_image(args[0].u_int, args[1].u_int, args[3].u_int, NULL, bufinfo.buf, bufinfo.len);
    }
#if 0
    else if(buf_ptr[0] == 'B' && buf_ptr[1] == 'M') {
        TFT_bmp_image(args[0].u_int, args[1].u_int, args[3].u_int, NULL, bufinfo.buf, bufinfo.len);
    }else if(buf_ptr[1] == 'P' && buf_ptr[2] == 'N'){
        TFT_png_image(args[0].u_int, args[1].u_int, args[3].u_int, NULL, bufinfo.buf, bufinfo.len);
    }
#endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_Image_buff_obj, 3, display_tft_Image_buff);

//-----------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_Image(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,     MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,     MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_file,  MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_scale,                   MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_type,                    MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_DEBUG, MP_ARG_KW_ONLY  | MP_ARG_INT, { .u_int = 0 } },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    if(!mp_obj_is_str(args[2].u_obj)) {
        return display_tft_Image_buff(n_args, pos_args, kw_args);
    }

    char *fname = NULL;
    char fullname[128] = {'\0'};
    int img_type = args[4].u_int;
    
    fname = (char *)mp_obj_str_get_str(args[2].u_obj);

    int res = 0; //physicalPath(fname, fullname);
    strcpy(fullname, fname);
    if ((res != 0) || (strlen(fullname) == 0)) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("Error resolving file name"));
    }

    if (img_type < 0) {
        // try to determine image type
        char upr_fname[128];
        strcpy(upr_fname, fname);
        for (int i=0; i < strlen(upr_fname); i++) {
          upr_fname[i] = toupper((unsigned char) upr_fname[i]);
        }
        if (strstr(upr_fname, ".JPG") != NULL) img_type = IMAGE_TYPE_JPG;
        else if (strstr(upr_fname, ".BMP") != NULL) img_type = IMAGE_TYPE_BMP;
        else if (strstr(upr_fname, ".PNG") != NULL) img_type = IMAGE_TYPE_PNG;
        if (img_type < 0) {
            mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("Cannot determine image type"));
        }
    }

    image_debug = (uint8_t)args[5].u_bool;
#if 0
    if (img_type == IMAGE_TYPE_BMP) {
        TFT_bmp_image(args[0].u_int, args[1].u_int, args[3].u_int, fullname, NULL, 0);
    }
    else
#endif
    if (img_type == IMAGE_TYPE_JPG) {
        TFT_jpg_image(args[0].u_int, args[1].u_int, args[3].u_int, fullname, NULL, 0);
    }
#if 0
    else if (img_type == IMAGE_TYPE_PNG)
    {
        TFT_png_image(args[0].u_int, args[1].u_int, args[3].u_int, fullname, NULL, 0);
    }
#endif
    else {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("Unsupported image type"));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_Image_obj, 3, display_tft_Image);

#if 0
//-----------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_compileFont(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_file,  MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_deMP_QSTR_DEBUGbug, MP_ARG_KW_ONLY  | MP_ARG_BOOL, { .u_bool = false } },
    };
    //
    //

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    char *fname = NULL;
    char fullname[128] = {'\0'};
    uint8_t debug = (uint8_t)args[1].u_bool;

    fname = (char *)mp_obj_str_get_str(args[0].u_obj);

    int res = 0; // physicalPath(fname, fullname);
    strcpy(fullname, fname);
    if ((res != 0) || (strlen(fullname) == 0)) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("Error resolving file name"));
    }

    res = compile_font_file(fullname, debug);
    if (res) return mp_const_false;
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_compileFont_obj, 1, display_tft_compileFont);

//-----------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_HSBtoRGB(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    const mp_arg_t allowed_args[] = {
        { MP_QSTR_hue,  MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_saturation,  MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_brightness,  MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_float_t hue = mp_obj_get_float(args[0].u_obj);
    mp_float_t sat = mp_obj_get_float(args[1].u_obj);
    mp_float_t bri = mp_obj_get_float(args[2].u_obj);

    color_t color = HSBtoRGB(hue, sat, bri);
    mp_int_t icolor = (int)((color.r << 16) | (color.g << 8) | color.b);

    return mp_obj_new_int(icolor);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_HSBtoRGB_obj, 3, display_tft_HSBtoRGB);

//--------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_setclipwin(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,  MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,  MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_fillcolor,                MP_ARG_INT, { .u_int = -1 } },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t x0 = args[0].u_int;
    mp_int_t y0 = args[1].u_int;
    mp_int_t x1 = args[2].u_int;
    mp_int_t y1 = args[3].u_int;

    TFT_setclipwin(x0, y0, x1, y1);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_setclipwin_obj, 4, display_tft_setclipwin);

//----------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_resetclipwin(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    TFT_resetclipwin();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_resetclipwin_obj, 0, display_tft_resetclipwin);

//---------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_saveclipwin(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    
    TFT_saveClipWin();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_saveclipwin_obj, 0, display_tft_saveclipwin);

//------------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_restoreclipwin(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    TFT_restoreClipWin();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_restoreclipwin_obj, 0, display_tft_restoreclipwin);

//------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_getSize(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    mp_obj_t tuple[2];
    tuple[0] = mp_obj_new_int(_width);
    tuple[1] = mp_obj_new_int(_height);

    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_getSize_obj, 0, display_tft_getSize);

//--------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_getWinSize(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    mp_obj_t tuple[2];
    tuple[0] = mp_obj_new_int(dispWin.x2 - dispWin.x1 + 1);
    tuple[1] = mp_obj_new_int(dispWin.y2 - dispWin.y1 + 1);

    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_getWinSize_obj, 0, display_tft_getWinSize);
#endif
//-------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_setColor(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_color,                    MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_bcolor,                   MP_ARG_INT, { .u_int = -1 } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[0].u_int >= 0) {
        _fg = intToColor(args[0].u_int);
    } else {
        _fg = intToColor(iTFT_WHITE);
    }

    if (args[1].u_int >= 0) {
        _bg = intToColor(args[1].u_int);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_setColor_obj, 0, display_tft_setColor);
#if 0
//-----------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_setCursor(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_x,            MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_y,            MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

      mp_int_t x = args[0].u_int;
    mp_int_t y = args[1].u_int;
      TFT_X = x;
    TFT_Y = y;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_setCursor_obj, 2, display_tft_setCursor);

//-------------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_getCursor(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    mp_obj_t tuple[2];

    tuple[0] = mp_obj_new_int(TFT_X);
    tuple[1] = mp_obj_new_int(TFT_Y);

    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_getCursor_obj, 0, display_tft_getCursor);

//-----------------------------------------------------------------------------------------------
// STATIC mp_obj_t display_tft_setBrightness(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
// {
    // const mp_arg_t allowed_args[] = {
    //     { MP_QSTR_duty,            MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
    // };
    

    // mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    // mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

      // mp_int_t dperc = args[0].u_int;
    // if (dperc > 100) dperc = 100;
    // if (dperc < 0) dperc = 0;

    // mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, dperc);

//     return mp_const_none;
// }
// STATIC MP_DEFINE_CONST_FUN_OBJ_KW(display_tft_setBrightness_obj, 0, display_tft_setBrightness);



//------------------------------------------------------------------------
// STATIC mp_obj_t display_tft_backlight(mp_obj_t self_in, mp_obj_t onoff_in)
// {
//     display_tft_obj_t *self = self_in;

//     int onoff = mp_obj_get_int(onoff_in);
//     if (onoff) bcklOn(&self->dconfig);
//     else bcklOff(&self->dconfig);

//     return mp_const_none;
// }
// STATIC MP_DEFINE_CONST_FUN_OBJ_2(display_tft_backlight_obj, display_tft_backlight);

//------------------------------------------------------
// STATIC mp_obj_t display_tft_touch_type(mp_obj_t self_in)
// {
//     display_tft_obj_t *self = self_in;
//     return mp_obj_new_int(self->dconfig.touch);
// }
// STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tft_touch_type_obj, display_tft_touch_type);


// ==== Low level functions ======================================

//------------------------------------------------------------------------
STATIC mp_obj_t display_tft_set_speed(mp_obj_t self_in, mp_obj_t speed_in)
{
    display_tft_obj_t *self = self_in;
    

    int speed = mp_obj_get_int(speed_in);

    // Set SPI clock used for display operations
    self->dconfig.speed = spi_set_speed(self->disp_spi, speed);

    // max_rdclock = find_rd_speed();
    self->dconfig.rdspeed = 8000000;

    mp_obj_t tuple[2];

    tuple[0] = mp_obj_new_int(self->dconfig.speed);
    tuple[1] = mp_obj_new_int(self->dconfig.rdspeed);

    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(display_tft_set_speed_obj, display_tft_set_speed);

//--------------------------------------------------
STATIC mp_obj_t display_tft_select(mp_obj_t self_in)
{
    disp_select();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tft_select_obj, display_tft_select);

//----------------------------------------------------
STATIC mp_obj_t display_tft_deselect(mp_obj_t self_in)
{
    disp_deselect();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tft_deselect_obj, display_tft_deselect);

//--------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_cmd_read(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t len_in)
{
    uint8_t cmd = (uint8_t)mp_obj_get_int(cmd_in);
    uint8_t len = (uint8_t)mp_obj_get_int(len_in);
    if ((len < 1) || (len > 4)) len = 1;

    uint32_t res = read_cmd(cmd, len);

    return mp_obj_new_int_from_uint(res);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(display_tft_cmd_read_obj, display_tft_cmd_read);

//-------------------------------------------------------------------------
STATIC mp_obj_t display_tft_send_command(mp_obj_t self_in, mp_obj_t cmd_in)
{
    uint8_t cmd = (uint8_t)mp_obj_get_int(cmd_in);

    disp_select();
    disp_spi_transfer_cmd(cmd);
    wait_trans_finish(0);
    disp_deselect();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(display_tft_send_command_obj, display_tft_send_command);

//--------------------------------------------------------------------------------------------
STATIC mp_obj_t display_tft_send_cmd_data(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t data_in)
{
    uint8_t cmd = (uint8_t)mp_obj_get_int(cmd_in);
    mp_buffer_info_t data;
    mp_get_buffer_raise(data_in, &data, MP_BUFFER_READ);

    disp_select();
    disp_spi_transfer_cmd_data(cmd, data.buf, data.len);
    wait_trans_finish(0);
    disp_deselect();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(display_tft_send_cmd_data_obj, display_tft_send_cmd_data);

//--------------------------------------------------
STATIC mp_obj_t display_tft_get_bg(mp_obj_t self_in)
{
    int icolor = (int)((_bg.r << 16) | (_bg.g << 8) | _bg.b);
    return mp_obj_new_int(icolor);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tft_get_bg_obj, display_tft_get_bg);

//--------------------------------------------------
STATIC mp_obj_t display_tft_get_fg(mp_obj_t self_in)
{
    int icolor = (int)((_fg.r << 16) | (_fg.g << 8) | _fg.b);
    return mp_obj_new_int(icolor);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tft_get_fg_obj, display_tft_get_fg);

//---------------------------------------------------------------------
STATIC mp_obj_t display_tft_set_bg(mp_obj_t self_in, mp_obj_t color_in)
{
    color_t color = intToColor(mp_obj_get_int(color_in));
    _bg = color;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(display_tft_set_bg_obj, display_tft_set_bg);

//---------------------------------------------------------------------
STATIC mp_obj_t display_tft_set_fg(mp_obj_t self_in, mp_obj_t color_in)
{
    color_t color = intToColor(mp_obj_get_int(color_in));
    _fg = color;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(display_tft_set_fg_obj, display_tft_set_fg);

//-------------------------------------------------
STATIC mp_obj_t display_tft_get_X(mp_obj_t self_in)
{
    int x = TFT_X;
    return mp_obj_new_int(x);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tft_get_X_obj, display_tft_get_X);

//-------------------------------------------------
STATIC mp_obj_t display_tft_get_Y(mp_obj_t self_in)
{
    int y = TFT_Y;
    return mp_obj_new_int(y);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tft_get_Y_obj, display_tft_get_Y);
#endif

// ===============================================================
STATIC const mp_rom_map_elem_t display_tft_locals_dict_table[] = {
#if 0
    // instance methods
    // { MP_ROM_QSTR(MP_QSTR_readPixel),            MP_ROM_PTR(&display_tft_readPixel_obj) },
#endif
    { MP_ROM_QSTR(MP_QSTR_pixel),                MP_ROM_PTR(&display_tft_drawPixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_line),                MP_ROM_PTR(&display_tft_drawLine_obj) },
#if 0
    { MP_ROM_QSTR(MP_QSTR_lineByAngle),            MP_ROM_PTR(&display_tft_drawLineByAngle_obj) },
    { MP_ROM_QSTR(MP_QSTR_triangle),            MP_ROM_PTR(&display_tft_drawTriangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle),                MP_ROM_PTR(&display_tft_drawCircle_obj) },
    { MP_ROM_QSTR(MP_QSTR_ellipse),                MP_ROM_PTR(&display_tft_drawEllipse_obj) },
    { MP_ROM_QSTR(MP_QSTR_arc),                    MP_ROM_PTR(&display_tft_drawArc_obj) },
    { MP_ROM_QSTR(MP_QSTR_polygon),                MP_ROM_PTR(&display_tft_drawPoly_obj) },
#endif
    { MP_ROM_QSTR(MP_QSTR_rect),                MP_ROM_PTR(&display_tft_drawRect_obj) },
#if 0
    // { MP_ROM_QSTR(MP_QSTR_readScreen),            MP_ROM_PTR(&display_tft_readScreen_obj) },
    { MP_ROM_QSTR(MP_QSTR_roundrect),            MP_ROM_PTR(&display_tft_drawRoundRect_obj) },
#endif
    { MP_ROM_QSTR(MP_QSTR_clear),                MP_ROM_PTR(&display_tft_fillScreen_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill),                MP_ROM_PTR(&display_tft_fillScreen_obj) },
    { MP_ROM_QSTR(MP_QSTR_clearwin),            MP_ROM_PTR(&display_tft_fillWin_obj) },

    { MP_ROM_QSTR(MP_QSTR_font),                MP_ROM_PTR(&display_tft_setFont_obj) },
    { MP_ROM_QSTR(MP_QSTR_fontSize),            MP_ROM_PTR(&display_tft_getFontSize_obj) },
    { MP_ROM_QSTR(MP_QSTR_text),                MP_ROM_PTR(&display_tft_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_orient),                MP_ROM_PTR(&display_tft_setRot_obj) },
    { MP_ROM_QSTR(MP_QSTR_textWidth),            MP_ROM_PTR(&display_tft_stringWidth_obj) },
    { MP_ROM_QSTR(MP_QSTR_textClear),            MP_ROM_PTR(&display_tft_clearStringRect_obj) },
#if 0
    { MP_ROM_QSTR(MP_QSTR_attrib7seg),            MP_ROM_PTR(&display_tft_7segAttrib_obj) },
#endif
    { MP_ROM_QSTR(MP_QSTR_image),                MP_ROM_PTR(&display_tft_Image_obj) },
    { MP_ROM_QSTR(MP_QSTR_image_buff),            MP_ROM_PTR(&display_tft_Image_buff_obj) },
    // { MP_ROM_QSTR(MP_QSTR_gettouch),            MP_ROM_PTR(&display_tft_getTouch_obj) },
#if 0
    { MP_ROM_QSTR(MP_QSTR_compileFont),            MP_ROM_PTR(&display_tft_compileFont_obj) },
    { MP_ROM_QSTR(MP_QSTR_hsb2rgb),                MP_ROM_PTR(&display_tft_HSBtoRGB_obj) },
    { MP_ROM_QSTR(MP_QSTR_setwin),                MP_ROM_PTR(&display_tft_setclipwin_obj) },
    { MP_ROM_QSTR(MP_QSTR_resetwin),            MP_ROM_PTR(&display_tft_resetclipwin_obj) },
    { MP_ROM_QSTR(MP_QSTR_savewin),                MP_ROM_PTR(&display_tft_saveclipwin_obj) },
    { MP_ROM_QSTR(MP_QSTR_restorewin),            MP_ROM_PTR(&display_tft_restoreclipwin_obj) },
    { MP_ROM_QSTR(MP_QSTR_screensize),            MP_ROM_PTR(&display_tft_getSize_obj) },
    { MP_ROM_QSTR(MP_QSTR_winsize),                MP_ROM_PTR(&display_tft_getWinSize_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_fg),                MP_ROM_PTR(&display_tft_get_fg_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_bg),                MP_ROM_PTR(&display_tft_get_bg_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_fg),                MP_ROM_PTR(&display_tft_set_fg_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_bg),                MP_ROM_PTR(&display_tft_set_bg_obj) },
    { MP_ROM_QSTR(MP_QSTR_text_x),                MP_ROM_PTR(&display_tft_get_X_obj) },
    { MP_ROM_QSTR(MP_QSTR_text_y),                MP_ROM_PTR(&display_tft_get_Y_obj) },
    { MP_ROM_QSTR(MP_QSTR_setColor),            MP_ROM_PTR(&display_tft_setColor_obj) },
#endif
    // Adafruit API
    { MP_ROM_QSTR(MP_QSTR_print),               MP_ROM_PTR(&display_tft_print_obj) },
    { MP_ROM_QSTR(MP_QSTR_println),             MP_ROM_PTR(&display_tft_println_obj) },
    { MP_ROM_QSTR(MP_QSTR_setRotation),         MP_ROM_PTR(&display_tft_setRot_obj) },
    { MP_ROM_QSTR(MP_QSTR_setTextColor),        MP_ROM_PTR(&display_tft_setColor_obj) },
#if 0
    { MP_ROM_QSTR(MP_QSTR_setCursor),           MP_ROM_PTR(&display_tft_setCursor_obj) },
    { MP_ROM_QSTR(MP_QSTR_getCursor),           MP_ROM_PTR(&display_tft_getCursor_obj) },
    { MP_ROM_QSTR(MP_QSTR_fillScreen),            MP_ROM_PTR(&display_tft_fillScreen_obj) },
#endif
    { MP_ROM_QSTR(MP_QSTR_drawPixel),            MP_ROM_PTR(&display_tft_drawPixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_drawLine),            MP_ROM_PTR(&display_tft_drawLine_obj) },
    { MP_ROM_QSTR(MP_QSTR_drawRect),            MP_ROM_PTR(&display_tft_drawRect_obj) },
    { MP_ROM_QSTR(MP_QSTR_fillRect),            MP_ROM_PTR(&display_tft_fillRect_obj) },
#if 0
    { MP_ROM_QSTR(MP_QSTR_drawCircle),            MP_ROM_PTR(&display_tft_drawCircle_obj) },
    { MP_ROM_QSTR(MP_QSTR_fillCircle),            MP_ROM_PTR(&display_tft_fillCircle_obj) },
    { MP_ROM_QSTR(MP_QSTR_drawSwitchBtn),        MP_ROM_PTR(&display_tft_drawSwitchBtn_obj) },
    { MP_ROM_QSTR(MP_QSTR_drawTriangle),        MP_ROM_PTR(&display_tft_drawTriangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_fillTriangle),        MP_ROM_PTR(&display_tft_fillTriangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_drawRoundRect),        MP_ROM_PTR(&display_tft_drawRoundRect_obj) },
    { MP_ROM_QSTR(MP_QSTR_fillRoundRect),        MP_ROM_PTR(&display_tft_fillRoundRect_obj) },

    // { MP_ROM_QSTR(MP_QSTR_setBrightness),       MP_ROM_PTR(&display_tft_setBrightness_obj) },
    // { MP_ROM_QSTR(MP_QSTR_backlight),           MP_ROM_PTR(&display_tft_setBrightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_qrcode),              MP_ROM_PTR(&display_tft_qrcode_obj) },

    { MP_ROM_QSTR(MP_QSTR_tft_setspeed),        MP_ROM_PTR(&display_tft_set_speed_obj) },
    { MP_ROM_QSTR(MP_QSTR_tft_select),            MP_ROM_PTR(&display_tft_select_obj) },
    { MP_ROM_QSTR(MP_QSTR_tft_deselect),        MP_ROM_PTR(&display_tft_deselect_obj) },
    { MP_ROM_QSTR(MP_QSTR_tft_writecmd),        MP_ROM_PTR(&display_tft_send_command_obj) },
    { MP_ROM_QSTR(MP_QSTR_tft_writecmddata),    MP_ROM_PTR(&display_tft_send_cmd_data_obj) },
    { MP_ROM_QSTR(MP_QSTR_tft_readcmd),            MP_ROM_PTR(&display_tft_cmd_read_obj) },

    { MP_ROM_QSTR(MP_QSTR_CENTER),                MP_ROM_INT(CENTER) },
    { MP_ROM_QSTR(MP_QSTR_RIGHT),                MP_ROM_INT(RIGHT) },
    { MP_ROM_QSTR(MP_QSTR_BOTTOM),                MP_ROM_INT(BOTTOM) },
    { MP_ROM_QSTR(MP_QSTR_LASTX),                MP_ROM_INT(LASTX) },
    { MP_ROM_QSTR(MP_QSTR_LASTY),                MP_ROM_INT(LASTY) },

    { MP_ROM_QSTR(MP_QSTR_PORTRAIT),            MP_ROM_INT(PORTRAIT) },
    { MP_ROM_QSTR(MP_QSTR_LANDSCAPE),            MP_ROM_INT(LANDSCAPE) },
    { MP_ROM_QSTR(MP_QSTR_PORTRAIT_FLIP),        MP_ROM_INT(PORTRAIT_FLIP) },
    { MP_ROM_QSTR(MP_QSTR_LANDSCAPE_FLIP),        MP_ROM_INT(LANDSCAPE_FLIP) },
#endif
    { MP_ROM_QSTR(MP_QSTR_FONT_Default),        MP_ROM_INT(DEFAULT_FONT) },
    { MP_ROM_QSTR(MP_QSTR_FONT_DejaVu18),        MP_ROM_INT(DEJAVU18_FONT) },
    { MP_ROM_QSTR(MP_QSTR_FONT_DejaVu24),        MP_ROM_INT(DEJAVU24_FONT) },
    { MP_ROM_QSTR(MP_QSTR_FONT_DejaVu40),        MP_ROM_INT(DEJAVU40_FONT) },
#if 0
    { MP_ROM_QSTR(MP_QSTR_FONT_DejaVu56),        MP_ROM_INT(DEJAVU56_FONT) },
    { MP_ROM_QSTR(MP_QSTR_FONT_DejaVu72),        MP_ROM_INT(DEJAVU72_FONT) },
    { MP_ROM_QSTR(MP_QSTR_FONT_Ubuntu),            MP_ROM_INT(UBUNTU16_FONT) },
    { MP_ROM_QSTR(MP_QSTR_FONT_Comic),            MP_ROM_INT(COMIC24_FONT) },
    { MP_ROM_QSTR(MP_QSTR_FONT_Minya),            MP_ROM_INT(MINYA24_FONT) },
    { MP_ROM_QSTR(MP_QSTR_FONT_Tooney),            MP_ROM_INT(TOONEY32_FONT) },
    { MP_ROM_QSTR(MP_QSTR_FONT_Small),            MP_ROM_INT(SMALL_FONT) },
    { MP_ROM_QSTR(MP_QSTR_FONT_DefaultSmall),    MP_ROM_INT(DEF_SMALL_FONT) },
    { MP_ROM_QSTR(MP_QSTR_FONT_7seg),             MP_ROM_INT(FONT_7SEG) },
    // { MP_ROM_QSTR(MP_QSTR_FONT_UNICODE),         MP_ROM_INT(UNICODE_FONT) },

    { MP_ROM_QSTR(MP_QSTR_ICON_SetMenu),        MP_ROM_INT(ICON_SETMENU) },
    { MP_ROM_QSTR(MP_QSTR_ICON_Machine),        MP_ROM_INT(ICON_MACHINE) },
    { MP_ROM_QSTR(MP_QSTR_ICON_ListON),         MP_ROM_INT(ICON_LIST_ON) },
    { MP_ROM_QSTR(MP_QSTR_ICON_ListOFF),        MP_ROM_INT(ICON_LIST_OFF) },
 

    { MP_ROM_QSTR(MP_QSTR_ICON_App),            MP_ROM_INT(ICON_APP) },
    { MP_ROM_QSTR(MP_QSTR_ICON_AppIcon),        MP_ROM_INT(ICON_APP_ICON) },
    { MP_ROM_QSTR(MP_QSTR_ICON_AppMenu),        MP_ROM_INT(ICON_APP_MENU) },
    { MP_ROM_QSTR(MP_QSTR_ICON_Eath),           MP_ROM_INT(ICON_EATH) },
    { MP_ROM_QSTR(MP_QSTR_ICON_Key),            MP_ROM_INT(ICON_KEY) },
    { MP_ROM_QSTR(MP_QSTR_ICON_Retry),          MP_ROM_INT(ICON_RETRY) },
    { MP_ROM_QSTR(MP_QSTR_ICON_Setup),          MP_ROM_INT(ICON_SETUP) },
    { MP_ROM_QSTR(MP_QSTR_ICON_Url),            MP_ROM_INT(ICON_URL) },
    { MP_ROM_QSTR(MP_QSTR_ICON_Wifi),           MP_ROM_INT(ICON_WIFI) },
    { MP_ROM_QSTR(MP_QSTR_ICON_WifiBig),        MP_ROM_INT(ICON_WIFI_BIG) },
    { MP_ROM_QSTR(MP_QSTR_ICON_USB),            MP_ROM_INT(ICON_USB) },
    { MP_ROM_QSTR(MP_QSTR_ICON_Cloud),          MP_ROM_INT(ICON_CLOUD) },
#endif
    { MP_ROM_QSTR(MP_QSTR_BLACK),                MP_ROM_INT(iTFT_BLACK) },
    { MP_ROM_QSTR(MP_QSTR_NAVY),                MP_ROM_INT(iTFT_NAVY) },
    { MP_ROM_QSTR(MP_QSTR_DARKGREEN),            MP_ROM_INT(iTFT_DARKGREEN) },
    { MP_ROM_QSTR(MP_QSTR_DARKCYAN),            MP_ROM_INT(iTFT_DARKCYAN) },
    { MP_ROM_QSTR(MP_QSTR_MAROON),                MP_ROM_INT(iTFT_MAROON) },
    { MP_ROM_QSTR(MP_QSTR_PURPLE),                MP_ROM_INT(iTFT_PURPLE) },
    { MP_ROM_QSTR(MP_QSTR_OLIVE),                MP_ROM_INT(iTFT_OLIVE) },
    { MP_ROM_QSTR(MP_QSTR_LIGHTGREY),            MP_ROM_INT(iTFT_LIGHTGREY) },
    { MP_ROM_QSTR(MP_QSTR_DARKGREY),            MP_ROM_INT(iTFT_DARKGREY) },
    { MP_ROM_QSTR(MP_QSTR_BLUE),                MP_ROM_INT(iTFT_BLUE) },
    { MP_ROM_QSTR(MP_QSTR_GREEN),                MP_ROM_INT(iTFT_GREEN) },
    { MP_ROM_QSTR(MP_QSTR_CYAN),                MP_ROM_INT(iTFT_CYAN) },
    { MP_ROM_QSTR(MP_QSTR_RED),                    MP_ROM_INT(iTFT_RED) },
    { MP_ROM_QSTR(MP_QSTR_MAGENTA),                MP_ROM_INT(iTFT_MAGENTA) },
    { MP_ROM_QSTR(MP_QSTR_YELLOW),                MP_ROM_INT(iTFT_YELLOW) },
    { MP_ROM_QSTR(MP_QSTR_WHITE),                MP_ROM_INT(iTFT_WHITE) },
    { MP_ROM_QSTR(MP_QSTR_ORANGE),                MP_ROM_INT(iTFT_ORANGE) },
    { MP_ROM_QSTR(MP_QSTR_GREENYELLOW),            MP_ROM_INT(iTFT_GREENYELLOW) },
    { MP_ROM_QSTR(MP_QSTR_PINK),                MP_ROM_INT(iTFT_PINK) },

    { MP_ROM_QSTR(MP_QSTR_COLOR_BITS16),        MP_ROM_INT(16) },
    { MP_ROM_QSTR(MP_QSTR_COLOR_BITS24),        MP_ROM_INT(24) },

    { MP_ROM_QSTR(MP_QSTR_JPG),                    MP_ROM_INT(IMAGE_TYPE_JPG) },
    { MP_ROM_QSTR(MP_QSTR_BMP),                    MP_ROM_INT(IMAGE_TYPE_BMP) },

    { MP_ROM_QSTR(MP_QSTR_HSPI),                MP_ROM_INT(HSPI_HOST) },
    { MP_ROM_QSTR(MP_QSTR_VSPI),                MP_ROM_INT(VSPI_HOST) },
};
STATIC MP_DEFINE_CONST_DICT(display_tft_locals_dict, display_tft_locals_dict_table);

// =====================================
const mp_obj_type_t display_tft_type = {
    { &mp_type_type },
    .name = MP_QSTR_TFT,
    .print = display_tft_printinfo,
    .make_new = display_tft_make_new,
    .locals_dict = (mp_obj_t)&display_tft_locals_dict,
};