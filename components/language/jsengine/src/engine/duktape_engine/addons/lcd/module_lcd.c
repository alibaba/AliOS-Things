/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "jse_common.h"
#include "be_inl.h"
#include "fontlib.h"

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
    lcd_handle_t *new_handle = jse_calloc(1, sizeof(*new_handle));
    if (NULL == new_handle) {
        return -1;
    }
    hal_lcd_init(NULL);
    new_handle->height = height;
    new_handle->width  = width;
    lcd_handle         = new_handle;
    return 0;
}

static int8_t lcd_deinit(void)
{
    if (NULL != lcd_handle) {
        jse_free(lcd_handle);
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
        for (i = y1; i <= y2; i++) hal_lcd_draw_line(x1, i, color, x2 - x1 + 1);
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

static duk_ret_t native_lcd_open(duk_context *ctx)
{
    int ret = -1;

    if (!duk_is_object(ctx, 0)) {
        jse_warn("parameter must be object\n");
        goto out;
    }
    duk_get_prop_string(ctx, 0, "height");
    duk_get_prop_string(ctx, 0, "width");
    /* [ parameter height width ] */
    if (!duk_is_number(ctx, -1) || !duk_is_number(ctx, -2)) {
        jse_warn("parameter object must be `{height:number,width:number}`\n");
        duk_pop_2(ctx);
        goto out;
    }
    uint32_t width  = duk_get_int(ctx, -1);
    uint32_t height = duk_get_int(ctx, -2);
    duk_pop_2(ctx);
    ret = lcd_init(width, height);
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_lcd_close(duk_context *ctx)
{
    lcd_deinit();
    duk_push_int(ctx, 0);
    return 1;
}

static duk_ret_t native_lcd_fill(duk_context *ctx)
{
    int ret = -1;

    if (!duk_is_object(ctx, 0)) {
        jse_warn("parameter must be object\n");
        goto out;
    }

    /* [ parameter sx sy ex ey color fill ] */
    duk_get_prop_string(ctx, 0, "sx");
    duk_get_prop_string(ctx, 0, "sy");
    duk_get_prop_string(ctx, 0, "ex");
    duk_get_prop_string(ctx, 0, "ey");
    duk_get_prop_string(ctx, 0, "color");
    duk_get_prop_string(ctx, 0, "fill");

    if (!duk_is_number(ctx, -6)    /* sx */
        || !duk_is_number(ctx, -5) /* sy */
        || !duk_is_number(ctx, -4) /* ex */
        || !duk_is_number(ctx, -3) /* ey */
        || !duk_is_number(ctx, -2) /* color */
    ) {
        jse_warn(
            "parameter object must be "
            "`{sx:number,sy:number,ex:number,ey:number,color:number}`\n");
        goto pop_out;
    }

    int sx      = duk_get_int(ctx, -6);
    int sy      = duk_get_int(ctx, -5);
    int ex      = duk_get_int(ctx, -4);
    int ey      = duk_get_int(ctx, -3);
    int color   = duk_get_int(ctx, -2);
    int is_fill = duk_get_int_default(ctx, -1, 0);
    lcd_draw_rect(sx, sy, ex, ey, color, is_fill);
    ret = 0;

pop_out:
    duk_pop_n(ctx, 6);

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_lcd_show(duk_context *ctx)
{
    int ret = -1;

    if (!duk_is_object(ctx, 0)) {
        jse_warn("parameter must be object\n");
        goto out;
    }

    /* [ parameter x y color newline str size ] */
    duk_get_prop_string(ctx, 0, "x");
    duk_get_prop_string(ctx, 0, "y");
    duk_get_prop_string(ctx, 0, "color");
    duk_get_prop_string(ctx, 0, "newline");
    duk_get_prop_string(ctx, 0, "str");
    duk_get_prop_string(ctx, 0, "size");

    if (!duk_is_number(ctx, -6)    /* x */
        || !duk_is_number(ctx, -5) /* y */
        || !duk_is_number(ctx, -4) /* color */
        || !duk_is_number(ctx, -3) /* newline */
        || !duk_is_string(ctx, -2) /* str */
        || !duk_is_number(ctx, -1) /* size */
    ) {
        jse_warn(
            "parameter object must be "
            "`{x:number,y:number,color:number,newline:number,str:string,size:"
            "number}`\n");
        goto pop_out;
    }

    int x_pos       = duk_get_int(ctx, -6);
    int y_pos       = duk_get_int(ctx, -5);
    int color       = duk_get_int(ctx, -4);
    int is_newline  = duk_get_int(ctx, -3);
    const char *str = duk_get_string(ctx, -2);
    int front_size  = duk_get_int(ctx, -1);

    if ((16 != front_size) && (12 != front_size) && (24 != front_size) &&
        (36 != front_size)) {
        ret = -1;
        goto pop_out;
    }

    lcd_draw_str(x_pos, y_pos, (uint8_t *)str, front_size, color, is_newline);
    ret = 0;

pop_out:
    duk_pop_n(ctx, 6);

out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_lcd_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_lcd_open, 1);
    duk_put_prop_string(ctx, -2, "open");

    duk_push_c_function(ctx, native_lcd_close, 1);
    duk_put_prop_string(ctx, -2, "close");

    duk_push_c_function(ctx, native_lcd_show, 1);
    duk_put_prop_string(ctx, -2, "show");

    duk_push_c_function(ctx, native_lcd_fill, 1);
    duk_put_prop_string(ctx, -2, "fill");

    duk_put_global_string(ctx, "LCD");
}
