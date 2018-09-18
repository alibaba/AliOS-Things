/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _DM_SUBSCRIBE_H_
#define _DM_SUBSCRIBE_H_

int dm_sub_multi(_IN_ char **subscribe, _IN_ int count);

int dm_sub_multi_next(_IN_ int devid, _IN_ int index);

int dm_sub_deprecated_shadow_create(int devid);

int dm_sub_shadow_destroy(int devid);

int dm_sub_shadow_next(int devid, int index);

int dm_sub_local_register(void);

#ifdef DEPRECATED_LINKKIT
    int dm_sub_deprecated_shadow_create(int devid);
#endif

#endif
