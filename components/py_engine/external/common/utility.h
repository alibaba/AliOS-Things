#ifndef PY_OBJ_UTILITY_H
#define PY_OBJ_UTILITY_H

#include "py/obj.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void mp_hal_wake_main_task_from_isr();

bool callback_to_python_LoBo(mp_obj_t function, mp_obj_t arg, void *carg);

bool callback_to_python(mp_obj_t function, mp_obj_t arg);

mp_obj_t mp_obj_new_strn(const char *data);

const char *get_str_from_dict(mp_obj_t dict, const char *key);

int get_int_from_dict(mp_obj_t dict, const char *key);

#endif