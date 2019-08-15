/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#define CONFIG_LOGMACRO_DETAILS

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jse_common.h"
#include "be_module.h"
#include "fontlib.h"
#include "module_lcd.h"

typedef struct lcd_handle {
    uint32_t width;
    uint32_t height;

} lcd_handle_t;

static lcd_handle_t *g_lcd = NULL;

static uint16_t lcd_rgb565(uint8_t r, uint8_t g, uint8_t b)
{
    return (((uint16_t)(r >> 3)) << 11) + (((uint16_t)(g >> 2)) << 5) +
           ((uint16_t)(b >> 3));
}

static int8_t lcd_init(uint32_t width, uint32_t height)
{
    if (NULL != g_lcd) {
        return (-1);
    }
    lcd_handle_t *new_handle = jse_calloc(1, sizeof(*new_handle));
    if (NULL == new_handle) {
        return (-1);
    }
    hal_lcd_init(NULL);
    new_handle->height = height;
    new_handle->width  = width;
    g_lcd              = new_handle;
    return (0);
}

static int8_t lcd_deinit(void)
{
    if (NULL != g_lcd) {
        jse_free(g_lcd);
        g_lcd = NULL;
    }
    return (0);
}

static int8_t lcd_draw_char(uint16_t x, uint16_t y, uint8_t num, uint8_t size,
                            uint32_t fcolor)
{
    uint8_t i            = 0;
    uint8_t j            = 0;
    uint8_t temp         = 0;
    uint16_t write_count = 0;
    uint16_t xtemp       = x;
    uint16_t ytemp       = y;
    uint8_t csize        = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
    uint8_t char_len     = 0;
    uint8_t *pos         = NULL;
    num                  = num - ' ';
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
                hal_lcd_draw_point(xtemp, ytemp, fcolor);
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
    if (NULL == g_lcd) {
        return (-1);
    }
    xres = g_lcd->width;
    yres = g_lcd->height;
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
    return (0);
}

static void lcd_draw_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                          uint32_t color, uint8_t need_fill)
{
    int32_t i = 0;
    int32_t j = 0;
    if (need_fill) {
        for (i = y1; i <= y2; i++) {
            hal_lcd_draw_line(x1, i, color, x2 - x1 + 1);
        }
    } else {
        for (i = x1; i <= x2; ++i) {
            hal_lcd_draw_point(i, y1, color);
            hal_lcd_draw_point(i, y2, color);
        }
        for (i = y1; i <= y2; ++i) {
            hal_lcd_draw_point(x1, i, color);
            hal_lcd_draw_point(x2, i, color);
        }
    }
}

static be_jse_symbol_t *lcd_open(void)
{
    be_jse_int_t ret                   = 0;
    be_jse_symbol_t *arg               = NULL;
    be_jse_symbol_t *lcd_width_symbol  = NULL;
    be_jse_symbol_t *lcd_height_symbol = NULL;
    be_jse_symbol_t *s0                = NULL;
    be_jse_symbol_t *s1                = NULL;
    be_jse_handle_function(0, &arg, NULL, NULL, NULL);
    if (!arg) {
        ret = -1;
        goto out;
    }
    s0 = lookup_named_child_symbol(get_symbol_node_id(arg), "height", false);
    s1 = lookup_named_child_symbol(get_symbol_node_id(arg), "width", false);
    lcd_width_symbol  = get_symbol_value(s0);
    lcd_height_symbol = get_symbol_value(s1);
    if (!lcd_width_symbol || !lcd_height_symbol) {
        ret = -1;
        goto out;
    }
    uint32_t width_value  = get_symbol_value_int(s0);
    uint32_t height_value = get_symbol_value_int(s1);
    ret                   = lcd_init(width_value, height_value);
out:

    if (arg) symbol_unlock(arg);
    if (s0) symbol_unlock(s0);
    if (s1) symbol_unlock(s1);
    if (lcd_width_symbol) symbol_unlock(lcd_width_symbol);
    if (lcd_height_symbol) symbol_unlock(lcd_height_symbol);
    return new_int_symbol(ret);
}

static be_jse_symbol_t *lcd_close(void)
{
    be_jse_handle_function(0, NULL, NULL, NULL, NULL);
    lcd_deinit();
    return new_int_symbol(0);
}

static be_jse_symbol_t *lcd_rect(void)
{
    int8_t is_fill                    = 0;
    be_jse_int_t ret                  = 0;
    be_jse_symbol_t *arg              = NULL;
    be_jse_symbol_t *lcd_color_symbol = NULL;
    be_jse_symbol_t *lcd_sx_symbol    = NULL;
    be_jse_symbol_t *lcd_sy_symbol    = NULL;
    be_jse_symbol_t *lcd_ex_symbol    = NULL;
    be_jse_symbol_t *lcd_ey_symbol    = NULL;
    be_jse_symbol_t *need_fill_symbol = NULL;

    be_jse_symbol_t *s0        = NULL;
    be_jse_symbol_t *s1        = NULL;
    be_jse_symbol_t *s2        = NULL;
    be_jse_symbol_t *s3        = NULL;
    be_jse_symbol_t *s4        = NULL;
    be_jse_symbol_t *need_fill = NULL;

    be_jse_handle_function(0, &arg, NULL, NULL, NULL);
    if (!arg) {
        ret = -1;
        goto out;
    }
    s0 = lookup_named_child_symbol(get_symbol_node_id(arg), "color", false);
    lcd_color_symbol = get_symbol_value(s0);
    if (!lcd_color_symbol || !symbol_is_int(lcd_color_symbol)) {
        ret = -1;
        goto out;
    }

    s1 = lookup_named_child_symbol(get_symbol_node_id(arg), "sx", false);
    s2 = lookup_named_child_symbol(get_symbol_node_id(arg), "sy", false);
    s3 = lookup_named_child_symbol(get_symbol_node_id(arg), "ex", false);
    s4 = lookup_named_child_symbol(get_symbol_node_id(arg), "ey", false);
    if (NULL == s1 || NULL == s2 || NULL == s3 || NULL == s4) {
        ret = -1;
        goto out;
    }
    lcd_sx_symbol = get_symbol_value(s1);
    lcd_sy_symbol = get_symbol_value(s2);
    lcd_ex_symbol = get_symbol_value(s3);
    lcd_ey_symbol = get_symbol_value(s4);

    need_fill =
        lookup_named_child_symbol(get_symbol_node_id(arg), "fill", false);
    if (need_fill) {
        need_fill_symbol = get_symbol_value(need_fill);
        is_fill          = get_symbol_int(need_fill_symbol) ? (0x01) : (0x00);
    }

    be_jse_int_t color_value = get_symbol_int(lcd_color_symbol);
    be_jse_int_t sx          = get_symbol_int(lcd_sx_symbol);
    be_jse_int_t sy          = get_symbol_int(lcd_sy_symbol);
    be_jse_int_t ex          = get_symbol_int(lcd_ex_symbol);
    be_jse_int_t ey          = get_symbol_int(lcd_ey_symbol);
    lcd_draw_rect(sx, sy, ex, ey, color_value, is_fill);
out:

    symbol_unlock(arg);
    symbol_unlock(s0);
    symbol_unlock(s1);
    symbol_unlock(s2);
    symbol_unlock(s3);
    symbol_unlock(s4);
    symbol_unlock(lcd_color_symbol);
    symbol_unlock(lcd_sx_symbol);
    symbol_unlock(lcd_sy_symbol);
    symbol_unlock(lcd_ex_symbol);
    symbol_unlock(lcd_ey_symbol);
    symbol_unlock(need_fill_symbol);
    symbol_unlock(need_fill);
    return new_int_symbol(ret);
}

static be_jse_symbol_t *lcd_write(void)
{
    be_jse_int_t ret                 = 0;
    be_jse_symbol_t *arg             = NULL;
    be_jse_symbol_t *lcd_x_symbol    = NULL;
    be_jse_symbol_t *lcd_y_symbol    = NULL;
    be_jse_symbol_t *lcd_fc_symbol   = NULL;
    be_jse_symbol_t *lcd_bc_symbol   = NULL;
    be_jse_symbol_t *lcd_str_symbol  = NULL;
    be_jse_symbol_t *lcd_size_symbol = NULL;
    be_jse_symbol_t *lcd_new_line    = NULL;
    be_jse_symbol_t *s0              = NULL;
    be_jse_symbol_t *s1              = NULL;
    be_jse_symbol_t *s2              = NULL;
    be_jse_symbol_t *s3              = NULL;
    be_jse_symbol_t *s4              = NULL;
    be_jse_symbol_t *s5              = NULL;
    be_jse_symbol_t *s6              = NULL;

    be_jse_handle_function(0, &arg, NULL, NULL, NULL);
    if (!arg) {
        ret = -1;
        goto out;
    }
    s0 = lookup_named_child_symbol(get_symbol_node_id(arg), "x", false);
    s1 = lookup_named_child_symbol(get_symbol_node_id(arg), "y", false);
    s2 = lookup_named_child_symbol(get_symbol_node_id(arg), "color", false);
    s3 = lookup_named_child_symbol(get_symbol_node_id(arg), "newline", false);
    s4 = lookup_named_child_symbol(get_symbol_node_id(arg), "str", false);
    s5 = lookup_named_child_symbol(get_symbol_node_id(arg), "size", false);
    if (NULL == s0 || NULL == s1 || NULL == s2 || NULL == s3 || NULL == s4 ||
        NULL == s5) {
        ret = -1;
        goto out;
    }

    lcd_x_symbol    = get_symbol_value(s0);
    lcd_y_symbol    = get_symbol_value(s1);
    lcd_fc_symbol   = get_symbol_value(s2);
    lcd_new_line    = get_symbol_value(s3);
    lcd_str_symbol  = get_symbol_value(s4);
    lcd_size_symbol = get_symbol_value(s5);

    be_jse_int_t x_pos          = get_symbol_int(lcd_x_symbol);
    be_jse_int_t y_pos          = get_symbol_int(lcd_y_symbol);
    be_jse_int_t front_size     = get_symbol_int(lcd_size_symbol);
    be_jse_int_t is_newline     = get_symbol_int(lcd_new_line);
    be_jse_int_t fc_color_value = get_symbol_int(lcd_fc_symbol);

    if ((16 != front_size) && (12 != front_size) && (24 != front_size) &&
        (36 != front_size)) {
        ret = -1;
        goto out;
    }
    int str_len    = symbol_str_len(lcd_str_symbol);
    char *str_buff = jse_malloc(sizeof(char) * (str_len + 1));
    if (NULL == str_buff) {
        ret = -1;
        goto out;
    }
    symbol_to_str(lcd_str_symbol, str_buff, str_len);
    str_buff[str_len] = 0x00;
    lcd_draw_str(x_pos, y_pos, (uint8_t *)str_buff, front_size, fc_color_value,
                 is_newline);
    if (NULL != str_buff) {
        jse_free(str_buff);
        str_buff = NULL;
    }

out:

    symbol_unlock(arg);
    symbol_unlock(lcd_x_symbol);
    symbol_unlock(lcd_y_symbol);
    symbol_unlock(lcd_fc_symbol);
    symbol_unlock(lcd_bc_symbol);
    symbol_unlock(lcd_str_symbol);
    symbol_unlock(lcd_size_symbol);
    symbol_unlock(lcd_new_line);
    symbol_unlock(s0);
    symbol_unlock(s1);
    symbol_unlock(s2);
    symbol_unlock(s3);
    symbol_unlock(s4);
    symbol_unlock(s5);
    symbol_unlock(s6);
    return new_int_symbol(ret);
}

be_jse_symbol_t *lcd_module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                      be_jse_symbol_t *var, const char *name)
{
    if (0 == strcmp(name, "open")) return lcd_open();
    if (0 == strcmp(name, "close")) return lcd_close();
    if (0 == strcmp(name, "show")) return lcd_write();
    if (0 == strcmp(name, "fill")) return lcd_rect();
    return BE_JSE_FUNC_UNHANDLED;
}

void module_lcd_register(void)
{
    be_jse_module_load("LCD", lcd_module_handle_cb);
}
