/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "py/runtime.h"
#include "py/smallint.h"
#include "py/obj.h"

#if MICROPY_PY_SDCARD

static FILE *fp = NULL;

STATIC mp_obj_t sdcard_open(mp_obj_t path_in, mp_obj_t mode_in)
{
    const char *path = mp_obj_str_get_str(path_in);
    const char *mode = mp_obj_str_get_str(mode_in);

    fp = fopen(path, mode);
    if (fp == NULL) {
        printf("[%s]: Failed to open file: %s\n", __func__, path);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_sdcard_open_obj, sdcard_open);

STATIC mp_obj_t sdcard_close(void)
{
    int status = -1;
    if (fp != NULL) {
        status = fclose(fp);
        fp = NULL;
    }
    return mp_obj_new_int(status);
}
MP_DEFINE_CONST_FUN_OBJ_0(mp_sdcard_close_obj, sdcard_close);

STATIC mp_obj_t sdcard_read(mp_obj_t read_buf, mp_obj_t size_want_in)
{
    mp_int_t size_want = mp_obj_get_int(size_want_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(read_buf, &bufinfo, MP_BUFFER_WRITE);
    memset(bufinfo.buf, 0, bufinfo.len);

    int size_read = -1;
    if (fp != NULL) {
        size_read = fread(bufinfo.buf, 1, size_want, fp);
    }

    return mp_obj_new_int(size_read);
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_sdcard_read_obj, sdcard_read);

STATIC mp_obj_t sdcard_write(mp_obj_t buf_in, mp_obj_t write_want_in)
{
    mp_int_t write_want = mp_obj_get_int(write_want_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    int size_write = -1;
    if (fp != NULL) {
        size_write = fwrite(bufinfo.buf, 1, write_want, fp);
    }

    return mp_obj_new_int(size_write);
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_sdcard_write_obj, sdcard_write);

STATIC const mp_rom_map_elem_t sdcard_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sdcard)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_sdcard_open_obj)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_sdcard_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_sdcard_read_obj)},
    {MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_sdcard_write_obj)},
};

STATIC MP_DEFINE_CONST_DICT(sdcard_module_globals, sdcard_module_globals_table);

const mp_obj_module_t sdcard_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&sdcard_module_globals,
};

#endif
