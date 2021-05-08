
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/stackctrl.h"

extern void mp_hal_wake_main_task_from_isr();

void callback_to_python(mp_obj_t function, mp_obj_t arg);

mp_obj_t mp_obj_new_strn(const char *data);
