/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) Alibaba AIoT
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

#include "utility.h"

void callback_to_python(mp_obj_t function, mp_obj_t arg)
{
    bool ret = mp_sched_schedule(function, MP_OBJ_FROM_PTR(arg));
    if (ret == false) {
        printf("[utility]: schedule queue is full !!!!\r\n");
    }
    mp_hal_wake_main_task_from_isr();
}

mp_obj_t mp_obj_new_strn(const char *data)
{
    return mp_obj_new_str(data, strlen(data));
}

const char *get_str_from_dict(mp_obj_t dict, const char *key)
{
    mp_obj_t value_obj = mp_obj_dict_get(dict, mp_obj_new_strn(key));
    return (char *)mp_obj_str_get_str(value_obj);
}

int get_int_from_dict(mp_obj_t dict, const char *key)
{
    mp_obj_t value_obj = mp_obj_dict_get(dict, mp_obj_new_strn(key));
    return mp_obj_get_int(value_obj);
}