/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * MJPEG Python module.
 */
#include "py/obj.h"
#include "py/runtime.h"

#if  MICROPY_PY_OMV_MJPGE

#include "ff_wrapper.h"
#include "framebuffer.h"
#include "py_assert.h"
#include "py_helper.h"
#include "py_image.h"

static const mp_obj_type_t py_mjpeg_type; // forward declare
// Mjpeg class
typedef struct py_mjpeg_obj {
    mp_obj_base_t base;
    int width;
    int height;
    uint32_t frames;
    uint32_t bytes;
    #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
    FIL fp;
    #endif
} py_mjpeg_obj_t;

static mp_obj_t py_mjpeg_open(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    py_mjpeg_obj_t *mjpeg = m_new_obj(py_mjpeg_obj_t);
    mjpeg->width  = py_helper_keyword_int(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_width), MAIN_FB()->w);
    mjpeg->height = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_height), MAIN_FB()->h);
    mjpeg->frames = 0; // private
    mjpeg->bytes = 0; // private
    mjpeg->base.type = &py_mjpeg_type;

    #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
    file_write_open(&mjpeg->fp, mp_obj_str_get_str(args[0]));
    mjpeg_open(&mjpeg->fp, mjpeg->width, mjpeg->height);
    #else
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Image I/O is not supported"));
    #endif
    return mjpeg;
}

static mp_obj_t py_mjpeg_width(mp_obj_t mjpeg_obj)
{
    py_mjpeg_obj_t *arg_mjpeg = mjpeg_obj;
    return mp_obj_new_int(arg_mjpeg->width);
}

static mp_obj_t py_mjpeg_height(mp_obj_t mjpeg_obj)
{
    py_mjpeg_obj_t *arg_mjpeg = mjpeg_obj;
    return mp_obj_new_int(arg_mjpeg->height);
}

static mp_obj_t py_mjpeg_size(mp_obj_t mjpeg_obj)
{
    #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
    py_mjpeg_obj_t *arg_mjpeg = mjpeg_obj;
    return mp_obj_new_int(f_size(&arg_mjpeg->fp));
    #else
    return mp_obj_new_int(0);
    #endif
}

static mp_obj_t py_mjpeg_add_frame(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
    py_mjpeg_obj_t *arg_mjpeg = args[0];
    image_t *arg_img = py_image_cobj(args[1]);
    PY_ASSERT_FALSE_MSG((arg_mjpeg->width != arg_img->w)
                     || (arg_mjpeg->height != arg_img->h),
            "Unexpected image geometry");

    int arg_q = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_quality), 50);
    arg_q = IM_MIN(IM_MAX(arg_q, 1), 100);
    mjpeg_add_frame(&arg_mjpeg->fp, &arg_mjpeg->frames, &arg_mjpeg->bytes, arg_img, arg_q);
    #else
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Image I/O is not supported"));
    #endif
    return mp_const_none;
}

static mp_obj_t py_mjpeg_close(mp_obj_t mjpeg_obj, mp_obj_t fps_obj)
{
    #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
    py_mjpeg_obj_t *arg_mjpeg = mjpeg_obj;
    mjpeg_close(&arg_mjpeg->fp, &arg_mjpeg->frames, &arg_mjpeg->bytes, mp_obj_get_float(fps_obj));
    #endif
    return mp_const_none;
}

static void py_mjpeg_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_mjpeg_obj_t *self = self_in;
    mp_printf(print, "<mjpeg width:%d height:%d>", self->width, self->height);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_mjpeg_width_obj, py_mjpeg_width);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_mjpeg_height_obj, py_mjpeg_height);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_mjpeg_size_obj, py_mjpeg_size);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_mjpeg_add_frame_obj, 2, py_mjpeg_add_frame);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(py_mjpeg_close_obj, py_mjpeg_close);
static const mp_map_elem_t locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_width),       (mp_obj_t)&py_mjpeg_width_obj     },
    { MP_OBJ_NEW_QSTR(MP_QSTR_height),      (mp_obj_t)&py_mjpeg_height_obj    },
    { MP_OBJ_NEW_QSTR(MP_QSTR_size),        (mp_obj_t)&py_mjpeg_size_obj      },
    { MP_OBJ_NEW_QSTR(MP_QSTR_add_frame),   (mp_obj_t)&py_mjpeg_add_frame_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_close),       (mp_obj_t)&py_mjpeg_close_obj     },
    { NULL, NULL },
};
STATIC MP_DEFINE_CONST_DICT(locals_dict, locals_dict_table);

static const mp_obj_type_t py_mjpeg_type = {
    { &mp_type_type },
    .name  = MP_QSTR_Mjpeg,
    .print = py_mjpeg_print,
    .locals_dict = (mp_obj_t)&locals_dict,
};

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_mjpeg_open_obj, 1, py_mjpeg_open);
static const mp_map_elem_t globals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__),    MP_OBJ_NEW_QSTR(MP_QSTR_mjpeg) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Mjpeg),       (mp_obj_t)&py_mjpeg_open_obj   },
    { NULL, NULL },
};
STATIC MP_DEFINE_CONST_DICT(globals_dict, globals_dict_table);

const mp_obj_module_t mjpeg_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_t)&globals_dict,
};

MP_REGISTER_MODULE(MP_QSTR_mjpeg, mjpeg_module, MICROPY_PY_OMV_MJPGE);

#endif // MICROPY_PY_OMV_MJPGE