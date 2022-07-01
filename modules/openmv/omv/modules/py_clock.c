/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Clock Python module.
 */
#include "py/obj.h"
#include "py/mphal.h"
#include "py_clock.h"

/* Clock Type */
typedef struct _py_clock_obj_t {
    mp_obj_base_t base;
    uint32_t t_start;
    uint32_t t_ticks;
    uint32_t t_frame;
} py_clock_obj_t;

mp_obj_t py_clock_tick(mp_obj_t clock_obj)
{
    py_clock_obj_t *clock = (py_clock_obj_t*) clock_obj;
    clock->t_start = mp_hal_ticks_ms();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_clock_tick_obj,  py_clock_tick);

mp_obj_t py_clock_fps(mp_obj_t clock_obj)
{
    py_clock_obj_t *clock = (py_clock_obj_t*) clock_obj;
    clock->t_frame++;
    clock->t_ticks += (mp_hal_ticks_ms() - clock->t_start);
    float fps = 1000.0f / (clock->t_ticks/(float)clock->t_frame);
    return mp_obj_new_float(fps);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_clock_fps_obj,   py_clock_fps);

mp_obj_t py_clock_avg(mp_obj_t clock_obj)
{
    py_clock_obj_t *clock = (py_clock_obj_t*) clock_obj;
    clock->t_frame++;
    clock->t_ticks += (mp_hal_ticks_ms() - clock->t_start);
    return mp_obj_new_float(clock->t_ticks/(float)clock->t_frame);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_clock_avg_obj,   py_clock_avg);

mp_obj_t py_clock_reset(mp_obj_t clock_obj)
{
    py_clock_obj_t *clock = (py_clock_obj_t*) clock_obj;
    clock->t_start = 0;
    clock->t_ticks = 0;
    clock->t_frame = 0;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_clock_reset_obj, py_clock_reset);

STATIC void py_clock_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    py_clock_obj_t *self = self_in;
    mp_printf(print, "t_start:%d t_ticks:%d t_frame:%d\n", self->t_start, self->t_ticks, self->t_frame);
}

mp_obj_t py_clock_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    py_clock_obj_t *clock =NULL;
    clock = m_new_obj(py_clock_obj_t);
    clock->base.type = &py_clock_type;
    clock->t_start = 0;
    clock->t_ticks = 0;
    clock->t_frame = 0;
    return MP_OBJ_FROM_PTR(clock);
}

STATIC const mp_rom_map_elem_t py_clock_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_tick),   MP_ROM_PTR(&py_clock_tick_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_fps),    MP_ROM_PTR(&py_clock_fps_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_avg),    MP_ROM_PTR(&py_clock_avg_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_reset),  MP_ROM_PTR(&py_clock_reset_obj)},
    { NULL, NULL },
};
STATIC MP_DEFINE_CONST_DICT(py_clock_locals_dict, py_clock_locals_dict_table);

const mp_obj_type_t py_clock_type = {
    { &mp_type_type },
    .name  = MP_QSTR_Clock,
    .print = py_clock_print,
    .make_new = py_clock_make_new,
    .locals_dict = (mp_obj_t)&py_clock_locals_dict,
};
