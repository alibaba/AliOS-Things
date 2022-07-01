/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Image I/O Python module.
 */
#include "imlib_config.h"
#if defined(IMLIB_ENABLE_IMAGE_IO)

#include <stdio.h>
#include "py/obj.h"
#include "py/nlr.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "py_assert.h"
#include "py_helper.h"
#include "py_image.h"
#include "py_imageio.h"
#include "fb_alloc.h"
#include "framebuffer.h"
#include "common.h"
#if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
#include "ff_wrapper.h"
#endif
#include "omv_boardconfig.h"

typedef enum image_io_stream_type {
    IMAGE_IO_FILE_STREAM,
    IMAGE_IO_MEMORY_STREAM,
    IMAGE_IO_FILE_READ  = 'r',
    IMAGE_IO_FILE_WRITE = 'w',
} image_io_stream_type_t ;

typedef struct _py_imageio_obj {
    mp_obj_base_t base;
    bool closed;
    union {
        #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
        struct{
            FIL fp;
            uint32_t ms;
            uint32_t mode;
            int version;
        };
        #endif
        struct {
            uint32_t w;
            uint32_t h;
            uint32_t bpp;
            uint32_t f_size;
            uint32_t f_count;
            uint32_t offset;
            uint8_t *buffer;
        };
    };
    image_io_stream_type_t type;
} py_imageio_obj_t;

static void py_imageio_print(const mp_print_t *print, mp_obj_t self, mp_print_kind_t kind)
{
    py_imageio_obj_t *stream = MP_OBJ_TO_PTR(self);
    if (0) {
    #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
    } else if (stream->type == IMAGE_IO_FILE_STREAM) {
        mp_printf(print, "{\"type\": file stream \"size\":%d}", f_size(&stream->fp));
    #endif
    } else if (stream->type == IMAGE_IO_MEMORY_STREAM) {
        mp_printf(print, "{\"type\": memory stream \"size\":%d}", stream->f_size * stream->f_count);
    } else {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Invalid image stream"));
    }
}

mp_obj_t py_imageio_size(mp_obj_t self)
{
    py_imageio_obj_t *stream = MP_OBJ_TO_PTR(self);
    if (0) {
    #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
    } else if (stream->type == IMAGE_IO_FILE_STREAM) {
        return mp_obj_new_int(f_size(&stream->fp));
    #endif
    } else if (stream->type == IMAGE_IO_MEMORY_STREAM) {
        return mp_obj_new_int(stream->f_size * stream->f_count);
    } else {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Invalid image stream"));
    }
    return mp_obj_new_int(-1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_imageio_size_obj, py_imageio_size);

mp_obj_t py_imageio_write(mp_obj_t self, mp_obj_t img_obj)
{
    py_imageio_obj_t *stream = MP_OBJ_TO_PTR(self);
    PY_ASSERT_TRUE_MSG((stream->closed == false), "Stream closed");
    image_t *image = py_image_cobj(img_obj);

    if (0) {
    #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
    } else if (stream->type == IMAGE_IO_FILE_STREAM) {
        PY_ASSERT_TRUE_MSG((stream->mode == IMAGE_IO_FILE_WRITE), "Invalid stream direction");

        FIL *fp = &stream->fp;
        uint32_t ms = mp_hal_ticks_ms(); // Write out elapsed ms.
        write_long(fp, ms - stream->ms);
        stream->ms = ms;

        write_long(fp, image->w);
        write_long(fp, image->h);
        write_long(fp, image->bpp);
        uint32_t size = image_size(image);

        write_data(fp, image->data, size);
        if (size % 16) {
            // Pad to multiple of 16 bytes.
            write_data(fp, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16 - (size % 16));
        }
    #endif
    } else if (stream->type == IMAGE_IO_MEMORY_STREAM) {
        if (stream->offset == stream->f_count) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("EOF"));
        }
        uint32_t size = image_size(image);
        PY_ASSERT_TRUE_MSG((size == stream->f_size), "Invalid frame size");
        memcpy(stream->buffer + (stream->offset * size), image->data, size);
        stream->offset += 1;
    } else {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Invalid image stream"));
    }

    return self;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(py_imageio_write_obj, py_imageio_write);

mp_obj_t py_imageio_read(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t image = {0};
    mp_obj_t copy_to_fb_obj;
    py_imageio_obj_t *stream = MP_OBJ_TO_PTR(args[0]);
    PY_ASSERT_TRUE_MSG((stream->closed == false), "Stream closed");

    bool copy_to_fb = true;
    image_t *arg_other = NULL;
    copy_to_fb_obj = py_helper_keyword_object(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_copy_to_fb), NULL);

    if (copy_to_fb_obj) {
        if (mp_obj_is_integer(copy_to_fb_obj)) {
            copy_to_fb = mp_obj_get_int(copy_to_fb_obj);
        } else {
            arg_other = py_helper_arg_to_image_mutable(copy_to_fb_obj);
        }
    }

    if (copy_to_fb) {
        framebuffer_update_jpeg_buffer();
    }

    if (0) {
    #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
    } else if (stream->type == IMAGE_IO_FILE_STREAM) {
        PY_ASSERT_TRUE_MSG((stream->mode == IMAGE_IO_FILE_READ), "Invalid stream direction");

        FIL *fp = &stream->fp;
        if (f_eof(fp)) {
            if (!py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_loop), true)) {
                return mp_const_none;
            }

            file_seek(fp, 16); // skip past the header

            if (f_eof(fp)) { // empty file
                return mp_const_none;
            }
        }

        uint32_t ms = 0, ms_tmp;
        read_long(fp, &ms_tmp);

        if (!py_helper_keyword_int(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_pause), true)) {
            for (ms = mp_hal_ticks_ms();
                    ((ms - stream->ms) < ms_tmp);
                    ms = mp_hal_ticks_ms()) {
                __WFI();
            }
        }
        stream->ms = ms;
        read_long(fp, (uint32_t *) &image.w);
        read_long(fp, (uint32_t *) &image.h);
        read_long(fp, (uint32_t *) &image.bpp);
        uint32_t size = image_size(&image);

        if (copy_to_fb) {
            py_helper_set_to_framebuffer(&image);
        } else if (arg_other) {
            PY_ASSERT_TRUE_MSG((size <= image_size(arg_other)), "The new image won't fit in the target frame buffer!");
            image.data = arg_other->data;
        } else {
            image.data = xalloc(size);
        }

        char ignore[15];
        read_data(fp, image.data, size);

        // Check if original byte reversed data.
        if ((image.bpp == IMAGE_BPP_RGB565) && (stream->version == 10)) {
            uint32_t *data_ptr = (uint32_t *) image.data;
            size_t data_len = image.w * image.h;

            for (; data_len >= 2; data_len -= 2, data_ptr += 1) {
                *data_ptr = __REV16(*data_ptr); // long aligned
            }

            if (data_len) {
                *((uint16_t *) data_ptr) = __REV16(*((uint16_t *) data_ptr)); // word aligned
            }
        }

        if (size % 16) {
            // Read in to multiple of 16 bytes.
            read_data(fp, ignore, 16 - (size % 16));
        }

        py_helper_update_framebuffer(&image);

        if (arg_other) {
            arg_other->w = image.w;
            arg_other->h = image.h;
            arg_other->bpp = image.bpp;
       }
    #endif
    } else if (stream->type == IMAGE_IO_MEMORY_STREAM) {
        if (stream->offset == stream->f_count) {
            return mp_const_none;
        }
        image.w = stream->w;
        image.h = stream->h;
        image.bpp = stream->bpp;
        uint32_t size = stream->f_size;

        if (copy_to_fb) {
            py_helper_set_to_framebuffer(&image);
        } else if (arg_other) {
            PY_ASSERT_TRUE_MSG((size <= image_size(arg_other)), "The new image won't fit in the target frame buffer!");
            image.data = arg_other->data;
        } else {
            image.data = xalloc(size);
        }
        memcpy(image.data, stream->buffer + stream->offset * size, size);
        stream->offset += 1;
    } else {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Invalid image stream"));
    }

    return py_image_from_struct(&image);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_imageio_read_obj, 1, py_imageio_read);

mp_obj_t py_imageio_seek(mp_obj_t self, mp_obj_t offs)
{
    py_imageio_obj_t *stream = MP_OBJ_TO_PTR(self);
    PY_ASSERT_TRUE_MSG((stream->closed == false), "Stream closed");

    if (0) {
    #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
    } else if (stream->type == IMAGE_IO_FILE_STREAM) {
        // TODO not implemented for file stream.
    #endif
    } else if (stream->type == IMAGE_IO_MEMORY_STREAM) {
        uint32_t offset = mp_obj_get_int(offs);
        if (offset < 0 || offset > stream->f_count) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Invalid stream offset"));
        }
        stream->offset = offset;
    } else {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Invalid image stream"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(py_imageio_seek_obj, py_imageio_seek);

mp_obj_t py_imageio_close(mp_obj_t self)
{
    py_imageio_obj_t *stream = MP_OBJ_TO_PTR(self);
    PY_ASSERT_TRUE_MSG((stream->closed == false), "Stream closed");

    if (0) {
    #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
    } else if (stream->type == IMAGE_IO_FILE_STREAM) {
        file_close(&stream->fp);
    #endif
    } else if (stream->type == IMAGE_IO_MEMORY_STREAM) {
        fb_alloc_free_till_mark_past_mark_permanent();
    } else {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Invalid image stream"));
    }
    stream->closed = true;
    return self;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_imageio_close_obj, py_imageio_close);

mp_obj_t py_imageio_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    // check arguments
    mp_arg_check_num(n_args, n_kw, 2, 2, false);

    py_imageio_obj_t *stream = NULL;
    stream = m_new_obj(py_imageio_obj_t);
    stream->base.type = &py_imageio_type;
    stream->closed = false;

    if (0) {
    #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
    } else if (mp_obj_is_str(args[0])) { // File I/O
        stream->type = IMAGE_IO_FILE_STREAM;
        stream->mode = mp_obj_str_get_str(args[1])[0];
        if (stream->mode == IMAGE_IO_FILE_READ) {
            uint8_t version_hi, version_lo;
            file_read_open(&stream->fp, mp_obj_str_get_str(args[0]));
            read_long_expect(&stream->fp, *((uint32_t *) "OMV ")); // OpenMV
            read_long_expect(&stream->fp, *((uint32_t *) "IMG ")); // Image
            read_long_expect(&stream->fp, *((uint32_t *) "STR ")); // Stream
            read_byte_expect(&stream->fp, 'V');
            read_byte(&stream->fp, &version_hi);
            read_byte_expect(&stream->fp, '.');
            read_byte(&stream->fp, &version_lo);
            if ((version_hi != '1') || ((version_lo != '0') && (version_lo != '1'))) {
                mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Expected version V1.0 or V1.1"));
            }
            stream->version = ((version_hi - '0') * 10) + (version_lo - '0');
        } else if (stream->mode == IMAGE_IO_FILE_WRITE) {
            file_write_open(&stream->fp, mp_obj_str_get_str(args[0]));
            write_long(&stream->fp, *((uint32_t *) "OMV ")); // OpenMV
            write_long(&stream->fp, *((uint32_t *) "IMG ")); // Image
            write_long(&stream->fp, *((uint32_t *) "STR ")); // Stream
            write_long(&stream->fp, *((uint32_t *) "V1.1")); // v1.1
            stream->version = 11;
        } else {
            mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Invalid stream mode, expected 'r' or 'w'"));
        }
        stream->ms = mp_hal_ticks_ms();
    #endif
    } else if (mp_obj_is_type(args[0], &mp_type_tuple)) { // Memory Stream I/O
        mp_obj_t *image_info;
        mp_obj_get_array_fixed_n(args[0], 3, &image_info);

        stream->type    = IMAGE_IO_MEMORY_STREAM;
        stream->f_count = mp_obj_get_int(args[1]);
        stream->w       = mp_obj_get_int(image_info[0]);
        stream->h       = mp_obj_get_int(image_info[1]);
        stream->bpp     = mp_obj_get_int(image_info[2]);
        image_t image = {.w = stream->w, .h = stream->h, .bpp = stream->bpp, .pixels = NULL};
        stream->f_size  = image_size(&image);
        stream->offset  = 0;

        fb_alloc_mark();
        stream->buffer = fb_alloc(stream->f_size * stream->f_count, FB_ALLOC_NO_HINT);
        fb_alloc_mark_permanent();
    } else {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Invalid stream type"));
    }

    return MP_OBJ_FROM_PTR(stream);
}

STATIC const mp_rom_map_elem_t py_imageio_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_size),            MP_ROM_PTR(&py_imageio_size_obj)  },
    { MP_ROM_QSTR(MP_QSTR_write),           MP_ROM_PTR(&py_imageio_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_read),            MP_ROM_PTR(&py_imageio_read_obj)  },
    { MP_ROM_QSTR(MP_QSTR_seek),            MP_ROM_PTR(&py_imageio_seek_obj)  },
    { MP_ROM_QSTR(MP_QSTR_close),           MP_ROM_PTR(&py_imageio_close_obj) }
};
STATIC MP_DEFINE_CONST_DICT(py_imageio_locals_dict, py_imageio_locals_dict_table);

const mp_obj_type_t py_imageio_type = {
    { &mp_type_type },
    .name = MP_QSTR_ImageIO,
    .print = py_imageio_print,
    .make_new = py_imageio_make_new,
    .locals_dict = (mp_obj_dict_t*)&py_imageio_locals_dict,
};
#endif //IMLIB_ENABLE_IMAGE_IO
