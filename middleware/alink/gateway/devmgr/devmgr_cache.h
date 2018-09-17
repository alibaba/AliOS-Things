/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __DEVMGR_CACHE__H__
#define __DEVMGR_CACHE__H__

#include "devmgr.h"
#include <unistd.h>
#ifdef __cplusplus
extern "C" {
#endif

void __dump_attr_cache(dlist_t *attr_head);

int devmgr_get_attr_cache(const char *devid_or_uuid, const char *attr_name,
                          char *attr_value_buff, int buff_size);
int devmgr_read_attr_cache(const char *devid_or_uuid, const char *attr_name,
                           char **attr_value);
int devmgr_update_attr_cache(const char *devid_or_uuid, const char *attr_name,
                             const char *attr_value);
void devmgr_free_device_cache(dev_info_t *devinfo);
int devmgr_cache_init();
void devmgr_cache_exit();

#ifdef __cplusplus
}
#endif
#endif

