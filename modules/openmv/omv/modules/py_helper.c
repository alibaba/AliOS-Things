/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Python helper functions.
 */
#include "py/obj.h"
#include "py/runtime.h"
#include "framebuffer.h"
#include "sensor.h"
#include "py_helper.h"
#include "py_assert.h"

extern void *py_image_cobj(mp_obj_t img_obj);

mp_obj_t py_func_unavailable(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    PY_ASSERT_TRUE_MSG(false, "This function is unavailable on your OpenMV Cam.");
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(py_func_unavailable_obj, 0, py_func_unavailable);

image_t *py_helper_arg_to_image_mutable(const mp_obj_t arg)
{
    image_t *arg_img = py_image_cobj(arg);
    PY_ASSERT_TRUE_MSG(IMAGE_IS_MUTABLE(arg_img), "Image is not mutable!");
    return arg_img;
}

image_t *py_helper_arg_to_image_mutable_bayer(const mp_obj_t arg)
{
    image_t *arg_img = py_image_cobj(arg);
    PY_ASSERT_TRUE_MSG(IMAGE_IS_MUTABLE_BAYER(arg_img), "Image is not mutable!");
    return arg_img;
}

image_t *py_helper_arg_to_image_mutable_bayer_jpeg(const mp_obj_t arg)
{
    image_t *arg_img = py_image_cobj(arg);
    PY_ASSERT_TRUE_MSG(IMAGE_IS_MUTABLE_BAYER_JPEG(arg_img), "Image is not mutable!");
    return arg_img;
}

image_t *py_helper_arg_to_image_grayscale(const mp_obj_t arg)
{
    image_t *arg_img = py_image_cobj(arg);
    PY_ASSERT_TRUE_MSG(arg_img->bpp == IMAGE_BPP_GRAYSCALE, "Image is not grayscale!");
    return arg_img;
}

image_t *py_helper_arg_to_image_color(const mp_obj_t arg)
{
    image_t *arg_img = py_image_cobj(arg);
    PY_ASSERT_TRUE_MSG(arg_img->bpp == IMAGE_BPP_RGB565, "Image is not RGB565!");
    return arg_img;
}

image_t *py_helper_keyword_to_image_mutable(uint n_args, const mp_obj_t *args, uint arg_index,
                                            mp_map_t *kw_args, mp_obj_t kw, image_t *default_val)
{
    mp_map_elem_t *kw_arg = mp_map_lookup(kw_args, kw, MP_MAP_LOOKUP);

    if (kw_arg) {
        default_val = py_helper_arg_to_image_mutable(kw_arg->value);
    } else if (n_args > arg_index) {
        default_val = py_helper_arg_to_image_mutable(args[arg_index]);
    }

    return default_val;
}

image_t *py_helper_keyword_to_image_mutable_mask(uint n_args, const mp_obj_t *args, uint arg_index,
                                                 mp_map_t *kw_args)
{
    return py_helper_keyword_to_image_mutable(n_args, args, arg_index, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_mask), NULL);
}

image_t *py_helper_keyword_to_image_mutable_color_palette(uint n_args, const mp_obj_t *args, uint arg_index,
                                                          mp_map_t *kw_args)
{
    return py_helper_keyword_to_image_mutable(n_args, args, arg_index, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_color_palette), NULL);
}

image_t *py_helper_keyword_to_image_mutable_alpha_palette(uint n_args, const mp_obj_t *args, uint arg_index,
                                                          mp_map_t *kw_args)
{
    return py_helper_keyword_to_image_mutable(n_args, args, arg_index, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_alpha_palette), NULL);
}

void py_helper_keyword_rectangle(image_t *img, uint n_args, const mp_obj_t *args, uint arg_index,
                                 mp_map_t *kw_args, mp_obj_t kw, rectangle_t *r)
{
    mp_map_elem_t *kw_arg = mp_map_lookup(kw_args, kw, MP_MAP_LOOKUP);

    if (kw_arg) {
        mp_obj_t *arg_rectangle;
        mp_obj_get_array_fixed_n(kw_arg->value, 4, &arg_rectangle);
        r->x = mp_obj_get_int(arg_rectangle[0]);
        r->y = mp_obj_get_int(arg_rectangle[1]);
        r->w = mp_obj_get_int(arg_rectangle[2]);
        r->h = mp_obj_get_int(arg_rectangle[3]);
    } else if (n_args > arg_index) {
        mp_obj_t *arg_rectangle;
        mp_obj_get_array_fixed_n(args[arg_index], 4, &arg_rectangle);
        r->x = mp_obj_get_int(arg_rectangle[0]);
        r->y = mp_obj_get_int(arg_rectangle[1]);
        r->w = mp_obj_get_int(arg_rectangle[2]);
        r->h = mp_obj_get_int(arg_rectangle[3]);
    } else {
        r->x = 0;
        r->y = 0;
        r->w = img->w;
        r->h = img->h;
    }

    PY_ASSERT_TRUE_MSG((r->w >= 1) && (r->h >= 1), "Invalid ROI dimensions!");
    rectangle_t temp;
    temp.x = 0;
    temp.y = 0;
    temp.w = img->w;
    temp.h = img->h;

    PY_ASSERT_TRUE_MSG(rectangle_overlap(r, &temp), "ROI does not overlap on the image!");
    rectangle_intersected(r, &temp);
}

void py_helper_keyword_rectangle_roi(image_t *img, uint n_args, const mp_obj_t *args, uint arg_index,
                                     mp_map_t *kw_args, rectangle_t *r)
{
    py_helper_keyword_rectangle(img, n_args, args, arg_index, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_roi), r);
}

int py_helper_keyword_int(uint n_args, const mp_obj_t *args, uint arg_index,
                          mp_map_t *kw_args, mp_obj_t kw, int default_val)
{
    mp_map_elem_t *kw_arg = mp_map_lookup(kw_args, kw, MP_MAP_LOOKUP);

    if (kw_arg) {
        default_val = mp_obj_get_int(kw_arg->value);
    } else if (n_args > arg_index) {
        default_val = mp_obj_get_int(args[arg_index]);
    }

    return default_val;
}

bool py_helper_keyword_int_maybe(uint n_args, const mp_obj_t *args, uint arg_index,
                                 mp_map_t *kw_args, mp_obj_t kw, int* value)
{
    mp_map_elem_t *kw_arg = mp_map_lookup(kw_args, kw, MP_MAP_LOOKUP);

    if (kw_arg) {
        return mp_obj_get_int_maybe(kw_arg->value, value);
    } else if (n_args > arg_index) {
        return mp_obj_get_int_maybe(args[arg_index], value);
    }

    return false;
}

float py_helper_keyword_float(uint n_args, const mp_obj_t *args, uint arg_index,
                              mp_map_t *kw_args, mp_obj_t kw, float default_val)
{
    mp_map_elem_t *kw_arg = mp_map_lookup(kw_args, kw, MP_MAP_LOOKUP);

    if (kw_arg) {
        default_val = mp_obj_get_float(kw_arg->value);
    } else if (n_args > arg_index) {
        default_val = mp_obj_get_float(args[arg_index]);
    }

    return default_val;
}

bool py_helper_keyword_float_maybe(uint n_args, const mp_obj_t *args, uint arg_index,
                                   mp_map_t *kw_args, mp_obj_t kw, float *value)
{
    mp_map_elem_t *kw_arg = mp_map_lookup(kw_args, kw, MP_MAP_LOOKUP);

    if (kw_arg) {
        return mp_obj_get_float_maybe(kw_arg->value, value);
    } else if (n_args > arg_index) {
        return mp_obj_get_float_maybe(args[arg_index], value);
    }

    return false;
}

void py_helper_keyword_int_array(uint n_args, const mp_obj_t *args, uint arg_index,
                                 mp_map_t *kw_args, mp_obj_t kw, int *x, int size)
{
    mp_map_elem_t *kw_arg = mp_map_lookup(kw_args, kw, MP_MAP_LOOKUP);

    if (kw_arg) {
        mp_obj_t *arg_array;
        mp_obj_get_array_fixed_n(kw_arg->value, size, &arg_array);
        for (int i = 0; i < size; i++) x[i] = mp_obj_get_int(arg_array[i]);
    } else if (n_args > arg_index) {
        mp_obj_t *arg_array;
        mp_obj_get_array_fixed_n(args[arg_index], size, &arg_array);
        for (int i = 0; i < size; i++) x[i] = mp_obj_get_int(arg_array[i]);
    }
}

void py_helper_keyword_float_array(uint n_args, const mp_obj_t *args, uint arg_index,
                                   mp_map_t *kw_args, mp_obj_t kw, float *x, int size)
{
    mp_map_elem_t *kw_arg = mp_map_lookup(kw_args, kw, MP_MAP_LOOKUP);

    if (kw_arg) {
        mp_obj_t *arg_array;
        mp_obj_get_array_fixed_n(kw_arg->value, size, &arg_array);
        for (int i = 0; i < size; i++) x[i] = mp_obj_get_float(arg_array[i]);
    } else if (n_args > arg_index) {
        mp_obj_t *arg_array;
        mp_obj_get_array_fixed_n(args[arg_index], size, &arg_array);
        for (int i = 0; i < size; i++) x[i] = mp_obj_get_float(arg_array[i]);
    }
}

float *py_helper_keyword_corner_array(uint n_args, const mp_obj_t *args, uint arg_index,
                                      mp_map_t *kw_args, mp_obj_t kw)
{
    mp_map_elem_t *kw_arg = mp_map_lookup(kw_args, kw, MP_MAP_LOOKUP);

    if (kw_arg) {
        mp_obj_t *arg_array;
        mp_obj_get_array_fixed_n(kw_arg->value, 4, &arg_array);
        float *corners = xalloc(sizeof(float) * 8);
        for (int i = 0; i < 4; i++) {
            mp_obj_t *arg_point;
            mp_obj_get_array_fixed_n(arg_array[i], 2, &arg_point);
            corners[(i*2)+0] = mp_obj_get_float(arg_point[0]);
            corners[(i*2)+1] = mp_obj_get_float(arg_point[1]);
        }
        return corners;
    } else if (n_args > arg_index) {
        mp_obj_t *arg_array;
        mp_obj_get_array_fixed_n(args[arg_index], 4, &arg_array);
        float *corners = xalloc(sizeof(float) * 8);
        for (int i = 0; i < 4; i++) {
            mp_obj_t *arg_point;
            mp_obj_get_array_fixed_n(arg_array[i], 2, &arg_point);
            corners[(i*2)+0] = mp_obj_get_float(arg_point[0]);
            corners[(i*2)+1] = mp_obj_get_float(arg_point[1]);
        }
        return corners;
    }

    return NULL;
}

mp_obj_t *py_helper_keyword_iterable(uint n_args, const mp_obj_t *args,
        uint arg_index, mp_map_t *kw_args, mp_obj_t kw, size_t *len)
{
    mp_obj_t itr = NULL;
    mp_obj_t *items = NULL;
    mp_map_elem_t *kw_arg = mp_map_lookup(kw_args, kw, MP_MAP_LOOKUP);

    if (kw_arg) {
        itr = kw_arg->value;
    } else if (n_args > arg_index) {
        itr = args[arg_index];
    }

    if (itr && (MP_OBJ_IS_TYPE(itr, &mp_type_tuple) ||
                MP_OBJ_IS_TYPE(itr, &mp_type_list))) {
        mp_obj_get_array(itr, len, &items);
    }
    return items;
}

uint py_helper_consume_array(uint n_args, const mp_obj_t *args, uint arg_index, size_t len, const mp_obj_t **items)
{
    if (MP_OBJ_IS_TYPE(args[arg_index], &mp_type_tuple) || MP_OBJ_IS_TYPE(args[arg_index], &mp_type_list)) {
        mp_obj_get_array_fixed_n(args[arg_index], len, (mp_obj_t **) items);
        return arg_index + 1;
    } else {
        PY_ASSERT_TRUE_MSG((n_args - arg_index) >= len, "Not enough positional arguments!");
        *items = args + arg_index;
        return arg_index + len;
    }
}

int py_helper_keyword_color(image_t *img, uint n_args, const mp_obj_t *args, uint arg_index,
                            mp_map_t *kw_args, int default_val)
{
    mp_map_elem_t *kw_arg = kw_args ? mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_color), MP_MAP_LOOKUP) : NULL;

    if (kw_arg) {
        if (mp_obj_is_integer(kw_arg->value)) {
            default_val = mp_obj_get_int(kw_arg->value);
        } else {
            mp_obj_t *arg_color;
            mp_obj_get_array_fixed_n(kw_arg->value, 3, &arg_color);
            default_val = COLOR_R8_G8_B8_TO_RGB565(IM_MAX(IM_MIN(mp_obj_get_int(arg_color[0]), COLOR_R8_MAX), COLOR_R8_MIN),
                                                   IM_MAX(IM_MIN(mp_obj_get_int(arg_color[1]), COLOR_G8_MAX), COLOR_G8_MIN),
                                                   IM_MAX(IM_MIN(mp_obj_get_int(arg_color[2]), COLOR_B8_MAX), COLOR_B8_MIN));
            switch(img->bpp) {
                case IMAGE_BPP_BINARY: {
                    default_val = COLOR_RGB565_TO_BINARY(default_val);
                    break;
                }
                case IMAGE_BPP_GRAYSCALE: {
                    default_val = COLOR_RGB565_TO_GRAYSCALE(default_val);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    } else if (n_args > arg_index) {
        if (mp_obj_is_integer(args[arg_index])) {
            default_val = mp_obj_get_int(args[arg_index]);
        } else {
            mp_obj_t *arg_color;
            mp_obj_get_array_fixed_n(args[arg_index], 3, &arg_color);
            default_val = COLOR_R8_G8_B8_TO_RGB565(IM_MAX(IM_MIN(mp_obj_get_int(arg_color[0]), COLOR_R8_MAX), COLOR_R8_MIN),
                                                   IM_MAX(IM_MIN(mp_obj_get_int(arg_color[1]), COLOR_G8_MAX), COLOR_G8_MIN),
                                                   IM_MAX(IM_MIN(mp_obj_get_int(arg_color[2]), COLOR_B8_MAX), COLOR_B8_MIN));
            switch(img->bpp) {
                case IMAGE_BPP_BINARY: {
                    default_val = COLOR_RGB565_TO_BINARY(default_val);
                    break;
                }
                case IMAGE_BPP_GRAYSCALE: {
                    default_val = COLOR_RGB565_TO_GRAYSCALE(default_val);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

    return default_val;
}

void py_helper_arg_to_thresholds(const mp_obj_t arg, list_t *thresholds)
{
    mp_uint_t arg_thresholds_len;
    mp_obj_t *arg_thresholds;
    mp_obj_get_array(arg, &arg_thresholds_len, &arg_thresholds);
    if (!arg_thresholds_len) return;
    for(mp_uint_t i = 0; i < arg_thresholds_len; i++) {
        mp_uint_t arg_threshold_len;
        mp_obj_t *arg_threshold;
        mp_obj_get_array(arg_thresholds[i], &arg_threshold_len, &arg_threshold);
        if (arg_threshold_len) {
            color_thresholds_list_lnk_data_t lnk_data;
            lnk_data.LMin = (arg_threshold_len > 0) ? IM_MAX(IM_MIN(mp_obj_get_int(arg_threshold[0]),
                        IM_MAX(COLOR_L_MAX, COLOR_GRAYSCALE_MAX)), IM_MIN(COLOR_L_MIN, COLOR_GRAYSCALE_MIN)) :
                        IM_MIN(COLOR_L_MIN, COLOR_GRAYSCALE_MIN);
            lnk_data.LMax = (arg_threshold_len > 1) ? IM_MAX(IM_MIN(mp_obj_get_int(arg_threshold[1]),
                        IM_MAX(COLOR_L_MAX, COLOR_GRAYSCALE_MAX)), IM_MIN(COLOR_L_MIN, COLOR_GRAYSCALE_MIN)) :
                        IM_MAX(COLOR_L_MAX, COLOR_GRAYSCALE_MAX);
            lnk_data.AMin = (arg_threshold_len > 2) ? IM_MAX(IM_MIN(mp_obj_get_int(arg_threshold[2]), COLOR_A_MAX), COLOR_A_MIN) : COLOR_A_MIN;
            lnk_data.AMax = (arg_threshold_len > 3) ? IM_MAX(IM_MIN(mp_obj_get_int(arg_threshold[3]), COLOR_A_MAX), COLOR_A_MIN) : COLOR_A_MAX;
            lnk_data.BMin = (arg_threshold_len > 4) ? IM_MAX(IM_MIN(mp_obj_get_int(arg_threshold[4]), COLOR_B_MAX), COLOR_B_MIN) : COLOR_B_MIN;
            lnk_data.BMax = (arg_threshold_len > 5) ? IM_MAX(IM_MIN(mp_obj_get_int(arg_threshold[5]), COLOR_B_MAX), COLOR_B_MIN) : COLOR_B_MAX;
            color_thresholds_list_lnk_data_t lnk_data_tmp;
            memcpy(&lnk_data_tmp, &lnk_data, sizeof(color_thresholds_list_lnk_data_t));
            lnk_data.LMin = IM_MIN(lnk_data_tmp.LMin, lnk_data_tmp.LMax);
            lnk_data.LMax = IM_MAX(lnk_data_tmp.LMin, lnk_data_tmp.LMax);
            lnk_data.AMin = IM_MIN(lnk_data_tmp.AMin, lnk_data_tmp.AMax);
            lnk_data.AMax = IM_MAX(lnk_data_tmp.AMin, lnk_data_tmp.AMax);
            lnk_data.BMin = IM_MIN(lnk_data_tmp.BMin, lnk_data_tmp.BMax);
            lnk_data.BMax = IM_MAX(lnk_data_tmp.BMin, lnk_data_tmp.BMax);
            list_push_back(thresholds, &lnk_data);
        }
    }
}

void py_helper_keyword_thresholds(uint n_args, const mp_obj_t *args, uint arg_index,
                                  mp_map_t *kw_args, list_t *thresholds)
{
    mp_map_elem_t *kw_arg = mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_thresholds), MP_MAP_LOOKUP);

    if (kw_arg) {
        py_helper_arg_to_thresholds(kw_arg->value, thresholds);
    } else if (n_args > arg_index) {
        py_helper_arg_to_thresholds(args[arg_index], thresholds);
    }
}

int py_helper_arg_to_ksize(const mp_obj_t arg)
{
    int ksize = mp_obj_get_int(arg);
    PY_ASSERT_TRUE_MSG(ksize >= 0, "KernelSize must be >= 0!");
    return ksize;
}

int py_helper_ksize_to_n(int ksize)
{
    return ((ksize * 2) + 1) * ((ksize * 2) + 1);
}

mp_obj_t py_helper_keyword_object(uint n_args, const mp_obj_t *args,
        uint arg_index, mp_map_t *kw_args, mp_obj_t kw, mp_obj_t default_val)
{
    mp_map_elem_t *kw_arg = mp_map_lookup(kw_args, kw, MP_MAP_LOOKUP);

    if (kw_arg) {
        return kw_arg->value;
    } else if (n_args > arg_index) {
        return args[arg_index];
    } else {
        return default_val;
    }
}

const uint16_t *py_helper_keyword_color_palette(uint n_args, const mp_obj_t *args,
        uint arg_index, mp_map_t *kw_args, const uint16_t *default_color_palette)
{
    int palette;

    mp_map_elem_t *kw_arg =
        mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_color_palette), MP_MAP_LOOKUP);

    if (kw_arg && MP_OBJ_IS_TYPE(kw_arg->value, mp_const_none)) {
        default_color_palette = NULL;
    } else if ((n_args > arg_index) && MP_OBJ_IS_TYPE(args[arg_index], mp_const_none)) {
        default_color_palette = NULL;
    } else if (py_helper_keyword_int_maybe(n_args, args, arg_index, kw_args,
            MP_OBJ_NEW_QSTR(MP_QSTR_color_palette), &palette)) {
        if (palette == COLOR_PALETTE_RAINBOW) {
            default_color_palette = rainbow_table;
        } else if (palette == COLOR_PALETTE_IRONBOW) {
            default_color_palette = ironbow_table;
        } else {
            mp_raise_msg(&mp_type_ValueError,
                    MP_ERROR_TEXT("Invalid pre-defined color palette!"));
        }
    } else {
        image_t *arg_color_palette =
            py_helper_keyword_to_image_mutable_color_palette(n_args, args, arg_index, kw_args);

        if (arg_color_palette) {
            if (arg_color_palette->bpp != IMAGE_BPP_RGB565) {
                mp_raise_msg(&mp_type_ValueError,
                        MP_ERROR_TEXT("Color palette must be RGB565!"));
            }

            if ((arg_color_palette->w * arg_color_palette->h) != 256) {
                mp_raise_msg(&mp_type_ValueError,
                        MP_ERROR_TEXT("Color palette must be 256 pixels!"));
            }

            default_color_palette = (uint16_t *) arg_color_palette->data;
        }
    }

    return default_color_palette;
}

const uint8_t *py_helper_keyword_alpha_palette(uint n_args, const mp_obj_t *args,
        uint arg_index, mp_map_t *kw_args, const uint8_t *default_alpha_palette)
{
    image_t *arg_alpha_palette =
        py_helper_keyword_to_image_mutable_alpha_palette(n_args, args, 9, kw_args);

    if (arg_alpha_palette) {
        if (arg_alpha_palette->bpp != IMAGE_BPP_GRAYSCALE) {
            mp_raise_msg(&mp_type_ValueError,
                    MP_ERROR_TEXT("Alpha palette must be GRAYSCALE!"));
        }

        if ((arg_alpha_palette->w * arg_alpha_palette->h) != 256) {
            mp_raise_msg(&mp_type_ValueError,
                    MP_ERROR_TEXT("Alpha palette must be 256 pixels!"));
        }

        default_alpha_palette = (uint8_t *) arg_alpha_palette->data;
    }

    return default_alpha_palette;
}

bool py_helper_is_equal_to_framebuffer(image_t *img)
{
    return framebuffer_get_buffer(framebuffer->head)->data == img->data;
}

void py_helper_update_framebuffer(image_t *img)
{
    if (py_helper_is_equal_to_framebuffer(img)) {
        framebuffer_set(img->w, img->h, img->bpp);
    }
}

void py_helper_set_to_framebuffer(image_t *img)
{
    #if MICROPY_PY_SENSOR
    sensor_set_framebuffers(1);
    #else
    framebuffer_set_buffers(1);
    #endif

    PY_ASSERT_TRUE_MSG((image_size(img) <= framebuffer_get_buffer_size()),
            "The image doesn't fit in the frame buffer!");
    framebuffer_set(img->w, img->h, img->bpp);
    img->data = framebuffer_get_buffer(framebuffer->head)->data;
}
