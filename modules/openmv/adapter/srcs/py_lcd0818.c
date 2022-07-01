/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2019 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2019 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * LCD Python module.
 */
#include "py/obj.h"
#include "py/nlr.h"
#include "py/runtime.h"

#include "py_helper.h"

extern void get_lcd_reslution(uint16_t *p_width, uint16_t *p_height);
extern void lcd_driver_flush(int x1, int y1, int x2, int y2, uint8_t *rgb565_data);
extern void st7789_enable_backlight(bool backlight);

static bool bl_intensity;
static uint16_t lcd_width = 0, lcd_height = 0;

static void spi_lcd_draw_image_cb(int x_start, int x_end, int y_row, imlib_draw_row_data_t *data)
{
  //printf("spi_lcd_draw_image_cb (%d->%d), %d\n", x_start, x_end, y_row);
  lcd_driver_flush(x_start, x_end, y_row, y_row, data->dst_row_override);
}

STATIC mp_obj_t py_lcd_display(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *src_img = py_helper_arg_to_image_mutable_bayer(args[0]);

    int arg_x_off = 0;
    int arg_y_off = 0;
    uint offset = 1;
    if (n_args > 1) {
        if (MP_OBJ_IS_TYPE(args[1], &mp_type_tuple) || MP_OBJ_IS_TYPE(args[1], &mp_type_list)) {
            mp_obj_t *arg_vec;
            mp_obj_get_array_fixed_n(args[1], 2, &arg_vec);
            arg_x_off = mp_obj_get_int(arg_vec[0]);
            arg_y_off = mp_obj_get_int(arg_vec[1]);
            offset = 2;
        } else if (n_args > 2) {
            arg_x_off = mp_obj_get_int(args[1]);
            arg_y_off = mp_obj_get_int(args[2]);
            offset = 3;
        } else if (n_args > 1) {
            mp_raise_msg(&mp_type_TypeError, MP_ERROR_TEXT("Expected x and y offset!"));
        }
    }

    float arg_x_scale = 1.f;
    bool got_x_scale = py_helper_keyword_float_maybe(n_args, args, offset + 0, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_scale), &arg_x_scale);

    float arg_y_scale = 1.f;
    bool got_y_scale = py_helper_keyword_float_maybe(n_args, args, offset + 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_scale), &arg_y_scale);

    rectangle_t arg_roi;
    py_helper_keyword_rectangle_roi(src_img, n_args, args, offset + 2, kw_args, &arg_roi);

    int arg_rgb_channel = py_helper_keyword_int(n_args, args, offset + 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_rgb_channel), -1);
    if ((arg_rgb_channel < -1) || (2 < arg_rgb_channel)) mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("-1 <= rgb_channel <= 2!"));

    int arg_alpha = py_helper_keyword_int(n_args, args, offset + 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_alpha), 256);
    if ((arg_alpha < 0) || (256 < arg_alpha)) mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("0 <= alpha <= 256!"));

    const uint16_t *color_palette = py_helper_keyword_color_palette(n_args, args, offset + 5, kw_args, NULL);
    const uint8_t *alpha_palette = py_helper_keyword_alpha_palette(n_args, args, offset + 6, kw_args, NULL);

    image_hint_t hint = py_helper_keyword_int(n_args, args, offset + 7, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_hint), 0);

    int arg_x_size;
    bool got_x_size = py_helper_keyword_int_maybe(n_args, args, offset + 8, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_size), &arg_x_size);

    int arg_y_size;
    bool got_y_size = py_helper_keyword_int_maybe(n_args, args, offset + 9, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_size), &arg_y_size);

    if (got_x_scale && got_x_size) mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Choose either x_scale or x_size not both!"));
    if (got_y_scale && got_y_size) mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Choose either y_scale or y_size not both!"));

    if (got_x_size) arg_x_scale = arg_x_size / ((float) arg_roi.w);
    if (got_y_size) arg_y_scale = arg_y_size / ((float) arg_roi.h);

    if ((!got_x_scale) && (!got_x_size) && got_y_size) arg_x_scale = arg_y_scale;
    if ((!got_y_scale) && (!got_y_size) && got_x_size) arg_y_scale = arg_x_scale;
    
    get_lcd_reslution(&lcd_width, &lcd_height);
    image_t dst_img;
    dst_img.w = lcd_width;
    dst_img.h = lcd_height;
    dst_img.bpp = IMAGE_BPP_RGB565; 
    
    int x0, x1, y0, y1;
    bool black = !imlib_draw_image_rectangle(&dst_img, src_img, arg_x_off, arg_y_off, arg_x_scale, arg_y_scale, &arg_roi, arg_alpha, alpha_palette, hint, &x0, &x1, &y0, &y1);
    if(black) {
      mp_raise_msg(&mp_type_OSError,
                         MP_ERROR_TEXT("Display parameters error!")); 
      goto out;
    }
    x1 = x1 > lcd_width?lcd_width:x1;
    y1 = y1 > lcd_height?lcd_height:y1;  
    
    printf("%s: (%d,%d) -> (%d,%d), src_img (%d, %d, %d), %p\n", __func__, x0, y0, x1, y1, src_img->w, src_img->h, src_img->bpp, src_img->data);
    printf("arg_x_off %d , arg_y_off %d, arg_x_scale %f, arg_y_scale %f, arg_roi(%d, %d, %d, %d), arg_rgb_channel %d, arg_alpha %d, color_palette %p, alpha_palette %p, hint %d\n",
      arg_x_off, arg_y_off, arg_x_scale, arg_y_scale, arg_roi.w, arg_roi.h, arg_roi.x, arg_roi.y, arg_rgb_channel, arg_alpha, color_palette, alpha_palette, hint);
    
    fb_alloc_mark();
    if(x1-x0 == lcd_width && y1-y0 == lcd_height && dst_img.bpp == src_img->bpp) {
      dst_img.data = src_img->data;
       
    } else {      
      dst_img.data = fb_alloc0(image_size(&dst_img), FB_ALLOC_NO_HINT);
    // Transmits left/right parts already zeroed...
      imlib_draw_image(&dst_img, src_img, arg_x_off, arg_y_off, arg_x_scale, arg_y_scale, &arg_roi,
                       arg_rgb_channel, arg_alpha, color_palette, alpha_palette, hint | IMAGE_HINT_BLACK_BACKGROUND, NULL, NULL);
    }
    if(src_img->bpp != IMAGE_BPP_RGB565)
      for(int k=0;k<image_size(&dst_img);k+=2) {
        uint16_t *ptemp = (uint16_t*) &dst_img.data[k];
        *ptemp = __builtin_bswap16(*ptemp);//((*ptemp&0xff)<<8) | ((*ptemp&0xff00)>>8);        
      }
    lcd_driver_flush(x0, y0, x1-1, y1-1, dst_img.data);    
    fb_free();
    fb_alloc_free_till_mark();
 out:   
    return mp_const_none;        
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_lcd_display_obj, 1, py_lcd_display);

/*Todo: added clear lvgl draw_buf memory here*/
STATIC mp_obj_t py_lcd_clear(uint n_args, const mp_obj_t *args)
{
  bl_intensity = false;
  st7789_enable_backlight(bl_intensity); 
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(py_lcd_clear_obj, 0, 1, py_lcd_clear);

STATIC mp_obj_t py_lcd_width()
{
    if(!lcd_width)
      get_lcd_reslution(&lcd_width, &lcd_height);
    return mp_obj_new_int(lcd_width);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_lcd_width_obj, py_lcd_width);

STATIC mp_obj_t py_lcd_height()
{
    if(!lcd_height)
      get_lcd_reslution(&lcd_width, &lcd_height);
    return mp_obj_new_int(lcd_height);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_lcd_height_obj, py_lcd_height);

STATIC mp_obj_t py_lcd_set_backlight(mp_obj_t intensity_obj)
{
    int intensity = mp_obj_get_int(intensity_obj);
    if ((intensity < 0) || (255 < intensity)) mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("0 <= intensity <= 255!"));
    bl_intensity = !!intensity;
    st7789_enable_backlight(bl_intensity);  
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_lcd_set_backlight_obj, py_lcd_set_backlight);

STATIC mp_obj_t py_lcd_get_backlight()
{
    return mp_obj_new_int(bl_intensity);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_lcd_get_backlight_obj, py_lcd_get_backlight);

STATIC const mp_rom_map_elem_t globals_dict_table[] = {
  { MP_ROM_QSTR(MP_QSTR___name__),                MP_OBJ_NEW_QSTR(MP_QSTR_lcd)                    },
  { MP_ROM_QSTR(MP_QSTR_width),                   MP_ROM_PTR(&py_lcd_width_obj)                   },
  { MP_ROM_QSTR(MP_QSTR_height),                  MP_ROM_PTR(&py_lcd_height_obj)                  },
  { MP_ROM_QSTR(MP_QSTR_get_backlight),           MP_ROM_PTR(&py_lcd_get_backlight_obj)           },
  { MP_ROM_QSTR(MP_QSTR_set_backlight),           MP_ROM_PTR(&py_lcd_set_backlight_obj)           },
  { MP_ROM_QSTR(MP_QSTR_display),                 MP_ROM_PTR(&py_lcd_display_obj)                 },
  { MP_ROM_QSTR(MP_QSTR_clear),                   MP_ROM_PTR(&py_lcd_clear_obj)                   }
};
STATIC MP_DEFINE_CONST_DICT(globals_dict, globals_dict_table);

const mp_obj_module_t lcd_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_t) &globals_dict,
};

