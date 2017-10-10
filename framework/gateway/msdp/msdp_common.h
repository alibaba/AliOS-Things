/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __MSDP_COMMON_H__
#define __MSDP_COMMON_H__

#include <stdint.h>
#include <unistd.h>
#include <stdarg.h>
#include "msdp.h"

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

int msdp_set_attr_each_cb(const char *attr_name, int name_len, int type,
                          va_list *ap);

int msdp_get_attr_each_cb(const char *attr_name, int name_len, int type,
                          va_list *ap);

int msdp_set_status_array(char *params);

int msdp_set_device_status_handler(char *params, void *cb, void *exec_cb);

int msdp_get_device_status_handler(char *params, void *cb, void *exec_cb,
                                   char **json_out);

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
