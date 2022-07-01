/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Image Python module.
 */
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <arm_math.h>
#include "py/nlr.h"
#include "py/obj.h"
#include "py/objlist.h"
#include "py/objstr.h"
#include "py/objtuple.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "py/mphal.h"

#include "imlib.h"
#include "array.h"
#include "ff_wrapper.h"
#include "xalloc.h"
#include "fb_alloc.h"
#include "framebuffer.h"
#include "py_assert.h"
#include "py_helper.h"
#include "py_image.h"
#include "omv_boardconfig.h"
#if defined(IMLIB_ENABLE_IMAGE_IO)
#include "py_imageio.h"
#endif

static const mp_obj_type_t py_cascade_type;
static const mp_obj_type_t py_image_type;

#if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
extern const char *ffs_strerror(FRESULT res);
#endif

// Haar Cascade ///////////////////////////////////////////////////////////////

typedef struct _py_cascade_obj_t {
    mp_obj_base_t base;
    struct cascade _cobj;
} py_cascade_obj_t;

void *py_cascade_cobj(mp_obj_t cascade)
{
    PY_ASSERT_TYPE(cascade, &py_cascade_type);
    return &((py_cascade_obj_t *)cascade)->_cobj;
}

static void py_cascade_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_cascade_obj_t *self = self_in;
    mp_printf(print, "{\"width\":%d, \"height\":%d, \"n_stages\":%d, \"n_features\":%d, \"n_rectangles\":%d}",
            self->_cobj.window.w, self->_cobj.window.h, self->_cobj.n_stages,
            self->_cobj.n_features, self->_cobj.n_rectangles);
}

static const mp_obj_type_t py_cascade_type = {
    { &mp_type_type },
    .name  = MP_QSTR_Cascade,
    .print = py_cascade_print,
};

// Keypoints object ///////////////////////////////////////////////////////////

#ifdef IMLIB_ENABLE_FIND_KEYPOINTS

typedef struct _py_kp_obj_t {
    mp_obj_base_t base;
    array_t *kpts;
    int threshold;
    bool normalized;
} py_kp_obj_t;

static void py_kp_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_kp_obj_t *self = self_in;
    mp_printf(print, "{\"size\":%d, \"threshold\":%d, \"normalized\":%d}", array_length(self->kpts), self->threshold, self->normalized);
}

mp_obj_t py_kp_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    py_kp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_LEN:
            return MP_OBJ_NEW_SMALL_INT(array_length(self->kpts));

        default:
            return MP_OBJ_NULL; // op not supported
    }
}

static mp_obj_t py_kp_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    if (value == MP_OBJ_SENTINEL) { // load
        py_kp_obj_t *self = self_in;
        int size = array_length(self->kpts);
        int i = mp_get_index(self->base.type, size, index, false);
        kp_t *kp = array_at(self->kpts, i);
        return mp_obj_new_tuple(5, (mp_obj_t []) {mp_obj_new_int(kp->x),
                                                  mp_obj_new_int(kp->y),
                                                  mp_obj_new_int(kp->score),
                                                  mp_obj_new_int(kp->octave),
                                                  mp_obj_new_int(kp->angle)});
    }

    return MP_OBJ_NULL; // op not supported
}

static const mp_obj_type_t py_kp_type = {
    { &mp_type_type },
    .name  = MP_QSTR_kp_desc,
    .print = py_kp_print,
    .subscr = py_kp_subscr,
    .unary_op = py_kp_unary_op,
};

py_kp_obj_t *py_kpts_obj(mp_obj_t kpts_obj)
{
    PY_ASSERT_TYPE(kpts_obj, &py_kp_type);
    return kpts_obj;
}

#endif // IMLIB_ENABLE_FIND_KEYPOINTS

// LBP descriptor /////////////////////////////////////////////////////////////

#ifdef IMLIB_ENABLE_FIND_LBP

typedef struct _py_lbp_obj_t {
    mp_obj_base_t base;
    uint8_t *hist;
} py_lbp_obj_t;

static void py_lbp_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_printf(print, "{}");
}

static const mp_obj_type_t py_lbp_type = {
    { &mp_type_type },
    .name  = MP_QSTR_lbp_desc,
    .print = py_lbp_print,
};

#endif // IMLIB_ENABLE_FIND_LBP

// Keypoints Match Object /////////////////////////////////////////////////////

#if defined(IMLIB_ENABLE_DESCRIPTOR) && defined(IMLIB_ENABLE_FIND_KEYPOINTS)

#define kptmatch_obj_size 9
typedef struct _py_kptmatch_obj_t {
    mp_obj_base_t base;
    mp_obj_t cx, cy;
    mp_obj_t x, y, w, h;
    mp_obj_t count;
    mp_obj_t theta;
    mp_obj_t match;
} py_kptmatch_obj_t;

static void py_kptmatch_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_kptmatch_obj_t *self = self_in;
    mp_printf(print, "{\"cx\":%d, \"cy\":%d, \"x\":%d, \"y\":%d, \"w\":%d, \"h\":%d, \"count\":%d, \"theta\":%d}",
              mp_obj_get_int(self->cx), mp_obj_get_int(self->cy), mp_obj_get_int(self->x), mp_obj_get_int(self->y),
              mp_obj_get_int(self->w),  mp_obj_get_int(self->h),  mp_obj_get_int(self->count), mp_obj_get_int(self->theta));
}

static mp_obj_t py_kptmatch_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    if (value == MP_OBJ_SENTINEL) { // load
        py_kptmatch_obj_t *self = self_in;
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(kptmatch_obj_size, index, &slice)) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
            }
            mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
            mp_seq_copy(result->items, &(self->x) + slice.start, result->len, mp_obj_t);
            return result;
        }
        switch (mp_get_index(self->base.type, kptmatch_obj_size, index, false)) {
            case 0: return self->cx;
            case 1: return self->cy;
            case 2: return self->x;
            case 3: return self->y;
            case 4: return self->w;
            case 5: return self->h;
            case 6: return self->count;
            case 7: return self->theta;
            case 8: return self->match;
        }
    }
    return MP_OBJ_NULL; // op not supported
}

mp_obj_t py_kptmatch_cx(mp_obj_t self_in)     { return ((py_kptmatch_obj_t *) self_in)->cx;    }
mp_obj_t py_kptmatch_cy(mp_obj_t self_in)     { return ((py_kptmatch_obj_t *) self_in)->cy;    }
mp_obj_t py_kptmatch_x (mp_obj_t self_in)     { return ((py_kptmatch_obj_t *) self_in)->x;     }
mp_obj_t py_kptmatch_y (mp_obj_t self_in)     { return ((py_kptmatch_obj_t *) self_in)->y;     }
mp_obj_t py_kptmatch_w (mp_obj_t self_in)     { return ((py_kptmatch_obj_t *) self_in)->w;     }
mp_obj_t py_kptmatch_h (mp_obj_t self_in)     { return ((py_kptmatch_obj_t *) self_in)->h;     }
mp_obj_t py_kptmatch_count(mp_obj_t self_in)  { return ((py_kptmatch_obj_t *) self_in)->count; }
mp_obj_t py_kptmatch_theta(mp_obj_t self_in)  { return ((py_kptmatch_obj_t *) self_in)->theta; }
mp_obj_t py_kptmatch_match(mp_obj_t self_in)  { return ((py_kptmatch_obj_t *) self_in)->match; }
mp_obj_t py_kptmatch_rect(mp_obj_t  self_in)  {
    return mp_obj_new_tuple(4, (mp_obj_t []) {((py_kptmatch_obj_t *) self_in)->x,
                                              ((py_kptmatch_obj_t *) self_in)->y,
                                              ((py_kptmatch_obj_t *) self_in)->w,
                                              ((py_kptmatch_obj_t *) self_in)->h});
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_kptmatch_cx_obj,    py_kptmatch_cx);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_kptmatch_cy_obj,    py_kptmatch_cy);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_kptmatch_x_obj,     py_kptmatch_x);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_kptmatch_y_obj,     py_kptmatch_y);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_kptmatch_w_obj,     py_kptmatch_w);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_kptmatch_h_obj,     py_kptmatch_h);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_kptmatch_count_obj, py_kptmatch_count);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_kptmatch_theta_obj, py_kptmatch_theta);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_kptmatch_match_obj, py_kptmatch_match);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_kptmatch_rect_obj,  py_kptmatch_rect);

STATIC const mp_rom_map_elem_t py_kptmatch_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_cx),      MP_ROM_PTR(&py_kptmatch_cx_obj)      },
    { MP_ROM_QSTR(MP_QSTR_cy),      MP_ROM_PTR(&py_kptmatch_cy_obj)      },
    { MP_ROM_QSTR(MP_QSTR_x),       MP_ROM_PTR(&py_kptmatch_x_obj)       },
    { MP_ROM_QSTR(MP_QSTR_y),       MP_ROM_PTR(&py_kptmatch_y_obj)       },
    { MP_ROM_QSTR(MP_QSTR_w),       MP_ROM_PTR(&py_kptmatch_w_obj)       },
    { MP_ROM_QSTR(MP_QSTR_h),       MP_ROM_PTR(&py_kptmatch_h_obj)       },
    { MP_ROM_QSTR(MP_QSTR_count),   MP_ROM_PTR(&py_kptmatch_count_obj)   },
    { MP_ROM_QSTR(MP_QSTR_theta),   MP_ROM_PTR(&py_kptmatch_theta_obj)   },
    { MP_ROM_QSTR(MP_QSTR_match),   MP_ROM_PTR(&py_kptmatch_match_obj)   },
    { MP_ROM_QSTR(MP_QSTR_rect),    MP_ROM_PTR(&py_kptmatch_rect_obj)    }
};

STATIC MP_DEFINE_CONST_DICT(py_kptmatch_locals_dict, py_kptmatch_locals_dict_table);

static const mp_obj_type_t py_kptmatch_type = {
    { &mp_type_type },
    .name   = MP_QSTR_kptmatch,
    .print  = py_kptmatch_print,
    .subscr = py_kptmatch_subscr,
    .locals_dict = (mp_obj_t) &py_kptmatch_locals_dict
};


#endif //IMLIB_ENABLE_DESCRIPTOR && IMLIB_ENABLE_FIND_KEYPOINTS

// Image //////////////////////////////////////////////////////////////////////

typedef struct _py_image_obj_t {
    mp_obj_base_t base;
    image_t _cobj;
} py_image_obj_t;

typedef struct _mp_obj_py_image_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    mp_obj_t py_image;
    size_t cur;
} mp_obj_py_image_it_t;

void *py_image_cobj(mp_obj_t img_obj)
{
    PY_ASSERT_TYPE(img_obj, &py_image_type);
    return &((py_image_obj_t *)img_obj)->_cobj;
}

mp_obj_t py_image_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    py_image_obj_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_LEN: {
            image_t *img = &self->_cobj;
            if (img->bpp >= IMAGE_BPP_JPEG) {
                // For JPEG images we create a 1D array.
                return mp_obj_new_int(img->bpp); //size in bytes is stored in bpp.
            } else {
                // For other formats, 2D array is created.
                return mp_obj_new_int(img->h);
            }
        }
        default:
            return MP_OBJ_NULL; // op not supported
    }
}

// image iterator
STATIC mp_obj_t py_image_it_iternext(mp_obj_t self_in)
{
    mp_obj_py_image_it_t *self = MP_OBJ_TO_PTR(self_in);
    py_image_obj_t *image = MP_OBJ_TO_PTR(self->py_image);
    image_t *img = &image->_cobj;
    switch (img->bpp) {
        case IMAGE_BPP_BINARY: {
            if (self->cur >= img->h) {
                return MP_OBJ_STOP_ITERATION;
            } else {
                mp_obj_t row = mp_obj_new_list(0, NULL);
                for (int i=0; i<img->w; i++) {
                    mp_obj_list_append(row, mp_obj_new_int(IMAGE_GET_BINARY_PIXEL(img, i, self->cur)));
                }
                self->cur++;
                return row;
            }
        }
        case IMAGE_BPP_BAYER:
        case IMAGE_BPP_GRAYSCALE: {
            if (self->cur >= img->h) {
                return MP_OBJ_STOP_ITERATION;
            } else {
                mp_obj_t row = mp_obj_new_list(0, NULL);
                for (int i=0; i<img->w; i++) {
                    mp_obj_list_append(row, mp_obj_new_int(IMAGE_GET_GRAYSCALE_PIXEL(img, i, self->cur)));
                }
                self->cur++;
                return row;
            }
        }
        case IMAGE_BPP_RGB565: {
            if (self->cur >= img->h) {
                return MP_OBJ_STOP_ITERATION;
            } else {
                mp_obj_t row = mp_obj_new_list(0, NULL);
                for (int i=0; i<img->w; i++) {
                    mp_obj_list_append(row, mp_obj_new_int(IMAGE_GET_RGB565_PIXEL(img, i, self->cur)));
                }
                self->cur++;
                return row;
            }
        }
        default: {// JPEG
            if (self->cur >= img->bpp) {
                return MP_OBJ_STOP_ITERATION;
            } else {
                return mp_obj_new_int(img->pixels[self->cur++]);
            }
        }
    }
}

STATIC mp_obj_t py_image_getiter(mp_obj_t o_in, mp_obj_iter_buf_t *iter_buf)
{
    assert(sizeof(mp_obj_py_image_it_t) <= sizeof(mp_obj_iter_buf_t));
    mp_obj_py_image_it_t *o = (mp_obj_py_image_it_t*)iter_buf;
    o->base.type = &mp_type_polymorph_iter;
    o->iternext = py_image_it_iternext;
    o->py_image = o_in;
    o->cur = 0;
    return MP_OBJ_FROM_PTR(o);
}

static void py_image_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_image_obj_t *self = self_in;
    switch(self->_cobj.bpp) {
        case IMAGE_BPP_BINARY: {
            mp_printf(print, "{\"w\":%d, \"h\":%d, \"type\"=\"binary\", \"size\":%d}",
                      self->_cobj.w, self->_cobj.h,
                      ((self->_cobj.w + UINT32_T_MASK) >> UINT32_T_SHIFT) * self->_cobj.h);
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            mp_printf(print, "{\"w\":%d, \"h\":%d, \"type\"=\"grayscale\", \"size\":%d}",
                      self->_cobj.w, self->_cobj.h,
                      (self->_cobj.w * self->_cobj.h) * sizeof(uint8_t));
            break;
        }
        case IMAGE_BPP_RGB565: {
            mp_printf(print, "{\"w\":%d, \"h\":%d, \"type\"=\"rgb565\", \"size\":%d}",
                      self->_cobj.w, self->_cobj.h,
                      (self->_cobj.w * self->_cobj.h) * sizeof(uint16_t));
            break;
        }
        case IMAGE_BPP_BAYER: {
            mp_printf(print, "{\"w\":%d, \"h\":%d, \"type\"=\"bayer\", \"size\":%d}",
                      self->_cobj.w, self->_cobj.h,
                      (self->_cobj.w * self->_cobj.h) * sizeof(uint8_t));
            break;
        }
        default: {
            if((self->_cobj.data[0] == 0xFE) && (self->_cobj.data[self->_cobj.bpp-1] == 0xFE)) { // for ide
                print->print_strn(print->data, (const char *) self->_cobj.data, self->_cobj.bpp);
            } else { // not for ide
                mp_printf(print, "{\"w\":%d, \"h\":%d, \"type\"=\"jpeg\", \"size\":%d}",
                          self->_cobj.w, self->_cobj.h,
                          self->_cobj.bpp);
            }
            break;
        }
    }
}

static mp_obj_t py_image_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    py_image_obj_t *self = self_in;
    if (value == MP_OBJ_NULL) { // delete
    } else if (value == MP_OBJ_SENTINEL) { // load
        switch (self->_cobj.bpp) {
            case IMAGE_BPP_BINARY: {
                if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
                    mp_bound_slice_t slice;
                    if (!mp_seq_get_fast_slice_indexes(self->_cobj.w * self->_cobj.h, index, &slice)) {
                        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
                    }
                    mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
                    for (mp_uint_t i = 0; i < result->len; i++) {
                        result->items[i] = mp_obj_new_int(IMAGE_GET_BINARY_PIXEL(&(self->_cobj), (slice.start + i) % self->_cobj.w, (slice.start + i) / self->_cobj.w));
                    }
                    return result;
                }
                mp_uint_t i = mp_get_index(self->base.type, self->_cobj.w * self->_cobj.h, index, false);
                return mp_obj_new_int(IMAGE_GET_BINARY_PIXEL(&(self->_cobj), i % self->_cobj.w, i / self->_cobj.w));
            }
            case IMAGE_BPP_BAYER:
            case IMAGE_BPP_GRAYSCALE: {
                if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
                    mp_bound_slice_t slice;
                    if (!mp_seq_get_fast_slice_indexes(self->_cobj.w * self->_cobj.h, index, &slice)) {
                        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
                    }
                    mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
                    for (mp_uint_t i = 0; i < result->len; i++) {
                        uint8_t p = IMAGE_GET_GRAYSCALE_PIXEL(&(self->_cobj), (slice.start + i) % self->_cobj.w, (slice.start + i) / self->_cobj.w);
                        result->items[i] = mp_obj_new_int(p);
                    }
                    return result;
                }
                mp_uint_t i = mp_get_index(self->base.type, self->_cobj.w * self->_cobj.h, index, false);
                uint8_t p = IMAGE_GET_GRAYSCALE_PIXEL(&(self->_cobj), i % self->_cobj.w, i / self->_cobj.w);
                return mp_obj_new_int(p);
            }
            case IMAGE_BPP_RGB565: {
                if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
                    mp_bound_slice_t slice;
                    if (!mp_seq_get_fast_slice_indexes(self->_cobj.w * self->_cobj.h, index, &slice)) {
                        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
                    }
                    mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
                    for (mp_uint_t i = 0; i < result->len; i++) {
                        uint16_t p = IMAGE_GET_RGB565_PIXEL(&(self->_cobj), (slice.start + i) % self->_cobj.w, (slice.start + i) / self->_cobj.w);
                        result->items[i] = mp_obj_new_tuple(3, (mp_obj_t []) {mp_obj_new_int(COLOR_RGB565_TO_R8(p)),
                                                                              mp_obj_new_int(COLOR_RGB565_TO_G8(p)),
                                                                              mp_obj_new_int(COLOR_RGB565_TO_B8(p))});
                    }
                    return result;
                }
                mp_uint_t i = mp_get_index(self->base.type, self->_cobj.w * self->_cobj.h, index, false);
                uint16_t p = IMAGE_GET_RGB565_PIXEL(&(self->_cobj), i % self->_cobj.w, i / self->_cobj.w);
                return mp_obj_new_tuple(3, (mp_obj_t []) {mp_obj_new_int(COLOR_RGB565_TO_R8(p)),
                                                          mp_obj_new_int(COLOR_RGB565_TO_G8(p)),
                                                          mp_obj_new_int(COLOR_RGB565_TO_B8(p))});
            }
            default: {
                if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
                    mp_bound_slice_t slice;
                    if (!mp_seq_get_fast_slice_indexes(self->_cobj.bpp, index, &slice)) {
                        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
                    }
                    mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
                    for (mp_uint_t i = 0; i < result->len; i++) {
                        result->items[i] = mp_obj_new_int(self->_cobj.data[slice.start + i]);
                    }
                    return result;
                }
                mp_uint_t i = mp_get_index(self->base.type, self->_cobj.bpp, index, false);
                return mp_obj_new_int(self->_cobj.data[i]);
            }
        }
    } else { // store
        switch (self->_cobj.bpp) {
            case IMAGE_BPP_BINARY: {
                if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
                    mp_bound_slice_t slice;
                    if (!mp_seq_get_fast_slice_indexes(self->_cobj.w * self->_cobj.h, index, &slice)) {
                        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
                    }
                    if (MP_OBJ_IS_TYPE(value, &mp_type_list)) {
                        mp_uint_t value_l_len;
                        mp_obj_t *value_l;
                        mp_obj_get_array(value, &value_l_len, &value_l);
                        PY_ASSERT_TRUE_MSG(value_l_len == (slice.stop - slice.start), "cannot grow or shrink image");
                        for (mp_uint_t i = 0; i < (slice.stop - slice.start); i++) {
                            IMAGE_PUT_BINARY_PIXEL(&(self->_cobj), (slice.start + i) % self->_cobj.w, (slice.start + i) / self->_cobj.w, mp_obj_get_int(value_l[i]));
                        }
                    } else {
                        mp_int_t v = mp_obj_get_int(value);
                        for (mp_uint_t i = 0; i < (slice.stop - slice.start); i++) {
                            IMAGE_PUT_BINARY_PIXEL(&(self->_cobj), (slice.start + i) % self->_cobj.w, (slice.start + i) / self->_cobj.w, v);
                        }
                    }
                    return mp_const_none;
                }
                mp_uint_t i = mp_get_index(self->base.type, self->_cobj.w * self->_cobj.h, index, false);
                IMAGE_PUT_BINARY_PIXEL(&(self->_cobj), i % self->_cobj.w, i / self->_cobj.w, mp_obj_get_int(value));
                return mp_const_none;
            }
            case IMAGE_BPP_BAYER:
            case IMAGE_BPP_GRAYSCALE: {
                if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
                    mp_bound_slice_t slice;
                    if (!mp_seq_get_fast_slice_indexes(self->_cobj.w * self->_cobj.h, index, &slice)) {
                        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
                    }
                    if (MP_OBJ_IS_TYPE(value, &mp_type_list)) {
                        mp_uint_t value_l_len;
                        mp_obj_t *value_l;
                        mp_obj_get_array(value, &value_l_len, &value_l);
                        PY_ASSERT_TRUE_MSG(value_l_len == (slice.stop - slice.start), "cannot grow or shrink image");
                        for (mp_uint_t i = 0; i < (slice.stop - slice.start); i++) {
                            uint8_t p = mp_obj_get_int(value_l[i]);
                            IMAGE_PUT_GRAYSCALE_PIXEL(&(self->_cobj), (slice.start + i) % self->_cobj.w, (slice.start + i) / self->_cobj.w, p);
                        }
                    } else {
                        uint8_t p = mp_obj_get_int(value);
                        for (mp_uint_t i = 0; i < (slice.stop - slice.start); i++) {
                            IMAGE_PUT_GRAYSCALE_PIXEL(&(self->_cobj), (slice.start + i) % self->_cobj.w, (slice.start + i) / self->_cobj.w, p);
                        }
                    }
                    return mp_const_none;
                }
                mp_uint_t i = mp_get_index(self->base.type, self->_cobj.w * self->_cobj.h, index, false);
                uint8_t p = mp_obj_get_int(value);
                IMAGE_PUT_GRAYSCALE_PIXEL(&(self->_cobj), i % self->_cobj.w, i / self->_cobj.w, p);
                return mp_const_none;
            }
            case IMAGE_BPP_RGB565: {
                if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
                    mp_bound_slice_t slice;
                    if (!mp_seq_get_fast_slice_indexes(self->_cobj.w * self->_cobj.h, index, &slice)) {
                        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
                    }
                    if (MP_OBJ_IS_TYPE(value, &mp_type_list)) {
                        mp_uint_t value_l_len;
                        mp_obj_t *value_l;
                        mp_obj_get_array(value, &value_l_len, &value_l);
                        PY_ASSERT_TRUE_MSG(value_l_len == (slice.stop - slice.start), "cannot grow or shrink image");
                        for (mp_uint_t i = 0; i < (slice.stop - slice.start); i++) {
                            mp_obj_t *value_2;
                            mp_obj_get_array_fixed_n(value_l[i], 3, &value_2);
                            uint16_t p = COLOR_R8_G8_B8_TO_RGB565(mp_obj_get_int(value_2[0]), mp_obj_get_int(value_2[1]), mp_obj_get_int(value_2[2]));
                            IMAGE_PUT_RGB565_PIXEL(&(self->_cobj), (slice.start + i) % self->_cobj.w, (slice.start + i) / self->_cobj.w, p);
                        }
                    } else {
                        mp_obj_t *value_2;
                        mp_obj_get_array_fixed_n(value, 3, &value_2);
                        uint16_t p = COLOR_R8_G8_B8_TO_RGB565(mp_obj_get_int(value_2[0]), mp_obj_get_int(value_2[1]), mp_obj_get_int(value_2[2]));
                        for (mp_uint_t i = 0; i < (slice.stop - slice.start); i++) {
                            IMAGE_PUT_RGB565_PIXEL(&(self->_cobj), (slice.start + i) % self->_cobj.w, (slice.start + i) / self->_cobj.w, p);
                        }
                    }
                    return mp_const_none;
                }
                mp_uint_t i = mp_get_index(self->base.type, self->_cobj.w * self->_cobj.h, index, false);
                mp_obj_t *value_2;
                mp_obj_get_array_fixed_n(value, 3, &value_2);
                uint16_t p = COLOR_R8_G8_B8_TO_RGB565(mp_obj_get_int(value_2[0]), mp_obj_get_int(value_2[1]), mp_obj_get_int(value_2[2]));
                IMAGE_PUT_RGB565_PIXEL(&(self->_cobj), i % self->_cobj.w, i / self->_cobj.w, p);
                return mp_const_none;
            }
            default: {
                if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
                    mp_bound_slice_t slice;
                    if (!mp_seq_get_fast_slice_indexes(self->_cobj.bpp, index, &slice)) {
                        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
                    }
                    if (MP_OBJ_IS_TYPE(value, &mp_type_list)) {
                        mp_uint_t value_l_len;
                        mp_obj_t *value_l;
                        mp_obj_get_array(value, &value_l_len, &value_l);
                        PY_ASSERT_TRUE_MSG(value_l_len == (slice.stop - slice.start), "cannot grow or shrink image");
                        for (mp_uint_t i = 0; i < (slice.stop - slice.start); i++) {
                            self->_cobj.data[slice.start + i] = mp_obj_get_int(value_l[i]);
                        }
                    } else {
                        mp_int_t v = mp_obj_get_int(value);
                        for (mp_uint_t i = 0; i < (slice.stop - slice.start); i++) {
                            self->_cobj.data[slice.start + i] = v;
                        }
                    }
                    return mp_const_none;
                }
                mp_uint_t i = mp_get_index(self->base.type, self->_cobj.bpp, index, false);
                self->_cobj.data[i] = mp_obj_get_int(value);
                return mp_const_none;
            }
        }
    }
    return MP_OBJ_NULL; // op not supported
}

static mp_int_t py_image_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags)
{
    py_image_obj_t *self = self_in;
    if (flags == MP_BUFFER_READ) {
        bufinfo->buf = self->_cobj.data;
        bufinfo->len = image_size(&self->_cobj);
        bufinfo->typecode = 'b';
        return 0;
    } else { // Can't write to an image!
        bufinfo->buf = NULL;
        bufinfo->len = 0;
        bufinfo->typecode = -1;
        return 1;
    }
}

////////////////
// Basic Methods
////////////////

static mp_obj_t py_image_width(mp_obj_t img_obj)
{
    return mp_obj_new_int(((image_t *) py_image_cobj(img_obj))->w);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_width_obj, py_image_width);

static mp_obj_t py_image_height(mp_obj_t img_obj)
{
    return mp_obj_new_int(((image_t *) py_image_cobj(img_obj))->h);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_height_obj, py_image_height);

static mp_obj_t py_image_format(mp_obj_t img_obj)
{
    switch (((image_t *) py_image_cobj(img_obj))->bpp) {
        case IMAGE_BPP_BINARY: return mp_obj_new_int(PIXFORMAT_BINARY);
        case IMAGE_BPP_GRAYSCALE: return mp_obj_new_int(PIXFORMAT_GRAYSCALE);
        case IMAGE_BPP_RGB565: return mp_obj_new_int(PIXFORMAT_RGB565);
        case IMAGE_BPP_BAYER: return mp_obj_new_int(PIXFORMAT_BAYER);
        default: return mp_obj_new_int(PIXFORMAT_JPEG);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_format_obj, py_image_format);

static mp_obj_t py_image_size(mp_obj_t img_obj)
{
    return mp_obj_new_int(image_size((image_t *) py_image_cobj(img_obj)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_size_obj, py_image_size);

static mp_obj_t py_image_bytearray(mp_obj_t img_obj)
{
    image_t *arg_img = (image_t *) py_image_cobj(img_obj);
    return mp_obj_new_bytearray_by_ref(image_size(arg_img), arg_img->data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_bytearray_obj, py_image_bytearray);

STATIC mp_obj_t py_image_get_pixel(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable_bayer(args[0]);

    const mp_obj_t *arg_vec;
    uint offset = py_helper_consume_array(n_args, args, 1, 2, &arg_vec);
    int arg_x = mp_obj_get_int(arg_vec[0]);
    int arg_y = mp_obj_get_int(arg_vec[1]);

    bool arg_rgbtuple =
        py_helper_keyword_int(n_args, args, offset, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_rgbtuple), arg_img->bpp == IMAGE_BPP_RGB565);

    if ((!IM_X_INSIDE(arg_img, arg_x)) || (!IM_Y_INSIDE(arg_img, arg_y))) {
        return mp_const_none;
    }

    switch (arg_img->bpp) {
        case IMAGE_BPP_BINARY: {
            if (arg_rgbtuple) {
                int pixel = IMAGE_GET_BINARY_PIXEL(arg_img, arg_x, arg_y);
                mp_obj_t pixel_tuple[3];
                pixel_tuple[0] = mp_obj_new_int(COLOR_RGB565_TO_R8(COLOR_BINARY_TO_RGB565(pixel)));
                pixel_tuple[1] = mp_obj_new_int(COLOR_RGB565_TO_G8(COLOR_BINARY_TO_RGB565(pixel)));
                pixel_tuple[2] = mp_obj_new_int(COLOR_RGB565_TO_B8(COLOR_BINARY_TO_RGB565(pixel)));
                return mp_obj_new_tuple(3, pixel_tuple);
            } else {
                return mp_obj_new_int(IMAGE_GET_BINARY_PIXEL(arg_img, arg_x, arg_y));
            }
        }
        case IMAGE_BPP_GRAYSCALE: {
            if (arg_rgbtuple) {
                int pixel = IMAGE_GET_GRAYSCALE_PIXEL(arg_img, arg_x, arg_y);
                mp_obj_t pixel_tuple[3];
                pixel_tuple[0] = mp_obj_new_int(COLOR_RGB565_TO_R8(COLOR_GRAYSCALE_TO_RGB565(pixel)));
                pixel_tuple[1] = mp_obj_new_int(COLOR_RGB565_TO_G8(COLOR_GRAYSCALE_TO_RGB565(pixel)));
                pixel_tuple[2] = mp_obj_new_int(COLOR_RGB565_TO_B8(COLOR_GRAYSCALE_TO_RGB565(pixel)));
                return mp_obj_new_tuple(3, pixel_tuple);
            } else {
                return mp_obj_new_int(IMAGE_GET_GRAYSCALE_PIXEL(arg_img, arg_x, arg_y));
            }
        }
        case IMAGE_BPP_RGB565: {
            if (arg_rgbtuple) {
                int pixel = IMAGE_GET_RGB565_PIXEL(arg_img, arg_x, arg_y);
                mp_obj_t pixel_tuple[3];
                pixel_tuple[0] = mp_obj_new_int(COLOR_RGB565_TO_R8(pixel));
                pixel_tuple[1] = mp_obj_new_int(COLOR_RGB565_TO_G8(pixel));
                pixel_tuple[2] = mp_obj_new_int(COLOR_RGB565_TO_B8(pixel));
                return mp_obj_new_tuple(3, pixel_tuple);
            } else {
                return mp_obj_new_int(IMAGE_GET_RGB565_PIXEL(arg_img, arg_x, arg_y));
            }
        }
        case IMAGE_BPP_BAYER:
            if (arg_rgbtuple) {
                uint16_t pixel; imlib_debayer_line_to_rgb565(arg_x, arg_x + 1, arg_y, &pixel, arg_img);
                mp_obj_t pixel_tuple[3];
                pixel_tuple[0] = mp_obj_new_int(COLOR_RGB565_TO_R8(pixel));
                pixel_tuple[1] = mp_obj_new_int(COLOR_RGB565_TO_G8(pixel));
                pixel_tuple[2] = mp_obj_new_int(COLOR_RGB565_TO_B8(pixel));
                return mp_obj_new_tuple(3, pixel_tuple);
            } else {
                return mp_obj_new_int(IMAGE_GET_GRAYSCALE_PIXEL(arg_img, arg_x, arg_y)); // Correct!
            }
        default: return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_get_pixel_obj, 2, py_image_get_pixel);

STATIC mp_obj_t py_image_set_pixel(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable_bayer(args[0]);

    const mp_obj_t *arg_vec;
    uint offset = py_helper_consume_array(n_args, args, 1, 2, &arg_vec);
    int arg_x = mp_obj_get_int(arg_vec[0]);
    int arg_y = mp_obj_get_int(arg_vec[1]);

    int arg_c =
        py_helper_keyword_color(arg_img, n_args, args, offset, kw_args, -1); // White.

    if ((!IM_X_INSIDE(arg_img, arg_x)) || (!IM_Y_INSIDE(arg_img, arg_y))) {
        return args[0];
    }

    switch (arg_img->bpp) {
        case IMAGE_BPP_BINARY: {
            IMAGE_PUT_BINARY_PIXEL(arg_img, arg_x, arg_y, arg_c);
            return args[0];
        }
        case IMAGE_BPP_GRAYSCALE: {
            IMAGE_PUT_GRAYSCALE_PIXEL(arg_img, arg_x, arg_y, arg_c);
            return args[0];
        }
        case IMAGE_BPP_RGB565: {
            IMAGE_PUT_RGB565_PIXEL(arg_img, arg_x, arg_y, arg_c);
            return args[0];
        }
        case IMAGE_BPP_BAYER: {
            IMAGE_PUT_GRAYSCALE_PIXEL(arg_img, arg_x, arg_y, arg_c); // Correct!
            return args[0];
        }
        default: return args[0];
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_set_pixel_obj, 2, py_image_set_pixel);

#ifdef IMLIB_ENABLE_MEAN_POOLING
static mp_obj_t py_image_mean_pool(mp_obj_t img_obj, mp_obj_t x_div_obj, mp_obj_t y_div_obj)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(img_obj);

    int arg_x_div = mp_obj_get_int(x_div_obj);
    PY_ASSERT_TRUE_MSG(arg_x_div >= 1, "Width divisor must be greater than >= 1");
    PY_ASSERT_TRUE_MSG(arg_x_div <= arg_img->w, "Width divisor must be less than <= img width");
    int arg_y_div = mp_obj_get_int(y_div_obj);
    PY_ASSERT_TRUE_MSG(arg_y_div >= 1, "Height divisor must be greater than >= 1");
    PY_ASSERT_TRUE_MSG(arg_y_div <= arg_img->h, "Height divisor must be less than <= img height");

    image_t out_img;
    out_img.w = arg_img->w / arg_x_div;
    out_img.h = arg_img->h / arg_y_div;
    out_img.bpp = arg_img->bpp;
    out_img.pixels = arg_img->pixels;
    PY_ASSERT_TRUE_MSG(image_size(&out_img) <= image_size(arg_img), "Can't pool in place!");

    imlib_mean_pool(arg_img, &out_img, arg_x_div, arg_y_div);
    arg_img->w = out_img.w;
    arg_img->h = out_img.h;
    py_helper_update_framebuffer(arg_img);
    return img_obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(py_image_mean_pool_obj, py_image_mean_pool);

static mp_obj_t py_image_mean_pooled(mp_obj_t img_obj, mp_obj_t x_div_obj, mp_obj_t y_div_obj)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(img_obj);

    int arg_x_div = mp_obj_get_int(x_div_obj);
    PY_ASSERT_TRUE_MSG(arg_x_div >= 1, "Width divisor must be greater than >= 1");
    PY_ASSERT_TRUE_MSG(arg_x_div <= arg_img->w, "Width divisor must be less than <= img width");
    int arg_y_div = mp_obj_get_int(y_div_obj);
    PY_ASSERT_TRUE_MSG(arg_y_div >= 1, "Height divisor must be greater than >= 1");
    PY_ASSERT_TRUE_MSG(arg_y_div <= arg_img->h, "Height divisor must be less than <= img height");

    image_t out_img;
    out_img.w = arg_img->w / arg_x_div;
    out_img.h = arg_img->h / arg_y_div;
    out_img.bpp = arg_img->bpp;
    out_img.pixels = xalloc(image_size(&out_img));

    imlib_mean_pool(arg_img, &out_img, arg_x_div, arg_y_div);
    return py_image_from_struct(&out_img);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(py_image_mean_pooled_obj, py_image_mean_pooled);
#endif // IMLIB_ENABLE_MEAN_POOLING

#ifdef IMLIB_ENABLE_MIDPOINT_POOLING
static mp_obj_t py_image_midpoint_pool(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    int arg_x_div = mp_obj_get_int(args[1]);
    PY_ASSERT_TRUE_MSG(arg_x_div >= 1, "Width divisor must be greater than >= 1");
    PY_ASSERT_TRUE_MSG(arg_x_div <= arg_img->w, "Width divisor must be less than <= img width");
    int arg_y_div = mp_obj_get_int(args[2]);
    PY_ASSERT_TRUE_MSG(arg_y_div >= 1, "Height divisor must be greater than >= 1");
    PY_ASSERT_TRUE_MSG(arg_y_div <= arg_img->h, "Height divisor must be less than <= img height");

    int arg_bias = py_helper_keyword_float(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_bias), 0.5) * 256;
    PY_ASSERT_TRUE_MSG((0 <= arg_bias) && (arg_bias <= 256), "Error: 0 <= bias <= 1!");

    image_t out_img;
    out_img.w = arg_img->w / arg_x_div;
    out_img.h = arg_img->h / arg_y_div;
    out_img.bpp = arg_img->bpp;
    out_img.pixels = arg_img->pixels;
    PY_ASSERT_TRUE_MSG(image_size(&out_img) <= image_size(arg_img), "Can't pool in place!");

    imlib_midpoint_pool(arg_img, &out_img, arg_x_div, arg_y_div, arg_bias);
    arg_img->w = out_img.w;
    arg_img->h = out_img.h;
    py_helper_update_framebuffer(arg_img);
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_midpoint_pool_obj, 3, py_image_midpoint_pool);

static mp_obj_t py_image_midpoint_pooled(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    int arg_x_div = mp_obj_get_int(args[1]);
    PY_ASSERT_TRUE_MSG(arg_x_div >= 1, "Width divisor must be greater than >= 1");
    PY_ASSERT_TRUE_MSG(arg_x_div <= arg_img->w, "Width divisor must be less than <= img width");
    int arg_y_div = mp_obj_get_int(args[2]);
    PY_ASSERT_TRUE_MSG(arg_y_div >= 1, "Height divisor must be greater than >= 1");
    PY_ASSERT_TRUE_MSG(arg_y_div <= arg_img->h, "Height divisor must be less than <= img height");

    int arg_bias = py_helper_keyword_float(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_bias), 0.5) * 256;
    PY_ASSERT_TRUE_MSG((0 <= arg_bias) && (arg_bias <= 256), "Error: 0 <= bias <= 1!");

    image_t out_img;
    out_img.w = arg_img->w / arg_x_div;
    out_img.h = arg_img->h / arg_y_div;
    out_img.bpp = arg_img->bpp;
    out_img.pixels = xalloc(image_size(&out_img));

    imlib_midpoint_pool(arg_img, &out_img, arg_x_div, arg_y_div, arg_bias);
    return py_image_from_struct(&out_img);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_midpoint_pooled_obj, 3, py_image_midpoint_pooled);
#endif // IMLIB_ENABLE_MIDPOINT_POOLING

static mp_obj_t py_image_to(int bpp, const uint16_t *default_color_palette, bool copy_to_fb,
                            uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *src_img = py_helper_arg_to_image_mutable_bayer_jpeg(args[0]);

    float arg_x_scale = 1.f;
    bool got_x_scale = py_helper_keyword_float_maybe(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_scale), &arg_x_scale);

    float arg_y_scale = 1.f;
    bool got_y_scale = py_helper_keyword_float_maybe(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_scale), &arg_y_scale);

    rectangle_t arg_roi;
    py_helper_keyword_rectangle_roi(src_img, n_args, args, 3, kw_args, &arg_roi);

    int arg_rgb_channel = py_helper_keyword_int(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_rgb_channel), -1);
    if ((arg_rgb_channel < -1) || (2 < arg_rgb_channel)) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("-1 <= rgb_channel <= 2!"));
    }

    int arg_alpha = py_helper_keyword_int(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_alpha), 256);
    if ((arg_alpha < 0) || (256 < arg_alpha)) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("0 <= alpha <= 256!"));
    }

    const uint16_t *color_palette = py_helper_keyword_color_palette(n_args, args, 6, kw_args, default_color_palette);
    const uint8_t *alpha_palette = py_helper_keyword_alpha_palette(n_args, args, 7, kw_args, NULL);

    image_hint_t hint = py_helper_keyword_int(n_args, args, 8, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_hint), 0);

    int arg_x_size;
    bool got_x_size = py_helper_keyword_int_maybe(n_args, args, 9, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_size), &arg_x_size);

    int arg_y_size;
    bool got_y_size = py_helper_keyword_int_maybe(n_args, args, 10, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_size), &arg_y_size);

    if (got_x_scale && got_x_size) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Choose either x_scale or x_size not both!"));
    }

    if (got_y_scale && got_y_size) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Choose either y_scale or y_size not both!"));
    }

    if (got_x_size) {
        arg_x_scale = arg_x_size / ((float) arg_roi.w);
    }

    if (got_y_size) {
        arg_y_scale = arg_y_size / ((float) arg_roi.h);
    }

    if ((!got_x_scale) && (!got_x_size) && got_y_size) {
        arg_x_scale = arg_y_scale;
    }

    if ((!got_y_scale) && (!got_y_size) && got_x_size) {
        arg_y_scale = arg_x_scale;
    }

    mp_obj_t copy_obj = py_helper_keyword_object(n_args, args, 11, kw_args,
            MP_OBJ_NEW_QSTR(copy_to_fb ? MP_QSTR_copy_to_fb : MP_QSTR_copy), NULL);
    bool copy = false;
    image_t *arg_other = copy_to_fb ? NULL : src_img;

    if (copy_obj) {
        if (mp_obj_is_integer(copy_obj)) {
            copy = mp_obj_get_int(copy_obj);
        } else {
            arg_other = py_helper_arg_to_image_mutable_bayer(copy_obj);
        }
    }

    if (copy_to_fb && copy) {
        framebuffer_update_jpeg_buffer();
    }

    image_t dst_img;
    dst_img.w = fast_floorf(arg_roi.w * arg_x_scale);
    dst_img.h = fast_floorf(arg_roi.h * arg_y_scale);
    dst_img.bpp = (bpp >= 0) ? bpp : src_img->bpp;

    if (dst_img.bpp == IMAGE_BPP_BAYER) {
        if (((arg_x_scale != 1) && (arg_x_scale != -1)) ||
            ((arg_y_scale != 1) && (arg_y_scale != -1)) ||
            (arg_rgb_channel != -1) ||
            (arg_alpha != 256) ||
            (color_palette != NULL) ||
            (alpha_palette != NULL)) {
            mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Only bayer copying/cropping is supported!"));
        } else {
            hint &= ~(IMAGE_HINT_AREA |
                      IMAGE_HINT_BICUBIC |
                      IMAGE_HINT_BILINEAR |
                      IMAGE_HINT_EXTRACT_RGB_CHANNEL_FIRST |
                      IMAGE_HINT_APPLY_COLOR_PALETTE_FIRST);
        }
    } else if (dst_img.bpp >= IMAGE_BPP_JPEG) {
        if ((arg_x_scale != 1) ||
            (arg_y_scale != 1) ||
            (arg_roi.x != 0) ||
            (arg_roi.y != 0) ||
            (arg_roi.w != src_img->w) ||
            (arg_roi.h != src_img->h) ||
            (arg_rgb_channel != -1) ||
            (arg_alpha != 256) ||
            (color_palette != NULL) ||
            (alpha_palette != NULL)) {
            mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Only jpeg copying is supported!"));
        }
    }

    if (copy) {
        if (copy_to_fb) {
            py_helper_set_to_framebuffer(&dst_img);
        } else {
            dst_img.data = xalloc(image_size(&dst_img));
        }
    } else if (arg_other) {
        bool fb = py_helper_is_equal_to_framebuffer(arg_other);
        size_t size = fb ? framebuffer_get_buffer_size() : image_size(arg_other);
        PY_ASSERT_TRUE_MSG((image_size(&dst_img) <= size),
                "The new image won't fit in the target frame buffer!");
        // DO NOT MODIFY arg_other YET (as it could point to src_img)!
        dst_img.data = arg_other->data;
        py_helper_update_framebuffer(&dst_img);
    } else {
        dst_img.data = xalloc(image_size(&dst_img));
    }

    if (dst_img.bpp >= IMAGE_BPP_JPEG) {
        if (dst_img.data != src_img->data) {
            memcpy(dst_img.data, src_img->data, dst_img.bpp);
        }
    } else {
        fb_alloc_mark();
        imlib_draw_image(&dst_img, src_img, 0, 0, arg_x_scale, arg_y_scale, &arg_roi,
                        arg_rgb_channel, arg_alpha, color_palette, alpha_palette,
                        (hint & (~IMAGE_HINT_CENTER)) | IMAGE_HINT_BLACK_BACKGROUND, NULL, NULL);
        fb_alloc_free_till_mark();
    }

    if (arg_other) {
        arg_other->w = dst_img.w;
        arg_other->h = dst_img.h;
        arg_other->bpp = dst_img.bpp;
    }

    return py_image_from_struct(&dst_img);
}

static mp_obj_t py_image_to_bitmap(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    return py_image_to(IMAGE_BPP_BINARY, NULL, false, n_args, args, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_to_bitmap_obj, 1, py_image_to_bitmap);

static mp_obj_t py_image_to_grayscale(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    return py_image_to(IMAGE_BPP_GRAYSCALE, NULL, false, n_args, args, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_to_grayscale_obj, 1, py_image_to_grayscale);

static mp_obj_t py_image_to_rgb565(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    return py_image_to(IMAGE_BPP_RGB565, NULL, false, n_args, args, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_to_rgb565_obj, 1, py_image_to_rgb565);

static mp_obj_t py_image_to_rainbow(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    return py_image_to(IMAGE_BPP_RGB565, rainbow_table, false, n_args, args, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_to_rainbow_obj, 1, py_image_to_rainbow);

static mp_obj_t py_image_copy(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    return py_image_to(-1, NULL, true, n_args, args, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_copy_obj, 1, py_image_copy);

static mp_obj_t py_image_crop(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    return py_image_to(-1, NULL, false, n_args, args, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_crop_obj, 1, py_image_crop);

static mp_obj_t py_image_jpeg_encode_for_ide(mp_obj_t img_obj)
{
    image_t *arg_img = py_image_cobj(img_obj);
    PY_ASSERT_TRUE_MSG(arg_img->bpp >= IMAGE_BPP_JPEG, "Image format is not supported!");
    PY_ASSERT_TRUE_MSG(py_helper_is_equal_to_framebuffer(arg_img), "Can't compress in place!");

    int new_size = fb_encode_for_ide_new_size(arg_img);
    fb_alloc_mark();
    uint8_t *temp = fb_alloc(new_size, FB_ALLOC_NO_HINT);
    fb_encode_for_ide(temp, arg_img);

    image_t out;
    out.w = arg_img->w;
    out.h = arg_img->h;
    out.bpp = new_size;
    py_helper_set_to_framebuffer(&out);
    arg_img->bpp = new_size;

    memcpy(arg_img->data, temp, new_size);
    fb_alloc_free_till_mark();

    return img_obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_jpeg_encode_for_ide_obj, py_image_jpeg_encode_for_ide);

static mp_obj_t py_image_jpeg_encoded_for_ide(mp_obj_t img_obj)
{
    image_t *arg_img = py_image_cobj(img_obj);
    PY_ASSERT_TRUE_MSG(arg_img->bpp >= IMAGE_BPP_JPEG, "Image format is not supported!");

    int new_size = fb_encode_for_ide_new_size(arg_img);
    uint8_t *temp = xalloc(new_size);
    fb_encode_for_ide(temp, arg_img);

    return py_image(arg_img->w, arg_img->h, new_size, temp);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_jpeg_encoded_for_ide_obj, py_image_jpeg_encoded_for_ide);

static mp_obj_t py_image_compress(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    if (IM_IS_JPEG((image_t *) py_image_cobj(args[0]))) return args[0];

    image_t *arg_img = py_helper_arg_to_image_mutable_bayer(args[0]);
    int arg_q = py_helper_keyword_int(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_quality), 50);
    PY_ASSERT_TRUE_MSG((1 <= arg_q) && (arg_q <= 100), "Error: 1 <= quality <= 100!");

    fb_alloc_mark();
    image_t out = { .w=arg_img->w, .h=arg_img->h, .bpp=0, .data=NULL }; // alloc in jpeg compress
    PY_ASSERT_FALSE_MSG(jpeg_compress(arg_img, &out, arg_q, false), "Out of Memory!");
    PY_ASSERT_TRUE_MSG(out.bpp <= image_size(arg_img), "Can't compress in place!");
    memcpy(arg_img->data, out.data, out.bpp);
    arg_img->bpp = out.bpp;
    fb_alloc_free_till_mark();
    py_helper_update_framebuffer(arg_img);
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_compress_obj, 1, py_image_compress);

static mp_obj_t py_image_compress_for_ide(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    if (IM_IS_JPEG((image_t *) py_image_cobj(args[0]))) return py_image_jpeg_encode_for_ide(args[0]);

    image_t *arg_img = py_helper_arg_to_image_mutable_bayer(args[0]);
    int arg_q = py_helper_keyword_int(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_quality), 50);
    PY_ASSERT_TRUE_MSG((1 <= arg_q) && (arg_q <= 100), "Error: 1 <= quality <= 100!");

    fb_alloc_mark();
    image_t out = { .w=arg_img->w, .h=arg_img->h, .bpp=0, .data=NULL }; // alloc in jpeg compress
    PY_ASSERT_FALSE_MSG(jpeg_compress(arg_img, &out, arg_q, false), "Out of Memory!");
    int new_size = fb_encode_for_ide_new_size(&out);
    PY_ASSERT_TRUE_MSG(new_size <= image_size(arg_img), "Can't compress in place!");
    fb_encode_for_ide(arg_img->data, &out);

    out.bpp = new_size;
    arg_img->bpp = out.bpp;
    fb_alloc_free_till_mark();
    py_helper_update_framebuffer(arg_img);
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_compress_for_ide_obj, 1, py_image_compress_for_ide);

static mp_obj_t py_image_compressed(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_image_cobj(args[0]);

    if (IM_IS_JPEG(arg_img)) {
        image_t out = { .w=arg_img->w, .h=arg_img->h, .bpp=arg_img->bpp, .data=xalloc(arg_img->bpp) };
        memcpy(out.data, arg_img->data, arg_img->bpp);
        return py_image_from_struct(&out);
    }

    arg_img = py_helper_arg_to_image_mutable_bayer(args[0]);
    int arg_q = py_helper_keyword_int(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_quality), 50);
    PY_ASSERT_TRUE_MSG((1 <= arg_q) && (arg_q <= 100), "Error: 1 <= quality <= 100!");

    fb_alloc_mark();
    image_t out = { .w=arg_img->w, .h=arg_img->h, .bpp=0, .data=NULL }; // alloc in jpeg compress
    PY_ASSERT_FALSE_MSG(jpeg_compress(arg_img, &out, arg_q, false), "Out of Memory!");
    uint8_t *temp = xalloc(out.bpp);
    memcpy(temp, out.data, out.bpp);
    out.data = temp;
    fb_alloc_free_till_mark();

    return py_image_from_struct(&out);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_compressed_obj, 1, py_image_compressed);

static mp_obj_t py_image_compressed_for_ide(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    if (IM_IS_JPEG((image_t *) py_image_cobj(args[0]))) return py_image_jpeg_encoded_for_ide(args[0]);

    image_t *arg_img = py_helper_arg_to_image_mutable_bayer(args[0]);
    int arg_q = py_helper_keyword_int(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_quality), 50);
    PY_ASSERT_TRUE_MSG((1 <= arg_q) && (arg_q <= 100), "Error: 1 <= quality <= 100!");

    fb_alloc_mark();
    image_t out = { .w=arg_img->w, .h=arg_img->h, .bpp=0, .data=NULL }; // alloc in jpeg compress
    PY_ASSERT_FALSE_MSG(jpeg_compress(arg_img, &out, arg_q, false), "Out of Memory!");
    int new_size = fb_encode_for_ide_new_size(&out);
    uint8_t *temp = xalloc(new_size);
    fb_encode_for_ide(temp, &out);

    out.bpp = new_size;
    out.data = temp;
    fb_alloc_free_till_mark();

    return py_image_from_struct(&out);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_compressed_for_ide_obj, 1, py_image_compressed_for_ide);

#if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
static mp_obj_t py_image_save(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_image_cobj(args[0]);
    const char *path = mp_obj_str_get_str(args[1]);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 2, kw_args, &roi);

    int arg_q = py_helper_keyword_int(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_quality), 50);
    PY_ASSERT_TRUE_MSG((1 <= arg_q) && (arg_q <= 100), "Error: 1 <= quality <= 100!");

    fb_alloc_mark();
    imlib_save_image(arg_img, path, &roi, arg_q);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_save_obj, 2, py_image_save);
#endif //IMLIB_ENABLE_IMAGE_FILE_IO

static mp_obj_t py_image_flush(mp_obj_t img_obj)
{
    framebuffer_update_jpeg_buffer();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_flush_obj, py_image_flush);

//////////////////
// Drawing Methods
//////////////////

STATIC mp_obj_t py_image_clear(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable_bayer(args[0]);

    image_t *arg_msk =
            py_helper_keyword_to_image_mutable_mask(n_args, args, 1, kw_args);

    if (!arg_msk) {
        memset(arg_img->data, 0, image_size(arg_img));
    } else {
        imlib_zero(arg_img, arg_msk, false);
    }

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_clear_obj, 1, py_image_clear);

STATIC mp_obj_t py_image_draw_line(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    const mp_obj_t *arg_vec;
    uint offset = py_helper_consume_array(n_args, args, 1, 4, &arg_vec);
    int arg_x0 = mp_obj_get_int(arg_vec[0]);
    int arg_y0 = mp_obj_get_int(arg_vec[1]);
    int arg_x1 = mp_obj_get_int(arg_vec[2]);
    int arg_y1 = mp_obj_get_int(arg_vec[3]);

    int arg_c =
        py_helper_keyword_color(arg_img, n_args, args, offset + 0, kw_args, -1); // White.
    int arg_thickness =
        py_helper_keyword_int(n_args, args, offset + 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_thickness), 1);

    imlib_draw_line(arg_img, arg_x0, arg_y0, arg_x1, arg_y1, arg_c, arg_thickness);
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_draw_line_obj, 2, py_image_draw_line);

STATIC mp_obj_t py_image_draw_rectangle(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    const mp_obj_t *arg_vec;
    uint offset = py_helper_consume_array(n_args, args, 1, 4, &arg_vec);
    int arg_rx = mp_obj_get_int(arg_vec[0]);
    int arg_ry = mp_obj_get_int(arg_vec[1]);
    int arg_rw = mp_obj_get_int(arg_vec[2]);
    int arg_rh = mp_obj_get_int(arg_vec[3]);

    int arg_c =
        py_helper_keyword_color(arg_img, n_args, args, offset + 0, kw_args, -1); // White.
    int arg_thickness =
        py_helper_keyword_int(n_args, args, offset + 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_thickness), 1);
    bool arg_fill =
        py_helper_keyword_int(n_args, args, offset + 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_fill), false);

    imlib_draw_rectangle(arg_img, arg_rx, arg_ry, arg_rw, arg_rh, arg_c, arg_thickness, arg_fill);
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_draw_rectangle_obj, 2, py_image_draw_rectangle);

STATIC mp_obj_t py_image_draw_circle(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    const mp_obj_t *arg_vec;
    uint offset = py_helper_consume_array(n_args, args, 1, 3, &arg_vec);
    int arg_cx = mp_obj_get_int(arg_vec[0]);
    int arg_cy = mp_obj_get_int(arg_vec[1]);
    int arg_cr = mp_obj_get_int(arg_vec[2]);

    int arg_c =
        py_helper_keyword_color(arg_img, n_args, args, offset + 0, kw_args, -1); // White.
    int arg_thickness =
        py_helper_keyword_int(n_args, args, offset + 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_thickness), 1);
    bool arg_fill =
        py_helper_keyword_int(n_args, args, offset + 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_fill), false);

    imlib_draw_circle(arg_img, arg_cx, arg_cy, arg_cr, arg_c, arg_thickness, arg_fill);
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_draw_circle_obj, 2, py_image_draw_circle);

STATIC mp_obj_t py_image_draw_ellipse(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    const mp_obj_t *arg_vec;
    uint offset = py_helper_consume_array(n_args, args, 1, 5, &arg_vec);
    int arg_cx = mp_obj_get_int(arg_vec[0]);
    int arg_cy = mp_obj_get_int(arg_vec[1]);
    int arg_rx = mp_obj_get_int(arg_vec[2]);
    int arg_ry = mp_obj_get_int(arg_vec[3]);
    int arg_r = mp_obj_get_int(arg_vec[4]);

    int arg_c =
        py_helper_keyword_color(arg_img, n_args, args, offset + 1, kw_args, -1); // White.
    int arg_thickness =
        py_helper_keyword_int(n_args, args, offset + 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_thickness), 1);
    bool arg_fill =
        py_helper_keyword_int(n_args, args, offset + 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_fill), false);

    imlib_draw_ellipse(arg_img, arg_cx, arg_cy, arg_rx, arg_ry, arg_r, arg_c, arg_thickness, arg_fill);
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_draw_ellipse_obj, 2, py_image_draw_ellipse);

STATIC mp_obj_t py_image_draw_string(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    const mp_obj_t *arg_vec;
    uint offset = py_helper_consume_array(n_args, args, 1, 3, &arg_vec);
    int arg_x_off = mp_obj_get_int(arg_vec[0]);
    int arg_y_off = mp_obj_get_int(arg_vec[1]);
    const char *arg_str = mp_obj_str_get_str(arg_vec[2]);

    int arg_c =
        py_helper_keyword_color(arg_img, n_args, args, offset + 0, kw_args, -1); // White.
    float arg_scale =
        py_helper_keyword_float(n_args, args, offset + 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_scale), 1.0);
    PY_ASSERT_TRUE_MSG(0 < arg_scale, "Error: 0 < scale!");
    int arg_x_spacing =
        py_helper_keyword_int(n_args, args, offset + 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_spacing), 0);
    int arg_y_spacing =
        py_helper_keyword_int(n_args, args, offset + 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_spacing), 0);
    bool arg_mono_space =
        py_helper_keyword_int(n_args, args, offset + 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_mono_space), true);
    int arg_char_rotation =
        py_helper_keyword_int(n_args, args, offset + 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_char_rotation), 0);
    int arg_char_hmirror =
        py_helper_keyword_int(n_args, args, offset + 6, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_char_hmirror), false);
    int arg_char_vflip =
        py_helper_keyword_int(n_args, args, offset + 7, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_char_vflip), false);
    int arg_string_rotation =
        py_helper_keyword_int(n_args, args, offset + 8, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_string_rotation), 0);
    int arg_string_hmirror =
        py_helper_keyword_int(n_args, args, offset + 9, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_string_hmirror), false);
    int arg_string_vflip =
        py_helper_keyword_int(n_args, args, offset + 10, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_string_vflip), false);

    imlib_draw_string(arg_img, arg_x_off, arg_y_off, arg_str,
                      arg_c, arg_scale, arg_x_spacing, arg_y_spacing, arg_mono_space,
                      arg_char_rotation, arg_char_hmirror, arg_char_vflip,
                      arg_string_rotation, arg_string_hmirror, arg_string_vflip);
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_draw_string_obj, 2, py_image_draw_string);

STATIC mp_obj_t py_image_draw_cross(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    const mp_obj_t *arg_vec;
    uint offset = py_helper_consume_array(n_args, args, 1, 2, &arg_vec);
    int arg_x = mp_obj_get_int(arg_vec[0]);
    int arg_y = mp_obj_get_int(arg_vec[1]);

    int arg_c =
        py_helper_keyword_color(arg_img, n_args, args, offset + 0, kw_args, -1); // White.
    int arg_s =
        py_helper_keyword_int(n_args, args, offset + 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_size), 5);
    int arg_thickness =
        py_helper_keyword_int(n_args, args, offset + 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_thickness), 1);

    imlib_draw_line(arg_img, arg_x - arg_s, arg_y        , arg_x + arg_s, arg_y        , arg_c, arg_thickness);
    imlib_draw_line(arg_img, arg_x        , arg_y - arg_s, arg_x        , arg_y + arg_s, arg_c, arg_thickness);
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_draw_cross_obj, 2, py_image_draw_cross);

STATIC mp_obj_t py_image_draw_arrow(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    const mp_obj_t *arg_vec;
    uint offset = py_helper_consume_array(n_args, args, 1, 4, &arg_vec);
    int arg_x0 = mp_obj_get_int(arg_vec[0]);
    int arg_y0 = mp_obj_get_int(arg_vec[1]);
    int arg_x1 = mp_obj_get_int(arg_vec[2]);
    int arg_y1 = mp_obj_get_int(arg_vec[3]);

    int arg_c =
        py_helper_keyword_color(arg_img, n_args, args, offset + 0, kw_args, -1); // White.
    int arg_s =
        py_helper_keyword_int(n_args, args, offset + 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_size), 10);
    int arg_thickness =
        py_helper_keyword_int(n_args, args, offset + 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_thickness), 1);

    int dx = (arg_x1 - arg_x0);
    int dy = (arg_y1 - arg_y0);
    float length = fast_sqrtf((dx * dx) + (dy * dy));

    float ux = IM_DIV(dx, length);
    float uy = IM_DIV(dy, length);
    float vx = -uy;
    float vy = ux;

    int a0x = fast_roundf(arg_x1 - (arg_s * ux) + (arg_s * vx * 0.5));
    int a0y = fast_roundf(arg_y1 - (arg_s * uy) + (arg_s * vy * 0.5));
    int a1x = fast_roundf(arg_x1 - (arg_s * ux) - (arg_s * vx * 0.5));
    int a1y = fast_roundf(arg_y1 - (arg_s * uy) - (arg_s * vy * 0.5));

    imlib_draw_line(arg_img, arg_x0, arg_y0, arg_x1, arg_y1, arg_c, arg_thickness);
    imlib_draw_line(arg_img, arg_x1, arg_y1, a0x, a0y, arg_c, arg_thickness);
    imlib_draw_line(arg_img, arg_x1, arg_y1, a1x, a1y, arg_c, arg_thickness);
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_draw_arrow_obj, 2, py_image_draw_arrow);

STATIC mp_obj_t py_image_draw_edges(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    mp_obj_t *corners, *p0, *p1, *p2, *p3;
    mp_obj_get_array_fixed_n(args[1], 4, &corners);
    mp_obj_get_array_fixed_n(corners[0], 2, &p0);
    mp_obj_get_array_fixed_n(corners[1], 2, &p1);
    mp_obj_get_array_fixed_n(corners[2], 2, &p2);
    mp_obj_get_array_fixed_n(corners[3], 2, &p3);

    int x0, y0, x1, y1, x2, y2, x3, y3;
    x0 = mp_obj_get_int(p0[0]);
    y0 = mp_obj_get_int(p0[1]);
    x1 = mp_obj_get_int(p1[0]);
    y1 = mp_obj_get_int(p1[1]);
    x2 = mp_obj_get_int(p2[0]);
    y2 = mp_obj_get_int(p2[1]);
    x3 = mp_obj_get_int(p3[0]);
    y3 = mp_obj_get_int(p3[1]);

    int arg_c =
        py_helper_keyword_color(arg_img, n_args, args, 2, kw_args, -1); // White.
    int arg_s =
        py_helper_keyword_int(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_size), 0);
    int arg_thickness =
        py_helper_keyword_int(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_thickness), 1);
    bool arg_fill =
        py_helper_keyword_int(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_fill), false);

    imlib_draw_line(arg_img, x0, y0, x1, y1, arg_c, arg_thickness);
    imlib_draw_line(arg_img, x1, y1, x2, y2, arg_c, arg_thickness);
    imlib_draw_line(arg_img, x2, y2, x3, y3, arg_c, arg_thickness);
    imlib_draw_line(arg_img, x3, y3, x0, y0, arg_c, arg_thickness);

    if (arg_s >= 1) {
        imlib_draw_circle(arg_img, x0, y0, arg_s, arg_c, arg_thickness, arg_fill);
        imlib_draw_circle(arg_img, x1, y1, arg_s, arg_c, arg_thickness, arg_fill);
        imlib_draw_circle(arg_img, x2, y2, arg_s, arg_c, arg_thickness, arg_fill);
        imlib_draw_circle(arg_img, x3, y3, arg_s, arg_c, arg_thickness, arg_fill);
    }

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_draw_edges_obj, 2, py_image_draw_edges);

STATIC mp_obj_t py_image_draw_image(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);
    image_t *arg_other = py_helper_arg_to_image_mutable_bayer(args[1]);

    const mp_obj_t *arg_vec;
    uint offset = py_helper_consume_array(n_args, args, 2, 2, &arg_vec);
    int arg_x_off = mp_obj_get_int(arg_vec[0]);
    int arg_y_off = mp_obj_get_int(arg_vec[1]);

    float arg_x_scale = 1.f;
    bool got_x_scale = py_helper_keyword_float_maybe(n_args, args, offset + 0, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_scale), &arg_x_scale);

    float arg_y_scale = 1.f;
    bool got_y_scale = py_helper_keyword_float_maybe(n_args, args, offset + 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_scale), &arg_y_scale);

    rectangle_t arg_roi;
    py_helper_keyword_rectangle_roi(arg_other, n_args, args, offset + 2, kw_args, &arg_roi);

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

    fb_alloc_mark();
    imlib_draw_image(arg_img, arg_other, arg_x_off, arg_y_off, arg_x_scale, arg_y_scale, &arg_roi,
                     arg_rgb_channel, arg_alpha, color_palette, alpha_palette, hint, NULL, NULL);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_draw_image_obj, 3, py_image_draw_image);

STATIC mp_obj_t py_image_draw_keypoints(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    int arg_c =
        py_helper_keyword_color(arg_img, n_args, args, 2, kw_args, -1); // White.
    int arg_s =
        py_helper_keyword_int(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_size), 10);
    int arg_thickness =
        py_helper_keyword_int(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_thickness), 1);
    bool arg_fill =
        py_helper_keyword_int(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_fill), false);

    if (MP_OBJ_IS_TYPE(args[1], &mp_type_tuple) || MP_OBJ_IS_TYPE(args[1], &mp_type_list)) {
        size_t len;
        mp_obj_t *items;
        mp_obj_get_array(args[1], &len, &items);
        for (size_t i = 0; i < len; i++) {
            mp_obj_t *tuple;
            mp_obj_get_array_fixed_n(items[i], 3, &tuple);
            int cx = mp_obj_get_int(tuple[0]);
            int cy = mp_obj_get_int(tuple[1]);
            int angle = mp_obj_get_int(tuple[2]) % 360;
            int si = sin_table[angle] * arg_s;
            int co = cos_table[angle] * arg_s;
            imlib_draw_line(arg_img, cx, cy, cx + co, cy + si, arg_c, arg_thickness);
            imlib_draw_circle(arg_img, cx, cy, (arg_s - 2) / 2, arg_c, arg_thickness, arg_fill);
        }
    } else {
#ifdef IMLIB_ENABLE_FIND_KEYPOINTS
        py_kp_obj_t *kpts_obj = py_kpts_obj(args[1]);
        for (int i = 0, ii = array_length(kpts_obj->kpts); i < ii; i++) {
            kp_t *kp = array_at(kpts_obj->kpts, i);
            int cx = kp->x;
            int cy = kp->y;
            int angle = kp->angle % 360;
            int si = sin_table[angle] * arg_s;
            int co = cos_table[angle] * arg_s;
            imlib_draw_line(arg_img, cx, cy, cx + co, cy + si, arg_c, arg_thickness);
            imlib_draw_circle(arg_img, cx, cy, (arg_s - 2) / 2, arg_c, arg_thickness, arg_fill);
        }
#else
        PY_ASSERT_TRUE_MSG(false, "Expected a list of tuples!");
#endif // IMLIB_ENABLE_FIND_KEYPOINTS
    }

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_draw_keypoints_obj, 2, py_image_draw_keypoints);

STATIC mp_obj_t py_image_mask_rectangle(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);
    int arg_rx;
    int arg_ry;
    int arg_rw;
    int arg_rh;

    if (n_args > 1) {
        const mp_obj_t *arg_vec;
        py_helper_consume_array(n_args, args, 1, 4, &arg_vec);
        arg_rx = mp_obj_get_int(arg_vec[0]);
        arg_ry = mp_obj_get_int(arg_vec[1]);
        arg_rw = mp_obj_get_int(arg_vec[2]);
        arg_rh = mp_obj_get_int(arg_vec[3]);
    } else {
        arg_rx = arg_img->w / 4;
        arg_ry = arg_img->h / 4;
        arg_rw = arg_img->w / 2;
        arg_rh = arg_img->h / 2;
    }

    fb_alloc_mark();
    image_t temp;
    temp.w = arg_img->w;
    temp.h = arg_img->h;
    temp.bpp = IMAGE_BPP_BINARY;
    temp.data = fb_alloc0(image_size(&temp), FB_ALLOC_NO_HINT);

    imlib_draw_rectangle(&temp, arg_rx, arg_ry, arg_rw, arg_rh, -1, 0, true);
    imlib_zero(arg_img, &temp, true);

    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_mask_rectangle_obj, 1, py_image_mask_rectangle);

STATIC mp_obj_t py_image_mask_circle(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);
    int arg_cx;
    int arg_cy;
    int arg_cr;

    if (n_args > 1) {
        const mp_obj_t *arg_vec;
        py_helper_consume_array(n_args, args, 1, 3, &arg_vec);
        arg_cx = mp_obj_get_int(arg_vec[0]);
        arg_cy = mp_obj_get_int(arg_vec[1]);
        arg_cr = mp_obj_get_int(arg_vec[2]);
    } else {
        arg_cx = arg_img->w / 2;
        arg_cy = arg_img->h / 2;
        arg_cr = IM_MIN(arg_img->w, arg_img->h) / 2;
    }

    fb_alloc_mark();
    image_t temp;
    temp.w = arg_img->w;
    temp.h = arg_img->h;
    temp.bpp = IMAGE_BPP_BINARY;
    temp.data = fb_alloc0(image_size(&temp), FB_ALLOC_NO_HINT);

    imlib_draw_circle(&temp, arg_cx, arg_cy, arg_cr, -1, 0, true);
    imlib_zero(arg_img, &temp, true);

    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_mask_circle_obj, 1, py_image_mask_circle);

STATIC mp_obj_t py_image_mask_ellipse(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);
    int arg_cx;
    int arg_cy;
    int arg_rx;
    int arg_ry;
    int arg_r;

    if (n_args > 1) {
        const mp_obj_t *arg_vec;
        py_helper_consume_array(n_args, args, 1, 5, &arg_vec);
        arg_cx = mp_obj_get_int(arg_vec[0]);
        arg_cy = mp_obj_get_int(arg_vec[1]);
        arg_rx = mp_obj_get_int(arg_vec[2]);
        arg_ry = mp_obj_get_int(arg_vec[3]);
        arg_r = mp_obj_get_int(arg_vec[4]);
    } else {
        arg_cx = arg_img->w / 2;
        arg_cy = arg_img->h / 2;
        arg_rx = arg_img->w / 2;
        arg_ry = arg_img->h / 2;
        arg_r = 0;
    }

    fb_alloc_mark();
    image_t temp;
    temp.w = arg_img->w;
    temp.h = arg_img->h;
    temp.bpp = IMAGE_BPP_BINARY;
    temp.data = fb_alloc0(image_size(&temp), FB_ALLOC_NO_HINT);

    imlib_draw_ellipse(&temp, arg_cx, arg_cy, arg_rx, arg_ry, arg_r, -1, 0, true);
    imlib_zero(arg_img, &temp, true);

    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_mask_ellipse_obj, 1, py_image_mask_ellipse);

#ifdef IMLIB_ENABLE_FLOOD_FILL
STATIC mp_obj_t py_image_flood_fill(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    const mp_obj_t *arg_vec;
    uint offset = py_helper_consume_array(n_args, args, 1, 2, &arg_vec);
    int arg_x_off = mp_obj_get_int(arg_vec[0]);
    int arg_y_off = mp_obj_get_int(arg_vec[1]);

    float arg_seed_threshold =
        py_helper_keyword_float(n_args, args, offset + 0, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_seed_threshold), 0.05);
    PY_ASSERT_TRUE_MSG((0.0f <= arg_seed_threshold) && (arg_seed_threshold <= 1.0f),
                       "Error: 0.0 <= seed_threshold <= 1.0!");
    float arg_floating_threshold =
        py_helper_keyword_float(n_args, args, offset + 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_floating_threshold), 0.05);
    PY_ASSERT_TRUE_MSG((0.0f <= arg_floating_threshold) && (arg_floating_threshold <= 1.0f),
                       "Error: 0.0 <= floating_threshold <= 1.0!");
    int arg_c =
        py_helper_keyword_color(arg_img, n_args, args, offset + 2, kw_args, -1); // White.
    bool arg_invert =
        py_helper_keyword_float(n_args, args, offset + 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_invert), false);
    bool clear_background =
        py_helper_keyword_float(n_args, args, offset + 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_clear_background), false);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, offset + 5, kw_args);

    fb_alloc_mark();
    imlib_flood_fill(arg_img, arg_x_off, arg_y_off,
                     arg_seed_threshold, arg_floating_threshold,
                     arg_c, arg_invert, clear_background, arg_msk);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_flood_fill_obj, 2, py_image_flood_fill);
#endif // IMLIB_ENABLE_FLOOD_FILL

#ifdef IMLIB_ENABLE_BINARY_OPS
/////////////////
// Binary Methods
/////////////////

STATIC mp_obj_t py_image_binary(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    list_t arg_thresholds;
    list_init(&arg_thresholds, sizeof(color_thresholds_list_lnk_data_t));
    py_helper_arg_to_thresholds(args[1], &arg_thresholds);

    bool arg_invert =
        py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_invert), false);
    bool arg_zero =
        py_helper_keyword_int(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_zero), false);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 4, kw_args);
    bool arg_to_bitmap =
        py_helper_keyword_int(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_to_bitmap), false);
    bool arg_copy =
        py_helper_keyword_int(n_args, args, 6, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_copy), false);

    if (arg_to_bitmap && (!arg_copy)) {
        switch(arg_img->bpp) {
            case IMAGE_BPP_GRAYSCALE: {
                PY_ASSERT_TRUE_MSG((arg_img->w >= (sizeof(uint32_t)/sizeof(uint8_t))),
                                   "Can't convert to bitmap in place!");
                break;
            }
            case IMAGE_BPP_RGB565: {
                PY_ASSERT_TRUE_MSG((arg_img->w >= (sizeof(uint32_t)/sizeof(uint16_t))),
                                   "Can't convert to bitmap in place!");
                break;
            }
            default: {
                break;
            }
        }
    }

    image_t out;
    out.w = arg_img->w;
    out.h = arg_img->h;
    out.bpp = arg_to_bitmap ? IMAGE_BPP_BINARY : arg_img->bpp;
    out.data = arg_copy ? xalloc(image_size(&out)) : arg_img->data;

    fb_alloc_mark();
    imlib_binary(&out, arg_img, &arg_thresholds, arg_invert, arg_zero, arg_msk);
    fb_alloc_free_till_mark();

    list_free(&arg_thresholds);

    if (arg_to_bitmap && (!arg_copy)) {
        arg_img->bpp = IMAGE_BPP_BINARY;
        py_helper_update_framebuffer(&out);
    }

    return py_image_from_struct(&out);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_binary_obj, 2, py_image_binary);

STATIC mp_obj_t py_image_invert(mp_obj_t img_obj)
{
    imlib_invert(py_helper_arg_to_image_mutable(img_obj));
    return img_obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_invert_obj, py_image_invert);

STATIC mp_obj_t py_image_b_and(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 2, kw_args);

    fb_alloc_mark();

    if (MP_OBJ_IS_STR(args[1])) {
        imlib_b_and(arg_img, mp_obj_str_get_str(args[1]), NULL, 0, arg_msk);
    } else if (MP_OBJ_IS_TYPE(args[1], &py_image_type)) {
        imlib_b_and(arg_img, NULL, py_helper_arg_to_image_mutable(args[1]), 0, arg_msk);
    } else {
        imlib_b_and(arg_img, NULL, NULL,
                    py_helper_keyword_color(arg_img, n_args, args, 1, NULL, 0),
                    arg_msk);
    }

    fb_alloc_free_till_mark();

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_b_and_obj, 2, py_image_b_and);

STATIC mp_obj_t py_image_b_nand(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 2, kw_args);

    fb_alloc_mark();

    if (MP_OBJ_IS_STR(args[1])) {
        imlib_b_nand(arg_img, mp_obj_str_get_str(args[1]), NULL, 0, arg_msk);
    } else if (MP_OBJ_IS_TYPE(args[1], &py_image_type)) {
        imlib_b_nand(arg_img, NULL, py_helper_arg_to_image_mutable(args[1]), 0, arg_msk);
    } else {
        imlib_b_nand(arg_img, NULL, NULL,
                     py_helper_keyword_color(arg_img, n_args, args, 1, NULL, 0),
                     arg_msk);
    }

    fb_alloc_free_till_mark();

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_b_nand_obj, 2, py_image_b_nand);

STATIC mp_obj_t py_image_b_or(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 2, kw_args);

    fb_alloc_mark();

    if (MP_OBJ_IS_STR(args[1])) {
        imlib_b_or(arg_img, mp_obj_str_get_str(args[1]), NULL, 0, arg_msk);
    } else if (MP_OBJ_IS_TYPE(args[1], &py_image_type)) {
        imlib_b_or(arg_img, NULL, py_helper_arg_to_image_mutable(args[1]), 0, arg_msk);
    } else {
        imlib_b_or(arg_img, NULL, NULL,
                   py_helper_keyword_color(arg_img, n_args, args, 1, NULL, 0),
                   arg_msk);
    }

    fb_alloc_free_till_mark();

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_b_or_obj, 2, py_image_b_or);

STATIC mp_obj_t py_image_b_nor(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 2, kw_args);

    fb_alloc_mark();

    if (MP_OBJ_IS_STR(args[1])) {
        imlib_b_nor(arg_img, mp_obj_str_get_str(args[1]), NULL, 0, arg_msk);
    } else if (MP_OBJ_IS_TYPE(args[1], &py_image_type)) {
        imlib_b_nor(arg_img, NULL, py_helper_arg_to_image_mutable(args[1]), 0, arg_msk);
    } else {
        imlib_b_nor(arg_img, NULL, NULL,
                    py_helper_keyword_color(arg_img, n_args, args, 1, NULL, 0),
                    arg_msk);
    }

    fb_alloc_free_till_mark();

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_b_nor_obj, 2, py_image_b_nor);

STATIC mp_obj_t py_image_b_xor(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 2, kw_args);

    fb_alloc_mark();

    if (MP_OBJ_IS_STR(args[1])) {
        imlib_b_xor(arg_img, mp_obj_str_get_str(args[1]), NULL, 0, arg_msk);
    } else if (MP_OBJ_IS_TYPE(args[1], &py_image_type)) {
        imlib_b_xor(arg_img, NULL, py_helper_arg_to_image_mutable(args[1]), 0, arg_msk);
    } else {
        imlib_b_xor(arg_img, NULL, NULL,
                    py_helper_keyword_color(arg_img, n_args, args, 1, NULL, 0),
                    arg_msk);
    }

    fb_alloc_free_till_mark();

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_b_xor_obj, 2, py_image_b_xor);

STATIC mp_obj_t py_image_b_xnor(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 2, kw_args);

    fb_alloc_mark();

    if (MP_OBJ_IS_STR(args[1])) {
        imlib_b_xnor(arg_img, mp_obj_str_get_str(args[1]), NULL, 0, arg_msk);
    } else if (MP_OBJ_IS_TYPE(args[1], &py_image_type)) {
        imlib_b_xnor(arg_img, NULL, py_helper_arg_to_image_mutable(args[1]), 0, arg_msk);
    } else {
        imlib_b_xnor(arg_img, NULL, NULL,
                     py_helper_keyword_color(arg_img, n_args, args, 1, NULL, 0),
                     arg_msk);
    }

    fb_alloc_free_till_mark();

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_b_xnor_obj, 2, py_image_b_xnor);

STATIC mp_obj_t py_image_erode(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    int arg_ksize =
        py_helper_arg_to_ksize(args[1]);
    int arg_threshold =
        py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold),
            py_helper_ksize_to_n(arg_ksize) - 1);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 3, kw_args);

    fb_alloc_mark();
    imlib_erode(py_helper_arg_to_image_mutable(args[0]), arg_ksize, arg_threshold, arg_msk);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_erode_obj, 2, py_image_erode);

STATIC mp_obj_t py_image_dilate(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    int arg_ksize =
        py_helper_arg_to_ksize(args[1]);
    int arg_threshold =
        py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold),
            0);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 3, kw_args);

    fb_alloc_mark();
    imlib_dilate(py_helper_arg_to_image_mutable(args[0]), arg_ksize, arg_threshold, arg_msk);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_dilate_obj, 2, py_image_dilate);

STATIC mp_obj_t py_image_open(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    int arg_ksize =
        py_helper_arg_to_ksize(args[1]);
    int arg_threshold =
        py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), 0);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 3, kw_args);

    fb_alloc_mark();
    imlib_open(py_helper_arg_to_image_mutable(args[0]), arg_ksize, arg_threshold, arg_msk);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_open_obj, 2, py_image_open);

STATIC mp_obj_t py_image_close(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    int arg_ksize =
        py_helper_arg_to_ksize(args[1]);
    int arg_threshold =
        py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), 0);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 3, kw_args);

    fb_alloc_mark();
    imlib_close(py_helper_arg_to_image_mutable(args[0]), arg_ksize, arg_threshold, arg_msk);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_close_obj, 2, py_image_close);
#endif // IMLIB_ENABLE_BINARY_OPS

#ifdef IMLIB_ENABLE_MATH_OPS
///////////////
// Math Methods
///////////////

STATIC mp_obj_t py_image_gamma_corr(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    float arg_gamma =
        py_helper_keyword_float(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_gamma), 1.0f);
    float arg_contrast =
        py_helper_keyword_float(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_contrast), 1.0f);
    float arg_brightness =
        py_helper_keyword_float(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_brightness), 0.0f);

    fb_alloc_mark();
    imlib_gamma_corr(arg_img, arg_gamma, arg_contrast, arg_brightness);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_gamma_corr_obj, 1, py_image_gamma_corr);

STATIC mp_obj_t py_image_negate(mp_obj_t img_obj)
{
    imlib_negate(py_helper_arg_to_image_mutable(img_obj));
    return img_obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_negate_obj, py_image_negate);

STATIC mp_obj_t py_image_replace(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    bool arg_hmirror =
        py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_hmirror), false);
    bool arg_vflip =
        py_helper_keyword_int(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_vflip), false);
    bool arg_transpose =
        py_helper_keyword_int(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_transpose), false);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 5, kw_args);

    if (arg_transpose) {
        size_t size0 = image_size(arg_img);
        int w = arg_img->w;
        int h = arg_img->h;
        arg_img->w = h;
        arg_img->h = w;
        size_t size1 = image_size(arg_img);
        arg_img->w = w;
        arg_img->h = h;
        PY_ASSERT_TRUE_MSG(size1 <= size0,
                           "Unable to transpose the image because it would grow in size!");
    }

    fb_alloc_mark();

    mp_obj_t arg_1 = (n_args > 1) ? args[1] : args[0];

    if (MP_OBJ_IS_STR(arg_1)) {
        imlib_replace(arg_img, mp_obj_str_get_str(arg_1), NULL, 0,
                      arg_hmirror, arg_vflip, arg_transpose, arg_msk);
    } else if (MP_OBJ_IS_TYPE(arg_1, &py_image_type)) {
        imlib_replace(arg_img, NULL, py_helper_arg_to_image_mutable(arg_1), 0,
                      arg_hmirror, arg_vflip, arg_transpose, arg_msk);
    } else {
        imlib_replace(arg_img, NULL, NULL,
                      py_helper_keyword_color(arg_img, n_args, args, 1, NULL, 0),
                      arg_hmirror, arg_vflip, arg_transpose, arg_msk);
    }

    fb_alloc_free_till_mark();
    py_helper_update_framebuffer(arg_img);
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_replace_obj, 1, py_image_replace);

STATIC mp_obj_t py_image_add(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 2, kw_args);

    fb_alloc_mark();

    if (MP_OBJ_IS_STR(args[1])) {
        imlib_add(arg_img, mp_obj_str_get_str(args[1]), NULL, 0, arg_msk);
    } else if (MP_OBJ_IS_TYPE(args[1], &py_image_type)) {
        imlib_add(arg_img, NULL, py_helper_arg_to_image_mutable(args[1]), 0, arg_msk);
    } else {
        imlib_add(arg_img, NULL, NULL,
                  py_helper_keyword_color(arg_img, n_args, args, 1, NULL, 0),
                  arg_msk);
    }

    fb_alloc_free_till_mark();

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_add_obj, 2, py_image_add);

STATIC mp_obj_t py_image_sub(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    bool arg_reverse =
        py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_reverse), false);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 3, kw_args);

    fb_alloc_mark();

    if (MP_OBJ_IS_STR(args[1])) {
        imlib_sub(arg_img, mp_obj_str_get_str(args[1]), NULL, 0, arg_reverse, arg_msk);
    } else if (MP_OBJ_IS_TYPE(args[1], &py_image_type)) {
        imlib_sub(arg_img, NULL, py_helper_arg_to_image_mutable(args[1]), 0, arg_reverse, arg_msk);
    } else {
        imlib_sub(arg_img, NULL, NULL,
                  py_helper_keyword_color(arg_img, n_args, args, 1, NULL, 0),
                  arg_reverse, arg_msk);
    }

    fb_alloc_free_till_mark();

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_sub_obj, 2, py_image_sub);

STATIC mp_obj_t py_image_mul(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    bool arg_invert =
        py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_invert), false);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 3, kw_args);

    fb_alloc_mark();

    if (MP_OBJ_IS_STR(args[1])) {
        imlib_mul(arg_img, mp_obj_str_get_str(args[1]), NULL, 0, arg_invert, arg_msk);
    } else if (MP_OBJ_IS_TYPE(args[1], &py_image_type)) {
        imlib_mul(arg_img, NULL, py_helper_arg_to_image_mutable(args[1]), 0, arg_invert, arg_msk);
    } else {
        imlib_mul(arg_img, NULL, NULL,
                  py_helper_keyword_color(arg_img, n_args, args, 1, NULL, 0),
                  arg_invert, arg_msk);
    }

    fb_alloc_free_till_mark();

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_mul_obj, 2, py_image_mul);

STATIC mp_obj_t py_image_div(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    bool arg_invert =
        py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_invert), false);
    bool arg_mod =
        py_helper_keyword_int(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_mod), false);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 4, kw_args);

    fb_alloc_mark();

    if (MP_OBJ_IS_STR(args[1])) {
        imlib_div(arg_img, mp_obj_str_get_str(args[1]), NULL, 0,
                  arg_invert, arg_mod, arg_msk);
    } else if (MP_OBJ_IS_TYPE(args[1], &py_image_type)) {
        imlib_div(arg_img, NULL, py_helper_arg_to_image_mutable(args[1]), 0,
                  arg_invert, arg_mod, arg_msk);
    } else {
        imlib_div(arg_img, NULL, NULL,
                  py_helper_keyword_color(arg_img, n_args, args, 1, NULL, 0),
                  arg_invert, arg_mod, arg_msk);
    }

    fb_alloc_free_till_mark();

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_div_obj, 2, py_image_div);

STATIC mp_obj_t py_image_min(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 2, kw_args);

    fb_alloc_mark();

    if (MP_OBJ_IS_STR(args[1])) {
        imlib_min(arg_img, mp_obj_str_get_str(args[1]), NULL, 0, arg_msk);
    } else if (MP_OBJ_IS_TYPE(args[1], &py_image_type)) {
        imlib_min(arg_img, NULL, py_helper_arg_to_image_mutable(args[1]), 0, arg_msk);
    } else {
        imlib_min(arg_img, NULL, NULL,
                  py_helper_keyword_color(arg_img, n_args, args, 1, NULL, 0),
                  arg_msk);
    }

    fb_alloc_free_till_mark();

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_min_obj, 2, py_image_min);

STATIC mp_obj_t py_image_max(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 2, kw_args);

    fb_alloc_mark();

    if (MP_OBJ_IS_STR(args[1])) {
        imlib_max(arg_img, mp_obj_str_get_str(args[1]), NULL, 0, arg_msk);
    } else if (MP_OBJ_IS_TYPE(args[1], &py_image_type)) {
        imlib_max(arg_img, NULL, py_helper_arg_to_image_mutable(args[1]), 0, arg_msk);
    } else {
        imlib_max(arg_img, NULL, NULL,
                  py_helper_keyword_color(arg_img, n_args, args, 1, NULL, 0),
                  arg_msk);
    }

    fb_alloc_free_till_mark();

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_max_obj, 2, py_image_max);

STATIC mp_obj_t py_image_difference(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 2, kw_args);

    fb_alloc_mark();

    if (MP_OBJ_IS_STR(args[1])) {
        imlib_difference(arg_img, mp_obj_str_get_str(args[1]), NULL, 0, arg_msk);
    } else if (MP_OBJ_IS_TYPE(args[1], &py_image_type)) {
        imlib_difference(arg_img, NULL, py_helper_arg_to_image_mutable(args[1]), 0, arg_msk);
    } else {
        imlib_difference(arg_img, NULL, NULL,
                         py_helper_keyword_color(arg_img, n_args, args, 1, NULL, 0),
                         arg_msk);
    }

    fb_alloc_free_till_mark();

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_difference_obj, 2, py_image_difference);

STATIC mp_obj_t py_image_blend(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    float arg_alpha =
        py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_alpha), 128) / 256.0f;
    PY_ASSERT_TRUE_MSG((0 <= arg_alpha) && (arg_alpha <= 1), "Error: 0 <= alpha <= 256!");
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 3, kw_args);

    fb_alloc_mark();

    if (MP_OBJ_IS_STR(args[1])) {
        imlib_blend(arg_img, mp_obj_str_get_str(args[1]), NULL, 0, arg_alpha, arg_msk);
    } else if (MP_OBJ_IS_TYPE(args[1], &py_image_type)) {
        imlib_blend(arg_img, NULL, py_helper_arg_to_image_mutable(args[1]), 0, arg_alpha, arg_msk);
    } else {
        imlib_blend(arg_img, NULL, NULL,
                    py_helper_keyword_color(arg_img, n_args, args, 1, NULL, 0),
                    arg_alpha, arg_msk);
    }

    fb_alloc_free_till_mark();

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_blend_obj, 2, py_image_blend);
#endif//IMLIB_ENABLE_MATH_OPS

#if defined(IMLIB_ENABLE_MATH_OPS) && defined (IMLIB_ENABLE_BINARY_OPS)
STATIC mp_obj_t py_image_top_hat(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    int arg_ksize =
        py_helper_arg_to_ksize(args[1]);
    int arg_threshold =
        py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), 0);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 3, kw_args);

    fb_alloc_mark();
    imlib_top_hat(py_helper_arg_to_image_mutable(args[0]), arg_ksize, arg_threshold, arg_msk);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_top_hat_obj, 2, py_image_top_hat);

STATIC mp_obj_t py_image_black_hat(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    int arg_ksize =
        py_helper_arg_to_ksize(args[1]);
    int arg_threshold =
        py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), 0);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 3, kw_args);

    fb_alloc_mark();
    imlib_black_hat(py_helper_arg_to_image_mutable(args[0]), arg_ksize, arg_threshold, arg_msk);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_black_hat_obj, 2, py_image_black_hat);
#endif // defined(IMLIB_ENABLE_MATH_OPS) && defined (IMLIB_ENABLE_BINARY_OPS)

////////////////////
// Filtering Methods
////////////////////

static mp_obj_t py_image_histeq(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    bool arg_adaptive =
        py_helper_keyword_int(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_adaptive), false);
    float arg_clip_limit =
        py_helper_keyword_float(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_clip_limit), -1);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 3, kw_args);

    fb_alloc_mark();
    if (arg_adaptive) imlib_clahe_histeq(arg_img, arg_clip_limit, arg_msk); else imlib_histeq(arg_img, arg_msk);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_histeq_obj, 1, py_image_histeq);

#ifdef IMLIB_ENABLE_MEAN
STATIC mp_obj_t py_image_mean(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    int arg_ksize =
        py_helper_arg_to_ksize(args[1]);
    bool arg_threshold =
        py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), false);
    int arg_offset =
        py_helper_keyword_int(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_offset), 0);
    bool arg_invert =
        py_helper_keyword_int(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_invert), false);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 5, kw_args);

    fb_alloc_mark();
    imlib_mean_filter(arg_img, arg_ksize, arg_threshold, arg_offset, arg_invert, arg_msk);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_mean_obj, 2, py_image_mean);
#endif // IMLIB_ENABLE_MEAN

#ifdef IMLIB_ENABLE_MEDIAN
STATIC mp_obj_t py_image_median(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    int arg_ksize =
        py_helper_arg_to_ksize(args[1]);
    float arg_percentile =
        py_helper_keyword_float(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_percentile), 0.5f);
    PY_ASSERT_TRUE_MSG((0 <= arg_percentile) && (arg_percentile <= 1), "Error: 0 <= percentile <= 1!");
    bool arg_threshold =
        py_helper_keyword_int(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), false);
    int arg_offset =
        py_helper_keyword_int(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_offset), 0);
    bool arg_invert =
        py_helper_keyword_int(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_invert), false);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 6, kw_args);

    fb_alloc_mark();
    imlib_median_filter(arg_img, arg_ksize, arg_percentile, arg_threshold, arg_offset, arg_invert, arg_msk);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_median_obj, 2, py_image_median);
#endif // IMLIB_ENABLE_MEDIAN

#ifdef IMLIB_ENABLE_MODE
STATIC mp_obj_t py_image_mode(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    int arg_ksize =
        py_helper_arg_to_ksize(args[1]);
    bool arg_threshold =
        py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), false);
    int arg_offset =
        py_helper_keyword_int(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_offset), 0);
    bool arg_invert =
        py_helper_keyword_int(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_invert), false);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 5, kw_args);

    fb_alloc_mark();
    imlib_mode_filter(arg_img, arg_ksize, arg_threshold, arg_offset, arg_invert, arg_msk);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_mode_obj, 2, py_image_mode);
#endif // IMLIB_ENABLE_MODE

#ifdef IMLIB_ENABLE_MIDPOINT
STATIC mp_obj_t py_image_midpoint(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    int arg_ksize =
        py_helper_arg_to_ksize(args[1]);
    float arg_bias =
        py_helper_keyword_float(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_bias), 0.5f);
    PY_ASSERT_TRUE_MSG((0 <= arg_bias) && (arg_bias <= 1), "Error: 0 <= bias <= 1!");
    bool arg_threshold =
        py_helper_keyword_int(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), false);
    int arg_offset =
        py_helper_keyword_int(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_offset), 0);
    bool arg_invert =
        py_helper_keyword_int(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_invert), false);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 6, kw_args);

    fb_alloc_mark();
    imlib_midpoint_filter(arg_img, arg_ksize, arg_bias, arg_threshold, arg_offset, arg_invert, arg_msk);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_midpoint_obj, 2, py_image_midpoint);
#endif // IMLIB_ENABLE_MIDPOINT

#ifdef IMLIB_ENABLE_MORPH
STATIC mp_obj_t py_image_morph(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    int arg_ksize =
        py_helper_arg_to_ksize(args[1]);

    int n = py_helper_ksize_to_n(arg_ksize);

    mp_obj_t *krn;
    mp_obj_get_array_fixed_n(args[2], n, &krn);

    fb_alloc_mark();

    int *arg_krn = fb_alloc(n * sizeof(int), FB_ALLOC_NO_HINT);
    int arg_m = 0;

    for (int i = 0; i < n; i++) {
        arg_krn[i] = mp_obj_get_int(krn[i]);
        arg_m += arg_krn[i];
    }

    if (arg_m == 0) {
        arg_m = 1;
    }

    float arg_mul =
        py_helper_keyword_float(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_mul), 1.0f / arg_m);
    float arg_add =
        py_helper_keyword_float(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_add), 0.0f);
    bool arg_threshold =
        py_helper_keyword_int(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), false);
    int arg_offset =
        py_helper_keyword_int(n_args, args, 6, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_offset), 0);
    bool arg_invert =
        py_helper_keyword_int(n_args, args, 7, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_invert), false);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 8, kw_args);

    imlib_morph(arg_img, arg_ksize, arg_krn, arg_mul, arg_add, arg_threshold, arg_offset, arg_invert, arg_msk);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_morph_obj, 3, py_image_morph);
#endif //IMLIB_ENABLE_MORPH

#ifdef IMLIB_ENABLE_GAUSSIAN
STATIC mp_obj_t py_image_gaussian(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    int arg_ksize =
        py_helper_arg_to_ksize(args[1]);

    int k_2 = arg_ksize * 2;
    int n = k_2 + 1;

    fb_alloc_mark();

    int *pascal = fb_alloc(n * sizeof(int), FB_ALLOC_NO_HINT);
    pascal[0] = 1;

    for (int i = 0; i < k_2; i++) { // Compute a row of pascal's triangle.
        pascal[i + 1] = (pascal[i] * (k_2 - i)) / (i + 1);
    }

    int *arg_krn = fb_alloc(n * n * sizeof(int), FB_ALLOC_NO_HINT);
    int arg_m = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int temp = pascal[i] * pascal[j];
            arg_krn[(i * n) + j] = temp;
            arg_m += temp;
        }
    }

    if (py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_unsharp), false)) {
        arg_krn[((n/2)*n)+(n/2)] -= arg_m * 2;
        arg_m = -arg_m;
    }

    float arg_mul =
        py_helper_keyword_float(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_mul), 1.0f / arg_m);
    float arg_add =
        py_helper_keyword_float(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_add), 0.0f);
    bool arg_threshold =
        py_helper_keyword_int(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), false);
    int arg_offset =
        py_helper_keyword_int(n_args, args, 6, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_offset), 0);
    bool arg_invert =
        py_helper_keyword_int(n_args, args, 7, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_invert), false);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 8, kw_args);

    imlib_morph(arg_img, arg_ksize, arg_krn, arg_mul, arg_add, arg_threshold, arg_offset, arg_invert, arg_msk);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_gaussian_obj, 2, py_image_gaussian);
#endif // IMLIB_ENABLE_GAUSSIAN

#ifdef IMLIB_ENABLE_LAPLACIAN
STATIC mp_obj_t py_image_laplacian(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    int arg_ksize =
        py_helper_arg_to_ksize(args[1]);

    int k_2 = arg_ksize * 2;
    int n = k_2 + 1;

    fb_alloc_mark();

    int *pascal = fb_alloc(n * sizeof(int), FB_ALLOC_NO_HINT);
    pascal[0] = 1;

    for (int i = 0; i < k_2; i++) { // Compute a row of pascal's triangle.
        pascal[i + 1] = (pascal[i] * (k_2 - i)) / (i + 1);
    }

    int *arg_krn = fb_alloc(n * n * sizeof(int), FB_ALLOC_NO_HINT);
    int arg_m = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int temp = pascal[i] * pascal[j];
            arg_krn[(i * n) + j] = -temp;
            arg_m += temp;
        }
    }

    arg_krn[((n/2)*n)+(n/2)] += arg_m;
    arg_m = arg_krn[((n/2)*n)+(n/2)];

    if (py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_sharpen), false)) {
        arg_krn[((n/2)*n)+(n/2)] += arg_m;
    }

    float arg_mul =
        py_helper_keyword_float(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_mul), 1.0f / arg_m);
    float arg_add =
        py_helper_keyword_float(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_add), 0.0f);
    bool arg_threshold =
        py_helper_keyword_int(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), false);
    int arg_offset =
        py_helper_keyword_int(n_args, args, 6, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_offset), 0);
    bool arg_invert =
        py_helper_keyword_int(n_args, args, 7, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_invert), false);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 8, kw_args);

    imlib_morph(arg_img, arg_ksize, arg_krn, arg_mul, arg_add, arg_threshold, arg_offset, arg_invert, arg_msk);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_laplacian_obj, 2, py_image_laplacian);
#endif // IMLIB_ENABLE_LAPLACIAN

#ifdef IMLIB_ENABLE_BILATERAL
STATIC mp_obj_t py_image_bilateral(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    int arg_ksize =
        py_helper_arg_to_ksize(args[1]);
    float arg_color_sigma =
        py_helper_keyword_float(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_color_sigma), 0.1);
    float arg_space_sigma =
        py_helper_keyword_float(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_space_sigma), 1);
    bool arg_threshold =
        py_helper_keyword_int(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), false);
    int arg_offset =
        py_helper_keyword_int(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_offset), 0);
    bool arg_invert =
        py_helper_keyword_int(n_args, args, 6, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_invert), false);
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 7, kw_args);

    fb_alloc_mark();
    imlib_bilateral_filter(arg_img, arg_ksize, arg_color_sigma, arg_space_sigma, arg_threshold, arg_offset, arg_invert, arg_msk);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_bilateral_obj, 2, py_image_bilateral);
#endif // IMLIB_ENABLE_BILATERAL

#ifdef IMLIB_ENABLE_CARTOON
STATIC mp_obj_t py_image_cartoon(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    float arg_seed_threshold =
        py_helper_keyword_float(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_seed_threshold), 0.05);
    PY_ASSERT_TRUE_MSG((0.0f <= arg_seed_threshold) && (arg_seed_threshold <= 1.0f),
                       "Error: 0.0 <= seed_threshold <= 1.0!");
    float arg_floating_threshold =
        py_helper_keyword_float(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_floating_threshold), 0.05);
    PY_ASSERT_TRUE_MSG((0.0f <= arg_floating_threshold) && (arg_floating_threshold <= 1.0f),
                       "Error: 0.0 <= floating_threshold <= 1.0!");
    image_t *arg_msk =
        py_helper_keyword_to_image_mutable_mask(n_args, args, 3, kw_args);

    fb_alloc_mark();
    imlib_cartoon_filter(arg_img, arg_seed_threshold, arg_floating_threshold, arg_msk);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_cartoon_obj, 1, py_image_cartoon);
#endif // IMLIB_ENABLE_CARTOON

////////////////////
// Geometric Methods
////////////////////

#ifdef IMLIB_ENABLE_LINPOLAR
static mp_obj_t py_image_linpolar(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    PY_ASSERT_FALSE_MSG(arg_img->w % 2, "Width must be even!");
    PY_ASSERT_FALSE_MSG(arg_img->h % 2, "Height must be even!");
    bool arg_reverse =
        py_helper_keyword_int(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_reverse), false);

    fb_alloc_mark();
    imlib_logpolar(arg_img, true, arg_reverse);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_linpolar_obj, 1, py_image_linpolar);
#endif // IMLIB_ENABLE_LINPOLAR

#ifdef IMLIB_ENABLE_LOGPOLAR
static mp_obj_t py_image_logpolar(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    PY_ASSERT_FALSE_MSG(arg_img->w % 2, "Width must be even!");
    PY_ASSERT_FALSE_MSG(arg_img->h % 2, "Height must be even!");
    bool arg_reverse =
        py_helper_keyword_int(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_reverse), false);

    fb_alloc_mark();
    imlib_logpolar(arg_img, false, arg_reverse);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_logpolar_obj, 1, py_image_logpolar);
#endif // IMLIB_ENABLE_LOGPOLAR

#ifdef IMLIB_ENABLE_LENS_CORR
STATIC mp_obj_t py_image_lens_corr(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    PY_ASSERT_FALSE_MSG(arg_img->w % 2, "Width must be even!");
    PY_ASSERT_FALSE_MSG(arg_img->h % 2, "Height must be even!");
    float arg_strength =
        py_helper_keyword_float(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_strength), 1.8f);
    PY_ASSERT_TRUE_MSG(arg_strength > 0.0f, "Strength must be > 0!");
    float arg_zoom =
        py_helper_keyword_float(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_zoom), 1.0f);
    PY_ASSERT_TRUE_MSG(arg_zoom > 0.0f, "Zoom must be > 0!");

    float arg_x_corr =
        py_helper_keyword_float(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_corr), 0.0f);
    float arg_y_corr =
        py_helper_keyword_float(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_corr), 0.0f);

    fb_alloc_mark();
    imlib_lens_corr(arg_img, arg_strength, arg_zoom, arg_x_corr, arg_y_corr);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_lens_corr_obj, 1, py_image_lens_corr);
#endif // IMLIB_ENABLE_LENS_CORR

#ifdef IMLIB_ENABLE_ROTATION_CORR
STATIC mp_obj_t py_image_rotation_corr(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img =
        py_helper_arg_to_image_mutable(args[0]);
    float arg_x_rotation =
        IM_DEG2RAD(py_helper_keyword_float(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_rotation), 0.0f));
    float arg_y_rotation =
        IM_DEG2RAD(py_helper_keyword_float(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_rotation), 0.0f));
    float arg_z_rotation =
        IM_DEG2RAD(py_helper_keyword_float(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_z_rotation), 0.0f));
    float arg_x_translation =
        py_helper_keyword_float(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_translation), 0.0f);
    float arg_y_translation =
        py_helper_keyword_float(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_translation), 0.0f);
    float arg_zoom =
        py_helper_keyword_float(n_args, args, 6, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_zoom), 1.0f);
    PY_ASSERT_TRUE_MSG(arg_zoom > 0.0f, "Zoom must be > 0!");
    float arg_fov =
        IM_DEG2RAD(py_helper_keyword_float(n_args, args, 7, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_fov), 60.0f));
    PY_ASSERT_TRUE_MSG((0.0f < arg_fov) && (arg_fov < 180.0f), "FOV must be > 0 and < 180!");
    float *arg_corners = py_helper_keyword_corner_array(n_args, args, 8, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_corners));

    fb_alloc_mark();
    imlib_rotation_corr(arg_img,
                        arg_x_rotation, arg_y_rotation, arg_z_rotation,
                        arg_x_translation, arg_y_translation,
                        arg_zoom, arg_fov, arg_corners);
    fb_alloc_free_till_mark();
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_rotation_corr_obj, 1, py_image_rotation_corr);
#endif // IMLIB_ENABLE_ROTATION_CORR

//////////////
// Get Methods
//////////////

#ifdef IMLIB_ENABLE_GET_SIMILARITY
// Similarity Object //
#define py_similarity_obj_size 4
typedef struct py_similarity_obj {
    mp_obj_base_t base;
    mp_obj_t avg, std, min, max;
} py_similarity_obj_t;

static void py_similarity_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_similarity_obj_t *self = self_in;
    mp_printf(print,
              "{\"mean\":%f, \"stdev\":%f, \"min\":%f, \"max\":%f}",
              (double) mp_obj_get_float(self->avg),
              (double) mp_obj_get_float(self->std),
              (double) mp_obj_get_float(self->min),
              (double) mp_obj_get_float(self->max));
}

static mp_obj_t py_similarity_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    if (value == MP_OBJ_SENTINEL) { // load
        py_similarity_obj_t *self = self_in;
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(py_similarity_obj_size, index, &slice)) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
            }
            mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
            mp_seq_copy(result->items, &(self->avg) + slice.start, result->len, mp_obj_t);
            return result;
        }
        switch (mp_get_index(self->base.type, py_similarity_obj_size, index, false)) {
            case 0: return self->avg;
            case 1: return self->std;
            case 2: return self->min;
            case 3: return self->max;
        }
    }
    return MP_OBJ_NULL; // op not supported
}

mp_obj_t py_similarity_mean(mp_obj_t self_in) { return ((py_similarity_obj_t *) self_in)->avg; }
mp_obj_t py_similarity_stdev(mp_obj_t self_in) { return ((py_similarity_obj_t *) self_in)->std; }
mp_obj_t py_similarity_min(mp_obj_t self_in) { return ((py_similarity_obj_t *) self_in)->min; }
mp_obj_t py_similarity_max(mp_obj_t self_in) { return ((py_similarity_obj_t *) self_in)->max; }

STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_similarity_mean_obj, py_similarity_mean);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_similarity_stdev_obj, py_similarity_stdev);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_similarity_min_obj, py_similarity_min);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_similarity_max_obj, py_similarity_max);

STATIC const mp_rom_map_elem_t py_similarity_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_mean), MP_ROM_PTR(&py_similarity_mean_obj) },
    { MP_ROM_QSTR(MP_QSTR_stdev), MP_ROM_PTR(&py_similarity_stdev_obj) },
    { MP_ROM_QSTR(MP_QSTR_min), MP_ROM_PTR(&py_similarity_min_obj) },
    { MP_ROM_QSTR(MP_QSTR_max), MP_ROM_PTR(&py_similarity_max_obj) }
};

STATIC MP_DEFINE_CONST_DICT(py_similarity_locals_dict, py_similarity_locals_dict_table);

static const mp_obj_type_t py_similarity_type = {
    { &mp_type_type },
    .name  = MP_QSTR_similarity,
    .print = py_similarity_print,
    .subscr = py_similarity_subscr,
    .locals_dict = (mp_obj_t) &py_similarity_locals_dict
};

static mp_obj_t py_image_get_similarity(mp_obj_t img_obj, mp_obj_t other_obj)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(img_obj);
    float avg, std, min, max;

    fb_alloc_mark();

    if (MP_OBJ_IS_STR(other_obj)) {
        imlib_get_similarity(arg_img, mp_obj_str_get_str(other_obj), NULL, 0, &avg, &std, &min, &max);
    } else if (MP_OBJ_IS_TYPE(other_obj, &py_image_type)) {
        imlib_get_similarity(arg_img, NULL, py_helper_arg_to_image_mutable(other_obj), 0, &avg, &std, &min, &max);
    } else {
        imlib_get_similarity(arg_img, NULL, NULL,
                             py_helper_keyword_color(arg_img, 1, &other_obj, 0, NULL, 0),
                             &avg, &std, &min, &max);
    }

    fb_alloc_free_till_mark();

    py_similarity_obj_t *o = m_new_obj(py_similarity_obj_t);
    o->base.type = &py_similarity_type;
    o->avg = mp_obj_new_float(avg);
    o->std = mp_obj_new_float(std);
    o->min = mp_obj_new_float(min);
    o->max = mp_obj_new_float(max);
    return o;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(py_image_get_similarity_obj, py_image_get_similarity);
#endif // IMLIB_ENABLE_GET_SIMILARITY

// Statistics Object //
#define py_statistics_obj_size 24
typedef struct py_statistics_obj {
    mp_obj_base_t base;
    image_bpp_t bpp;
    mp_obj_t LMean, LMedian, LMode, LSTDev, LMin, LMax, LLQ, LUQ,
        AMean, AMedian, AMode, ASTDev, AMin, AMax, ALQ, AUQ,
        BMean, BMedian, BMode, BSTDev, BMin, BMax, BLQ, BUQ;
} py_statistics_obj_t;

static void py_statistics_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_statistics_obj_t *self = self_in;
    switch(self->bpp) {
        case IMAGE_BPP_BINARY: {
            mp_printf(print, "{\"mean\":%d, \"median\":%d, \"mode\":%d, \"stdev\":%d, \"min\":%d, \"max\":%d, \"lq\":%d, \"uq\":%d}",
                      mp_obj_get_int(self->LMean),
                      mp_obj_get_int(self->LMedian),
                      mp_obj_get_int(self->LMode),
                      mp_obj_get_int(self->LSTDev),
                      mp_obj_get_int(self->LMin),
                      mp_obj_get_int(self->LMax),
                      mp_obj_get_int(self->LLQ),
                      mp_obj_get_int(self->LUQ));
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            mp_printf(print, "{\"mean\":%d, \"median\":%d, \"mode\":%d, \"stdev\":%d, \"min\":%d, \"max\":%d, \"lq\":%d, \"uq\":%d}",
                      mp_obj_get_int(self->LMean),
                      mp_obj_get_int(self->LMedian),
                      mp_obj_get_int(self->LMode),
                      mp_obj_get_int(self->LSTDev),
                      mp_obj_get_int(self->LMin),
                      mp_obj_get_int(self->LMax),
                      mp_obj_get_int(self->LLQ),
                      mp_obj_get_int(self->LUQ));
            break;
        }
        case IMAGE_BPP_RGB565: {
            mp_printf(print, "{\"l_mean\":%d, \"l_median\":%d, \"l_mode\":%d, \"l_stdev\":%d, \"l_min\":%d, \"l_max\":%d, \"l_lq\":%d, \"l_uq\":%d,"
                             " \"a_mean\":%d, \"a_median\":%d, \"a_mode\":%d, \"a_stdev\":%d, \"a_min\":%d, \"a_max\":%d, \"a_lq\":%d, \"a_uq\":%d,"
                             " \"b_mean\":%d, \"b_median\":%d, \"b_mode\":%d, \"b_stdev\":%d, \"b_min\":%d, \"b_max\":%d, \"b_lq\":%d, \"b_uq\":%d}",
                      mp_obj_get_int(self->LMean),
                      mp_obj_get_int(self->LMedian),
                      mp_obj_get_int(self->LMode),
                      mp_obj_get_int(self->LSTDev),
                      mp_obj_get_int(self->LMin),
                      mp_obj_get_int(self->LMax),
                      mp_obj_get_int(self->LLQ),
                      mp_obj_get_int(self->LUQ),
                      mp_obj_get_int(self->AMean),
                      mp_obj_get_int(self->AMedian),
                      mp_obj_get_int(self->AMode),
                      mp_obj_get_int(self->ASTDev),
                      mp_obj_get_int(self->AMin),
                      mp_obj_get_int(self->AMax),
                      mp_obj_get_int(self->ALQ),
                      mp_obj_get_int(self->AUQ),
                      mp_obj_get_int(self->BMean),
                      mp_obj_get_int(self->BMedian),
                      mp_obj_get_int(self->BMode),
                      mp_obj_get_int(self->BSTDev),
                      mp_obj_get_int(self->BMin),
                      mp_obj_get_int(self->BMax),
                      mp_obj_get_int(self->BLQ),
                      mp_obj_get_int(self->BUQ));
            break;
        }
        default: {
            mp_printf(print, "{}");
            break;
        }
    }
}

static mp_obj_t py_statistics_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    if (value == MP_OBJ_SENTINEL) { // load
        py_statistics_obj_t *self = self_in;
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(py_statistics_obj_size, index, &slice)) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
            }
            mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
            mp_seq_copy(result->items, &(self->LMean) + slice.start, result->len, mp_obj_t);
            return result;
        }
        switch (mp_get_index(self->base.type, py_statistics_obj_size, index, false)) {
            case 0: return self->LMean;
            case 1: return self->LMedian;
            case 2: return self->LMode;
            case 3: return self->LSTDev;
            case 4: return self->LMin;
            case 5: return self->LMax;
            case 6: return self->LLQ;
            case 7: return self->LUQ;
            case 8: return self->AMean;
            case 9: return self->AMedian;
            case 10: return self->AMode;
            case 11: return self->ASTDev;
            case 12: return self->AMin;
            case 13: return self->AMax;
            case 14: return self->ALQ;
            case 15: return self->AUQ;
            case 16: return self->BMean;
            case 17: return self->BMedian;
            case 18: return self->BMode;
            case 19: return self->BSTDev;
            case 20: return self->BMin;
            case 21: return self->BMax;
            case 22: return self->BLQ;
            case 23: return self->BUQ;
        }
    }
    return MP_OBJ_NULL; // op not supported
}

mp_obj_t py_statistics_mean(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->LMean; }
mp_obj_t py_statistics_median(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->LMedian; }
mp_obj_t py_statistics_mode(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->LMode; }
mp_obj_t py_statistics_stdev(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->LSTDev; }
mp_obj_t py_statistics_min(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->LMin; }
mp_obj_t py_statistics_max(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->LMax; }
mp_obj_t py_statistics_lq(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->LLQ; }
mp_obj_t py_statistics_uq(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->LUQ; }
mp_obj_t py_statistics_l_mean(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->LMean; }
mp_obj_t py_statistics_l_median(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->LMedian; }
mp_obj_t py_statistics_l_mode(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->LMode; }
mp_obj_t py_statistics_l_stdev(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->LSTDev; }
mp_obj_t py_statistics_l_min(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->LMin; }
mp_obj_t py_statistics_l_max(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->LMax; }
mp_obj_t py_statistics_l_lq(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->LLQ; }
mp_obj_t py_statistics_l_uq(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->LUQ; }
mp_obj_t py_statistics_a_mean(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->AMean; }
mp_obj_t py_statistics_a_median(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->AMedian; }
mp_obj_t py_statistics_a_mode(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->AMode; }
mp_obj_t py_statistics_a_stdev(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->ASTDev; }
mp_obj_t py_statistics_a_min(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->AMin; }
mp_obj_t py_statistics_a_max(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->AMax; }
mp_obj_t py_statistics_a_lq(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->ALQ; }
mp_obj_t py_statistics_a_uq(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->AUQ; }
mp_obj_t py_statistics_b_mean(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->BMean; }
mp_obj_t py_statistics_b_median(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->BMedian; }
mp_obj_t py_statistics_b_mode(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->BMode; }
mp_obj_t py_statistics_b_stdev(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->BSTDev; }
mp_obj_t py_statistics_b_min(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->BMin; }
mp_obj_t py_statistics_b_max(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->BMax; }
mp_obj_t py_statistics_b_lq(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->BLQ; }
mp_obj_t py_statistics_b_uq(mp_obj_t self_in) { return ((py_statistics_obj_t *) self_in)->BUQ; }

STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_mean_obj, py_statistics_mean);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_median_obj, py_statistics_median);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_mode_obj, py_statistics_mode);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_stdev_obj, py_statistics_stdev);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_min_obj, py_statistics_min);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_max_obj, py_statistics_max);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_lq_obj, py_statistics_lq);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_uq_obj, py_statistics_uq);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_l_mean_obj, py_statistics_l_mean);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_l_median_obj, py_statistics_l_median);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_l_mode_obj, py_statistics_l_mode);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_l_stdev_obj, py_statistics_l_stdev);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_l_min_obj, py_statistics_l_min);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_l_max_obj, py_statistics_l_max);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_l_lq_obj, py_statistics_l_lq);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_l_uq_obj, py_statistics_l_uq);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_a_mean_obj, py_statistics_a_mean);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_a_median_obj, py_statistics_a_median);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_a_mode_obj, py_statistics_a_mode);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_a_stdev_obj, py_statistics_a_stdev);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_a_min_obj, py_statistics_a_min);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_a_max_obj, py_statistics_a_max);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_a_lq_obj, py_statistics_a_lq);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_a_uq_obj, py_statistics_a_uq);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_b_mean_obj, py_statistics_b_mean);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_b_median_obj, py_statistics_b_median);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_b_mode_obj, py_statistics_b_mode);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_b_stdev_obj, py_statistics_b_stdev);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_b_min_obj, py_statistics_b_min);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_b_max_obj, py_statistics_b_max);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_b_lq_obj, py_statistics_b_lq);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_statistics_b_uq_obj, py_statistics_b_uq);

STATIC const mp_rom_map_elem_t py_statistics_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_mean), MP_ROM_PTR(&py_statistics_mean_obj) },
    { MP_ROM_QSTR(MP_QSTR_median), MP_ROM_PTR(&py_statistics_median_obj) },
    { MP_ROM_QSTR(MP_QSTR_mode), MP_ROM_PTR(&py_statistics_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_stdev), MP_ROM_PTR(&py_statistics_stdev_obj) },
    { MP_ROM_QSTR(MP_QSTR_min), MP_ROM_PTR(&py_statistics_min_obj) },
    { MP_ROM_QSTR(MP_QSTR_max), MP_ROM_PTR(&py_statistics_max_obj) },
    { MP_ROM_QSTR(MP_QSTR_lq), MP_ROM_PTR(&py_statistics_lq_obj) },
    { MP_ROM_QSTR(MP_QSTR_uq), MP_ROM_PTR(&py_statistics_uq_obj) },
    { MP_ROM_QSTR(MP_QSTR_l_mean), MP_ROM_PTR(&py_statistics_l_mean_obj) },
    { MP_ROM_QSTR(MP_QSTR_l_median), MP_ROM_PTR(&py_statistics_l_median_obj) },
    { MP_ROM_QSTR(MP_QSTR_l_mode), MP_ROM_PTR(&py_statistics_l_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_l_stdev), MP_ROM_PTR(&py_statistics_l_stdev_obj) },
    { MP_ROM_QSTR(MP_QSTR_l_min), MP_ROM_PTR(&py_statistics_l_min_obj) },
    { MP_ROM_QSTR(MP_QSTR_l_max), MP_ROM_PTR(&py_statistics_l_max_obj) },
    { MP_ROM_QSTR(MP_QSTR_l_lq), MP_ROM_PTR(&py_statistics_l_lq_obj) },
    { MP_ROM_QSTR(MP_QSTR_l_uq), MP_ROM_PTR(&py_statistics_l_uq_obj) },
    { MP_ROM_QSTR(MP_QSTR_a_mean), MP_ROM_PTR(&py_statistics_a_mean_obj) },
    { MP_ROM_QSTR(MP_QSTR_a_median), MP_ROM_PTR(&py_statistics_a_median_obj) },
    { MP_ROM_QSTR(MP_QSTR_a_mode), MP_ROM_PTR(&py_statistics_a_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_a_stdev), MP_ROM_PTR(&py_statistics_a_stdev_obj) },
    { MP_ROM_QSTR(MP_QSTR_a_min), MP_ROM_PTR(&py_statistics_a_min_obj) },
    { MP_ROM_QSTR(MP_QSTR_a_max), MP_ROM_PTR(&py_statistics_a_max_obj) },
    { MP_ROM_QSTR(MP_QSTR_a_lq), MP_ROM_PTR(&py_statistics_a_lq_obj) },
    { MP_ROM_QSTR(MP_QSTR_a_uq), MP_ROM_PTR(&py_statistics_a_uq_obj) },
    { MP_ROM_QSTR(MP_QSTR_b_mean), MP_ROM_PTR(&py_statistics_b_mean_obj) },
    { MP_ROM_QSTR(MP_QSTR_b_median), MP_ROM_PTR(&py_statistics_b_median_obj) },
    { MP_ROM_QSTR(MP_QSTR_b_mode), MP_ROM_PTR(&py_statistics_b_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_b_stdev), MP_ROM_PTR(&py_statistics_b_stdev_obj) },
    { MP_ROM_QSTR(MP_QSTR_b_min), MP_ROM_PTR(&py_statistics_b_min_obj) },
    { MP_ROM_QSTR(MP_QSTR_b_max), MP_ROM_PTR(&py_statistics_b_max_obj) },
    { MP_ROM_QSTR(MP_QSTR_b_lq), MP_ROM_PTR(&py_statistics_b_lq_obj) },
    { MP_ROM_QSTR(MP_QSTR_b_uq), MP_ROM_PTR(&py_statistics_b_uq_obj) }
};

STATIC MP_DEFINE_CONST_DICT(py_statistics_locals_dict, py_statistics_locals_dict_table);

static const mp_obj_type_t py_statistics_type = {
    { &mp_type_type },
    .name  = MP_QSTR_statistics,
    .print = py_statistics_print,
    .subscr = py_statistics_subscr,
    .locals_dict = (mp_obj_t) &py_statistics_locals_dict
};

// Percentile Object //
#define py_percentile_obj_size 3
typedef struct py_percentile_obj {
    mp_obj_base_t base;
    image_bpp_t bpp;
    mp_obj_t LValue, AValue, BValue;
} py_percentile_obj_t;

static void py_percentile_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_percentile_obj_t *self = self_in;
    switch(self->bpp) {
        case IMAGE_BPP_BINARY: {
            mp_printf(print, "{\"value\":%d}",
                      mp_obj_get_int(self->LValue));
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            mp_printf(print, "{\"value\":%d}",
                      mp_obj_get_int(self->LValue));
            break;
        }
        case IMAGE_BPP_RGB565: {
            mp_printf(print, "{\"l_value:%d\", \"a_value\":%d, \"b_value\":%d}",
                      mp_obj_get_int(self->LValue),
                      mp_obj_get_int(self->AValue),
                      mp_obj_get_int(self->BValue));
            break;
        }
        default: {
            mp_printf(print, "{}");
            break;
        }
    }
}

static mp_obj_t py_percentile_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    if (value == MP_OBJ_SENTINEL) { // load
        py_percentile_obj_t *self = self_in;
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(py_percentile_obj_size, index, &slice)) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
            }
            mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
            mp_seq_copy(result->items, &(self->LValue) + slice.start, result->len, mp_obj_t);
            return result;
        }
        switch (mp_get_index(self->base.type, py_percentile_obj_size, index, false)) {
            case 0: return self->LValue;
            case 1: return self->AValue;
            case 2: return self->BValue;
        }
    }
    return MP_OBJ_NULL; // op not supported
}

mp_obj_t py_percentile_value(mp_obj_t self_in) { return ((py_percentile_obj_t *) self_in)->LValue; }
mp_obj_t py_percentile_l_value(mp_obj_t self_in) { return ((py_percentile_obj_t *) self_in)->LValue; }
mp_obj_t py_percentile_a_value(mp_obj_t self_in) { return ((py_percentile_obj_t *) self_in)->AValue; }
mp_obj_t py_percentile_b_value(mp_obj_t self_in) { return ((py_percentile_obj_t *) self_in)->BValue; }

STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_percentile_value_obj, py_percentile_value);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_percentile_l_value_obj, py_percentile_l_value);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_percentile_a_value_obj, py_percentile_a_value);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_percentile_b_value_obj, py_percentile_b_value);

STATIC const mp_rom_map_elem_t py_percentile_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&py_percentile_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_l_value), MP_ROM_PTR(&py_percentile_l_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_a_value), MP_ROM_PTR(&py_percentile_a_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_b_value), MP_ROM_PTR(&py_percentile_b_value_obj) }
};

STATIC MP_DEFINE_CONST_DICT(py_percentile_locals_dict, py_percentile_locals_dict_table);

static const mp_obj_type_t py_percentile_type = {
    { &mp_type_type },
    .name  = MP_QSTR_percentile,
    .print = py_percentile_print,
    .subscr = py_percentile_subscr,
    .locals_dict = (mp_obj_t) &py_percentile_locals_dict
};

// Threshold Object //
#define py_threshold_obj_size 3
typedef struct py_threshold_obj {
    mp_obj_base_t base;
    image_bpp_t bpp;
    mp_obj_t LValue, AValue, BValue;
} py_threshold_obj_t;

static void py_threshold_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_threshold_obj_t *self = self_in;
    switch(self->bpp) {
        case IMAGE_BPP_BINARY: {
            mp_printf(print, "{\"value\":%d}",
                      mp_obj_get_int(self->LValue));
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            mp_printf(print, "{\"value\":%d}",
                      mp_obj_get_int(self->LValue));
            break;
        }
        case IMAGE_BPP_RGB565: {
            mp_printf(print, "{\"l_value\":%d, \"a_value\":%d, \"b_value\":%d}",
                      mp_obj_get_int(self->LValue),
                      mp_obj_get_int(self->AValue),
                      mp_obj_get_int(self->BValue));
            break;
        }
        default: {
            mp_printf(print, "{}");
            break;
        }
    }
}

static mp_obj_t py_threshold_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    if (value == MP_OBJ_SENTINEL) { // load
        py_threshold_obj_t *self = self_in;
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(py_threshold_obj_size, index, &slice)) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
            }
            mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
            mp_seq_copy(result->items, &(self->LValue) + slice.start, result->len, mp_obj_t);
            return result;
        }
        switch (mp_get_index(self->base.type, py_threshold_obj_size, index, false)) {
            case 0: return self->LValue;
            case 1: return self->AValue;
            case 2: return self->BValue;
        }
    }
    return MP_OBJ_NULL; // op not supported
}

mp_obj_t py_threshold_value(mp_obj_t self_in) { return ((py_threshold_obj_t *) self_in)->LValue; }
mp_obj_t py_threshold_l_value(mp_obj_t self_in) { return ((py_threshold_obj_t *) self_in)->LValue; }
mp_obj_t py_threshold_a_value(mp_obj_t self_in) { return ((py_threshold_obj_t *) self_in)->AValue; }
mp_obj_t py_threshold_b_value(mp_obj_t self_in) { return ((py_threshold_obj_t *) self_in)->BValue; }

STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_threshold_value_obj, py_threshold_value);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_threshold_l_value_obj, py_threshold_l_value);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_threshold_a_value_obj, py_threshold_a_value);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_threshold_b_value_obj, py_threshold_b_value);

STATIC const mp_rom_map_elem_t py_threshold_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&py_threshold_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_l_value), MP_ROM_PTR(&py_threshold_l_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_a_value), MP_ROM_PTR(&py_threshold_a_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_b_value), MP_ROM_PTR(&py_threshold_b_value_obj) }
};

STATIC MP_DEFINE_CONST_DICT(py_threshold_locals_dict, py_threshold_locals_dict_table);

static const mp_obj_type_t py_threshold_type = {
    { &mp_type_type },
    .name  = MP_QSTR_threshold,
    .print = py_threshold_print,
    .subscr = py_threshold_subscr,
    .locals_dict = (mp_obj_t) &py_threshold_locals_dict
};

// Histogram Object //
#define py_histogram_obj_size 3
typedef struct py_histogram_obj {
    mp_obj_base_t base;
    image_bpp_t bpp;
    mp_obj_t LBins, ABins, BBins;
} py_histogram_obj_t;

static void py_histogram_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_histogram_obj_t *self = self_in;
    switch(self->bpp) {
        case IMAGE_BPP_BINARY: {
            mp_printf(print, "{\"bins\":");
            mp_obj_print_helper(print, self->LBins, kind);
            mp_printf(print, "}");
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            mp_printf(print, "{\"bins\":");
            mp_obj_print_helper(print, self->LBins, kind);
            mp_printf(print, "}");
            break;
        }
        case IMAGE_BPP_RGB565: {
            mp_printf(print, "{\"l_bins\":");
            mp_obj_print_helper(print, self->LBins, kind);
            mp_printf(print, ", \"a_bins\":");
            mp_obj_print_helper(print, self->ABins, kind);
            mp_printf(print, ", \"b_bins\":");
            mp_obj_print_helper(print, self->BBins, kind);
            mp_printf(print, "}");
            break;
        }
        default: {
            mp_printf(print, "{}");
            break;
        }
    }
}

static mp_obj_t py_histogram_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    if (value == MP_OBJ_SENTINEL) { // load
        py_histogram_obj_t *self = self_in;
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(py_histogram_obj_size, index, &slice)) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
            }
            mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
            mp_seq_copy(result->items, &(self->LBins) + slice.start, result->len, mp_obj_t);
            return result;
        }
        switch (mp_get_index(self->base.type, py_histogram_obj_size, index, false)) {
            case 0: return self->LBins;
            case 1: return self->ABins;
            case 2: return self->BBins;
        }
    }
    return MP_OBJ_NULL; // op not supported
}

mp_obj_t py_histogram_bins(mp_obj_t self_in) { return ((py_histogram_obj_t *) self_in)->LBins; }
mp_obj_t py_histogram_l_bins(mp_obj_t self_in) { return ((py_histogram_obj_t *) self_in)->LBins; }
mp_obj_t py_histogram_a_bins(mp_obj_t self_in) { return ((py_histogram_obj_t *) self_in)->ABins; }
mp_obj_t py_histogram_b_bins(mp_obj_t self_in) { return ((py_histogram_obj_t *) self_in)->BBins; }

mp_obj_t py_histogram_get_percentile(mp_obj_t self_in, mp_obj_t percentile)
{
    histogram_t hist;
    hist.LBinCount = ((mp_obj_list_t *) ((py_histogram_obj_t *) self_in)->LBins)->len;
    hist.ABinCount = ((mp_obj_list_t *) ((py_histogram_obj_t *) self_in)->ABins)->len;
    hist.BBinCount = ((mp_obj_list_t *) ((py_histogram_obj_t *) self_in)->BBins)->len;
    fb_alloc_mark();
    hist.LBins = fb_alloc(hist.LBinCount * sizeof(float), FB_ALLOC_NO_HINT);
    hist.ABins = fb_alloc(hist.ABinCount * sizeof(float), FB_ALLOC_NO_HINT);
    hist.BBins = fb_alloc(hist.BBinCount * sizeof(float), FB_ALLOC_NO_HINT);

    for (int i = 0; i < hist.LBinCount; i++) {
        hist.LBins[i] = mp_obj_get_float(((mp_obj_list_t *) ((py_histogram_obj_t *) self_in)->LBins)->items[i]);
    }

    for (int i = 0; i < hist.ABinCount; i++) {
        hist.ABins[i] = mp_obj_get_float(((mp_obj_list_t *) ((py_histogram_obj_t *) self_in)->ABins)->items[i]);
    }

    for (int i = 0; i < hist.BBinCount; i++) {
        hist.BBins[i] = mp_obj_get_float(((mp_obj_list_t *) ((py_histogram_obj_t *) self_in)->BBins)->items[i]);
    }

    percentile_t p;
    imlib_get_percentile(&p, ((py_histogram_obj_t *) self_in)->bpp, &hist, mp_obj_get_float(percentile));
    fb_alloc_free_till_mark();

    py_percentile_obj_t *o = m_new_obj(py_percentile_obj_t);
    o->base.type = &py_percentile_type;
    o->bpp = ((py_histogram_obj_t *) self_in)->bpp;

    o->LValue = mp_obj_new_int(p.LValue);
    o->AValue = mp_obj_new_int(p.AValue);
    o->BValue = mp_obj_new_int(p.BValue);

    return o;
}

mp_obj_t py_histogram_get_threshold(mp_obj_t self_in)
{
    histogram_t hist;
    hist.LBinCount = ((mp_obj_list_t *) ((py_histogram_obj_t *) self_in)->LBins)->len;
    hist.ABinCount = ((mp_obj_list_t *) ((py_histogram_obj_t *) self_in)->ABins)->len;
    hist.BBinCount = ((mp_obj_list_t *) ((py_histogram_obj_t *) self_in)->BBins)->len;
    fb_alloc_mark();
    hist.LBins = fb_alloc(hist.LBinCount * sizeof(float), FB_ALLOC_NO_HINT);
    hist.ABins = fb_alloc(hist.ABinCount * sizeof(float), FB_ALLOC_NO_HINT);
    hist.BBins = fb_alloc(hist.BBinCount * sizeof(float), FB_ALLOC_NO_HINT);

    for (int i = 0; i < hist.LBinCount; i++) {
        hist.LBins[i] = mp_obj_get_float(((mp_obj_list_t *) ((py_histogram_obj_t *) self_in)->LBins)->items[i]);
    }

    for (int i = 0; i < hist.ABinCount; i++) {
        hist.ABins[i] = mp_obj_get_float(((mp_obj_list_t *) ((py_histogram_obj_t *) self_in)->ABins)->items[i]);
    }

    for (int i = 0; i < hist.BBinCount; i++) {
        hist.BBins[i] = mp_obj_get_float(((mp_obj_list_t *) ((py_histogram_obj_t *) self_in)->BBins)->items[i]);
    }

    threshold_t t;
    imlib_get_threshold(&t, ((py_histogram_obj_t *) self_in)->bpp, &hist);
    fb_alloc_free_till_mark();

    py_threshold_obj_t *o = m_new_obj(py_threshold_obj_t);
    o->base.type = &py_threshold_type;
    o->bpp = ((py_threshold_obj_t *) self_in)->bpp;

    o->LValue = mp_obj_new_int(t.LValue);
    o->AValue = mp_obj_new_int(t.AValue);
    o->BValue = mp_obj_new_int(t.BValue);

    return o;
}

mp_obj_t py_histogram_get_statistics(mp_obj_t self_in)
{
    histogram_t hist;
    hist.LBinCount = ((mp_obj_list_t *) ((py_histogram_obj_t *) self_in)->LBins)->len;
    hist.ABinCount = ((mp_obj_list_t *) ((py_histogram_obj_t *) self_in)->ABins)->len;
    hist.BBinCount = ((mp_obj_list_t *) ((py_histogram_obj_t *) self_in)->BBins)->len;
    fb_alloc_mark();
    hist.LBins = fb_alloc(hist.LBinCount * sizeof(float), FB_ALLOC_NO_HINT);
    hist.ABins = fb_alloc(hist.ABinCount * sizeof(float), FB_ALLOC_NO_HINT);
    hist.BBins = fb_alloc(hist.BBinCount * sizeof(float), FB_ALLOC_NO_HINT);

    for (int i = 0; i < hist.LBinCount; i++) {
        hist.LBins[i] = mp_obj_get_float(((mp_obj_list_t *) ((py_histogram_obj_t *) self_in)->LBins)->items[i]);
    }

    for (int i = 0; i < hist.ABinCount; i++) {
        hist.ABins[i] = mp_obj_get_float(((mp_obj_list_t *) ((py_histogram_obj_t *) self_in)->ABins)->items[i]);
    }

    for (int i = 0; i < hist.BBinCount; i++) {
        hist.BBins[i] = mp_obj_get_float(((mp_obj_list_t *) ((py_histogram_obj_t *) self_in)->BBins)->items[i]);
    }

    statistics_t stats;
    imlib_get_statistics(&stats, ((py_histogram_obj_t *) self_in)->bpp, &hist);
    fb_alloc_free_till_mark();

    py_statistics_obj_t *o = m_new_obj(py_statistics_obj_t);
    o->base.type = &py_statistics_type;
    o->bpp = ((py_histogram_obj_t *) self_in)->bpp;

    o->LMean = mp_obj_new_int(stats.LMean);
    o->LMedian = mp_obj_new_int(stats.LMedian);
    o->LMode= mp_obj_new_int(stats.LMode);
    o->LSTDev = mp_obj_new_int(stats.LSTDev);
    o->LMin = mp_obj_new_int(stats.LMin);
    o->LMax = mp_obj_new_int(stats.LMax);
    o->LLQ = mp_obj_new_int(stats.LLQ);
    o->LUQ = mp_obj_new_int(stats.LUQ);
    o->AMean = mp_obj_new_int(stats.AMean);
    o->AMedian = mp_obj_new_int(stats.AMedian);
    o->AMode= mp_obj_new_int(stats.AMode);
    o->ASTDev = mp_obj_new_int(stats.ASTDev);
    o->AMin = mp_obj_new_int(stats.AMin);
    o->AMax = mp_obj_new_int(stats.AMax);
    o->ALQ = mp_obj_new_int(stats.ALQ);
    o->AUQ = mp_obj_new_int(stats.AUQ);
    o->BMean = mp_obj_new_int(stats.BMean);
    o->BMedian = mp_obj_new_int(stats.BMedian);
    o->BMode= mp_obj_new_int(stats.BMode);
    o->BSTDev = mp_obj_new_int(stats.BSTDev);
    o->BMin = mp_obj_new_int(stats.BMin);
    o->BMax = mp_obj_new_int(stats.BMax);
    o->BLQ = mp_obj_new_int(stats.BLQ);
    o->BUQ = mp_obj_new_int(stats.BUQ);

    return o;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_histogram_bins_obj, py_histogram_bins);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_histogram_l_bins_obj, py_histogram_l_bins);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_histogram_a_bins_obj, py_histogram_a_bins);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_histogram_b_bins_obj, py_histogram_b_bins);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(py_histogram_get_percentile_obj, py_histogram_get_percentile);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_histogram_get_threshold_obj, py_histogram_get_threshold);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_histogram_get_statistics_obj, py_histogram_get_statistics);

STATIC const mp_rom_map_elem_t py_histogram_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_bins), MP_ROM_PTR(&py_histogram_bins_obj) },
    { MP_ROM_QSTR(MP_QSTR_l_bins), MP_ROM_PTR(&py_histogram_l_bins_obj) },
    { MP_ROM_QSTR(MP_QSTR_a_bins), MP_ROM_PTR(&py_histogram_a_bins_obj) },
    { MP_ROM_QSTR(MP_QSTR_b_bins), MP_ROM_PTR(&py_histogram_b_bins_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_percentile), MP_ROM_PTR(&py_histogram_get_percentile_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_threshold), MP_ROM_PTR(&py_histogram_get_threshold_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_stats), MP_ROM_PTR(&py_histogram_get_statistics_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_statistics), MP_ROM_PTR(&py_histogram_get_statistics_obj) },
    { MP_ROM_QSTR(MP_QSTR_statistics), MP_ROM_PTR(&py_histogram_get_statistics_obj) }
};

STATIC MP_DEFINE_CONST_DICT(py_histogram_locals_dict, py_histogram_locals_dict_table);

static const mp_obj_type_t py_histogram_type = {
    { &mp_type_type },
    .name  = MP_QSTR_histogram,
    .print = py_histogram_print,
    .subscr = py_histogram_subscr,
    .locals_dict = (mp_obj_t) &py_histogram_locals_dict
};

static mp_obj_t py_image_get_histogram(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    list_t thresholds;
    list_init(&thresholds, sizeof(color_thresholds_list_lnk_data_t));
    py_helper_keyword_thresholds(n_args, args, 1, kw_args, &thresholds);
    bool invert = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_invert), false);
    image_t *other = py_helper_keyword_to_image_mutable(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_difference), NULL);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 3, kw_args, &roi);

    histogram_t hist;
    switch(arg_img->bpp) {
        case IMAGE_BPP_BINARY: {
            int bins = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_bins),
                                             (COLOR_BINARY_MAX-COLOR_BINARY_MIN+1));
            PY_ASSERT_TRUE_MSG(bins >= 2, "bins must be >= 2");
            hist.LBinCount = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_l_bins), bins);
            PY_ASSERT_TRUE_MSG(hist.LBinCount >= 2, "l_bins must be >= 2");
            hist.ABinCount = 0;
            hist.BBinCount = 0;
            fb_alloc_mark();
            hist.LBins = fb_alloc(hist.LBinCount * sizeof(float), FB_ALLOC_NO_HINT);
            hist.ABins = NULL;
            hist.BBins = NULL;
            imlib_get_histogram(&hist, arg_img, &roi, &thresholds, invert, other);
            list_free(&thresholds);
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            int bins = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_bins),
                                             (COLOR_GRAYSCALE_MAX-COLOR_GRAYSCALE_MIN+1));
            PY_ASSERT_TRUE_MSG(bins >= 2, "bins must be >= 2");
            hist.LBinCount = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_l_bins), bins);
            PY_ASSERT_TRUE_MSG(hist.LBinCount >= 2, "l_bins must be >= 2");
            hist.ABinCount = 0;
            hist.BBinCount = 0;
            fb_alloc_mark();
            hist.LBins = fb_alloc(hist.LBinCount * sizeof(float), FB_ALLOC_NO_HINT);
            hist.ABins = NULL;
            hist.BBins = NULL;
            imlib_get_histogram(&hist, arg_img, &roi, &thresholds, invert, other);
            list_free(&thresholds);
            break;
        }
        case IMAGE_BPP_RGB565: {
            int l_bins = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_bins),
                                               (COLOR_L_MAX-COLOR_L_MIN+1));
            PY_ASSERT_TRUE_MSG(l_bins >= 2, "bins must be >= 2");
            hist.LBinCount = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_l_bins), l_bins);
            PY_ASSERT_TRUE_MSG(hist.LBinCount >= 2, "l_bins must be >= 2");
            int a_bins = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_bins),
                                               (COLOR_A_MAX-COLOR_A_MIN+1));
            PY_ASSERT_TRUE_MSG(a_bins >= 2, "bins must be >= 2");
            hist.ABinCount = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_a_bins), a_bins);
            PY_ASSERT_TRUE_MSG(hist.ABinCount >= 2, "a_bins must be >= 2");
            int b_bins = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_bins),
                                               (COLOR_B_MAX-COLOR_B_MIN+1));
            PY_ASSERT_TRUE_MSG(b_bins >= 2, "bins must be >= 2");
            hist.BBinCount = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_b_bins), b_bins);
            PY_ASSERT_TRUE_MSG(hist.BBinCount >= 2, "b_bins must be >= 2");
            fb_alloc_mark();
            hist.LBins = fb_alloc(hist.LBinCount * sizeof(float), FB_ALLOC_NO_HINT);
            hist.ABins = fb_alloc(hist.ABinCount * sizeof(float), FB_ALLOC_NO_HINT);
            hist.BBins = fb_alloc(hist.BBinCount * sizeof(float), FB_ALLOC_NO_HINT);
            imlib_get_histogram(&hist, arg_img, &roi, &thresholds, invert, other);
            list_free(&thresholds);
            break;
        }
        default: {
            return MP_OBJ_NULL;
        }
    }

    py_histogram_obj_t *o = m_new_obj(py_histogram_obj_t);
    o->base.type = &py_histogram_type;
    o->bpp = arg_img->bpp;

    o->LBins = mp_obj_new_list(hist.LBinCount, NULL);
    o->ABins = mp_obj_new_list(hist.ABinCount, NULL);
    o->BBins = mp_obj_new_list(hist.BBinCount, NULL);

    for (int i = 0; i < hist.LBinCount; i++) {
        ((mp_obj_list_t *) o->LBins)->items[i] = mp_obj_new_float(hist.LBins[i]);
    }

    for (int i = 0; i < hist.ABinCount; i++) {
        ((mp_obj_list_t *) o->ABins)->items[i] = mp_obj_new_float(hist.ABins[i]);
    }

    for (int i = 0; i < hist.BBinCount; i++) {
        ((mp_obj_list_t *) o->BBins)->items[i] = mp_obj_new_float(hist.BBins[i]);
    }

    fb_alloc_free_till_mark();

    return o;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_get_histogram_obj, 1, py_image_get_histogram);

static mp_obj_t py_image_get_statistics(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    list_t thresholds;
    list_init(&thresholds, sizeof(color_thresholds_list_lnk_data_t));
    py_helper_keyword_thresholds(n_args, args, 1, kw_args, &thresholds);
    bool invert = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_invert), false);
    image_t *other = py_helper_keyword_to_image_mutable(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_difference), NULL);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 3, kw_args, &roi);

    histogram_t hist;
    switch(arg_img->bpp) {
        case IMAGE_BPP_BINARY: {
            int bins = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_bins),
                                             (COLOR_BINARY_MAX-COLOR_BINARY_MIN+1));
            PY_ASSERT_TRUE_MSG(bins >= 2, "bins must be >= 2");
            hist.LBinCount = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_l_bins), bins);
            PY_ASSERT_TRUE_MSG(hist.LBinCount >= 2, "l_bins must be >= 2");
            hist.ABinCount = 0;
            hist.BBinCount = 0;
            fb_alloc_mark();
            hist.LBins = fb_alloc(hist.LBinCount * sizeof(float), FB_ALLOC_NO_HINT);
            hist.ABins = NULL;
            hist.BBins = NULL;
            imlib_get_histogram(&hist, arg_img, &roi, &thresholds, invert, other);
            list_free(&thresholds);
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            int bins = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_bins),
                                             (COLOR_GRAYSCALE_MAX-COLOR_GRAYSCALE_MIN+1));
            PY_ASSERT_TRUE_MSG(bins >= 2, "bins must be >= 2");
            hist.LBinCount = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_l_bins), bins);
            PY_ASSERT_TRUE_MSG(hist.LBinCount >= 2, "l_bins must be >= 2");
            hist.ABinCount = 0;
            hist.BBinCount = 0;
            fb_alloc_mark();
            hist.LBins = fb_alloc(hist.LBinCount * sizeof(float), FB_ALLOC_NO_HINT);
            hist.ABins = NULL;
            hist.BBins = NULL;
            imlib_get_histogram(&hist, arg_img, &roi, &thresholds, invert, other);
            list_free(&thresholds);
            break;
        }
        case IMAGE_BPP_RGB565: {
            int l_bins = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_bins),
                                               (COLOR_L_MAX-COLOR_L_MIN+1));
            PY_ASSERT_TRUE_MSG(l_bins >= 2, "bins must be >= 2");
            hist.LBinCount = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_l_bins), l_bins);
            PY_ASSERT_TRUE_MSG(hist.LBinCount >= 2, "l_bins must be >= 2");
            int a_bins = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_bins),
                                               (COLOR_A_MAX-COLOR_A_MIN+1));
            PY_ASSERT_TRUE_MSG(a_bins >= 2, "bins must be >= 2");
            hist.ABinCount = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_a_bins), a_bins);
            PY_ASSERT_TRUE_MSG(hist.ABinCount >= 2, "a_bins must be >= 2");
            int b_bins = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_bins),
                                               (COLOR_B_MAX-COLOR_B_MIN+1));
            PY_ASSERT_TRUE_MSG(b_bins >= 2, "bins must be >= 2");
            hist.BBinCount = py_helper_keyword_int(n_args, args, n_args, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_b_bins), b_bins);
            PY_ASSERT_TRUE_MSG(hist.BBinCount >= 2, "b_bins must be >= 2");
            fb_alloc_mark();
            hist.LBins = fb_alloc(hist.LBinCount * sizeof(float), FB_ALLOC_NO_HINT);
            hist.ABins = fb_alloc(hist.ABinCount * sizeof(float), FB_ALLOC_NO_HINT);
            hist.BBins = fb_alloc(hist.BBinCount * sizeof(float), FB_ALLOC_NO_HINT);
            imlib_get_histogram(&hist, arg_img, &roi, &thresholds, invert, other);
            list_free(&thresholds);
            break;
        }
        default: {
            return MP_OBJ_NULL;
        }
    }

    statistics_t stats;
    imlib_get_statistics(&stats, arg_img->bpp, &hist);
    fb_alloc_free_till_mark();

    py_statistics_obj_t *o = m_new_obj(py_statistics_obj_t);
    o->base.type = &py_statistics_type;
    o->bpp = arg_img->bpp;

    o->LMean = mp_obj_new_int(stats.LMean);
    o->LMedian = mp_obj_new_int(stats.LMedian);
    o->LMode= mp_obj_new_int(stats.LMode);
    o->LSTDev = mp_obj_new_int(stats.LSTDev);
    o->LMin = mp_obj_new_int(stats.LMin);
    o->LMax = mp_obj_new_int(stats.LMax);
    o->LLQ = mp_obj_new_int(stats.LLQ);
    o->LUQ = mp_obj_new_int(stats.LUQ);
    o->AMean = mp_obj_new_int(stats.AMean);
    o->AMedian = mp_obj_new_int(stats.AMedian);
    o->AMode= mp_obj_new_int(stats.AMode);
    o->ASTDev = mp_obj_new_int(stats.ASTDev);
    o->AMin = mp_obj_new_int(stats.AMin);
    o->AMax = mp_obj_new_int(stats.AMax);
    o->ALQ = mp_obj_new_int(stats.ALQ);
    o->AUQ = mp_obj_new_int(stats.AUQ);
    o->BMean = mp_obj_new_int(stats.BMean);
    o->BMedian = mp_obj_new_int(stats.BMedian);
    o->BMode= mp_obj_new_int(stats.BMode);
    o->BSTDev = mp_obj_new_int(stats.BSTDev);
    o->BMin = mp_obj_new_int(stats.BMin);
    o->BMax = mp_obj_new_int(stats.BMax);
    o->BLQ = mp_obj_new_int(stats.BLQ);
    o->BUQ = mp_obj_new_int(stats.BUQ);

    return o;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_get_statistics_obj, 1, py_image_get_statistics);

// Line Object //
#define py_line_obj_size 8
typedef struct py_line_obj {
    mp_obj_base_t base;
    mp_obj_t x1, y1, x2, y2, length, magnitude, theta, rho;
} py_line_obj_t;

static void py_line_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_line_obj_t *self = self_in;
    mp_printf(print,
              "{\"x1\":%d, \"y1\":%d, \"x2\":%d, \"y2\":%d, \"length\":%d, \"magnitude\":%d, \"theta\":%d, \"rho\":%d}",
              mp_obj_get_int(self->x1),
              mp_obj_get_int(self->y1),
              mp_obj_get_int(self->x2),
              mp_obj_get_int(self->y2),
              mp_obj_get_int(self->length),
              mp_obj_get_int(self->magnitude),
              mp_obj_get_int(self->theta),
              mp_obj_get_int(self->rho));
}

static mp_obj_t py_line_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    if (value == MP_OBJ_SENTINEL) { // load
        py_line_obj_t *self = self_in;
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(py_line_obj_size, index, &slice)) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
            }
            mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
            mp_seq_copy(result->items, &(self->x1) + slice.start, result->len, mp_obj_t);
            return result;
        }
        switch (mp_get_index(self->base.type, py_line_obj_size, index, false)) {
            case 0: return self->x1;
            case 1: return self->y1;
            case 2: return self->x2;
            case 3: return self->y2;
            case 4: return self->length;
            case 5: return self->magnitude;
            case 6: return self->theta;
            case 7: return self->rho;
        }
    }
    return MP_OBJ_NULL; // op not supported
}

mp_obj_t py_line_line(mp_obj_t self_in)
{
    return mp_obj_new_tuple(4, (mp_obj_t []) {((py_line_obj_t *) self_in)->x1,
                                              ((py_line_obj_t *) self_in)->y1,
                                              ((py_line_obj_t *) self_in)->x2,
                                              ((py_line_obj_t *) self_in)->y2});
}

mp_obj_t py_line_x1(mp_obj_t self_in) { return ((py_line_obj_t *) self_in)->x1; }
mp_obj_t py_line_y1(mp_obj_t self_in) { return ((py_line_obj_t *) self_in)->y1; }
mp_obj_t py_line_x2(mp_obj_t self_in) { return ((py_line_obj_t *) self_in)->x2; }
mp_obj_t py_line_y2(mp_obj_t self_in) { return ((py_line_obj_t *) self_in)->y2; }
mp_obj_t py_line_length(mp_obj_t self_in) { return ((py_line_obj_t *) self_in)->length; }
mp_obj_t py_line_magnitude(mp_obj_t self_in) { return ((py_line_obj_t *) self_in)->magnitude; }
mp_obj_t py_line_theta(mp_obj_t self_in) { return ((py_line_obj_t *) self_in)->theta; }
mp_obj_t py_line_rho(mp_obj_t self_in) { return ((py_line_obj_t *) self_in)->rho; }

STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_line_line_obj, py_line_line);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_line_x1_obj, py_line_x1);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_line_y1_obj, py_line_y1);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_line_x2_obj, py_line_x2);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_line_y2_obj, py_line_y2);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_line_length_obj, py_line_length);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_line_magnitude_obj, py_line_magnitude);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_line_theta_obj, py_line_theta);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_line_rho_obj, py_line_rho);

STATIC const mp_rom_map_elem_t py_line_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_line), MP_ROM_PTR(&py_line_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_x1), MP_ROM_PTR(&py_line_x1_obj) },
    { MP_ROM_QSTR(MP_QSTR_y1), MP_ROM_PTR(&py_line_y1_obj) },
    { MP_ROM_QSTR(MP_QSTR_x2), MP_ROM_PTR(&py_line_x2_obj) },
    { MP_ROM_QSTR(MP_QSTR_y2), MP_ROM_PTR(&py_line_y2_obj) },
    { MP_ROM_QSTR(MP_QSTR_length), MP_ROM_PTR(&py_line_length_obj) },
    { MP_ROM_QSTR(MP_QSTR_magnitude), MP_ROM_PTR(&py_line_magnitude_obj) },
    { MP_ROM_QSTR(MP_QSTR_theta), MP_ROM_PTR(&py_line_theta_obj) },
    { MP_ROM_QSTR(MP_QSTR_rho), MP_ROM_PTR(&py_line_rho_obj) }
};

STATIC MP_DEFINE_CONST_DICT(py_line_locals_dict, py_line_locals_dict_table);

static const mp_obj_type_t py_line_type = {
    { &mp_type_type },
    .name  = MP_QSTR_line,
    .print = py_line_print,
    .subscr = py_line_subscr,
    .locals_dict = (mp_obj_t) &py_line_locals_dict
};

static mp_obj_t py_image_get_regression(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    list_t thresholds;
    list_init(&thresholds, sizeof(color_thresholds_list_lnk_data_t));
    py_helper_arg_to_thresholds(args[1], &thresholds);
    if (!list_size(&thresholds)) return mp_const_none;
    bool invert = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_invert), false);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 3, kw_args, &roi);

    unsigned int x_stride = py_helper_keyword_int(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_stride), 2);
    PY_ASSERT_TRUE_MSG(x_stride > 0, "x_stride must not be zero.");
    unsigned int y_stride = py_helper_keyword_int(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_stride), 1);
    PY_ASSERT_TRUE_MSG(y_stride > 0, "y_stride must not be zero.");
    unsigned int area_threshold = py_helper_keyword_int(n_args, args, 6, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_area_threshold), 10);
    unsigned int pixels_threshold = py_helper_keyword_int(n_args, args, 7, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_pixels_threshold), 10);
    bool robust = py_helper_keyword_int(n_args, args, 8, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_robust), false);

    find_lines_list_lnk_data_t out;
    fb_alloc_mark();
    bool result = imlib_get_regression(&out, arg_img, &roi, x_stride, y_stride, &thresholds, invert, area_threshold, pixels_threshold, robust);
    fb_alloc_free_till_mark();
    list_free(&thresholds);
    if (!result) {
        return mp_const_none;
    }

    py_line_obj_t *o = m_new_obj(py_line_obj_t);
    o->base.type = &py_line_type;
    o->x1 = mp_obj_new_int(out.line.x1);
    o->y1 = mp_obj_new_int(out.line.y1);
    o->x2 = mp_obj_new_int(out.line.x2);
    o->y2 = mp_obj_new_int(out.line.y2);
    int x_diff = out.line.x2 - out.line.x1;
    int y_diff = out.line.y2 - out.line.y1;
    o->length = mp_obj_new_int(fast_roundf(fast_sqrtf((x_diff * x_diff) + (y_diff * y_diff))));
    o->magnitude = mp_obj_new_int(out.magnitude);
    o->theta = mp_obj_new_int(out.theta);
    o->rho = mp_obj_new_int(out.rho);

    return o;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_get_regression_obj, 2, py_image_get_regression);

///////////////
// Find Methods
///////////////

// Blob Object //
#define py_blob_obj_size 12
typedef struct py_blob_obj {
    mp_obj_base_t base;
    mp_obj_t corners;
    mp_obj_t min_corners;
    mp_obj_t x, y, w, h, pixels, cx, cy, rotation, code, count, perimeter, roundness;
    mp_obj_t x_hist_bins;
    mp_obj_t y_hist_bins;
} py_blob_obj_t;

static void py_blob_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_blob_obj_t *self = self_in;
    mp_printf(print,
              "{\"x\":%d, \"y\":%d, \"w\":%d, \"h\":%d,"
              " \"pixels\":%d, \"cx\":%d, \"cy\":%d, \"rotation\":%f, \"code\":%d, \"count\":%d,"
              " \"perimeter\":%d, \"roundness\":%f}",
              mp_obj_get_int(self->x),
              mp_obj_get_int(self->y),
              mp_obj_get_int(self->w),
              mp_obj_get_int(self->h),
              mp_obj_get_int(self->pixels),
              fast_roundf(mp_obj_get_float(self->cx)),
              fast_roundf(mp_obj_get_float(self->cy)),
              (double) mp_obj_get_float(self->rotation),
              mp_obj_get_int(self->code),
              mp_obj_get_int(self->count),
              mp_obj_get_int(self->perimeter),
              (double) mp_obj_get_float(self->roundness));
}

static mp_obj_t py_blob_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    if (value == MP_OBJ_SENTINEL) { // load
        py_blob_obj_t *self = self_in;
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(py_blob_obj_size, index, &slice)) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
            }
            mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
            mp_seq_copy(result->items, &(self->x) + slice.start, result->len, mp_obj_t);
            return result;
        }
        switch (mp_get_index(self->base.type, py_blob_obj_size, index, false)) {
            case 0: return self->x;
            case 1: return self->y;
            case 2: return self->w;
            case 3: return self->h;
            case 4: return self->pixels;
            case 5: return mp_obj_new_int(fast_roundf(mp_obj_get_float(self->cx)));
            case 6: return mp_obj_new_int(fast_roundf(mp_obj_get_float(self->cy)));
            case 7: return self->rotation;
            case 8: return self->code;
            case 9: return self->count;
            case 10: return self->perimeter;
            case 11: return self->roundness;
        }
    }
    return MP_OBJ_NULL; // op not supported
}

mp_obj_t py_blob_corners(mp_obj_t self_in) { return ((py_blob_obj_t *) self_in)->corners; }
mp_obj_t py_blob_min_corners(mp_obj_t self_in) { return ((py_blob_obj_t *) self_in)->min_corners; }
mp_obj_t py_blob_rect(mp_obj_t self_in)
{
    return mp_obj_new_tuple(4, (mp_obj_t []) {((py_blob_obj_t *) self_in)->x,
                                              ((py_blob_obj_t *) self_in)->y,
                                              ((py_blob_obj_t *) self_in)->w,
                                              ((py_blob_obj_t *) self_in)->h});
}

mp_obj_t py_blob_x(mp_obj_t self_in) { return ((py_blob_obj_t *) self_in)->x; }
mp_obj_t py_blob_y(mp_obj_t self_in) { return ((py_blob_obj_t *) self_in)->y; }
mp_obj_t py_blob_w(mp_obj_t self_in) { return ((py_blob_obj_t *) self_in)->w; }
mp_obj_t py_blob_h(mp_obj_t self_in) { return ((py_blob_obj_t *) self_in)->h; }
mp_obj_t py_blob_pixels(mp_obj_t self_in) { return ((py_blob_obj_t *) self_in)->pixels; }
mp_obj_t py_blob_cx(mp_obj_t self_in) { return mp_obj_new_int(fast_roundf(mp_obj_get_float(((py_blob_obj_t *) self_in)->cx))); }
mp_obj_t py_blob_cxf(mp_obj_t self_in) { return ((py_blob_obj_t *) self_in)->cx; }
mp_obj_t py_blob_cy(mp_obj_t self_in) { return mp_obj_new_int(fast_roundf(mp_obj_get_float(((py_blob_obj_t *) self_in)->cy))); }
mp_obj_t py_blob_cyf(mp_obj_t self_in) { return ((py_blob_obj_t *) self_in)->cy; }
mp_obj_t py_blob_rotation(mp_obj_t self_in) { return ((py_blob_obj_t *) self_in)->rotation; }
mp_obj_t py_blob_rotation_deg(mp_obj_t self_in) { return mp_obj_new_int(IM_RAD2DEG(mp_obj_get_float(((py_blob_obj_t *) self_in)->rotation))); }
mp_obj_t py_blob_rotation_rad(mp_obj_t self_in) { return ((py_blob_obj_t *) self_in)->rotation; }
mp_obj_t py_blob_code(mp_obj_t self_in) { return ((py_blob_obj_t *) self_in)->code; }
mp_obj_t py_blob_count(mp_obj_t self_in) { return ((py_blob_obj_t *) self_in)->count; }
mp_obj_t py_blob_perimeter(mp_obj_t self_in) { return ((py_blob_obj_t *) self_in)->perimeter; }
mp_obj_t py_blob_roundness(mp_obj_t self_in) { return ((py_blob_obj_t *) self_in)->roundness; }
mp_obj_t py_blob_elongation(mp_obj_t self_in) { return mp_obj_new_float(1 - mp_obj_get_float(((py_blob_obj_t *) self_in)->roundness)); }
mp_obj_t py_blob_area(mp_obj_t self_in) {
    return mp_obj_new_int(mp_obj_get_int(((py_blob_obj_t *) self_in)->w) * mp_obj_get_int(((py_blob_obj_t *) self_in)->h));
}
mp_obj_t py_blob_density(mp_obj_t self_in) {
    int area = mp_obj_get_int(((py_blob_obj_t *) self_in)->w) * mp_obj_get_int(((py_blob_obj_t *) self_in)->h);
    int pixels = mp_obj_get_int(((py_blob_obj_t *) self_in)->pixels);
    return mp_obj_new_float(IM_DIV(pixels, ((float) area)));
}
// Rect-area versus pixels (e.g. blob area) -> Above.
// Rect-area versus perimeter -> Basically the same as the above with a different scale factor.
// Rect-perimeter versus pixels (e.g. blob area) -> Basically the same as the above with a different scale factor.
// Rect-perimeter versus perimeter -> Basically the same as the above with a different scale factor.
mp_obj_t py_blob_compactness(mp_obj_t self_in) {
    int pixels = mp_obj_get_int(((py_blob_obj_t *) self_in)->pixels);
    float perimeter = mp_obj_get_int(((py_blob_obj_t *) self_in)->perimeter);
    return mp_obj_new_float(IM_DIV((pixels * 4 * M_PI), (perimeter * perimeter)));
}
mp_obj_t py_blob_solidity(mp_obj_t self_in) {
    mp_obj_t *corners, *p0, *p1, *p2, *p3;
    mp_obj_get_array_fixed_n(((py_blob_obj_t *) self_in)->min_corners, 4, &corners);
    mp_obj_get_array_fixed_n(corners[0], 2, &p0);
    mp_obj_get_array_fixed_n(corners[1], 2, &p1);
    mp_obj_get_array_fixed_n(corners[2], 2, &p2);
    mp_obj_get_array_fixed_n(corners[3], 2, &p3);

    int x0, y0, x1, y1, x2, y2, x3, y3;
    x0 = mp_obj_get_int(p0[0]);
    y0 = mp_obj_get_int(p0[1]);
    x1 = mp_obj_get_int(p1[0]);
    y1 = mp_obj_get_int(p1[1]);
    x2 = mp_obj_get_int(p2[0]);
    y2 = mp_obj_get_int(p2[1]);
    x3 = mp_obj_get_int(p3[0]);
    y3 = mp_obj_get_int(p3[1]);

    // Shoelace Formula
    float min_area = (((x0*y1)+(x1*y2)+(x2*y3)+(x3*y0))-((y0*x1)+(y1*x2)+(y2*x3)+(y3*x0)))/2.0f;
    int pixels = mp_obj_get_int(((py_blob_obj_t *) self_in)->pixels);
    return mp_obj_new_float(IM_MIN(IM_DIV(pixels, min_area), 1));
}
mp_obj_t py_blob_convexity(mp_obj_t self_in) {
    mp_obj_t *corners, *p0, *p1, *p2, *p3;
    mp_obj_get_array_fixed_n(((py_blob_obj_t *) self_in)->min_corners, 4, &corners);
    mp_obj_get_array_fixed_n(corners[0], 2, &p0);
    mp_obj_get_array_fixed_n(corners[1], 2, &p1);
    mp_obj_get_array_fixed_n(corners[2], 2, &p2);
    mp_obj_get_array_fixed_n(corners[3], 2, &p3);

    int x0, y0, x1, y1, x2, y2, x3, y3;
    x0 = mp_obj_get_int(p0[0]);
    y0 = mp_obj_get_int(p0[1]);
    x1 = mp_obj_get_int(p1[0]);
    y1 = mp_obj_get_int(p1[1]);
    x2 = mp_obj_get_int(p2[0]);
    y2 = mp_obj_get_int(p2[1]);
    x3 = mp_obj_get_int(p3[0]);
    y3 = mp_obj_get_int(p3[1]);

    float d0 = fast_sqrtf(((x0 - x1) * (x0 - x1)) + ((y0 - y1) * (y0 - y1)));
    float d1 = fast_sqrtf(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
    float d2 = fast_sqrtf(((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)));
    float d3 = fast_sqrtf(((x3 - x0) * (x3 - x0)) + ((y3 - y0) * (y3 - y0)));
    int perimeter = mp_obj_get_int(((py_blob_obj_t *) self_in)->perimeter);
    return mp_obj_new_float(IM_MIN(IM_DIV(d0 + d1 + d2 + d3, perimeter), 1));
}
// Min rect-area versus pixels (e.g. blob area) -> Above.
// Min rect-area versus perimeter -> Basically the same as the above with a different scale factor.
// Min rect-perimeter versus pixels (e.g. blob area) -> Basically the same as the above with a different scale factor.
// Min rect-perimeter versus perimeter -> Above
mp_obj_t py_blob_x_hist_bins(mp_obj_t self_in) { return ((py_blob_obj_t *) self_in)->x_hist_bins; }
mp_obj_t py_blob_y_hist_bins(mp_obj_t self_in) { return ((py_blob_obj_t *) self_in)->y_hist_bins; }
mp_obj_t py_blob_major_axis_line(mp_obj_t self_in) {
    mp_obj_t *corners, *p0, *p1, *p2, *p3;
    mp_obj_get_array_fixed_n(((py_blob_obj_t *) self_in)->min_corners, 4, &corners);
    mp_obj_get_array_fixed_n(corners[0], 2, &p0);
    mp_obj_get_array_fixed_n(corners[1], 2, &p1);
    mp_obj_get_array_fixed_n(corners[2], 2, &p2);
    mp_obj_get_array_fixed_n(corners[3], 2, &p3);

    int x0, y0, x1, y1, x2, y2, x3, y3;
    x0 = mp_obj_get_int(p0[0]);
    y0 = mp_obj_get_int(p0[1]);
    x1 = mp_obj_get_int(p1[0]);
    y1 = mp_obj_get_int(p1[1]);
    x2 = mp_obj_get_int(p2[0]);
    y2 = mp_obj_get_int(p2[1]);
    x3 = mp_obj_get_int(p3[0]);
    y3 = mp_obj_get_int(p3[1]);

    int m0x = (x0 + x1) / 2;
    int m0y = (y0 + y1) / 2;
    int m1x = (x1 + x2) / 2;
    int m1y = (y1 + y2) / 2;
    int m2x = (x2 + x3) / 2;
    int m2y = (y2 + y3) / 2;
    int m3x = (x3 + x0) / 2;
    int m3y = (y3 + y0) / 2;

    float l0 = fast_sqrtf(((m0x - m2x) * (m0x - m2x)) + ((m0y - m2y) * (m0y - m2y)));
    float l1 = fast_sqrtf(((m1x - m3x) * (m1x - m3x)) + ((m1y - m3y) * (m1y - m3y)));

    if (l0 >= l1) {
        return mp_obj_new_tuple(4, (mp_obj_t []) {mp_obj_new_int(m0x),
                                                  mp_obj_new_int(m0y),
                                                  mp_obj_new_int(m2x),
                                                  mp_obj_new_int(m2y)});
    } else {
        return mp_obj_new_tuple(4, (mp_obj_t []) {mp_obj_new_int(m1x),
                                                  mp_obj_new_int(m1y),
                                                  mp_obj_new_int(m3x),
                                                  mp_obj_new_int(m3y)});
    }
}
mp_obj_t py_blob_minor_axis_line(mp_obj_t self_in) {
    mp_obj_t *corners, *p0, *p1, *p2, *p3;
    mp_obj_get_array_fixed_n(((py_blob_obj_t *) self_in)->min_corners, 4, &corners);
    mp_obj_get_array_fixed_n(corners[0], 2, &p0);
    mp_obj_get_array_fixed_n(corners[1], 2, &p1);
    mp_obj_get_array_fixed_n(corners[2], 2, &p2);
    mp_obj_get_array_fixed_n(corners[3], 2, &p3);

    int x0, y0, x1, y1, x2, y2, x3, y3;
    x0 = mp_obj_get_int(p0[0]);
    y0 = mp_obj_get_int(p0[1]);
    x1 = mp_obj_get_int(p1[0]);
    y1 = mp_obj_get_int(p1[1]);
    x2 = mp_obj_get_int(p2[0]);
    y2 = mp_obj_get_int(p2[1]);
    x3 = mp_obj_get_int(p3[0]);
    y3 = mp_obj_get_int(p3[1]);

    int m0x = (x0 + x1) / 2;
    int m0y = (y0 + y1) / 2;
    int m1x = (x1 + x2) / 2;
    int m1y = (y1 + y2) / 2;
    int m2x = (x2 + x3) / 2;
    int m2y = (y2 + y3) / 2;
    int m3x = (x3 + x0) / 2;
    int m3y = (y3 + y0) / 2;

    float l0 = fast_sqrtf(((m0x - m2x) * (m0x - m2x)) + ((m0y - m2y) * (m0y - m2y)));
    float l1 = fast_sqrtf(((m1x - m3x) * (m1x - m3x)) + ((m1y - m3y) * (m1y - m3y)));

    if (l0 < l1) {
        return mp_obj_new_tuple(4, (mp_obj_t []) {mp_obj_new_int(m0x),
                                                  mp_obj_new_int(m0y),
                                                  mp_obj_new_int(m2x),
                                                  mp_obj_new_int(m2y)});
    } else {
        return mp_obj_new_tuple(4, (mp_obj_t []) {mp_obj_new_int(m1x),
                                                  mp_obj_new_int(m1y),
                                                  mp_obj_new_int(m3x),
                                                  mp_obj_new_int(m3y)});
    }
}
mp_obj_t py_blob_enclosing_circle(mp_obj_t self_in) {
    mp_obj_t *corners, *p0, *p1, *p2, *p3;
    mp_obj_get_array_fixed_n(((py_blob_obj_t *) self_in)->min_corners, 4, &corners);
    mp_obj_get_array_fixed_n(corners[0], 2, &p0);
    mp_obj_get_array_fixed_n(corners[1], 2, &p1);
    mp_obj_get_array_fixed_n(corners[2], 2, &p2);
    mp_obj_get_array_fixed_n(corners[3], 2, &p3);

    int x0, y0, x1, y1, x2, y2, x3, y3;
    x0 = mp_obj_get_int(p0[0]);
    y0 = mp_obj_get_int(p0[1]);
    x1 = mp_obj_get_int(p1[0]);
    y1 = mp_obj_get_int(p1[1]);
    x2 = mp_obj_get_int(p2[0]);
    y2 = mp_obj_get_int(p2[1]);
    x3 = mp_obj_get_int(p3[0]);
    y3 = mp_obj_get_int(p3[1]);

    int cx = (x0 + x1 + x2 + x3) / 4;
    int cy = (y0 + y1 + y2 + y3) / 4;

    float d0 = fast_sqrtf(((x0 - cx) * (x0 - cx)) + ((y0 - cy) * (y0 - cy)));
    float d1 = fast_sqrtf(((x1 - cx) * (x1 - cx)) + ((y1 - cy) * (y1 - cy)));
    float d2 = fast_sqrtf(((x2 - cx) * (x2 - cx)) + ((y2 - cy) * (y2 - cy)));
    float d3 = fast_sqrtf(((x3 - cx) * (x3 - cx)) + ((y3 - cy) * (y3 - cy)));
    float d = IM_MAX(d0, IM_MAX(d1, IM_MAX(d2, d3)));

    return mp_obj_new_tuple(3, (mp_obj_t []) {mp_obj_new_int(cx),
                                              mp_obj_new_int(cy),
                                              mp_obj_new_int(fast_roundf(d))});
}
mp_obj_t py_blob_enclosed_ellipse(mp_obj_t self_in) {
    mp_obj_t *corners, *p0, *p1, *p2, *p3;
    mp_obj_get_array_fixed_n(((py_blob_obj_t *) self_in)->min_corners, 4, &corners);
    mp_obj_get_array_fixed_n(corners[0], 2, &p0);
    mp_obj_get_array_fixed_n(corners[1], 2, &p1);
    mp_obj_get_array_fixed_n(corners[2], 2, &p2);
    mp_obj_get_array_fixed_n(corners[3], 2, &p3);

    int x0, y0, x1, y1, x2, y2, x3, y3;
    x0 = mp_obj_get_int(p0[0]);
    y0 = mp_obj_get_int(p0[1]);
    x1 = mp_obj_get_int(p1[0]);
    y1 = mp_obj_get_int(p1[1]);
    x2 = mp_obj_get_int(p2[0]);
    y2 = mp_obj_get_int(p2[1]);
    x3 = mp_obj_get_int(p3[0]);
    y3 = mp_obj_get_int(p3[1]);

    int m0x = (x0 + x1) / 2;
    int m0y = (y0 + y1) / 2;
    int m1x = (x1 + x2) / 2;
    int m1y = (y1 + y2) / 2;
    int m2x = (x2 + x3) / 2;
    int m2y = (y2 + y3) / 2;
    int m3x = (x3 + x0) / 2;
    int m3y = (y3 + y0) / 2;

    int cx = (x0 + x1 + x2 + x3) / 4;
    int cy = (y0 + y1 + y2 + y3) / 4;

    float d0 = fast_sqrtf(((m0x - cx) * (m0x - cx)) + ((m0y - cy) * (m0y - cy)));
    float d1 = fast_sqrtf(((m1x - cx) * (m1x - cx)) + ((m1y - cy) * (m1y - cy)));
    float d2 = fast_sqrtf(((m2x - cx) * (m2x - cx)) + ((m2y - cy) * (m2y - cy)));
    float d3 = fast_sqrtf(((m3x - cx) * (m3x - cx)) + ((m3y - cy) * (m3y - cy)));
    float a = IM_MIN(d0, d2);
    float b = IM_MIN(d1, d3);

    float l0 = fast_sqrtf(((m0x - m2x) * (m0x - m2x)) + ((m0y - m2y) * (m0y - m2y)));
    float l1 = fast_sqrtf(((m1x - m3x) * (m1x - m3x)) + ((m1y - m3y) * (m1y - m3y)));

    float r;

    if (l0 >= l1) {
        r = IM_RAD2DEG(fast_atan2f(m0y - m2y, m0x - m2x));
    } else {
        r = IM_RAD2DEG(fast_atan2f(m1y - m3y, m1x - m3x) + M_PI_2);
    }

    return mp_obj_new_tuple(5, (mp_obj_t []) {mp_obj_new_int(cx),
                                              mp_obj_new_int(cy),
                                              mp_obj_new_int(a),
                                              mp_obj_new_int(b),
                                              mp_obj_new_int(r)});
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_corners_obj, py_blob_corners);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_min_corners_obj, py_blob_min_corners);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_rect_obj, py_blob_rect);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_x_obj, py_blob_x);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_y_obj, py_blob_y);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_w_obj, py_blob_w);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_h_obj, py_blob_h);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_pixels_obj, py_blob_pixels);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_cx_obj, py_blob_cx);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_cxf_obj, py_blob_cxf);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_cy_obj, py_blob_cy);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_cyf_obj, py_blob_cyf);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_rotation_obj, py_blob_rotation);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_rotation_deg_obj, py_blob_rotation_deg);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_rotation_rad_obj, py_blob_rotation_rad);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_code_obj, py_blob_code);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_count_obj, py_blob_count);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_perimeter_obj, py_blob_perimeter);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_roundness_obj, py_blob_roundness);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_elongation_obj, py_blob_elongation);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_area_obj, py_blob_area);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_density_obj, py_blob_density);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_compactness_obj, py_blob_compactness);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_solidity_obj, py_blob_solidity);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_convexity_obj, py_blob_convexity);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_x_hist_bins_obj, py_blob_x_hist_bins);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_y_hist_bins_obj, py_blob_y_hist_bins);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_major_axis_line_obj, py_blob_major_axis_line);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_minor_axis_line_obj, py_blob_minor_axis_line);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_enclosing_circle_obj, py_blob_enclosing_circle);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_blob_enclosed_ellipse_obj, py_blob_enclosed_ellipse);

STATIC const mp_rom_map_elem_t py_blob_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_corners), MP_ROM_PTR(&py_blob_corners_obj) },
    { MP_ROM_QSTR(MP_QSTR_min_corners), MP_ROM_PTR(&py_blob_min_corners_obj) },
    { MP_ROM_QSTR(MP_QSTR_rect), MP_ROM_PTR(&py_blob_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&py_blob_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&py_blob_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_w), MP_ROM_PTR(&py_blob_w_obj) },
    { MP_ROM_QSTR(MP_QSTR_h), MP_ROM_PTR(&py_blob_h_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixels), MP_ROM_PTR(&py_blob_pixels_obj) },
    { MP_ROM_QSTR(MP_QSTR_cx), MP_ROM_PTR(&py_blob_cx_obj) },
    { MP_ROM_QSTR(MP_QSTR_cxf), MP_ROM_PTR(&py_blob_cxf_obj) },
    { MP_ROM_QSTR(MP_QSTR_cy), MP_ROM_PTR(&py_blob_cy_obj) },
    { MP_ROM_QSTR(MP_QSTR_cyf), MP_ROM_PTR(&py_blob_cyf_obj) },
    { MP_ROM_QSTR(MP_QSTR_rotation), MP_ROM_PTR(&py_blob_rotation_obj) },
    { MP_ROM_QSTR(MP_QSTR_rotation_deg), MP_ROM_PTR(&py_blob_rotation_deg_obj) },
    { MP_ROM_QSTR(MP_QSTR_rotation_rad), MP_ROM_PTR(&py_blob_rotation_rad_obj) },
    { MP_ROM_QSTR(MP_QSTR_code), MP_ROM_PTR(&py_blob_code_obj) },
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&py_blob_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_perimeter), MP_ROM_PTR(&py_blob_perimeter_obj) },
    { MP_ROM_QSTR(MP_QSTR_roundness), MP_ROM_PTR(&py_blob_roundness_obj) },
    { MP_ROM_QSTR(MP_QSTR_elongation), MP_ROM_PTR(&py_blob_elongation_obj) },
    { MP_ROM_QSTR(MP_QSTR_area), MP_ROM_PTR(&py_blob_area_obj) } ,
    { MP_ROM_QSTR(MP_QSTR_density), MP_ROM_PTR(&py_blob_density_obj) },
    { MP_ROM_QSTR(MP_QSTR_extent), MP_ROM_PTR(&py_blob_density_obj) },
    { MP_ROM_QSTR(MP_QSTR_compactness), MP_ROM_PTR(&py_blob_compactness_obj) },
    { MP_ROM_QSTR(MP_QSTR_solidity), MP_ROM_PTR(&py_blob_solidity_obj) },
    { MP_ROM_QSTR(MP_QSTR_convexity), MP_ROM_PTR(&py_blob_convexity_obj) },
    { MP_ROM_QSTR(MP_QSTR_x_hist_bins), MP_ROM_PTR(&py_blob_x_hist_bins_obj) },
    { MP_ROM_QSTR(MP_QSTR_y_hist_bins), MP_ROM_PTR(&py_blob_y_hist_bins_obj) },
    { MP_ROM_QSTR(MP_QSTR_major_axis_line), MP_ROM_PTR(&py_blob_major_axis_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_minor_axis_line), MP_ROM_PTR(&py_blob_minor_axis_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_enclosing_circle), MP_ROM_PTR(&py_blob_enclosing_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_enclosed_ellipse), MP_ROM_PTR(&py_blob_enclosed_ellipse_obj) }
};

STATIC MP_DEFINE_CONST_DICT(py_blob_locals_dict, py_blob_locals_dict_table);

static const mp_obj_type_t py_blob_type = {
    { &mp_type_type },
    .name  = MP_QSTR_blob,
    .print = py_blob_print,
    .subscr = py_blob_subscr,
    .locals_dict = (mp_obj_t) &py_blob_locals_dict
};

static bool py_image_find_blobs_threshold_cb(void *fun_obj, find_blobs_list_lnk_data_t *blob)
{
    py_blob_obj_t *o = m_new_obj(py_blob_obj_t);
    o->base.type = &py_blob_type;
    o->corners = mp_obj_new_tuple(4, (mp_obj_t [])
        {mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(blob->corners[(FIND_BLOBS_CORNERS_RESOLUTION*0)/4].x),
                                            mp_obj_new_int(blob->corners[(FIND_BLOBS_CORNERS_RESOLUTION*0)/4].y)}),
         mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(blob->corners[(FIND_BLOBS_CORNERS_RESOLUTION*1)/4].x),
                                            mp_obj_new_int(blob->corners[(FIND_BLOBS_CORNERS_RESOLUTION*1)/4].y)}),
         mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(blob->corners[(FIND_BLOBS_CORNERS_RESOLUTION*2)/4].x),
                                            mp_obj_new_int(blob->corners[(FIND_BLOBS_CORNERS_RESOLUTION*2)/4].y)}),
         mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(blob->corners[(FIND_BLOBS_CORNERS_RESOLUTION*3)/4].x),
                                            mp_obj_new_int(blob->corners[(FIND_BLOBS_CORNERS_RESOLUTION*3)/4].y)})});
    point_t min_corners[4];
    point_min_area_rectangle(blob->corners, min_corners, FIND_BLOBS_CORNERS_RESOLUTION);
    o->min_corners = mp_obj_new_tuple(4, (mp_obj_t [])
        {mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(min_corners[0].x), mp_obj_new_int(min_corners[0].y)}),
         mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(min_corners[1].x), mp_obj_new_int(min_corners[1].y)}),
         mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(min_corners[2].x), mp_obj_new_int(min_corners[2].y)}),
         mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(min_corners[3].x), mp_obj_new_int(min_corners[3].y)})});
    o->x = mp_obj_new_int(blob->rect.x);
    o->y = mp_obj_new_int(blob->rect.y);
    o->w = mp_obj_new_int(blob->rect.w);
    o->h = mp_obj_new_int(blob->rect.h);
    o->pixels = mp_obj_new_int(blob->pixels);
    o->cx = mp_obj_new_float(blob->centroid_x);
    o->cy = mp_obj_new_float(blob->centroid_y);
    o->rotation = mp_obj_new_float(blob->rotation);
    o->code = mp_obj_new_int(blob->code);
    o->count = mp_obj_new_int(blob->count);
    o->perimeter = mp_obj_new_int(blob->perimeter);
    o->roundness = mp_obj_new_float(blob->roundness);
    o->x_hist_bins = mp_obj_new_list(blob->x_hist_bins_count, NULL);
    o->y_hist_bins = mp_obj_new_list(blob->y_hist_bins_count, NULL);

    for (int i = 0; i < blob->x_hist_bins_count; i++) {
        ((mp_obj_list_t *) o->x_hist_bins)->items[i] = mp_obj_new_int(blob->x_hist_bins[i]);
    }

    for (int i = 0; i < blob->y_hist_bins_count; i++) {
        ((mp_obj_list_t *) o->y_hist_bins)->items[i] = mp_obj_new_int(blob->y_hist_bins[i]);
    }

    return mp_obj_is_true(mp_call_function_1(fun_obj, o));
}

static bool py_image_find_blobs_merge_cb(void *fun_obj, find_blobs_list_lnk_data_t *blob0, find_blobs_list_lnk_data_t *blob1)
{
    py_blob_obj_t *o0 = m_new_obj(py_blob_obj_t);
    o0->base.type = &py_blob_type;
    o0->corners = mp_obj_new_tuple(4, (mp_obj_t [])
        {mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(blob0->corners[(FIND_BLOBS_CORNERS_RESOLUTION*0)/4].x),
                                            mp_obj_new_int(blob0->corners[(FIND_BLOBS_CORNERS_RESOLUTION*0)/4].y)}),
         mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(blob0->corners[(FIND_BLOBS_CORNERS_RESOLUTION*1)/4].x),
                                            mp_obj_new_int(blob0->corners[(FIND_BLOBS_CORNERS_RESOLUTION*1)/4].y)}),
         mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(blob0->corners[(FIND_BLOBS_CORNERS_RESOLUTION*2)/4].x),
                                            mp_obj_new_int(blob0->corners[(FIND_BLOBS_CORNERS_RESOLUTION*2)/4].y)}),
         mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(blob0->corners[(FIND_BLOBS_CORNERS_RESOLUTION*3)/4].x),
                                            mp_obj_new_int(blob0->corners[(FIND_BLOBS_CORNERS_RESOLUTION*3)/4].y)})});
    point_t min_area_rect_corners0[4];
    point_min_area_rectangle(blob0->corners, min_area_rect_corners0, FIND_BLOBS_CORNERS_RESOLUTION);
    o0->min_corners = mp_obj_new_tuple(4, (mp_obj_t [])
        {mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(min_area_rect_corners0[0].x), mp_obj_new_int(min_area_rect_corners0[0].y)}),
         mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(min_area_rect_corners0[1].x), mp_obj_new_int(min_area_rect_corners0[1].y)}),
         mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(min_area_rect_corners0[2].x), mp_obj_new_int(min_area_rect_corners0[2].y)}),
         mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(min_area_rect_corners0[3].x), mp_obj_new_int(min_area_rect_corners0[3].y)})});
    o0->x = mp_obj_new_int(blob0->rect.x);
    o0->y = mp_obj_new_int(blob0->rect.y);
    o0->w = mp_obj_new_int(blob0->rect.w);
    o0->h = mp_obj_new_int(blob0->rect.h);
    o0->pixels = mp_obj_new_int(blob0->pixels);
    o0->cx = mp_obj_new_float(blob0->centroid_x);
    o0->cy = mp_obj_new_float(blob0->centroid_y);
    o0->rotation = mp_obj_new_float(blob0->rotation);
    o0->code = mp_obj_new_int(blob0->code);
    o0->count = mp_obj_new_int(blob0->count);
    o0->perimeter = mp_obj_new_int(blob0->perimeter);
    o0->roundness = mp_obj_new_float(blob0->roundness);
    o0->x_hist_bins = mp_obj_new_list(blob0->x_hist_bins_count, NULL);
    o0->y_hist_bins = mp_obj_new_list(blob0->y_hist_bins_count, NULL);

    for (int i = 0; i < blob0->x_hist_bins_count; i++) {
        ((mp_obj_list_t *) o0->x_hist_bins)->items[i] = mp_obj_new_int(blob0->x_hist_bins[i]);
    }

    for (int i = 0; i < blob0->y_hist_bins_count; i++) {
        ((mp_obj_list_t *) o0->y_hist_bins)->items[i] = mp_obj_new_int(blob0->y_hist_bins[i]);
    }

    py_blob_obj_t *o1 = m_new_obj(py_blob_obj_t);
    o1->base.type = &py_blob_type;
    o1->corners = mp_obj_new_tuple(4, (mp_obj_t [])
        {mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(blob1->corners[(FIND_BLOBS_CORNERS_RESOLUTION*0)/4].x),
                                            mp_obj_new_int(blob1->corners[(FIND_BLOBS_CORNERS_RESOLUTION*0)/4].y)}),
         mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(blob1->corners[(FIND_BLOBS_CORNERS_RESOLUTION*1)/4].x),
                                            mp_obj_new_int(blob1->corners[(FIND_BLOBS_CORNERS_RESOLUTION*1)/4].y)}),
         mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(blob1->corners[(FIND_BLOBS_CORNERS_RESOLUTION*2)/4].x),
                                            mp_obj_new_int(blob1->corners[(FIND_BLOBS_CORNERS_RESOLUTION*2)/4].y)}),
         mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(blob1->corners[(FIND_BLOBS_CORNERS_RESOLUTION*3)/4].x),
                                            mp_obj_new_int(blob1->corners[(FIND_BLOBS_CORNERS_RESOLUTION*3)/4].y)})});
    point_t min_area_rect_corners1[4];
    point_min_area_rectangle(blob1->corners, min_area_rect_corners1, FIND_BLOBS_CORNERS_RESOLUTION);
    o1->min_corners = mp_obj_new_tuple(4, (mp_obj_t [])
        {mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(min_area_rect_corners1[0].x), mp_obj_new_int(min_area_rect_corners1[0].y)}),
         mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(min_area_rect_corners1[1].x), mp_obj_new_int(min_area_rect_corners1[1].y)}),
         mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(min_area_rect_corners1[2].x), mp_obj_new_int(min_area_rect_corners1[2].y)}),
         mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(min_area_rect_corners1[3].x), mp_obj_new_int(min_area_rect_corners1[3].y)})});
    o1->x = mp_obj_new_int(blob1->rect.x);
    o1->y = mp_obj_new_int(blob1->rect.y);
    o1->w = mp_obj_new_int(blob1->rect.w);
    o1->h = mp_obj_new_int(blob1->rect.h);
    o1->pixels = mp_obj_new_int(blob1->pixels);
    o1->cx = mp_obj_new_float(blob1->centroid_x);
    o1->cy = mp_obj_new_float(blob1->centroid_y);
    o1->rotation = mp_obj_new_float(blob1->rotation);
    o1->code = mp_obj_new_int(blob1->code);
    o1->count = mp_obj_new_int(blob1->count);
    o1->perimeter = mp_obj_new_int(blob1->perimeter);
    o1->roundness = mp_obj_new_float(blob1->roundness);
    o1->x_hist_bins = mp_obj_new_list(blob1->x_hist_bins_count, NULL);
    o1->y_hist_bins = mp_obj_new_list(blob1->y_hist_bins_count, NULL);

    for (int i = 0; i < blob1->x_hist_bins_count; i++) {
        ((mp_obj_list_t *) o1->x_hist_bins)->items[i] = mp_obj_new_int(blob1->x_hist_bins[i]);
    }

    for (int i = 0; i < blob1->y_hist_bins_count; i++) {
        ((mp_obj_list_t *) o1->y_hist_bins)->items[i] = mp_obj_new_int(blob1->y_hist_bins[i]);
    }

    return mp_obj_is_true(mp_call_function_2(fun_obj, o0, o1));
}

static mp_obj_t py_image_find_blobs(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    list_t thresholds;
    list_init(&thresholds, sizeof(color_thresholds_list_lnk_data_t));
    py_helper_arg_to_thresholds(args[1], &thresholds);
    if (!list_size(&thresholds)) return mp_obj_new_list(0, NULL);
    bool invert = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_invert), false);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 3, kw_args, &roi);

    unsigned int x_stride =
        py_helper_keyword_int(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_stride), 2);
    PY_ASSERT_TRUE_MSG(x_stride > 0, "x_stride must not be zero.");
    unsigned int y_stride =
        py_helper_keyword_int(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_stride), 1);
    PY_ASSERT_TRUE_MSG(y_stride > 0, "y_stride must not be zero.");
    unsigned int area_threshold =
        py_helper_keyword_int(n_args, args, 6, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_area_threshold), 10);
    unsigned int pixels_threshold =
        py_helper_keyword_int(n_args, args, 7, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_pixels_threshold), 10);
    bool merge =
        py_helper_keyword_int(n_args, args, 8, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_merge), false);
    int margin =
        py_helper_keyword_int(n_args, args, 9, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_margin), 0);
    mp_obj_t threshold_cb =
        py_helper_keyword_object(n_args, args, 10, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold_cb), NULL);
    mp_obj_t merge_cb =
        py_helper_keyword_object(n_args, args, 11, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_merge_cb), NULL);
    unsigned int x_hist_bins_max =
        py_helper_keyword_int(n_args, args, 12, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_hist_bins_max), 0);
    unsigned int y_hist_bins_max =
        py_helper_keyword_int(n_args, args, 13, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_hist_bins_max), 0);

    list_t out;
    fb_alloc_mark();
    imlib_find_blobs(&out, arg_img, &roi, x_stride, y_stride, &thresholds, invert,
            area_threshold, pixels_threshold, merge, margin,
            py_image_find_blobs_threshold_cb, threshold_cb, py_image_find_blobs_merge_cb, merge_cb, x_hist_bins_max, y_hist_bins_max);
    fb_alloc_free_till_mark();
    list_free(&thresholds);

    mp_obj_list_t *objects_list = mp_obj_new_list(list_size(&out), NULL);
    for (size_t i = 0; list_size(&out); i++) {
        find_blobs_list_lnk_data_t lnk_data;
        list_pop_front(&out, &lnk_data);

        py_blob_obj_t *o = m_new_obj(py_blob_obj_t);
        o->base.type = &py_blob_type;
        o->corners = mp_obj_new_tuple(4, (mp_obj_t [])
            {mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[(FIND_BLOBS_CORNERS_RESOLUTION*0)/4].x),
                                                mp_obj_new_int(lnk_data.corners[(FIND_BLOBS_CORNERS_RESOLUTION*0)/4].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[(FIND_BLOBS_CORNERS_RESOLUTION*1)/4].x),
                                                mp_obj_new_int(lnk_data.corners[(FIND_BLOBS_CORNERS_RESOLUTION*1)/4].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[(FIND_BLOBS_CORNERS_RESOLUTION*2)/4].x),
                                                mp_obj_new_int(lnk_data.corners[(FIND_BLOBS_CORNERS_RESOLUTION*2)/4].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[(FIND_BLOBS_CORNERS_RESOLUTION*3)/4].x),
                                                mp_obj_new_int(lnk_data.corners[(FIND_BLOBS_CORNERS_RESOLUTION*3)/4].y)})});
        point_t min_corners[4];
        point_min_area_rectangle(lnk_data.corners, min_corners, FIND_BLOBS_CORNERS_RESOLUTION);
        o->min_corners = mp_obj_new_tuple(4, (mp_obj_t [])
            {mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(min_corners[0].x), mp_obj_new_int(min_corners[0].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(min_corners[1].x), mp_obj_new_int(min_corners[1].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(min_corners[2].x), mp_obj_new_int(min_corners[2].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(min_corners[3].x), mp_obj_new_int(min_corners[3].y)})});
        o->x = mp_obj_new_int(lnk_data.rect.x);
        o->y = mp_obj_new_int(lnk_data.rect.y);
        o->w = mp_obj_new_int(lnk_data.rect.w);
        o->h = mp_obj_new_int(lnk_data.rect.h);
        o->pixels = mp_obj_new_int(lnk_data.pixels);
        o->cx = mp_obj_new_float(lnk_data.centroid_x);
        o->cy = mp_obj_new_float(lnk_data.centroid_y);
        o->rotation = mp_obj_new_float(lnk_data.rotation);
        o->code = mp_obj_new_int(lnk_data.code);
        o->count = mp_obj_new_int(lnk_data.count);
        o->perimeter = mp_obj_new_int(lnk_data.perimeter);
        o->roundness = mp_obj_new_float(lnk_data.roundness);
        o->x_hist_bins = mp_obj_new_list(lnk_data.x_hist_bins_count, NULL);
        o->y_hist_bins = mp_obj_new_list(lnk_data.y_hist_bins_count, NULL);

        for (int i = 0; i < lnk_data.x_hist_bins_count; i++) {
            ((mp_obj_list_t *) o->x_hist_bins)->items[i] = mp_obj_new_int(lnk_data.x_hist_bins[i]);
        }

        for (int i = 0; i < lnk_data.y_hist_bins_count; i++) {
            ((mp_obj_list_t *) o->y_hist_bins)->items[i] = mp_obj_new_int(lnk_data.y_hist_bins[i]);
        }

        objects_list->items[i] = o;
        if (lnk_data.x_hist_bins) xfree(lnk_data.x_hist_bins);
        if (lnk_data.y_hist_bins) xfree(lnk_data.y_hist_bins);
    }

    return objects_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_find_blobs_obj, 2, py_image_find_blobs);

#ifdef IMLIB_ENABLE_FIND_LINES
static mp_obj_t py_image_find_lines(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 1, kw_args, &roi);

    unsigned int x_stride = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_stride), 2);
    PY_ASSERT_TRUE_MSG(x_stride > 0, "x_stride must not be zero.");
    unsigned int y_stride = py_helper_keyword_int(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_stride), 1);
    PY_ASSERT_TRUE_MSG(y_stride > 0, "y_stride must not be zero.");
    uint32_t threshold = py_helper_keyword_int(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), 1000);
    unsigned int theta_margin = py_helper_keyword_int(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_theta_margin), 25);
    unsigned int rho_margin = py_helper_keyword_int(n_args, args, 6, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_rho_margin), 25);

    list_t out;
    fb_alloc_mark();
    imlib_find_lines(&out, arg_img, &roi, x_stride, y_stride, threshold, theta_margin, rho_margin);
    fb_alloc_free_till_mark();

    mp_obj_list_t *objects_list = mp_obj_new_list(list_size(&out), NULL);
    for (size_t i = 0; list_size(&out); i++) {
        find_lines_list_lnk_data_t lnk_data;
        list_pop_front(&out, &lnk_data);

        py_line_obj_t *o = m_new_obj(py_line_obj_t);
        o->base.type = &py_line_type;
        o->x1 = mp_obj_new_int(lnk_data.line.x1);
        o->y1 = mp_obj_new_int(lnk_data.line.y1);
        o->x2 = mp_obj_new_int(lnk_data.line.x2);
        o->y2 = mp_obj_new_int(lnk_data.line.y2);
        int x_diff = lnk_data.line.x2 - lnk_data.line.x1;
        int y_diff = lnk_data.line.y2 - lnk_data.line.y1;
        o->length = mp_obj_new_int(fast_roundf(fast_sqrtf((x_diff * x_diff) + (y_diff * y_diff))));
        o->magnitude = mp_obj_new_int(lnk_data.magnitude);
        o->theta = mp_obj_new_int(lnk_data.theta);
        o->rho = mp_obj_new_int(lnk_data.rho);

        objects_list->items[i] = o;
    }

    return objects_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_find_lines_obj, 1, py_image_find_lines);
#endif // IMLIB_ENABLE_FIND_LINES

#ifdef IMLIB_ENABLE_FIND_LINE_SEGMENTS
static mp_obj_t py_image_find_line_segments(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 1, kw_args, &roi);

    unsigned int merge_distance = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_merge_distance), 0);
    unsigned int max_theta_diff = py_helper_keyword_int(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_max_theta_diff), 15);

    list_t out;
    fb_alloc_mark();
    imlib_lsd_find_line_segments(&out, arg_img, &roi, merge_distance, max_theta_diff);
    fb_alloc_free_till_mark();

    mp_obj_list_t *objects_list = mp_obj_new_list(list_size(&out), NULL);
    for (size_t i = 0; list_size(&out); i++) {
        find_lines_list_lnk_data_t lnk_data;
        list_pop_front(&out, &lnk_data);

        py_line_obj_t *o = m_new_obj(py_line_obj_t);
        o->base.type = &py_line_type;
        o->x1 = mp_obj_new_int(lnk_data.line.x1);
        o->y1 = mp_obj_new_int(lnk_data.line.y1);
        o->x2 = mp_obj_new_int(lnk_data.line.x2);
        o->y2 = mp_obj_new_int(lnk_data.line.y2);
        int x_diff = lnk_data.line.x2 - lnk_data.line.x1;
        int y_diff = lnk_data.line.y2 - lnk_data.line.y1;
        o->length = mp_obj_new_int(fast_roundf(fast_sqrtf((x_diff * x_diff) + (y_diff * y_diff))));
        o->magnitude = mp_obj_new_int(lnk_data.magnitude);
        o->theta = mp_obj_new_int(lnk_data.theta);
        o->rho = mp_obj_new_int(lnk_data.rho);

        objects_list->items[i] = o;
    }

    return objects_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_find_line_segments_obj, 1, py_image_find_line_segments);
#endif // IMLIB_ENABLE_FIND_LINE_SEGMENTS

#ifdef IMLIB_ENABLE_FIND_CIRCLES
// Circle Object //
#define py_circle_obj_size 4
typedef struct py_circle_obj {
    mp_obj_base_t base;
    mp_obj_t x, y, r, magnitude;
} py_circle_obj_t;

static void py_circle_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_circle_obj_t *self = self_in;
    mp_printf(print,
              "{\"x\":%d, \"y\":%d, \"r\":%d, \"magnitude\":%d}",
              mp_obj_get_int(self->x),
              mp_obj_get_int(self->y),
              mp_obj_get_int(self->r),
              mp_obj_get_int(self->magnitude));
}

static mp_obj_t py_circle_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    if (value == MP_OBJ_SENTINEL) { // load
        py_circle_obj_t *self = self_in;
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(py_circle_obj_size, index, &slice)) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
            }
            mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
            mp_seq_copy(result->items, &(self->x) + slice.start, result->len, mp_obj_t);
            return result;
        }
        switch (mp_get_index(self->base.type, py_circle_obj_size, index, false)) {
            case 0: return self->x;
            case 1: return self->y;
            case 2: return self->r;
            case 3: return self->magnitude;
        }
    }
    return MP_OBJ_NULL; // op not supported
}

mp_obj_t py_circle_circle(mp_obj_t self_in)
{
    return mp_obj_new_tuple(3, (mp_obj_t []) {((py_circle_obj_t *) self_in)->x,
                                              ((py_circle_obj_t *) self_in)->y,
                                              ((py_circle_obj_t *) self_in)->r});
}

mp_obj_t py_circle_x(mp_obj_t self_in) { return ((py_circle_obj_t *) self_in)->x; }
mp_obj_t py_circle_y(mp_obj_t self_in) { return ((py_circle_obj_t *) self_in)->y; }
mp_obj_t py_circle_r(mp_obj_t self_in) { return ((py_circle_obj_t *) self_in)->r; }
mp_obj_t py_circle_magnitude(mp_obj_t self_in) { return ((py_circle_obj_t *) self_in)->magnitude; }

STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_circle_circle_obj, py_circle_circle);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_circle_x_obj, py_circle_x);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_circle_y_obj, py_circle_y);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_circle_r_obj, py_circle_r);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_circle_magnitude_obj, py_circle_magnitude);

STATIC const mp_rom_map_elem_t py_circle_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_circle), MP_ROM_PTR(&py_circle_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&py_circle_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&py_circle_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_r), MP_ROM_PTR(&py_circle_r_obj) },
    { MP_ROM_QSTR(MP_QSTR_magnitude), MP_ROM_PTR(&py_circle_magnitude_obj) }
};

STATIC MP_DEFINE_CONST_DICT(py_circle_locals_dict, py_circle_locals_dict_table);

static const mp_obj_type_t py_circle_type = {
    { &mp_type_type },
    .name  = MP_QSTR_circle,
    .print = py_circle_print,
    .subscr = py_circle_subscr,
    .locals_dict = (mp_obj_t) &py_circle_locals_dict
};

static mp_obj_t py_image_find_circles(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 1, kw_args, &roi);

    unsigned int x_stride = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_stride), 2);
    PY_ASSERT_TRUE_MSG(x_stride > 0, "x_stride must not be zero.");
    unsigned int y_stride = py_helper_keyword_int(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_stride), 1);
    PY_ASSERT_TRUE_MSG(y_stride > 0, "y_stride must not be zero.");
    uint32_t threshold = py_helper_keyword_int(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), 2000);
    unsigned int x_margin = py_helper_keyword_int(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_x_margin), 10);
    unsigned int y_margin = py_helper_keyword_int(n_args, args, 6, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_y_margin), 10);
    unsigned int r_margin = py_helper_keyword_int(n_args, args, 7, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_r_margin), 10);
    unsigned int r_min = IM_MAX(py_helper_keyword_int(n_args, args, 8, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_r_min),
            2), 2);
    unsigned int r_max = IM_MIN(py_helper_keyword_int(n_args, args, 9, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_r_max),
            IM_MIN((roi.w / 2), (roi.h / 2))), IM_MIN((roi.w / 2), (roi.h / 2)));
    unsigned int r_step = py_helper_keyword_int(n_args, args, 10, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_r_step), 2);

    list_t out;
    fb_alloc_mark();
    imlib_find_circles(&out, arg_img, &roi, x_stride, y_stride, threshold, x_margin, y_margin, r_margin,
                       r_min, r_max, r_step);
    fb_alloc_free_till_mark();

    mp_obj_list_t *objects_list = mp_obj_new_list(list_size(&out), NULL);
    for (size_t i = 0; list_size(&out); i++) {
        find_circles_list_lnk_data_t lnk_data;
        list_pop_front(&out, &lnk_data);

        py_circle_obj_t *o = m_new_obj(py_circle_obj_t);
        o->base.type = &py_circle_type;
        o->x = mp_obj_new_int(lnk_data.p.x);
        o->y = mp_obj_new_int(lnk_data.p.y);
        o->r = mp_obj_new_int(lnk_data.r);
        o->magnitude = mp_obj_new_int(lnk_data.magnitude);

        objects_list->items[i] = o;
    }

    return objects_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_find_circles_obj, 1, py_image_find_circles);
#endif // IMLIB_ENABLE_FIND_CIRCLES

#ifdef IMLIB_ENABLE_FIND_RECTS
// Rect Object //
#define py_rect_obj_size 5
typedef struct py_rect_obj {
    mp_obj_base_t base;
    mp_obj_t corners;
    mp_obj_t x, y, w, h, magnitude;
} py_rect_obj_t;

static void py_rect_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_rect_obj_t *self = self_in;
    mp_printf(print,
              "{\"x\":%d, \"y\":%d, \"w\":%d, \"h\":%d, \"magnitude\":%d}",
              mp_obj_get_int(self->x),
              mp_obj_get_int(self->y),
              mp_obj_get_int(self->w),
              mp_obj_get_int(self->h),
              mp_obj_get_int(self->magnitude));
}

static mp_obj_t py_rect_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    if (value == MP_OBJ_SENTINEL) { // load
        py_rect_obj_t *self = self_in;
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(py_rect_obj_size, index, &slice)) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
            }
            mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
            mp_seq_copy(result->items, &(self->x) + slice.start, result->len, mp_obj_t);
            return result;
        }
        switch (mp_get_index(self->base.type, py_rect_obj_size, index, false)) {
            case 0: return self->x;
            case 1: return self->y;
            case 2: return self->w;
            case 3: return self->h;
            case 4: return self->magnitude;
        }
    }
    return MP_OBJ_NULL; // op not supported
}

mp_obj_t py_rect_corners(mp_obj_t self_in) { return ((py_rect_obj_t *) self_in)->corners; }
mp_obj_t py_rect_rect(mp_obj_t self_in)
{
    return mp_obj_new_tuple(4, (mp_obj_t []) {((py_rect_obj_t *) self_in)->x,
                                              ((py_rect_obj_t *) self_in)->y,
                                              ((py_rect_obj_t *) self_in)->w,
                                              ((py_rect_obj_t *) self_in)->h});
}

mp_obj_t py_rect_x(mp_obj_t self_in) { return ((py_rect_obj_t *) self_in)->x; }
mp_obj_t py_rect_y(mp_obj_t self_in) { return ((py_rect_obj_t *) self_in)->y; }
mp_obj_t py_rect_w(mp_obj_t self_in) { return ((py_rect_obj_t *) self_in)->w; }
mp_obj_t py_rect_h(mp_obj_t self_in) { return ((py_rect_obj_t *) self_in)->h; }
mp_obj_t py_rect_magnitude(mp_obj_t self_in) { return ((py_rect_obj_t *) self_in)->magnitude; }

STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_rect_corners_obj, py_rect_corners);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_rect_rect_obj, py_rect_rect);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_rect_x_obj, py_rect_x);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_rect_y_obj, py_rect_y);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_rect_w_obj, py_rect_w);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_rect_h_obj, py_rect_h);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_rect_magnitude_obj, py_rect_magnitude);

STATIC const mp_rom_map_elem_t py_rect_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_corners), MP_ROM_PTR(&py_rect_corners_obj) },
    { MP_ROM_QSTR(MP_QSTR_rect), MP_ROM_PTR(&py_rect_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&py_rect_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&py_rect_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_w), MP_ROM_PTR(&py_rect_w_obj) },
    { MP_ROM_QSTR(MP_QSTR_h), MP_ROM_PTR(&py_rect_h_obj) },
    { MP_ROM_QSTR(MP_QSTR_magnitude), MP_ROM_PTR(&py_rect_magnitude_obj) }
};

STATIC MP_DEFINE_CONST_DICT(py_rect_locals_dict, py_rect_locals_dict_table);

static const mp_obj_type_t py_rect_type = {
    { &mp_type_type },
    .name  = MP_QSTR_rect,
    .print = py_rect_print,
    .subscr = py_rect_subscr,
    .locals_dict = (mp_obj_t) &py_rect_locals_dict
};

static mp_obj_t py_image_find_rects(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 1, kw_args, &roi);

    uint32_t threshold = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), 1000);

    list_t out;
    fb_alloc_mark();
    imlib_find_rects(&out, arg_img, &roi, threshold);
    fb_alloc_free_till_mark();

    mp_obj_list_t *objects_list = mp_obj_new_list(list_size(&out), NULL);
    for (size_t i = 0; list_size(&out); i++) {
        find_rects_list_lnk_data_t lnk_data;
        list_pop_front(&out, &lnk_data);

        py_rect_obj_t *o = m_new_obj(py_rect_obj_t);
        o->base.type = &py_rect_type;
        o->corners = mp_obj_new_tuple(4, (mp_obj_t [])
            {mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[0].x), mp_obj_new_int(lnk_data.corners[0].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[1].x), mp_obj_new_int(lnk_data.corners[1].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[2].x), mp_obj_new_int(lnk_data.corners[2].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[3].x), mp_obj_new_int(lnk_data.corners[3].y)})});
        o->x = mp_obj_new_int(lnk_data.rect.x);
        o->y = mp_obj_new_int(lnk_data.rect.y);
        o->w = mp_obj_new_int(lnk_data.rect.w);
        o->h = mp_obj_new_int(lnk_data.rect.h);
        o->magnitude = mp_obj_new_int(lnk_data.magnitude);

        objects_list->items[i] = o;
    }

    return objects_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_find_rects_obj, 1, py_image_find_rects);
#endif // IMLIB_ENABLE_FIND_RECTS

#ifdef IMLIB_ENABLE_QRCODES
// QRCode Object //
#define py_qrcode_obj_size 10
typedef struct py_qrcode_obj {
    mp_obj_base_t base;
    mp_obj_t corners;
    mp_obj_t x, y, w, h, payload, version, ecc_level, mask, data_type, eci;
} py_qrcode_obj_t;

static void py_qrcode_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_qrcode_obj_t *self = self_in;
    mp_printf(print,
              "{\"x\":%d, \"y\":%d, \"w\":%d, \"h\":%d, \"payload\":\"%s\","
              " \"version\":%d, \"ecc_level\":%d, \"mask\":%d, \"data_type\":%d, \"eci\":%d}",
              mp_obj_get_int(self->x),
              mp_obj_get_int(self->y),
              mp_obj_get_int(self->w),
              mp_obj_get_int(self->h),
              mp_obj_str_get_str(self->payload),
              mp_obj_get_int(self->version),
              mp_obj_get_int(self->ecc_level),
              mp_obj_get_int(self->mask),
              mp_obj_get_int(self->data_type),
              mp_obj_get_int(self->eci));
}

static mp_obj_t py_qrcode_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    if (value == MP_OBJ_SENTINEL) { // load
        py_qrcode_obj_t *self = self_in;
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(py_qrcode_obj_size, index, &slice)) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
            }
            mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
            mp_seq_copy(result->items, &(self->x) + slice.start, result->len, mp_obj_t);
            return result;
        }
        switch (mp_get_index(self->base.type, py_qrcode_obj_size, index, false)) {
            case 0: return self->x;
            case 1: return self->y;
            case 2: return self->w;
            case 3: return self->h;
            case 4: return self->payload;
            case 5: return self->version;
            case 6: return self->ecc_level;
            case 7: return self->mask;
            case 8: return self->data_type;
            case 9: return self->eci;
        }
    }
    return MP_OBJ_NULL; // op not supported
}

mp_obj_t py_qrcode_corners(mp_obj_t self_in) { return ((py_qrcode_obj_t *) self_in)->corners; }
mp_obj_t py_qrcode_rect(mp_obj_t self_in)
{
    return mp_obj_new_tuple(4, (mp_obj_t []) {((py_qrcode_obj_t *) self_in)->x,
                                              ((py_qrcode_obj_t *) self_in)->y,
                                              ((py_qrcode_obj_t *) self_in)->w,
                                              ((py_qrcode_obj_t *) self_in)->h});
}

mp_obj_t py_qrcode_x(mp_obj_t self_in) { return ((py_qrcode_obj_t *) self_in)->x; }
mp_obj_t py_qrcode_y(mp_obj_t self_in) { return ((py_qrcode_obj_t *) self_in)->y; }
mp_obj_t py_qrcode_w(mp_obj_t self_in) { return ((py_qrcode_obj_t *) self_in)->w; }
mp_obj_t py_qrcode_h(mp_obj_t self_in) { return ((py_qrcode_obj_t *) self_in)->h; }
mp_obj_t py_qrcode_payload(mp_obj_t self_in) { return ((py_qrcode_obj_t *) self_in)->payload; }
mp_obj_t py_qrcode_version(mp_obj_t self_in) { return ((py_qrcode_obj_t *) self_in)->version; }
mp_obj_t py_qrcode_ecc_level(mp_obj_t self_in) { return ((py_qrcode_obj_t *) self_in)->ecc_level; }
mp_obj_t py_qrcode_mask(mp_obj_t self_in) { return ((py_qrcode_obj_t *) self_in)->mask; }
mp_obj_t py_qrcode_data_type(mp_obj_t self_in) { return ((py_qrcode_obj_t *) self_in)->data_type; }
mp_obj_t py_qrcode_eci(mp_obj_t self_in) { return ((py_qrcode_obj_t *) self_in)->eci; }
mp_obj_t py_qrcode_is_numeric(mp_obj_t self_in) { return mp_obj_new_bool(mp_obj_get_int(((py_qrcode_obj_t *) self_in)->data_type) == 1); }
mp_obj_t py_qrcode_is_alphanumeric(mp_obj_t self_in) { return mp_obj_new_bool(mp_obj_get_int(((py_qrcode_obj_t *) self_in)->data_type) == 2); }
mp_obj_t py_qrcode_is_binary(mp_obj_t self_in) { return mp_obj_new_bool(mp_obj_get_int(((py_qrcode_obj_t *) self_in)->data_type) == 4); }
mp_obj_t py_qrcode_is_kanji(mp_obj_t self_in) { return mp_obj_new_bool(mp_obj_get_int(((py_qrcode_obj_t *) self_in)->data_type) == 8); }

STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_qrcode_corners_obj, py_qrcode_corners);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_qrcode_rect_obj, py_qrcode_rect);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_qrcode_x_obj, py_qrcode_x);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_qrcode_y_obj, py_qrcode_y);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_qrcode_w_obj, py_qrcode_w);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_qrcode_h_obj, py_qrcode_h);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_qrcode_payload_obj, py_qrcode_payload);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_qrcode_version_obj, py_qrcode_version);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_qrcode_ecc_level_obj, py_qrcode_ecc_level);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_qrcode_mask_obj, py_qrcode_mask);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_qrcode_data_type_obj, py_qrcode_data_type);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_qrcode_eci_obj, py_qrcode_eci);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_qrcode_is_numeric_obj, py_qrcode_is_numeric);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_qrcode_is_alphanumeric_obj, py_qrcode_is_alphanumeric);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_qrcode_is_binary_obj, py_qrcode_is_binary);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_qrcode_is_kanji_obj, py_qrcode_is_kanji);

STATIC const mp_rom_map_elem_t py_qrcode_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_corners), MP_ROM_PTR(&py_qrcode_corners_obj) },
    { MP_ROM_QSTR(MP_QSTR_rect), MP_ROM_PTR(&py_qrcode_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&py_qrcode_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&py_qrcode_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_w), MP_ROM_PTR(&py_qrcode_w_obj) },
    { MP_ROM_QSTR(MP_QSTR_h), MP_ROM_PTR(&py_qrcode_h_obj) },
    { MP_ROM_QSTR(MP_QSTR_payload), MP_ROM_PTR(&py_qrcode_payload_obj) },
    { MP_ROM_QSTR(MP_QSTR_version), MP_ROM_PTR(&py_qrcode_version_obj) },
    { MP_ROM_QSTR(MP_QSTR_ecc_level), MP_ROM_PTR(&py_qrcode_ecc_level_obj) },
    { MP_ROM_QSTR(MP_QSTR_mask), MP_ROM_PTR(&py_qrcode_mask_obj) },
    { MP_ROM_QSTR(MP_QSTR_data_type), MP_ROM_PTR(&py_qrcode_data_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_eci), MP_ROM_PTR(&py_qrcode_eci_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_numeric), MP_ROM_PTR(&py_qrcode_is_numeric_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_alphanumeric), MP_ROM_PTR(&py_qrcode_is_alphanumeric_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_binary), MP_ROM_PTR(&py_qrcode_is_binary_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_kanji), MP_ROM_PTR(&py_qrcode_is_kanji_obj) }
};

STATIC MP_DEFINE_CONST_DICT(py_qrcode_locals_dict, py_qrcode_locals_dict_table);

static const mp_obj_type_t py_qrcode_type = {
    { &mp_type_type },
    .name  = MP_QSTR_qrcode,
    .print = py_qrcode_print,
    .subscr = py_qrcode_subscr,
    .locals_dict = (mp_obj_t) &py_qrcode_locals_dict
};

static mp_obj_t py_image_find_qrcodes(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 1, kw_args, &roi);

    list_t out;
    fb_alloc_mark();
    imlib_find_qrcodes(&out, arg_img, &roi);
    fb_alloc_free_till_mark();

    mp_obj_list_t *objects_list = mp_obj_new_list(list_size(&out), NULL);
    for (size_t i = 0; list_size(&out); i++) {
        find_qrcodes_list_lnk_data_t lnk_data;
        list_pop_front(&out, &lnk_data);

        py_qrcode_obj_t *o = m_new_obj(py_qrcode_obj_t);
        o->base.type = &py_qrcode_type;
        o->corners = mp_obj_new_tuple(4, (mp_obj_t [])
            {mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[0].x), mp_obj_new_int(lnk_data.corners[0].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[1].x), mp_obj_new_int(lnk_data.corners[1].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[2].x), mp_obj_new_int(lnk_data.corners[2].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[3].x), mp_obj_new_int(lnk_data.corners[3].y)})});
        o->x = mp_obj_new_int(lnk_data.rect.x);
        o->y = mp_obj_new_int(lnk_data.rect.y);
        o->w = mp_obj_new_int(lnk_data.rect.w);
        o->h = mp_obj_new_int(lnk_data.rect.h);
        o->payload = mp_obj_new_str(lnk_data.payload, lnk_data.payload_len);
        o->version = mp_obj_new_int(lnk_data.version);
        o->ecc_level = mp_obj_new_int(lnk_data.ecc_level);
        o->mask = mp_obj_new_int(lnk_data.mask);
        o->data_type = mp_obj_new_int(lnk_data.data_type);
        o->eci = mp_obj_new_int(lnk_data.eci);

        objects_list->items[i] = o;
        xfree(lnk_data.payload);
    }

    return objects_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_find_qrcodes_obj, 1, py_image_find_qrcodes);
#endif // IMLIB_ENABLE_QRCODES

#ifdef IMLIB_ENABLE_APRILTAGS
// AprilTag Object //
#define py_apriltag_obj_size 18
typedef struct py_apriltag_obj {
    mp_obj_base_t base;
    mp_obj_t corners;
    mp_obj_t x, y, w, h, id, family, cx, cy, rotation, decision_margin, hamming, goodness;
    mp_obj_t x_translation, y_translation, z_translation;
    mp_obj_t x_rotation, y_rotation, z_rotation;
} py_apriltag_obj_t;

static void py_apriltag_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_apriltag_obj_t *self = self_in;
    mp_printf(print,
              "{\"x\":%d, \"y\":%d, \"w\":%d, \"h\":%d, \"id\":%d,"
              " \"family\":%d, \"cx\":%d, \"cy\":%d, \"rotation\":%f, \"decision_margin\":%f, \"hamming\":%d, \"goodness\":%f,"
              " \"x_translation\":%f, \"y_translation\":%f, \"z_translation\":%f,"
              " \"x_rotation\":%f, \"y_rotation\":%f, \"z_rotation\":%f}",
              mp_obj_get_int(self->x),
              mp_obj_get_int(self->y),
              mp_obj_get_int(self->w),
              mp_obj_get_int(self->h),
              mp_obj_get_int(self->id),
              mp_obj_get_int(self->family),
              mp_obj_get_int(self->cx),
              mp_obj_get_int(self->cy),
              (double) mp_obj_get_float(self->rotation),
              (double) mp_obj_get_float(self->decision_margin),
              mp_obj_get_int(self->hamming),
              (double) mp_obj_get_float(self->goodness),
              (double) mp_obj_get_float(self->x_translation),
              (double) mp_obj_get_float(self->y_translation),
              (double) mp_obj_get_float(self->z_translation),
              (double) mp_obj_get_float(self->x_rotation),
              (double) mp_obj_get_float(self->y_rotation),
              (double) mp_obj_get_float(self->z_rotation));
}

static mp_obj_t py_apriltag_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    if (value == MP_OBJ_SENTINEL) { // load
        py_apriltag_obj_t *self = self_in;
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(py_apriltag_obj_size, index, &slice)) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
            }
            mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
            mp_seq_copy(result->items, &(self->x) + slice.start, result->len, mp_obj_t);
            return result;
        }
        switch (mp_get_index(self->base.type, py_apriltag_obj_size, index, false)) {
            case 0: return self->x;
            case 1: return self->y;
            case 2: return self->w;
            case 3: return self->h;
            case 4: return self->id;
            case 5: return self->family;
            case 6: return self->cx;
            case 7: return self->cy;
            case 8: return self->rotation;
            case 9: return self->decision_margin;
            case 10: return self->hamming;
            case 11: return self->goodness;
            case 12: return self->x_translation;
            case 13: return self->y_translation;
            case 14: return self->z_translation;
            case 15: return self->x_rotation;
            case 16: return self->y_rotation;
            case 17: return self->z_rotation;
        }
    }
    return MP_OBJ_NULL; // op not supported
}

mp_obj_t py_apriltag_corners(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->corners; }
mp_obj_t py_apriltag_rect(mp_obj_t self_in)
{
    return mp_obj_new_tuple(4, (mp_obj_t []) {((py_apriltag_obj_t *) self_in)->x,
                                              ((py_apriltag_obj_t *) self_in)->y,
                                              ((py_apriltag_obj_t *) self_in)->w,
                                              ((py_apriltag_obj_t *) self_in)->h});
}

mp_obj_t py_apriltag_x(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->x; }
mp_obj_t py_apriltag_y(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->y; }
mp_obj_t py_apriltag_w(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->w; }
mp_obj_t py_apriltag_h(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->h; }
mp_obj_t py_apriltag_id(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->id; }
mp_obj_t py_apriltag_family(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->family; }
mp_obj_t py_apriltag_cx(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->cx; }
mp_obj_t py_apriltag_cy(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->cy; }
mp_obj_t py_apriltag_rotation(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->rotation; }
mp_obj_t py_apriltag_decision_margin(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->decision_margin; }
mp_obj_t py_apriltag_hamming(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->hamming; }
mp_obj_t py_apriltag_goodness(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->goodness; }
mp_obj_t py_apriltag_x_translation(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->x_translation; }
mp_obj_t py_apriltag_y_translation(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->y_translation; }
mp_obj_t py_apriltag_z_translation(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->z_translation; }
mp_obj_t py_apriltag_x_rotation(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->x_rotation; }
mp_obj_t py_apriltag_y_rotation(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->y_rotation; }
mp_obj_t py_apriltag_z_rotation(mp_obj_t self_in) { return ((py_apriltag_obj_t *) self_in)->z_rotation; }

STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_corners_obj, py_apriltag_corners);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_rect_obj, py_apriltag_rect);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_x_obj, py_apriltag_x);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_y_obj, py_apriltag_y);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_w_obj, py_apriltag_w);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_h_obj, py_apriltag_h);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_id_obj, py_apriltag_id);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_family_obj, py_apriltag_family);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_cx_obj, py_apriltag_cx);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_cy_obj, py_apriltag_cy);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_rotation_obj, py_apriltag_rotation);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_decision_margin_obj, py_apriltag_decision_margin);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_hamming_obj, py_apriltag_hamming);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_goodness_obj, py_apriltag_goodness);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_x_translation_obj, py_apriltag_x_translation);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_y_translation_obj, py_apriltag_y_translation);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_z_translation_obj, py_apriltag_z_translation);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_x_rotation_obj, py_apriltag_x_rotation);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_y_rotation_obj, py_apriltag_y_rotation);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_apriltag_z_rotation_obj, py_apriltag_z_rotation);

STATIC const mp_rom_map_elem_t py_apriltag_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_corners), MP_ROM_PTR(&py_apriltag_corners_obj) },
    { MP_ROM_QSTR(MP_QSTR_rect), MP_ROM_PTR(&py_apriltag_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&py_apriltag_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&py_apriltag_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_w), MP_ROM_PTR(&py_apriltag_w_obj) },
    { MP_ROM_QSTR(MP_QSTR_h), MP_ROM_PTR(&py_apriltag_h_obj) },
    { MP_ROM_QSTR(MP_QSTR_id), MP_ROM_PTR(&py_apriltag_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_family), MP_ROM_PTR(&py_apriltag_family_obj) },
    { MP_ROM_QSTR(MP_QSTR_cx), MP_ROM_PTR(&py_apriltag_cx_obj) },
    { MP_ROM_QSTR(MP_QSTR_cy), MP_ROM_PTR(&py_apriltag_cy_obj) },
    { MP_ROM_QSTR(MP_QSTR_rotation), MP_ROM_PTR(&py_apriltag_rotation_obj) },
    { MP_ROM_QSTR(MP_QSTR_decision_margin), MP_ROM_PTR(&py_apriltag_decision_margin_obj) },
    { MP_ROM_QSTR(MP_QSTR_hamming), MP_ROM_PTR(&py_apriltag_hamming_obj) },
    { MP_ROM_QSTR(MP_QSTR_goodness), MP_ROM_PTR(&py_apriltag_goodness_obj) },
    { MP_ROM_QSTR(MP_QSTR_x_translation), MP_ROM_PTR(&py_apriltag_x_translation_obj) },
    { MP_ROM_QSTR(MP_QSTR_y_translation), MP_ROM_PTR(&py_apriltag_y_translation_obj) },
    { MP_ROM_QSTR(MP_QSTR_z_translation), MP_ROM_PTR(&py_apriltag_z_translation_obj) },
    { MP_ROM_QSTR(MP_QSTR_x_rotation), MP_ROM_PTR(&py_apriltag_x_rotation_obj) },
    { MP_ROM_QSTR(MP_QSTR_y_rotation), MP_ROM_PTR(&py_apriltag_y_rotation_obj) },
    { MP_ROM_QSTR(MP_QSTR_z_rotation), MP_ROM_PTR(&py_apriltag_z_rotation_obj) }
};

STATIC MP_DEFINE_CONST_DICT(py_apriltag_locals_dict, py_apriltag_locals_dict_table);

static const mp_obj_type_t py_apriltag_type = {
    { &mp_type_type },
    .name  = MP_QSTR_apriltag,
    .print = py_apriltag_print,
    .subscr = py_apriltag_subscr,
    .locals_dict = (mp_obj_t) &py_apriltag_locals_dict
};

static mp_obj_t py_image_find_apriltags(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable_bayer(args[0]);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 1, kw_args, &roi);
#ifndef IMLIB_ENABLE_HIGH_RES_APRILTAGS
    PY_ASSERT_TRUE_MSG((roi.w * roi.h) < 65536, "The maximum supported resolution for find_apriltags() is < 64K pixels.");
#endif
    if ((roi.w < 4) || (roi.h < 4)) {
        return mp_obj_new_list(0, NULL);
    }

    apriltag_families_t families = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_families), TAG36H11);
    // 2.8mm Focal Length w/ OV7725 sensor for reference.
    float fx = py_helper_keyword_float(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_fx), (2.8 / 3.984) * arg_img->w);
    // 2.8mm Focal Length w/ OV7725 sensor for reference.
    float fy = py_helper_keyword_float(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_fy), (2.8 / 2.952) * arg_img->h);
    // Use the image versus the roi here since the image should be projected from the camera center.
    float cx = py_helper_keyword_float(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_cx), arg_img->w * 0.5);
    // Use the image versus the roi here since the image should be projected from the camera center.
    float cy = py_helper_keyword_float(n_args, args, 6, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_cy), arg_img->h * 0.5);

    list_t out;
    fb_alloc_mark();
    imlib_find_apriltags(&out, arg_img, &roi, families, fx, fy, cx, cy);
    fb_alloc_free_till_mark();

    mp_obj_list_t *objects_list = mp_obj_new_list(list_size(&out), NULL);
    for (size_t i = 0; list_size(&out); i++) {
        find_apriltags_list_lnk_data_t lnk_data;
        list_pop_front(&out, &lnk_data);

        py_apriltag_obj_t *o = m_new_obj(py_apriltag_obj_t);
        o->base.type = &py_apriltag_type;
        o->corners = mp_obj_new_tuple(4, (mp_obj_t [])
            {mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[0].x), mp_obj_new_int(lnk_data.corners[0].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[1].x), mp_obj_new_int(lnk_data.corners[1].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[2].x), mp_obj_new_int(lnk_data.corners[2].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[3].x), mp_obj_new_int(lnk_data.corners[3].y)})});
        o->x = mp_obj_new_int(lnk_data.rect.x);
        o->y = mp_obj_new_int(lnk_data.rect.y);
        o->w = mp_obj_new_int(lnk_data.rect.w);
        o->h = mp_obj_new_int(lnk_data.rect.h);
        o->id = mp_obj_new_int(lnk_data.id);
        o->family = mp_obj_new_int(lnk_data.family);
        o->cx = mp_obj_new_int(lnk_data.centroid.x);
        o->cy = mp_obj_new_int(lnk_data.centroid.y);
        o->rotation = mp_obj_new_float(lnk_data.z_rotation);
        o->decision_margin = mp_obj_new_float(lnk_data.decision_margin);
        o->hamming = mp_obj_new_int(lnk_data.hamming);
        o->goodness = mp_obj_new_float(lnk_data.goodness);
        o->x_translation = mp_obj_new_float(lnk_data.x_translation);
        o->y_translation = mp_obj_new_float(lnk_data.y_translation);
        o->z_translation = mp_obj_new_float(lnk_data.z_translation);
        o->x_rotation = mp_obj_new_float(lnk_data.x_rotation);
        o->y_rotation = mp_obj_new_float(lnk_data.y_rotation);
        o->z_rotation = mp_obj_new_float(lnk_data.z_rotation);

        objects_list->items[i] = o;
    }

    return objects_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_find_apriltags_obj, 1, py_image_find_apriltags);
#endif // IMLIB_ENABLE_APRILTAGS

#ifdef IMLIB_ENABLE_DATAMATRICES
// DataMatrix Object //
#define py_datamatrix_obj_size 10
typedef struct py_datamatrix_obj {
    mp_obj_base_t base;
    mp_obj_t corners;
    mp_obj_t x, y, w, h, payload, rotation, rows, columns, capacity, padding;
} py_datamatrix_obj_t;

static void py_datamatrix_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_datamatrix_obj_t *self = self_in;
    mp_printf(print,
              "{\"x\":%d, \"y\":%d, \"w\":%d, \"h\":%d, \"payload\":\"%s\","
              " \"rotation\":%f, \"rows\":%d, \"columns\":%d, \"capacity\":%d, \"padding\":%d}",
              mp_obj_get_int(self->x),
              mp_obj_get_int(self->y),
              mp_obj_get_int(self->w),
              mp_obj_get_int(self->h),
              mp_obj_str_get_str(self->payload),
              (double) mp_obj_get_float(self->rotation),
              mp_obj_get_int(self->rows),
              mp_obj_get_int(self->columns),
              mp_obj_get_int(self->capacity),
              mp_obj_get_int(self->padding));
}

static mp_obj_t py_datamatrix_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    if (value == MP_OBJ_SENTINEL) { // load
        py_datamatrix_obj_t *self = self_in;
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(py_datamatrix_obj_size, index, &slice)) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
            }
            mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
            mp_seq_copy(result->items, &(self->x) + slice.start, result->len, mp_obj_t);
            return result;
        }
        switch (mp_get_index(self->base.type, py_datamatrix_obj_size, index, false)) {
            case 0: return self->x;
            case 1: return self->y;
            case 2: return self->w;
            case 3: return self->h;
            case 4: return self->payload;
            case 5: return self->rotation;
            case 6: return self->rows;
            case 7: return self->columns;
            case 8: return self->capacity;
            case 9: return self->padding;
        }
    }
    return MP_OBJ_NULL; // op not supported
}

mp_obj_t py_datamatrix_corners(mp_obj_t self_in) { return ((py_datamatrix_obj_t *) self_in)->corners; }
mp_obj_t py_datamatrix_rect(mp_obj_t self_in)
{
    return mp_obj_new_tuple(4, (mp_obj_t []) {((py_datamatrix_obj_t *) self_in)->x,
                                              ((py_datamatrix_obj_t *) self_in)->y,
                                              ((py_datamatrix_obj_t *) self_in)->w,
                                              ((py_datamatrix_obj_t *) self_in)->h});
}

mp_obj_t py_datamatrix_x(mp_obj_t self_in) { return ((py_datamatrix_obj_t *) self_in)->x; }
mp_obj_t py_datamatrix_y(mp_obj_t self_in) { return ((py_datamatrix_obj_t *) self_in)->y; }
mp_obj_t py_datamatrix_w(mp_obj_t self_in) { return ((py_datamatrix_obj_t *) self_in)->w; }
mp_obj_t py_datamatrix_h(mp_obj_t self_in) { return ((py_datamatrix_obj_t *) self_in)->h; }
mp_obj_t py_datamatrix_payload(mp_obj_t self_in) { return ((py_datamatrix_obj_t *) self_in)->payload; }
mp_obj_t py_datamatrix_rotation(mp_obj_t self_in) { return ((py_datamatrix_obj_t *) self_in)->rotation; }
mp_obj_t py_datamatrix_rows(mp_obj_t self_in) { return ((py_datamatrix_obj_t *) self_in)->rows; }
mp_obj_t py_datamatrix_columns(mp_obj_t self_in) { return ((py_datamatrix_obj_t *) self_in)->columns; }
mp_obj_t py_datamatrix_capacity(mp_obj_t self_in) { return ((py_datamatrix_obj_t *) self_in)->capacity; }
mp_obj_t py_datamatrix_padding(mp_obj_t self_in) { return ((py_datamatrix_obj_t *) self_in)->padding; }

STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_datamatrix_corners_obj, py_datamatrix_corners);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_datamatrix_rect_obj, py_datamatrix_rect);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_datamatrix_x_obj, py_datamatrix_x);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_datamatrix_y_obj, py_datamatrix_y);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_datamatrix_w_obj, py_datamatrix_w);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_datamatrix_h_obj, py_datamatrix_h);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_datamatrix_payload_obj, py_datamatrix_payload);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_datamatrix_rotation_obj, py_datamatrix_rotation);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_datamatrix_rows_obj, py_datamatrix_rows);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_datamatrix_columns_obj, py_datamatrix_columns);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_datamatrix_capacity_obj, py_datamatrix_capacity);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_datamatrix_padding_obj, py_datamatrix_padding);

STATIC const mp_rom_map_elem_t py_datamatrix_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_corners), MP_ROM_PTR(&py_datamatrix_corners_obj) },
    { MP_ROM_QSTR(MP_QSTR_rect), MP_ROM_PTR(&py_datamatrix_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&py_datamatrix_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&py_datamatrix_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_w), MP_ROM_PTR(&py_datamatrix_w_obj) },
    { MP_ROM_QSTR(MP_QSTR_h), MP_ROM_PTR(&py_datamatrix_h_obj) },
    { MP_ROM_QSTR(MP_QSTR_payload), MP_ROM_PTR(&py_datamatrix_payload_obj) },
    { MP_ROM_QSTR(MP_QSTR_rotation), MP_ROM_PTR(&py_datamatrix_rotation_obj) },
    { MP_ROM_QSTR(MP_QSTR_rows), MP_ROM_PTR(&py_datamatrix_rows_obj) },
    { MP_ROM_QSTR(MP_QSTR_columns), MP_ROM_PTR(&py_datamatrix_columns_obj) },
    { MP_ROM_QSTR(MP_QSTR_capacity), MP_ROM_PTR(&py_datamatrix_capacity_obj) },
    { MP_ROM_QSTR(MP_QSTR_padding), MP_ROM_PTR(&py_datamatrix_padding_obj) }
};

STATIC MP_DEFINE_CONST_DICT(py_datamatrix_locals_dict, py_datamatrix_locals_dict_table);

static const mp_obj_type_t py_datamatrix_type = {
    { &mp_type_type },
    .name  = MP_QSTR_datamatrix,
    .print = py_datamatrix_print,
    .subscr = py_datamatrix_subscr,
    .locals_dict = (mp_obj_t) &py_datamatrix_locals_dict
};

static mp_obj_t py_image_find_datamatrices(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 1, kw_args, &roi);

    int effort = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_effort), 200);

    list_t out;
    fb_alloc_mark();
    imlib_find_datamatrices(&out, arg_img, &roi, effort);
    fb_alloc_free_till_mark();

    mp_obj_list_t *objects_list = mp_obj_new_list(list_size(&out), NULL);
    for (size_t i = 0; list_size(&out); i++) {
        find_datamatrices_list_lnk_data_t lnk_data;
        list_pop_front(&out, &lnk_data);

        py_datamatrix_obj_t *o = m_new_obj(py_datamatrix_obj_t);
        o->base.type = &py_datamatrix_type;
        o->corners = mp_obj_new_tuple(4, (mp_obj_t [])
            {mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[0].x), mp_obj_new_int(lnk_data.corners[0].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[1].x), mp_obj_new_int(lnk_data.corners[1].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[2].x), mp_obj_new_int(lnk_data.corners[2].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[3].x), mp_obj_new_int(lnk_data.corners[3].y)})});
        o->x = mp_obj_new_int(lnk_data.rect.x);
        o->y = mp_obj_new_int(lnk_data.rect.y);
        o->w = mp_obj_new_int(lnk_data.rect.w);
        o->h = mp_obj_new_int(lnk_data.rect.h);
        o->payload = mp_obj_new_str(lnk_data.payload, lnk_data.payload_len);
        o->rotation = mp_obj_new_float(IM_DEG2RAD(lnk_data.rotation));
        o->rows = mp_obj_new_int(lnk_data.rows);
        o->columns = mp_obj_new_int(lnk_data.columns);
        o->capacity = mp_obj_new_int(lnk_data.capacity);
        o->padding = mp_obj_new_int(lnk_data.padding);

        objects_list->items[i] = o;
        xfree(lnk_data.payload);
    }

    return objects_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_find_datamatrices_obj, 1, py_image_find_datamatrices);
#endif // IMLIB_ENABLE_DATAMATRICES

#ifdef IMLIB_ENABLE_BARCODES
// BarCode Object //
#define py_barcode_obj_size 8
typedef struct py_barcode_obj {
    mp_obj_base_t base;
    mp_obj_t corners;
    mp_obj_t x, y, w, h, payload, type, rotation, quality;
} py_barcode_obj_t;

static void py_barcode_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_barcode_obj_t *self = self_in;
    mp_printf(print,
              "{\"x\":%d, \"y\":%d, \"w\":%d, \"h\":%d, \"payload\":\"%s\","
              " \"type\":%d, \"rotation\":%f, \"quality\":%d}",
              mp_obj_get_int(self->x),
              mp_obj_get_int(self->y),
              mp_obj_get_int(self->w),
              mp_obj_get_int(self->h),
              mp_obj_str_get_str(self->payload),
              mp_obj_get_int(self->type),
              (double) mp_obj_get_float(self->rotation),
              mp_obj_get_int(self->quality));
}

static mp_obj_t py_barcode_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    if (value == MP_OBJ_SENTINEL) { // load
        py_barcode_obj_t *self = self_in;
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(py_barcode_obj_size, index, &slice)) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
            }
            mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
            mp_seq_copy(result->items, &(self->x) + slice.start, result->len, mp_obj_t);
            return result;
        }
        switch (mp_get_index(self->base.type, py_barcode_obj_size, index, false)) {
            case 0: return self->x;
            case 1: return self->y;
            case 2: return self->w;
            case 3: return self->h;
            case 4: return self->payload;
            case 5: return self->type;
            case 6: return self->rotation;
            case 7: return self->quality;
        }
    }
    return MP_OBJ_NULL; // op not supported
}

mp_obj_t py_barcode_corners(mp_obj_t self_in) { return ((py_barcode_obj_t *) self_in)->corners; }
mp_obj_t py_barcode_rect(mp_obj_t self_in)
{
    return mp_obj_new_tuple(4, (mp_obj_t []) {((py_barcode_obj_t *) self_in)->x,
                                              ((py_barcode_obj_t *) self_in)->y,
                                              ((py_barcode_obj_t *) self_in)->w,
                                              ((py_barcode_obj_t *) self_in)->h});
}

mp_obj_t py_barcode_x(mp_obj_t self_in) { return ((py_barcode_obj_t *) self_in)->x; }
mp_obj_t py_barcode_y(mp_obj_t self_in) { return ((py_barcode_obj_t *) self_in)->y; }
mp_obj_t py_barcode_w(mp_obj_t self_in) { return ((py_barcode_obj_t *) self_in)->w; }
mp_obj_t py_barcode_h(mp_obj_t self_in) { return ((py_barcode_obj_t *) self_in)->h; }
mp_obj_t py_barcode_payload_fun(mp_obj_t self_in) { return ((py_barcode_obj_t *) self_in)->payload; }
mp_obj_t py_barcode_type_fun(mp_obj_t self_in) { return ((py_barcode_obj_t *) self_in)->type; }
mp_obj_t py_barcode_rotation_fun(mp_obj_t self_in) { return ((py_barcode_obj_t *) self_in)->rotation; }
mp_obj_t py_barcode_quality_fun(mp_obj_t self_in) { return ((py_barcode_obj_t *) self_in)->quality; }

STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_barcode_corners_obj, py_barcode_corners);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_barcode_rect_obj, py_barcode_rect);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_barcode_x_obj, py_barcode_x);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_barcode_y_obj, py_barcode_y);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_barcode_w_obj, py_barcode_w);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_barcode_h_obj, py_barcode_h);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_barcode_payload_fun_obj, py_barcode_payload_fun);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_barcode_type_fun_obj, py_barcode_type_fun);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_barcode_rotation_fun_obj, py_barcode_rotation_fun);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_barcode_quality_fun_obj, py_barcode_quality_fun);

STATIC const mp_rom_map_elem_t py_barcode_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_corners), MP_ROM_PTR(&py_barcode_corners_obj) },
    { MP_ROM_QSTR(MP_QSTR_rect), MP_ROM_PTR(&py_barcode_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&py_barcode_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&py_barcode_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_w), MP_ROM_PTR(&py_barcode_w_obj) },
    { MP_ROM_QSTR(MP_QSTR_h), MP_ROM_PTR(&py_barcode_h_obj) },
    { MP_ROM_QSTR(MP_QSTR_payload), MP_ROM_PTR(&py_barcode_payload_fun_obj) },
    { MP_ROM_QSTR(MP_QSTR_type), MP_ROM_PTR(&py_barcode_type_fun_obj) },
    { MP_ROM_QSTR(MP_QSTR_rotation), MP_ROM_PTR(&py_barcode_rotation_fun_obj) },
    { MP_ROM_QSTR(MP_QSTR_quality), MP_ROM_PTR(&py_barcode_quality_fun_obj) }
};

STATIC MP_DEFINE_CONST_DICT(py_barcode_locals_dict, py_barcode_locals_dict_table);

static const mp_obj_type_t py_barcode_type = {
    { &mp_type_type },
    .name  = MP_QSTR_barcode,
    .print = py_barcode_print,
    .subscr = py_barcode_subscr,
    .locals_dict = (mp_obj_t) &py_barcode_locals_dict
};

static mp_obj_t py_image_find_barcodes(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 1, kw_args, &roi);

    list_t out;
    fb_alloc_mark();
    imlib_find_barcodes(&out, arg_img, &roi);
    fb_alloc_free_till_mark();

    mp_obj_list_t *objects_list = mp_obj_new_list(list_size(&out), NULL);
    for (size_t i = 0; list_size(&out); i++) {
        find_barcodes_list_lnk_data_t lnk_data;
        list_pop_front(&out, &lnk_data);

        py_barcode_obj_t *o = m_new_obj(py_barcode_obj_t);
        o->base.type = &py_barcode_type;
        o->corners = mp_obj_new_tuple(4, (mp_obj_t [])
            {mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[0].x), mp_obj_new_int(lnk_data.corners[0].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[1].x), mp_obj_new_int(lnk_data.corners[1].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[2].x), mp_obj_new_int(lnk_data.corners[2].y)}),
             mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_int(lnk_data.corners[3].x), mp_obj_new_int(lnk_data.corners[3].y)})});
        o->x = mp_obj_new_int(lnk_data.rect.x);
        o->y = mp_obj_new_int(lnk_data.rect.y);
        o->w = mp_obj_new_int(lnk_data.rect.w);
        o->h = mp_obj_new_int(lnk_data.rect.h);
        o->payload = mp_obj_new_str(lnk_data.payload, lnk_data.payload_len);
        o->type = mp_obj_new_int(lnk_data.type);
        o->rotation = mp_obj_new_float(IM_DEG2RAD(lnk_data.rotation));
        o->quality = mp_obj_new_int(lnk_data.quality);

        objects_list->items[i] = o;
        xfree(lnk_data.payload);
    }

    return objects_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_find_barcodes_obj, 1, py_image_find_barcodes);
#endif // IMLIB_ENABLE_BARCODES

#ifdef IMLIB_ENABLE_FIND_DISPLACEMENT
// Displacement Object //
#define py_displacement_obj_size 5
typedef struct py_displacement_obj {
    mp_obj_base_t base;
    mp_obj_t x_translation, y_translation, rotation, scale, response;
} py_displacement_obj_t;

static void py_displacement_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_displacement_obj_t *self = self_in;
    mp_printf(print,
              "{\"x_translation\":%f, \"y_translation\":%f, \"rotation\":%f, \"scale\":%f, \"response\":%f}",
              (double) mp_obj_get_float(self->x_translation),
              (double) mp_obj_get_float(self->y_translation),
              (double) mp_obj_get_float(self->rotation),
              (double) mp_obj_get_float(self->scale),
              (double) mp_obj_get_float(self->response));
}

static mp_obj_t py_displacement_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value)
{
    if (value == MP_OBJ_SENTINEL) { // load
        py_displacement_obj_t *self = self_in;
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(py_displacement_obj_size, index, &slice)) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
            }
            mp_obj_tuple_t *result = mp_obj_new_tuple(slice.stop - slice.start, NULL);
            mp_seq_copy(result->items, &(self->x_translation) + slice.start, result->len, mp_obj_t);
            return result;
        }
        switch (mp_get_index(self->base.type, py_displacement_obj_size, index, false)) {
            case 0: return self->x_translation;
            case 1: return self->y_translation;
            case 2: return self->rotation;
            case 3: return self->scale;
            case 4: return self->response;
        }
    }
    return MP_OBJ_NULL; // op not supported
}

mp_obj_t py_displacement_x_translation(mp_obj_t self_in) { return ((py_displacement_obj_t *) self_in)->x_translation; }
mp_obj_t py_displacement_y_translation(mp_obj_t self_in) { return ((py_displacement_obj_t *) self_in)->y_translation; }
mp_obj_t py_displacement_rotation(mp_obj_t self_in) { return ((py_displacement_obj_t *) self_in)->rotation; }
mp_obj_t py_displacement_scale(mp_obj_t self_in) { return ((py_displacement_obj_t *) self_in)->scale; }
mp_obj_t py_displacement_response(mp_obj_t self_in) { return ((py_displacement_obj_t *) self_in)->response; }

STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_displacement_x_translation_obj, py_displacement_x_translation);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_displacement_y_translation_obj, py_displacement_y_translation);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_displacement_rotation_obj, py_displacement_rotation);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_displacement_scale_obj, py_displacement_scale);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_displacement_response_obj, py_displacement_response);

STATIC const mp_rom_map_elem_t py_displacement_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_x_translation), MP_ROM_PTR(&py_displacement_x_translation_obj) },
    { MP_ROM_QSTR(MP_QSTR_y_translation), MP_ROM_PTR(&py_displacement_y_translation_obj) },
    { MP_ROM_QSTR(MP_QSTR_rotation), MP_ROM_PTR(&py_displacement_rotation_obj) },
    { MP_ROM_QSTR(MP_QSTR_scale), MP_ROM_PTR(&py_displacement_scale_obj) },
    { MP_ROM_QSTR(MP_QSTR_response), MP_ROM_PTR(&py_displacement_response_obj) }
};

STATIC MP_DEFINE_CONST_DICT(py_displacement_locals_dict, py_displacement_locals_dict_table);

static const mp_obj_type_t py_displacement_type = {
    { &mp_type_type },
    .name  = MP_QSTR_displacement,
    .print = py_displacement_print,
    .subscr = py_displacement_subscr,
    .locals_dict = (mp_obj_t) &py_displacement_locals_dict
};

static mp_obj_t py_image_find_displacement(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);
    image_t *arg_template_img = py_helper_arg_to_image_mutable(args[1]);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 2, kw_args, &roi);

    rectangle_t template_roi;
    py_helper_keyword_rectangle(arg_template_img, n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_template_roi), &template_roi);

    PY_ASSERT_FALSE_MSG((roi.w != template_roi.w) || (roi.h != template_roi.h), "ROI(w,h) != TEMPLATE_ROI(w,h)");

    bool logpolar = py_helper_keyword_int(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_logpolar), false);
    bool fix_rotation_scale = py_helper_keyword_int(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_fix_rotation_scale), false);

    float x, y, r, s, response;
    fb_alloc_mark();
    imlib_phasecorrelate(arg_img, arg_template_img, &roi, &template_roi, logpolar, fix_rotation_scale, &x, &y, &r, &s, &response);
    fb_alloc_free_till_mark();

    py_displacement_obj_t *o = m_new_obj(py_displacement_obj_t);
    o->base.type = &py_displacement_type;
    o->x_translation = mp_obj_new_float(x);
    o->y_translation = mp_obj_new_float(y);
    o->rotation = mp_obj_new_float(r);
    o->scale = mp_obj_new_float(s);
    o->response = mp_obj_new_float(response);

    return o;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_find_displacement_obj, 2, py_image_find_displacement);
#endif // IMLIB_ENABLE_FIND_DISPLACEMENT

#ifdef IMLIB_FIND_TEMPLATE
static mp_obj_t py_image_find_template(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_grayscale(args[0]);
    image_t *arg_template = py_helper_arg_to_image_grayscale(args[1]);
    float arg_thresh = mp_obj_get_float(args[2]);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 3, kw_args, &roi);

    // Make sure ROI is bigger than or equal to template size
    PY_ASSERT_TRUE_MSG((roi.w >= arg_template->w && roi.h >= arg_template->h),
            "Region of interest is smaller than template!");

    // Make sure ROI is smaller than or equal to image size
    PY_ASSERT_TRUE_MSG(((roi.x + roi.w) <= arg_img->w && (roi.y + roi.h) <= arg_img->h),
            "Region of interest is bigger than image!");

    int step = py_helper_keyword_int(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_step), 2);
    int search = py_helper_keyword_int(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_search), SEARCH_EX);

    // Find template
    rectangle_t r;
    float corr;
    fb_alloc_mark();
    if (search == SEARCH_DS) {
        corr = imlib_template_match_ds(arg_img, arg_template, &r);
    } else {
        corr = imlib_template_match_ex(arg_img, arg_template, &roi, step, &r);
    }
    fb_alloc_free_till_mark();

    if (corr > arg_thresh) {
        mp_obj_t rec_obj[4] = {
            mp_obj_new_int(r.x),
            mp_obj_new_int(r.y),
            mp_obj_new_int(r.w),
            mp_obj_new_int(r.h)
        };
        return mp_obj_new_tuple(4, rec_obj);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_find_template_obj, 3, py_image_find_template);
#endif // IMLIB_FIND_TEMPLATE

static mp_obj_t py_image_find_features(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);
    cascade_t *cascade = py_cascade_cobj(args[1]);
    cascade->threshold = py_helper_keyword_float(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), 0.5f);
    cascade->scale_factor = py_helper_keyword_float(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_scale_factor), 1.5f);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 4, kw_args, &roi);

    // Make sure ROI is bigger than feature size
    PY_ASSERT_TRUE_MSG((roi.w > cascade->window.w && roi.h > cascade->window.h),
            "Region of interest is smaller than detector window!");

    // Detect objects
    fb_alloc_mark();
    array_t *objects_array = imlib_detect_objects(arg_img, cascade, &roi);
    fb_alloc_free_till_mark();

    // Add detected objects to a new Python list...
    mp_obj_t objects_list = mp_obj_new_list(0, NULL);
    for (int i=0; i<array_length(objects_array); i++) {
        rectangle_t *r = array_at(objects_array, i);
        mp_obj_t rec_obj[4] = {
            mp_obj_new_int(r->x),
            mp_obj_new_int(r->y),
            mp_obj_new_int(r->w),
            mp_obj_new_int(r->h),
        };
        mp_obj_list_append(objects_list, mp_obj_new_tuple(4, rec_obj));
    }
    array_free(objects_array);
    return objects_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_find_features_obj, 2, py_image_find_features);

static mp_obj_t py_image_find_eye(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_grayscale(args[0]);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 1, kw_args, &roi);

    point_t iris;
    imlib_find_iris(arg_img, &iris, &roi);

    mp_obj_t eye_obj[2] = {
        mp_obj_new_int(iris.x),
        mp_obj_new_int(iris.y),
    };

    return mp_obj_new_tuple(2, eye_obj);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_find_eye_obj, 2, py_image_find_eye);

#ifdef IMLIB_ENABLE_FIND_LBP
static mp_obj_t py_image_find_lbp(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_grayscale(args[0]);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 1, kw_args, &roi);

    py_lbp_obj_t *lbp_obj = m_new_obj(py_lbp_obj_t);
    lbp_obj->base.type = &py_lbp_type;
    lbp_obj->hist = imlib_lbp_desc(arg_img, &roi);
    return lbp_obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_find_lbp_obj, 2, py_image_find_lbp);
#endif // IMLIB_ENABLE_FIND_LBP

#ifdef IMLIB_ENABLE_FIND_KEYPOINTS
static mp_obj_t py_image_find_keypoints(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_mutable(args[0]);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 1, kw_args, &roi);

    int threshold =
        py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), 20);
    bool normalized =
        py_helper_keyword_int(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_normalized), false);
    float scale_factor =
        py_helper_keyword_float(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_scale_factor), 1.5f);
    int max_keypoints =
        py_helper_keyword_int(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_max_keypoints), 100);
    corner_detector_t corner_detector =
        py_helper_keyword_int(n_args, args, 6, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_corner_detector), CORNER_AGAST);

    #ifndef IMLIB_ENABLE_FAST
    // Force AGAST when FAST is disabled.
    corner_detector = CORNER_AGAST;
    #endif

    // Find keypoints
    fb_alloc_mark();
    array_t *kpts = orb_find_keypoints(arg_img, normalized, threshold, scale_factor, max_keypoints, corner_detector, &roi);
    fb_alloc_free_till_mark();

    if (array_length(kpts)) {
        py_kp_obj_t *kp_obj = m_new_obj(py_kp_obj_t);
        kp_obj->base.type = &py_kp_type;
        kp_obj->kpts = kpts;
        kp_obj->threshold = threshold;
        kp_obj->normalized = normalized;
        return kp_obj;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_find_keypoints_obj, 1, py_image_find_keypoints);
#endif // IMLIB_ENABLE_FIND_KEYPOINTS

#ifdef IMLIB_ENABLE_BINARY_OPS
static mp_obj_t py_image_find_edges(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_grayscale(args[0]);
    edge_detector_t edge_type = mp_obj_get_int(args[1]);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 2, kw_args, &roi);

    int thresh[2] = {100, 200};
    mp_obj_t thresh_obj = py_helper_keyword_object(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), NULL);

    if (thresh_obj) {
        mp_obj_t *thresh_array;
        mp_obj_get_array_fixed_n(thresh_obj, 2, &thresh_array);
        thresh[0] = mp_obj_get_int(thresh_array[0]);
        thresh[1] = mp_obj_get_int(thresh_array[1]);
    }

    switch (edge_type) {
        case EDGE_SIMPLE: {
            fb_alloc_mark();
            imlib_edge_simple(arg_img, &roi, thresh[0], thresh[1]);
            fb_alloc_free_till_mark();
            break;
        }
        case EDGE_CANNY: {
            fb_alloc_mark();
            imlib_edge_canny(arg_img, &roi, thresh[0], thresh[1]);
            fb_alloc_free_till_mark();
            break;
        }

    }

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_find_edges_obj, 2, py_image_find_edges);
#endif

#ifdef IMLIB_ENABLE_HOG
static mp_obj_t py_image_find_hog(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_helper_arg_to_image_grayscale(args[0]);

    rectangle_t roi;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 1, kw_args, &roi);

    int size = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_size), 8);

    fb_alloc_mark();
    imlib_find_hog(arg_img, &roi, size);
    fb_alloc_free_till_mark();

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_find_hog_obj, 1, py_image_find_hog);
#endif // IMLIB_ENABLE_HOG

#ifdef IMLIB_ENABLE_SELECTIVE_SEARCH
static mp_obj_t py_image_selective_search(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *img = py_helper_arg_to_image_mutable(args[0]);
    int t = py_helper_keyword_int(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), 500);
    int s = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_size), 20);
    float a1 = py_helper_keyword_float(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_a1), 1.0f);
    float a2 = py_helper_keyword_float(n_args, args, 4, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_a1), 1.0f);
    float a3 = py_helper_keyword_float(n_args, args, 5, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_a1), 1.0f);
    array_t *proposals_array = imlib_selective_search(img, t, s, a1, a2, a3);

    // Add proposals to a new Python list...
    mp_obj_t proposals_list = mp_obj_new_list(0, NULL);
    for (int i=0; i<array_length(proposals_array); i++) {
        rectangle_t *r = array_at(proposals_array, i);
        mp_obj_t rec_obj[4] = {
            mp_obj_new_int(r->x),
            mp_obj_new_int(r->y),
            mp_obj_new_int(r->w),
            mp_obj_new_int(r->h),
        };
        mp_obj_list_append(proposals_list, mp_obj_new_tuple(4, rec_obj));
    }

    array_free(proposals_array);
    return proposals_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_selective_search_obj, 1, py_image_selective_search);
#endif // IMLIB_ENABLE_SELECTIVE_SEARCH

static const mp_rom_map_elem_t locals_dict_table[] = {
    /* Basic Methods */
    {MP_ROM_QSTR(MP_QSTR_width),               MP_ROM_PTR(&py_image_width_obj)},
    {MP_ROM_QSTR(MP_QSTR_height),              MP_ROM_PTR(&py_image_height_obj)},
    {MP_ROM_QSTR(MP_QSTR_format),              MP_ROM_PTR(&py_image_format_obj)},
    {MP_ROM_QSTR(MP_QSTR_size),                MP_ROM_PTR(&py_image_size_obj)},
    {MP_ROM_QSTR(MP_QSTR_bytearray),           MP_ROM_PTR(&py_image_bytearray_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_pixel),           MP_ROM_PTR(&py_image_get_pixel_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_pixel),           MP_ROM_PTR(&py_image_set_pixel_obj)},
#ifdef IMLIB_ENABLE_MEAN_POOLING
    {MP_ROM_QSTR(MP_QSTR_mean_pool),           MP_ROM_PTR(&py_image_mean_pool_obj)},
    {MP_ROM_QSTR(MP_QSTR_mean_pooled),         MP_ROM_PTR(&py_image_mean_pooled_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_mean_pool),           MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_mean_pooled),         MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_MIDPOINT_POOLING
    {MP_ROM_QSTR(MP_QSTR_midpoint_pool),       MP_ROM_PTR(&py_image_midpoint_pool_obj)},
    {MP_ROM_QSTR(MP_QSTR_midpoint_pooled),     MP_ROM_PTR(&py_image_midpoint_pooled_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_midpoint_pool),       MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_midpoint_pooled),     MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
    {MP_ROM_QSTR(MP_QSTR_to_bitmap),           MP_ROM_PTR(&py_image_to_bitmap_obj)},
    {MP_ROM_QSTR(MP_QSTR_to_grayscale),        MP_ROM_PTR(&py_image_to_grayscale_obj)},
    {MP_ROM_QSTR(MP_QSTR_to_rgb565),           MP_ROM_PTR(&py_image_to_rgb565_obj)},
    {MP_ROM_QSTR(MP_QSTR_to_rainbow),          MP_ROM_PTR(&py_image_to_rainbow_obj)},
    {MP_ROM_QSTR(MP_QSTR_compress),            MP_ROM_PTR(&py_image_compress_obj)},
    {MP_ROM_QSTR(MP_QSTR_compress_for_ide),    MP_ROM_PTR(&py_image_compress_for_ide_obj)},
    {MP_ROM_QSTR(MP_QSTR_compressed),          MP_ROM_PTR(&py_image_compressed_obj)},
    {MP_ROM_QSTR(MP_QSTR_compressed_for_ide),  MP_ROM_PTR(&py_image_compressed_for_ide_obj)},
    {MP_ROM_QSTR(MP_QSTR_jpeg_encode_for_ide), MP_ROM_PTR(&py_image_jpeg_encode_for_ide_obj)},
    {MP_ROM_QSTR(MP_QSTR_jpeg_encoded_for_ide),MP_ROM_PTR(&py_image_jpeg_encoded_for_ide_obj)},
    {MP_ROM_QSTR(MP_QSTR_copy),                MP_ROM_PTR(&py_image_copy_obj)},
    {MP_ROM_QSTR(MP_QSTR_crop),                MP_ROM_PTR(&py_image_crop_obj)},
    {MP_ROM_QSTR(MP_QSTR_scale),               MP_ROM_PTR(&py_image_crop_obj)},
    #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
    {MP_ROM_QSTR(MP_QSTR_save),                MP_ROM_PTR(&py_image_save_obj)},
    #else
    {MP_ROM_QSTR(MP_QSTR_save),                MP_ROM_PTR(&py_func_unavailable_obj)},
    #endif
    {MP_ROM_QSTR(MP_QSTR_flush),               MP_ROM_PTR(&py_image_flush_obj)},
    /* Drawing Methods */
    {MP_ROM_QSTR(MP_QSTR_clear),               MP_ROM_PTR(&py_image_clear_obj)},
    {MP_ROM_QSTR(MP_QSTR_draw_line),           MP_ROM_PTR(&py_image_draw_line_obj)},
    {MP_ROM_QSTR(MP_QSTR_draw_rectangle),      MP_ROM_PTR(&py_image_draw_rectangle_obj)},
    {MP_ROM_QSTR(MP_QSTR_draw_circle),         MP_ROM_PTR(&py_image_draw_circle_obj)},
    {MP_ROM_QSTR(MP_QSTR_draw_ellipse),        MP_ROM_PTR(&py_image_draw_ellipse_obj)},
    {MP_ROM_QSTR(MP_QSTR_draw_string),         MP_ROM_PTR(&py_image_draw_string_obj)},
    {MP_ROM_QSTR(MP_QSTR_draw_cross),          MP_ROM_PTR(&py_image_draw_cross_obj)},
    {MP_ROM_QSTR(MP_QSTR_draw_arrow),          MP_ROM_PTR(&py_image_draw_arrow_obj)},
    {MP_ROM_QSTR(MP_QSTR_draw_edges),          MP_ROM_PTR(&py_image_draw_edges_obj)},
    {MP_ROM_QSTR(MP_QSTR_draw_image),          MP_ROM_PTR(&py_image_draw_image_obj)},
#ifdef IMLIB_ENABLE_FLOOD_FILL
    {MP_ROM_QSTR(MP_QSTR_flood_fill),          MP_ROM_PTR(&py_image_flood_fill_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_flood_fill),          MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
    {MP_ROM_QSTR(MP_QSTR_draw_keypoints),      MP_ROM_PTR(&py_image_draw_keypoints_obj)},
    {MP_ROM_QSTR(MP_QSTR_mask_rectangle),      MP_ROM_PTR(&py_image_mask_rectangle_obj)},
    {MP_ROM_QSTR(MP_QSTR_mask_circle),         MP_ROM_PTR(&py_image_mask_circle_obj)},
    {MP_ROM_QSTR(MP_QSTR_mask_ellipse),        MP_ROM_PTR(&py_image_mask_ellipse_obj)},
    /* Binary Methods */
#ifdef IMLIB_ENABLE_BINARY_OPS
    {MP_ROM_QSTR(MP_QSTR_binary),              MP_ROM_PTR(&py_image_binary_obj)},
    {MP_ROM_QSTR(MP_QSTR_invert),              MP_ROM_PTR(&py_image_invert_obj)},
    {MP_ROM_QSTR(MP_QSTR_and),                 MP_ROM_PTR(&py_image_b_and_obj)},
    {MP_ROM_QSTR(MP_QSTR_b_and),               MP_ROM_PTR(&py_image_b_and_obj)},
    {MP_ROM_QSTR(MP_QSTR_nand),                MP_ROM_PTR(&py_image_b_nand_obj)},
    {MP_ROM_QSTR(MP_QSTR_b_nand),              MP_ROM_PTR(&py_image_b_nand_obj)},
    {MP_ROM_QSTR(MP_QSTR_or),                  MP_ROM_PTR(&py_image_b_or_obj)},
    {MP_ROM_QSTR(MP_QSTR_b_or),                MP_ROM_PTR(&py_image_b_or_obj)},
    {MP_ROM_QSTR(MP_QSTR_nor),                 MP_ROM_PTR(&py_image_b_nor_obj)},
    {MP_ROM_QSTR(MP_QSTR_b_nor),               MP_ROM_PTR(&py_image_b_nor_obj)},
    {MP_ROM_QSTR(MP_QSTR_xor),                 MP_ROM_PTR(&py_image_b_xor_obj)},
    {MP_ROM_QSTR(MP_QSTR_b_xor),               MP_ROM_PTR(&py_image_b_xor_obj)},
    {MP_ROM_QSTR(MP_QSTR_xnor),                MP_ROM_PTR(&py_image_b_xnor_obj)},
    {MP_ROM_QSTR(MP_QSTR_b_xnor),              MP_ROM_PTR(&py_image_b_xnor_obj)},
    {MP_ROM_QSTR(MP_QSTR_erode),               MP_ROM_PTR(&py_image_erode_obj)},
    {MP_ROM_QSTR(MP_QSTR_dilate),              MP_ROM_PTR(&py_image_dilate_obj)},
    {MP_ROM_QSTR(MP_QSTR_open),                MP_ROM_PTR(&py_image_open_obj)},
    {MP_ROM_QSTR(MP_QSTR_close),               MP_ROM_PTR(&py_image_close_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_binary),              MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_invert),              MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_and),                 MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_b_and),               MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_nand),                MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_b_nand),              MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_or),                  MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_b_or),                MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_nor),                 MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_b_nor),               MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_xor),                 MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_b_xor),               MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_xnor),                MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_b_xnor),              MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_erode),               MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_dilate),              MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_open),                MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_close),               MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_MATH_OPS
    /* Math Methods */
    {MP_ROM_QSTR(MP_QSTR_gamma_corr),          MP_ROM_PTR(&py_image_gamma_corr_obj)},
    {MP_ROM_QSTR(MP_QSTR_negate),              MP_ROM_PTR(&py_image_negate_obj)},
    {MP_ROM_QSTR(MP_QSTR_assign),              MP_ROM_PTR(&py_image_replace_obj)},
    {MP_ROM_QSTR(MP_QSTR_replace),             MP_ROM_PTR(&py_image_replace_obj)},
    {MP_ROM_QSTR(MP_QSTR_set),                 MP_ROM_PTR(&py_image_replace_obj)},
    {MP_ROM_QSTR(MP_QSTR_add),                 MP_ROM_PTR(&py_image_add_obj)},
    {MP_ROM_QSTR(MP_QSTR_sub),                 MP_ROM_PTR(&py_image_sub_obj)},
    {MP_ROM_QSTR(MP_QSTR_mul),                 MP_ROM_PTR(&py_image_mul_obj)},
    {MP_ROM_QSTR(MP_QSTR_div),                 MP_ROM_PTR(&py_image_div_obj)},
    {MP_ROM_QSTR(MP_QSTR_min),                 MP_ROM_PTR(&py_image_min_obj)},
    {MP_ROM_QSTR(MP_QSTR_max),                 MP_ROM_PTR(&py_image_max_obj)},
    {MP_ROM_QSTR(MP_QSTR_difference),          MP_ROM_PTR(&py_image_difference_obj)},
    {MP_ROM_QSTR(MP_QSTR_blend),               MP_ROM_PTR(&py_image_blend_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_negate),              MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_assign),              MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_replace),             MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_set),                 MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_add),                 MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_sub),                 MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_mul),                 MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_div),                 MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_min),                 MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_max),                 MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_difference),          MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_blend),               MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#if defined(IMLIB_ENABLE_MATH_OPS) && defined (IMLIB_ENABLE_BINARY_OPS)
    {MP_ROM_QSTR(MP_QSTR_top_hat),             MP_ROM_PTR(&py_image_top_hat_obj)},
    {MP_ROM_QSTR(MP_QSTR_black_hat),           MP_ROM_PTR(&py_image_black_hat_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_top_hat),             MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_black_hat),           MP_ROM_PTR(&py_func_unavailable_obj)},
#endif //defined(IMLIB_ENABLE_MATH_OPS) && defined (IMLIB_ENABLE_BINARY_OPS)
    /* Filtering Methods */
    {MP_ROM_QSTR(MP_QSTR_histeq),              MP_ROM_PTR(&py_image_histeq_obj)},
#ifdef IMLIB_ENABLE_MEAN
    {MP_ROM_QSTR(MP_QSTR_mean),                MP_ROM_PTR(&py_image_mean_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_mean),                MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_MEDIAN
    {MP_ROM_QSTR(MP_QSTR_median),              MP_ROM_PTR(&py_image_median_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_median),              MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_MODE
    {MP_ROM_QSTR(MP_QSTR_mode),                MP_ROM_PTR(&py_image_mode_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_mode),                MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_MIDPOINT
    {MP_ROM_QSTR(MP_QSTR_midpoint),            MP_ROM_PTR(&py_image_midpoint_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_midpoint),            MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_MORPH
    {MP_ROM_QSTR(MP_QSTR_morph),               MP_ROM_PTR(&py_image_morph_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_morph),               MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_GAUSSIAN
    {MP_ROM_QSTR(MP_QSTR_blur),                MP_ROM_PTR(&py_image_gaussian_obj)},
    {MP_ROM_QSTR(MP_QSTR_gaussian),            MP_ROM_PTR(&py_image_gaussian_obj)},
    {MP_ROM_QSTR(MP_QSTR_gaussian_blur),       MP_ROM_PTR(&py_image_gaussian_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_blur),                MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_gaussian),            MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_gaussian_blur),       MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_LAPLACIAN
    {MP_ROM_QSTR(MP_QSTR_laplacian),           MP_ROM_PTR(&py_image_laplacian_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_laplacian),           MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_BILATERAL
    {MP_ROM_QSTR(MP_QSTR_bilateral),           MP_ROM_PTR(&py_image_bilateral_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_bilateral),           MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_CARTOON
    {MP_ROM_QSTR(MP_QSTR_cartoon),             MP_ROM_PTR(&py_image_cartoon_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_cartoon),             MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
    /* Geometric Methods */
#ifdef IMLIB_ENABLE_LINPOLAR
    {MP_ROM_QSTR(MP_QSTR_linpolar),            MP_ROM_PTR(&py_image_linpolar_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_linpolar),            MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_LOGPOLAR
    {MP_ROM_QSTR(MP_QSTR_logpolar),            MP_ROM_PTR(&py_image_logpolar_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_logpolar),            MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_LENS_CORR
    {MP_ROM_QSTR(MP_QSTR_lens_corr),           MP_ROM_PTR(&py_image_lens_corr_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_lens_corr),           MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_ROTATION_CORR
    {MP_ROM_QSTR(MP_QSTR_rotation_corr),       MP_ROM_PTR(&py_image_rotation_corr_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_rotation_corr),       MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
    /* Get Methods */
#ifdef IMLIB_ENABLE_GET_SIMILARITY
    {MP_ROM_QSTR(MP_QSTR_get_similarity),      MP_ROM_PTR(&py_image_get_similarity_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_get_similarity),      MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
    {MP_ROM_QSTR(MP_QSTR_get_hist),            MP_ROM_PTR(&py_image_get_histogram_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_histogram),       MP_ROM_PTR(&py_image_get_histogram_obj)},
    {MP_ROM_QSTR(MP_QSTR_histogram),           MP_ROM_PTR(&py_image_get_histogram_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_stats),           MP_ROM_PTR(&py_image_get_statistics_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_statistics),      MP_ROM_PTR(&py_image_get_statistics_obj)},
    {MP_ROM_QSTR(MP_QSTR_statistics),          MP_ROM_PTR(&py_image_get_statistics_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_regression),      MP_ROM_PTR(&py_image_get_regression_obj)},
    /* Find Methods */
    {MP_ROM_QSTR(MP_QSTR_find_blobs),          MP_ROM_PTR(&py_image_find_blobs_obj)},
#ifdef IMLIB_ENABLE_FIND_LINES
    {MP_ROM_QSTR(MP_QSTR_find_lines),          MP_ROM_PTR(&py_image_find_lines_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_find_lines),          MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_FIND_LINE_SEGMENTS
    {MP_ROM_QSTR(MP_QSTR_find_line_segments),  MP_ROM_PTR(&py_image_find_line_segments_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_find_line_segments),  MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_FIND_CIRCLES
    {MP_ROM_QSTR(MP_QSTR_find_circles),        MP_ROM_PTR(&py_image_find_circles_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_find_circles),        MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_FIND_RECTS
    {MP_ROM_QSTR(MP_QSTR_find_rects),          MP_ROM_PTR(&py_image_find_rects_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_find_rects),          MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_QRCODES
    {MP_ROM_QSTR(MP_QSTR_find_qrcodes),        MP_ROM_PTR(&py_image_find_qrcodes_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_find_qrcodes),        MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_APRILTAGS
    {MP_ROM_QSTR(MP_QSTR_find_apriltags),      MP_ROM_PTR(&py_image_find_apriltags_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_find_apriltags),      MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_DATAMATRICES
    {MP_ROM_QSTR(MP_QSTR_find_datamatrices),   MP_ROM_PTR(&py_image_find_datamatrices_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_find_datamatrices),   MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_BARCODES
    {MP_ROM_QSTR(MP_QSTR_find_barcodes),       MP_ROM_PTR(&py_image_find_barcodes_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_find_barcodes),       MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_FIND_DISPLACEMENT
    {MP_ROM_QSTR(MP_QSTR_find_displacement),   MP_ROM_PTR(&py_image_find_displacement_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_find_displacement),   MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_FIND_TEMPLATE
    {MP_ROM_QSTR(MP_QSTR_find_template),       MP_ROM_PTR(&py_image_find_template_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_find_template),       MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
    {MP_ROM_QSTR(MP_QSTR_find_features),       MP_ROM_PTR(&py_image_find_features_obj)},
    {MP_ROM_QSTR(MP_QSTR_find_eye),            MP_ROM_PTR(&py_image_find_eye_obj)},
#ifdef IMLIB_ENABLE_FIND_LBP
    {MP_ROM_QSTR(MP_QSTR_find_lbp),            MP_ROM_PTR(&py_image_find_lbp_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_find_lbp),            MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_FIND_KEYPOINTS
    {MP_ROM_QSTR(MP_QSTR_find_keypoints),      MP_ROM_PTR(&py_image_find_keypoints_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_find_keypoints),      MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_BINARY_OPS
    {MP_ROM_QSTR(MP_QSTR_find_edges),          MP_ROM_PTR(&py_image_find_edges_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_find_edges),          MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_HOG
    {MP_ROM_QSTR(MP_QSTR_find_hog),            MP_ROM_PTR(&py_image_find_hog_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_find_hog),            MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
#ifdef IMLIB_ENABLE_SELECTIVE_SEARCH
    {MP_ROM_QSTR(MP_QSTR_selective_search),    MP_ROM_PTR(&py_image_selective_search_obj)},
#else
    {MP_ROM_QSTR(MP_QSTR_selective_search),    MP_ROM_PTR(&py_func_unavailable_obj)},
#endif
};

STATIC MP_DEFINE_CONST_DICT(locals_dict, locals_dict_table);

static const mp_obj_type_t py_image_type = {
    { &mp_type_type },
    .name  = MP_QSTR_Image,
    .print = py_image_print,
    .buffer_p = { .get_buffer = py_image_get_buffer },
    .subscr = py_image_subscr,
    .getiter = py_image_getiter,
    .unary_op = py_image_unary_op,
    .locals_dict = (mp_obj_t) &locals_dict
};

mp_obj_t py_image_binary_to_grayscale(mp_obj_t arg)
{
    int8_t b = mp_obj_get_int(arg) & 1;
    return mp_obj_new_int(COLOR_BINARY_TO_GRAYSCALE(b));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_binary_to_grayscale_obj, py_image_binary_to_grayscale);

mp_obj_t py_image_binary_to_rgb(mp_obj_t arg)
{
    int8_t b = mp_obj_get_int(arg) & 1;
    uint16_t rgb565 = COLOR_BINARY_TO_RGB565(b);
    return mp_obj_new_tuple(3, (mp_obj_t[3])
            {mp_obj_new_int(COLOR_RGB565_TO_R8(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_G8(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_B8(rgb565))});
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_binary_to_rgb_obj, py_image_binary_to_rgb);

mp_obj_t py_image_binary_to_lab(mp_obj_t arg)
{
    int8_t b = mp_obj_get_int(arg) & 1;
    uint16_t rgb565 = COLOR_BINARY_TO_RGB565(b);
    return mp_obj_new_tuple(3, (mp_obj_t[3])
            {mp_obj_new_int(COLOR_RGB565_TO_L(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_A(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_B(rgb565))});
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_binary_to_lab_obj, py_image_binary_to_lab);

mp_obj_t py_image_binary_to_yuv(mp_obj_t arg)
{
    int8_t b = mp_obj_get_int(arg) & 1;
    uint16_t rgb565 = COLOR_BINARY_TO_RGB565(b);
    return mp_obj_new_tuple(3, (mp_obj_t[3])
            {mp_obj_new_int(COLOR_RGB565_TO_Y(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_U(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_V(rgb565))});
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_binary_to_yuv_obj, py_image_binary_to_yuv);

mp_obj_t py_image_grayscale_to_binary(mp_obj_t arg)
{
    int8_t g = mp_obj_get_int(arg) & 255;
    return mp_obj_new_int(COLOR_GRAYSCALE_TO_BINARY(g));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_grayscale_to_binary_obj, py_image_grayscale_to_binary);

mp_obj_t py_image_grayscale_to_rgb(mp_obj_t arg)
{
    int8_t g = mp_obj_get_int(arg) & 255;
    uint16_t rgb565 = COLOR_GRAYSCALE_TO_RGB565(g);
    return mp_obj_new_tuple(3, (mp_obj_t[3])
            {mp_obj_new_int(COLOR_RGB565_TO_R8(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_G8(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_B8(rgb565))});
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_grayscale_to_rgb_obj, py_image_grayscale_to_rgb);

mp_obj_t py_image_grayscale_to_lab(mp_obj_t arg)
{
    int8_t g = mp_obj_get_int(arg) & 255;
    uint16_t rgb565 = COLOR_GRAYSCALE_TO_RGB565(g);
    return mp_obj_new_tuple(3, (mp_obj_t[3])
            {mp_obj_new_int(COLOR_RGB565_TO_L(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_A(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_B(rgb565))});
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_grayscale_to_lab_obj, py_image_grayscale_to_lab);

mp_obj_t py_image_grayscale_to_yuv(mp_obj_t arg)
{
    int8_t g = mp_obj_get_int(arg) & 255;
    uint16_t rgb565 = COLOR_GRAYSCALE_TO_RGB565(g);
    return mp_obj_new_tuple(3, (mp_obj_t[3])
            {mp_obj_new_int(COLOR_RGB565_TO_Y(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_U(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_V(rgb565))});
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_image_grayscale_to_yuv_obj, py_image_grayscale_to_yuv);

mp_obj_t py_image_rgb_to_binary(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    const mp_obj_t *arg_vec;
    py_helper_consume_array(n_args, args, 0, 3, &arg_vec);
    uint8_t r = mp_obj_get_int(arg_vec[0]) & 255;
    uint8_t g = mp_obj_get_int(arg_vec[1]) & 255;
    uint8_t b = mp_obj_get_int(arg_vec[2]) & 255;
    uint16_t rgb565 = COLOR_R8_G8_B8_TO_RGB565(r, g, b);
    return mp_obj_new_int(COLOR_RGB565_TO_BINARY(rgb565));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_rgb_to_binary_obj, 1, py_image_rgb_to_binary);

mp_obj_t py_image_rgb_to_grayscale(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    const mp_obj_t *arg_vec;
    py_helper_consume_array(n_args, args, 0, 3, &arg_vec);
    uint8_t r = mp_obj_get_int(arg_vec[0]) & 255;
    uint8_t g = mp_obj_get_int(arg_vec[1]) & 255;
    uint8_t b = mp_obj_get_int(arg_vec[2]) & 255;
    uint16_t rgb565 = COLOR_R8_G8_B8_TO_RGB565(r, g, b);
    return mp_obj_new_int(COLOR_RGB565_TO_GRAYSCALE(rgb565));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_rgb_to_grayscale_obj, 1, py_image_rgb_to_grayscale);

mp_obj_t py_image_rgb_to_lab(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    const mp_obj_t *arg_vec;
    py_helper_consume_array(n_args, args, 0, 3, &arg_vec);
    uint8_t r = mp_obj_get_int(arg_vec[0]) & 255;
    uint8_t g = mp_obj_get_int(arg_vec[1]) & 255;
    uint8_t b = mp_obj_get_int(arg_vec[2]) & 255;
    uint16_t rgb565 = COLOR_R8_G8_B8_TO_RGB565(r, g, b);
    return mp_obj_new_tuple(3, (mp_obj_t[3])
            {mp_obj_new_int(COLOR_RGB565_TO_L(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_A(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_B(rgb565))});
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_rgb_to_lab_obj, 1, py_image_rgb_to_lab);

mp_obj_t py_image_rgb_to_yuv(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    const mp_obj_t *arg_vec;
    py_helper_consume_array(n_args, args, 0, 3, &arg_vec);
    uint8_t r = mp_obj_get_int(arg_vec[0]) & 255;
    uint8_t g = mp_obj_get_int(arg_vec[1]) & 255;
    uint8_t b = mp_obj_get_int(arg_vec[2]) & 255;
    uint16_t rgb565 = COLOR_R8_G8_B8_TO_RGB565(r, g, b);
    return mp_obj_new_tuple(3, (mp_obj_t[3])
            {mp_obj_new_int(COLOR_RGB565_TO_Y(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_U(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_V(rgb565))});
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_rgb_to_yuv_obj, 1, py_image_rgb_to_yuv);

mp_obj_t py_image_lab_to_binary(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    const mp_obj_t *arg_vec;
    py_helper_consume_array(n_args, args, 0, 3, &arg_vec);
    int8_t l = (mp_obj_get_int(arg_vec[0]) & 255) % 100;
    int8_t a = mp_obj_get_int(arg_vec[1]) & 255;
    int8_t b = mp_obj_get_int(arg_vec[2]) & 255;
    uint16_t rgb565 = COLOR_LAB_TO_RGB565(l, a, b);
    return mp_obj_new_int(COLOR_RGB565_TO_BINARY(rgb565));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_lab_to_binary_obj, 1, py_image_lab_to_binary);

mp_obj_t py_image_lab_to_grayscale(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    const mp_obj_t *arg_vec;
    py_helper_consume_array(n_args, args, 0, 3, &arg_vec);
    int8_t l = (mp_obj_get_int(arg_vec[0]) & 255) % 100;
    int8_t a = mp_obj_get_int(arg_vec[1]) & 255;
    int8_t b = mp_obj_get_int(arg_vec[2]) & 255;
    uint16_t rgb565 = COLOR_LAB_TO_RGB565(l, a, b);
    return mp_obj_new_int(COLOR_RGB565_TO_GRAYSCALE(rgb565));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_lab_to_grayscale_obj, 1, py_image_lab_to_grayscale);

mp_obj_t py_image_lab_to_rgb(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    const mp_obj_t *arg_vec;
    py_helper_consume_array(n_args, args, 0, 3, &arg_vec);
    int8_t l = (mp_obj_get_int(arg_vec[0]) & 255) % 100;
    int8_t a = mp_obj_get_int(arg_vec[1]) & 255;
    int8_t b = mp_obj_get_int(arg_vec[2]) & 255;
    uint16_t rgb565 = COLOR_LAB_TO_RGB565(l, a, b);
    return mp_obj_new_tuple(3, (mp_obj_t[3])
            {mp_obj_new_int(COLOR_RGB565_TO_R8(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_G8(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_B8(rgb565))});
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_lab_to_rgb_obj, 1, py_image_lab_to_rgb);

mp_obj_t py_image_lab_to_yuv(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    const mp_obj_t *arg_vec;
    py_helper_consume_array(n_args, args, 0, 3, &arg_vec);
    int8_t l = (mp_obj_get_int(arg_vec[0]) & 255) % 100;
    int8_t a = mp_obj_get_int(arg_vec[1]) & 255;
    int8_t b = mp_obj_get_int(arg_vec[2]) & 255;
    uint16_t rgb565 = COLOR_LAB_TO_RGB565(l, a, b);
    return mp_obj_new_tuple(3, (mp_obj_t[3])
            {mp_obj_new_int(COLOR_RGB565_TO_Y(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_U(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_V(rgb565))});
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_lab_to_yuv_obj, 1, py_image_lab_to_yuv);

mp_obj_t py_image_yuv_to_binary(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    const mp_obj_t *arg_vec;
    py_helper_consume_array(n_args, args, 0, 3, &arg_vec);
    int8_t y = mp_obj_get_int(arg_vec[0]) & 255;
    int8_t u = mp_obj_get_int(arg_vec[1]) & 255;
    int8_t v = mp_obj_get_int(arg_vec[2]) & 255;
    uint16_t rgb565 = COLOR_YUV_TO_RGB565(y, u, v);
    return mp_obj_new_int(COLOR_RGB565_TO_BINARY(rgb565));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_yuv_to_binary_obj, 1, py_image_yuv_to_binary);

mp_obj_t py_image_yuv_to_grayscale(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    const mp_obj_t *arg_vec;
    py_helper_consume_array(n_args, args, 0, 3, &arg_vec);
    int8_t y = mp_obj_get_int(arg_vec[0]) & 255;
    int8_t u = mp_obj_get_int(arg_vec[1]) & 255;
    int8_t v = mp_obj_get_int(arg_vec[2]) & 255;
    uint16_t rgb565 = COLOR_YUV_TO_RGB565(y, u, v);
    return mp_obj_new_int(COLOR_RGB565_TO_GRAYSCALE(rgb565));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_yuv_to_grayscale_obj, 1, py_image_yuv_to_grayscale);

mp_obj_t py_image_yuv_to_rgb(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    const mp_obj_t *arg_vec;
    py_helper_consume_array(n_args, args, 0, 3, &arg_vec);
    int8_t y = mp_obj_get_int(arg_vec[0]) & 255;
    int8_t u = mp_obj_get_int(arg_vec[1]) & 255;
    int8_t v = mp_obj_get_int(arg_vec[2]) & 255;
    uint16_t rgb565 = COLOR_YUV_TO_RGB565(y, u, v);
    return mp_obj_new_tuple(3, (mp_obj_t[3])
            {mp_obj_new_int(COLOR_RGB565_TO_R8(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_G8(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_B8(rgb565))});
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_yuv_to_rgb_obj, 1, py_image_yuv_to_rgb);

mp_obj_t py_image_yuv_to_lab(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    const mp_obj_t *arg_vec;
    py_helper_consume_array(n_args, args, 0, 3, &arg_vec);
    int8_t y = mp_obj_get_int(arg_vec[0]) & 255;
    int8_t u = mp_obj_get_int(arg_vec[1]) & 255;
    int8_t v = mp_obj_get_int(arg_vec[2]) & 255;
    uint16_t rgb565 = COLOR_YUV_TO_RGB565(y, u, v);
    return mp_obj_new_tuple(3, (mp_obj_t[3])
            {mp_obj_new_int(COLOR_RGB565_TO_L(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_A(rgb565)),
             mp_obj_new_int(COLOR_RGB565_TO_B(rgb565))});
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_yuv_to_lab_obj, 1, py_image_yuv_to_lab);

mp_obj_t py_image(int w, int h, int bpp, void *pixels)
{
    py_image_obj_t *o = m_new_obj(py_image_obj_t);
    o->base.type = &py_image_type;
    o->_cobj.w = w;
    o->_cobj.h = h;
    o->_cobj.bpp = bpp;
    o->_cobj.pixels = pixels;
    return o;
}

mp_obj_t py_image_from_struct(image_t *img)
{
    py_image_obj_t *o = m_new_obj(py_image_obj_t);
    o->base.type = &py_image_type;
    o->_cobj = *img;
    return o;
}

mp_obj_t py_image_load_image(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    // mode == false -> load behavior
    // mode == true -> make behavior
    bool mode = mp_obj_is_integer(args[0]);
    const char *path = mode ? NULL : mp_obj_str_get_str(args[0]);

    mp_obj_t copy_to_fb_obj = py_helper_keyword_object(n_args, args,
            mode ? 3 : 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_copy_to_fb), NULL);
    bool copy_to_fb = false;
    image_t *arg_other = NULL;

    if (copy_to_fb_obj) {
        if (mp_obj_is_integer(copy_to_fb_obj)) {
            copy_to_fb = mp_obj_get_int(copy_to_fb_obj);
        } else {
            arg_other = py_helper_arg_to_image_mutable(copy_to_fb_obj);
        }
    }

    image_t image = {0};

    if (mode) {
        PY_ASSERT_TRUE_MSG(n_args >= 3, "Expected width, height, and type");

        image.w = mp_obj_get_int(args[0]);
        PY_ASSERT_TRUE_MSG(image.w > 0, "Width must be > 0");

        image.h = mp_obj_get_int(args[1]);
        PY_ASSERT_TRUE_MSG(image.h > 0, "Height must be > 0");

        switch(mp_obj_get_int(args[2])) {
            case PIXFORMAT_BINARY:
                image.bpp = IMAGE_BPP_BINARY;
                break;
            case PIXFORMAT_GRAYSCALE:
                image.bpp = IMAGE_BPP_GRAYSCALE;
                break;
            case PIXFORMAT_RGB565:
                image.bpp = IMAGE_BPP_RGB565;
                break;
            default:
                PY_ASSERT_TRUE_MSG(false, "Unsupported type");
                break;
        }
    } else {
        #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
        fb_alloc_mark();
        FIL fp;
        img_read_settings_t rs;
        imlib_read_geometry(&fp, &image, path, &rs);
        file_buffer_off(&fp);
        file_close(&fp);
        #else
        (void) path;
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Image I/O is not supported"));
        #endif //IMLIB_ENABLE_IMAGE_FILE_IO
    }

    if (copy_to_fb) {
        py_helper_set_to_framebuffer(&image);
    } else if (arg_other) {
        PY_ASSERT_TRUE_MSG((image_size(&image) <= image_size(arg_other)), "The new image won't fit in the target frame buffer!");
        image.data = arg_other->data;
    } else if (mode) {
        image.data = xalloc(image_size(&image));
    }

    if (mode) {
        memset(image.data, 0, image_size(&image));
    } else {
        #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
        imlib_load_image(&image, path);
        fb_alloc_free_till_mark();
        #endif
    }

    py_helper_update_framebuffer(&image);

    if (arg_other) {
        arg_other->w = image.w;
        arg_other->h = image.h;
        arg_other->bpp = image.bpp;
    }

    if (copy_to_fb) {
        framebuffer_update_jpeg_buffer();
    }

    return py_image_from_struct(&image);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_load_image_obj, 1, py_image_load_image);

mp_obj_t py_image_load_cascade(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    cascade_t cascade;
    const char *path = mp_obj_str_get_str(args[0]);

    // Load cascade from file or flash
    int res = imlib_load_cascade(&cascade, path);
    if (res != FR_OK) {
        #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
        // cascade is not built-in and failed to load it from file.
        mp_raise_msg(&mp_type_OSError, ffs_strerror(res));
        #else
        // cascade is not built-in.
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Image I/O is not supported"));
        #endif
    }

    // Read the number of stages
    int stages = py_helper_keyword_int(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(qstr_from_str("stages")), cascade.n_stages);
    // Check the number of stages
    if (stages > 0 && stages < cascade.n_stages) {
        cascade.n_stages = stages;
    }

    // Return micropython cascade object
    py_cascade_obj_t *o = m_new_obj(py_cascade_obj_t);
    o->base.type = &py_cascade_type;
    o->_cobj = cascade;
    return o;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_load_cascade_obj, 1, py_image_load_cascade);

#if defined(IMLIB_ENABLE_DESCRIPTOR)
#if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
mp_obj_t py_image_load_descriptor(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    FIL fp;
    UINT bytes;
    FRESULT res;

    uint32_t desc_type;
    mp_obj_t desc = mp_const_none;
    const char *path = mp_obj_str_get_str(args[0]);

    if ((res = f_open_helper(&fp, path, FA_READ|FA_OPEN_EXISTING)) == FR_OK) {
        // Read descriptor type
        res = f_read(&fp, &desc_type, sizeof(desc_type), &bytes);
        if (res != FR_OK || bytes  != sizeof(desc_type)) {
            goto error;
        }

        // Load descriptor
        switch (desc_type) {
            #if defined(IMLIB_ENABLE_FIND_LBP)
            case DESC_LBP: {
                py_lbp_obj_t *lbp = m_new_obj(py_lbp_obj_t);
                lbp->base.type = &py_lbp_type;

                res = imlib_lbp_desc_load(&fp, &lbp->hist);
                if (res == FR_OK) {
                    desc = lbp;
                }
                break;
            }
            #endif  //IMLIB_ENABLE_FIND_LBP
            #if defined(IMLIB_ENABLE_FIND_KEYPOINTS)
            case DESC_ORB: {
                array_t *kpts = NULL;
                array_alloc(&kpts, xfree);

                res = orb_load_descriptor(&fp, kpts);
                if (res == FR_OK) {
                    // Return keypoints MP object
                    py_kp_obj_t *kp_obj = m_new_obj(py_kp_obj_t);
                    kp_obj->base.type = &py_kp_type;
                    kp_obj->kpts = kpts;
                    kp_obj->threshold = 10;
                    kp_obj->normalized = false;
                    desc = kp_obj;
                }
                break;
            }
            #endif //IMLIB_ENABLE_FIND_KEYPOINTS
        }

        f_close(&fp);
    }

error:
    // File open or write error
    if (res != FR_OK) {
        mp_raise_msg(&mp_type_OSError, ffs_strerror(res));
    }

    // If no file error and descriptor is still none, then it's not supported.
    if (desc == mp_const_none) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Descriptor type is not supported"));
    }
    return desc;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_load_descriptor_obj, 1, py_image_load_descriptor);

mp_obj_t py_image_save_descriptor(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    FIL fp;
    UINT bytes;
    FRESULT res;

    uint32_t desc_type;
    const char *path = mp_obj_str_get_str(args[1]);

    if ((res = f_open_helper(&fp, path, FA_WRITE|FA_CREATE_ALWAYS)) == FR_OK) {
        // Find descriptor type
        const mp_obj_type_t *desc_obj_type = mp_obj_get_type(args[0]);
        if (0) {
        #if defined(IMLIB_ENABLE_FIND_LBP)
        } else if (desc_obj_type ==  &py_lbp_type) {
            desc_type = DESC_LBP;
        #endif //IMLIB_ENABLE_FIND_LBP
        #if defined(IMLIB_ENABLE_FIND_KEYPOINTS)
        } else if (desc_obj_type ==  &py_kp_type) {
            desc_type = DESC_ORB;
        #endif //IMLIB_ENABLE_FIND_KEYPOINTS
        } else {
            (void) desc_obj_type;
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Descriptor type is not supported"));
        }

        // Write descriptor type
        res = f_write(&fp, &desc_type, sizeof(desc_type), &bytes);
        if (res != FR_OK || bytes  !=  sizeof(desc_type)) {
            goto error;
        }

        // Write descriptor
        switch (desc_type) {
            #if defined(IMLIB_ENABLE_FIND_LBP)
            case DESC_LBP: {
                py_lbp_obj_t *lbp = ((py_lbp_obj_t*)args[0]);
                res = imlib_lbp_desc_save(&fp, lbp->hist);
                break;
            }
            #endif //IMLIB_ENABLE_FIND_LBP
            #if defined(IMLIB_ENABLE_FIND_KEYPOINTS)
            case DESC_ORB: {
                py_kp_obj_t *kpts = ((py_kp_obj_t*)args[0]);
                res = orb_save_descriptor(&fp, kpts->kpts);
                break;
            }
            #endif //IMLIB_ENABLE_FIND_KEYPOINTS
        }
        // ignore unsupported descriptors when saving
        f_close(&fp);
    }

error:
    // File open or read error
    if (res != FR_OK) {
        mp_raise_msg(&mp_type_OSError, ffs_strerror(res));
    }
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_save_descriptor_obj, 2, py_image_save_descriptor);
#endif //IMLIB_ENABLE_IMAGE_FILE_IO

static mp_obj_t py_image_match_descriptor(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    mp_obj_t match_obj = mp_const_none;
    const mp_obj_type_t *desc1_type = mp_obj_get_type(args[0]);
    const mp_obj_type_t *desc2_type = mp_obj_get_type(args[1]);
    PY_ASSERT_TRUE_MSG((desc1_type == desc2_type), "Descriptors have different types!");

    if (0) {
    #if defined(IMLIB_ENABLE_FIND_LBP)
    } else if (desc1_type ==  &py_lbp_type) {
        py_lbp_obj_t *lbp1 = ((py_lbp_obj_t*)args[0]);
        py_lbp_obj_t *lbp2 = ((py_lbp_obj_t*)args[1]);

        // Sanity checks
        PY_ASSERT_TYPE(lbp1, &py_lbp_type);
        PY_ASSERT_TYPE(lbp2, &py_lbp_type);

        // Match descriptors
        match_obj = mp_obj_new_int(imlib_lbp_desc_distance(lbp1->hist, lbp2->hist));
    #endif //IMLIB_ENABLE_FIND_LBP
    #if defined(IMLIB_ENABLE_FIND_KEYPOINTS)
    } else if (desc1_type == &py_kp_type) {
        py_kp_obj_t *kpts1 = ((py_kp_obj_t*)args[0]);
        py_kp_obj_t *kpts2 = ((py_kp_obj_t*)args[1]);
        int threshold = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_threshold), 85);
        int filter_outliers = py_helper_keyword_int(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_filter_outliers), false);

        // Sanity checks
        PY_ASSERT_TYPE(kpts1, &py_kp_type);
        PY_ASSERT_TYPE(kpts2, &py_kp_type);
        PY_ASSERT_TRUE_MSG((threshold >=0 && threshold <= 100), "Expected threshold between 0 and 100");

        int theta = 0;          // Estimated angle of rotation
        int count = 0;          // Number of matches
        point_t c = {0};        // Centroid
        rectangle_t r = {0};    // Bounding rectangle
        // List of matching keypoints indices
        mp_obj_t match_list = mp_obj_new_list(0, NULL);

        if (array_length(kpts1->kpts) && array_length(kpts1->kpts)) {
            fb_alloc_mark();
            int *match = fb_alloc(array_length(kpts1->kpts) * sizeof(int) * 2, FB_ALLOC_NO_HINT);

            // Match the two keypoint sets
            count = orb_match_keypoints(kpts1->kpts, kpts2->kpts, match, threshold, &r, &c, &theta);

            // Add matching keypoints to Python list.
            for (int i=0; i<count*2; i+=2) {
                mp_obj_t index_obj[2] = {
                    mp_obj_new_int(match[i+0]),
                    mp_obj_new_int(match[i+1]),
                };
                mp_obj_list_append(match_list, mp_obj_new_tuple(2, index_obj));
            }

            // Free match list
            fb_alloc_free_till_mark();

            if (filter_outliers == true) {
                count = orb_filter_keypoints(kpts2->kpts, &r, &c);
            }
        }

        py_kptmatch_obj_t *o = m_new_obj(py_kptmatch_obj_t);
        o->base.type = &py_kptmatch_type;
        o->cx    = mp_obj_new_int(c.x);
        o->cy    = mp_obj_new_int(c.y);
        o->x     = mp_obj_new_int(r.x);
        o->y     = mp_obj_new_int(r.y);
        o->w     = mp_obj_new_int(r.w);
        o->h     = mp_obj_new_int(r.h);
        o->count = mp_obj_new_int(count);
        o->theta = mp_obj_new_int(theta);
        o->match = match_list;
        match_obj = o;
    #endif //IMLIB_ENABLE_FIND_KEYPOINTS
    } else {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Descriptor type is not supported"));
    }

    return match_obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_image_match_descriptor_obj, 2, py_image_match_descriptor);
#endif //IMLIB_ENABLE_DESCRIPTOR

#if defined(IMLIB_ENABLE_FIND_KEYPOINTS) && defined(IMLIB_ENABLE_IMAGE_FILE_IO)
int py_image_descriptor_from_roi(image_t *img, const char *path, rectangle_t *roi)
{
    FIL fp;
    FRESULT res = FR_OK;

    printf("Save Descriptor: ROI(%d %d %d %d)\n", roi->x, roi->y, roi->w, roi->h);
    array_t *kpts = orb_find_keypoints(img, false, 20, 1.5f, 100, CORNER_AGAST, roi);
    printf("Save Descriptor: KPTS(%d)\n", array_length(kpts));

    if (array_length(kpts)) {
        if ((res = f_open_helper(&fp, path, FA_WRITE|FA_CREATE_ALWAYS)) == FR_OK) {
            res = orb_save_descriptor(&fp, kpts);
            f_close(&fp);
        }
        // File open/write error
        if (res != FR_OK) {
            mp_raise_msg(&mp_type_OSError, ffs_strerror(res));
        }
    }
    return 0;
}
#endif // IMLIB_ENABLE_KEYPOINTS && IMLIB_ENABLE_IMAGE_FILE_IO

static const mp_rom_map_elem_t globals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__),            MP_OBJ_NEW_QSTR(MP_QSTR_image)},
    // Pixel formats
    {MP_ROM_QSTR(MP_QSTR_BINARY),              MP_ROM_INT(PIXFORMAT_BINARY)},   /* 1BPP/BINARY*/
    {MP_ROM_QSTR(MP_QSTR_GRAYSCALE),           MP_ROM_INT(PIXFORMAT_GRAYSCALE)},/* 1BPP/GRAYSCALE*/
    {MP_ROM_QSTR(MP_QSTR_RGB565),              MP_ROM_INT(PIXFORMAT_RGB565)},   /* 2BPP/RGB565*/
    {MP_ROM_QSTR(MP_QSTR_YUV422),              MP_ROM_INT(PIXFORMAT_YUV422)},   /* 2BPP/YUV422*/
    {MP_ROM_QSTR(MP_QSTR_BAYER),               MP_ROM_INT(PIXFORMAT_BAYER)},    /* 1BPP/RAW*/
    {MP_ROM_QSTR(MP_QSTR_JPEG),                MP_ROM_INT(PIXFORMAT_JPEG)},     /* JPEG/COMPRESSED*/
    {MP_ROM_QSTR(MP_QSTR_AREA),                MP_ROM_INT(IMAGE_HINT_AREA)},
    {MP_ROM_QSTR(MP_QSTR_BILINEAR),            MP_ROM_INT(IMAGE_HINT_BILINEAR)},
    {MP_ROM_QSTR(MP_QSTR_BICUBIC),             MP_ROM_INT(IMAGE_HINT_BICUBIC)},
    {MP_ROM_QSTR(MP_QSTR_CENTER),              MP_ROM_INT(IMAGE_HINT_CENTER)},
    {MP_ROM_QSTR(MP_QSTR_EXTRACT_RGB_CHANNEL_FIRST), MP_ROM_INT(IMAGE_HINT_EXTRACT_RGB_CHANNEL_FIRST)},
    {MP_ROM_QSTR(MP_QSTR_APPLY_COLOR_PALETTE_FIRST), MP_ROM_INT(IMAGE_HINT_APPLY_COLOR_PALETTE_FIRST)},
    #ifdef IMLIB_FIND_TEMPLATE
    {MP_ROM_QSTR(MP_QSTR_SEARCH_EX),           MP_ROM_INT(SEARCH_EX)},
    {MP_ROM_QSTR(MP_QSTR_SEARCH_DS),           MP_ROM_INT(SEARCH_DS)},
    #endif
    {MP_ROM_QSTR(MP_QSTR_EDGE_CANNY),          MP_ROM_INT(EDGE_CANNY)},
    {MP_ROM_QSTR(MP_QSTR_EDGE_SIMPLE),         MP_ROM_INT(EDGE_SIMPLE)},
    {MP_ROM_QSTR(MP_QSTR_CORNER_FAST),         MP_ROM_INT(CORNER_FAST)},
    {MP_ROM_QSTR(MP_QSTR_CORNER_AGAST),        MP_ROM_INT(CORNER_AGAST)},
    #ifdef IMLIB_ENABLE_APRILTAGS
    {MP_ROM_QSTR(MP_QSTR_TAG16H5),             MP_ROM_INT(TAG16H5)},
    {MP_ROM_QSTR(MP_QSTR_TAG25H7),             MP_ROM_INT(TAG25H7)},
    {MP_ROM_QSTR(MP_QSTR_TAG25H9),             MP_ROM_INT(TAG25H9)},
    {MP_ROM_QSTR(MP_QSTR_TAG36H10),            MP_ROM_INT(TAG36H10)},
    {MP_ROM_QSTR(MP_QSTR_TAG36H11),            MP_ROM_INT(TAG36H11)},
    {MP_ROM_QSTR(MP_QSTR_ARTOOLKIT),           MP_ROM_INT(ARTOOLKIT)},
    #endif
    #ifdef IMLIB_ENABLE_BARCODES
    {MP_ROM_QSTR(MP_QSTR_EAN2),                MP_ROM_INT(BARCODE_EAN2)},
    {MP_ROM_QSTR(MP_QSTR_EAN5),                MP_ROM_INT(BARCODE_EAN5)},
    {MP_ROM_QSTR(MP_QSTR_EAN8),                MP_ROM_INT(BARCODE_EAN8)},
    {MP_ROM_QSTR(MP_QSTR_UPCE),                MP_ROM_INT(BARCODE_UPCE)},
    {MP_ROM_QSTR(MP_QSTR_ISBN10),              MP_ROM_INT(BARCODE_ISBN10)},
    {MP_ROM_QSTR(MP_QSTR_UPCA),                MP_ROM_INT(BARCODE_UPCA)},
    {MP_ROM_QSTR(MP_QSTR_EAN13),               MP_ROM_INT(BARCODE_EAN13)},
    {MP_ROM_QSTR(MP_QSTR_ISBN13),              MP_ROM_INT(BARCODE_ISBN13)},
    {MP_ROM_QSTR(MP_QSTR_I25),                 MP_ROM_INT(BARCODE_I25)},
    {MP_ROM_QSTR(MP_QSTR_DATABAR),             MP_ROM_INT(BARCODE_DATABAR)},
    {MP_ROM_QSTR(MP_QSTR_DATABAR_EXP),         MP_ROM_INT(BARCODE_DATABAR_EXP)},
    {MP_ROM_QSTR(MP_QSTR_CODABAR),             MP_ROM_INT(BARCODE_CODABAR)},
    {MP_ROM_QSTR(MP_QSTR_CODE39),              MP_ROM_INT(BARCODE_CODE39)},
    {MP_ROM_QSTR(MP_QSTR_PDF417),              MP_ROM_INT(BARCODE_PDF417)},
    {MP_ROM_QSTR(MP_QSTR_CODE93),              MP_ROM_INT(BARCODE_CODE93)},
    {MP_ROM_QSTR(MP_QSTR_CODE128),             MP_ROM_INT(BARCODE_CODE128)},
    #endif
    #if defined(IMLIB_ENABLE_IMAGE_IO)
    {MP_ROM_QSTR(MP_QSTR_ImageIO),             MP_ROM_PTR(&py_imageio_type) },
    #else
    {MP_ROM_QSTR(MP_QSTR_ImageIO),             MP_ROM_PTR(&py_func_unavailable_obj)},
    #endif
    {MP_ROM_QSTR(MP_QSTR_binary_to_grayscale), MP_ROM_PTR(&py_image_binary_to_grayscale_obj)},
    {MP_ROM_QSTR(MP_QSTR_binary_to_rgb),       MP_ROM_PTR(&py_image_binary_to_rgb_obj)},
    {MP_ROM_QSTR(MP_QSTR_binary_to_lab),       MP_ROM_PTR(&py_image_binary_to_lab_obj)},
    {MP_ROM_QSTR(MP_QSTR_binary_to_yuv),       MP_ROM_PTR(&py_image_binary_to_yuv_obj)},
    {MP_ROM_QSTR(MP_QSTR_grayscale_to_binary), MP_ROM_PTR(&py_image_grayscale_to_binary_obj)},
    {MP_ROM_QSTR(MP_QSTR_grayscale_to_rgb),    MP_ROM_PTR(&py_image_grayscale_to_rgb_obj)},
    {MP_ROM_QSTR(MP_QSTR_grayscale_to_lab),    MP_ROM_PTR(&py_image_grayscale_to_lab_obj)},
    {MP_ROM_QSTR(MP_QSTR_grayscale_to_yuv),    MP_ROM_PTR(&py_image_grayscale_to_yuv_obj)},
    {MP_ROM_QSTR(MP_QSTR_rgb_to_binary),       MP_ROM_PTR(&py_image_rgb_to_binary_obj)},
    {MP_ROM_QSTR(MP_QSTR_rgb_to_grayscale),    MP_ROM_PTR(&py_image_rgb_to_grayscale_obj)},
    {MP_ROM_QSTR(MP_QSTR_rgb_to_lab),          MP_ROM_PTR(&py_image_rgb_to_lab_obj)},
    {MP_ROM_QSTR(MP_QSTR_rgb_to_yuv),          MP_ROM_PTR(&py_image_rgb_to_yuv_obj)},
    {MP_ROM_QSTR(MP_QSTR_lab_to_binary),       MP_ROM_PTR(&py_image_lab_to_binary_obj)},
    {MP_ROM_QSTR(MP_QSTR_lab_to_grayscale),    MP_ROM_PTR(&py_image_lab_to_grayscale_obj)},
    {MP_ROM_QSTR(MP_QSTR_lab_to_rgb),          MP_ROM_PTR(&py_image_lab_to_rgb_obj)},
    {MP_ROM_QSTR(MP_QSTR_lab_to_yuv),          MP_ROM_PTR(&py_image_lab_to_yuv_obj)},
    {MP_ROM_QSTR(MP_QSTR_yuv_to_binary),       MP_ROM_PTR(&py_image_yuv_to_binary_obj)},
    {MP_ROM_QSTR(MP_QSTR_yuv_to_grayscale),    MP_ROM_PTR(&py_image_yuv_to_grayscale_obj)},
    {MP_ROM_QSTR(MP_QSTR_yuv_to_rgb),          MP_ROM_PTR(&py_image_yuv_to_rgb_obj)},
    {MP_ROM_QSTR(MP_QSTR_yuv_to_lab),          MP_ROM_PTR(&py_image_yuv_to_lab_obj)},
    {MP_ROM_QSTR(MP_QSTR_Image),               MP_ROM_PTR(&py_image_load_image_obj)},
    {MP_ROM_QSTR(MP_QSTR_HaarCascade),         MP_ROM_PTR(&py_image_load_cascade_obj)},
    #if defined(IMLIB_ENABLE_DESCRIPTOR) && defined(IMLIB_ENABLE_IMAGE_FILE_IO)
    {MP_ROM_QSTR(MP_QSTR_load_descriptor),     MP_ROM_PTR(&py_image_load_descriptor_obj)},
    {MP_ROM_QSTR(MP_QSTR_save_descriptor),     MP_ROM_PTR(&py_image_save_descriptor_obj)},
    #else
    {MP_ROM_QSTR(MP_QSTR_load_descriptor),     MP_ROM_PTR(&py_func_unavailable_obj)},
    {MP_ROM_QSTR(MP_QSTR_save_descriptor),     MP_ROM_PTR(&py_func_unavailable_obj)},
    #endif //IMLIB_ENABLE_DESCRIPTOR && IMLIB_ENABLE_IMAGE_FILE_IO
    #if defined(IMLIB_ENABLE_DESCRIPTOR)
    {MP_ROM_QSTR(MP_QSTR_match_descriptor),    MP_ROM_PTR(&py_image_match_descriptor_obj)}
    #else
    {MP_ROM_QSTR(MP_QSTR_match_descriptor),    MP_ROM_PTR(&py_func_unavailable_obj)}
    #endif
};

STATIC MP_DEFINE_CONST_DICT(globals_dict, globals_dict_table);

const mp_obj_module_t image_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_t) &globals_dict
};
