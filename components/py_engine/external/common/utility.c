/*
 * The MIT License (MIT)
 *
 * Copyright (c) Alibaba AIoT
 *
 */

#include "utility.h"

bool callback_to_python_LoBo(mp_obj_t function, mp_obj_t arg, void *carg) {
  bool ret = false;
  if (function != MP_OBJ_NULL && mp_obj_is_callable(function)) {
    ret = mp_sched_schedule_LoBo(function, arg, carg);
    if (ret == false) {
      printf("[utility]: schedule queue is full !!!!\r\n");
    }
    mp_hal_wake_main_task_from_isr();
  }
  return ret;
}

bool callback_to_python(mp_obj_t function, mp_obj_t arg) {
  bool ret = false;
  if (function != MP_OBJ_NULL && mp_obj_is_callable(function)) {
    ret = mp_sched_schedule(function, arg);
    if (ret == false) {
      printf("[utility]: schedule queue is full !!!!\r\n");
    }
    mp_hal_wake_main_task_from_isr();
  }
  return ret;
}

mp_obj_t mp_obj_new_strn(const char *data) {
  return mp_obj_new_str(data, strlen(data));
}

const char *get_str_from_dict(mp_obj_t dict, const char *key) {
  mp_obj_t value_obj = mp_obj_dict_get(dict, mp_obj_new_strn(key));
  return (char *)mp_obj_str_get_str(value_obj);
}

int get_int_from_dict(mp_obj_t dict, const char *key) {
  mp_obj_t value_obj = mp_obj_dict_get(dict, mp_obj_new_strn(key));
  return mp_obj_get_int(value_obj);
}