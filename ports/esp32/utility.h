#ifndef PY_OBJ_UTILITY_H
#define PY_OBJ_UTILITY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "py/builtin.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mpstate.h"

extern void mp_hal_wake_main_task_from_isr();

bool callback_to_python_LoBo(mp_obj_t function, mp_obj_t arg, void *carg);

mp_obj_t mp_obj_new_strn(const char *data);

const char *get_str_from_dict(mp_obj_t dict, const char *key);

int get_int_from_dict(mp_obj_t dict, const char *key);

#endif
