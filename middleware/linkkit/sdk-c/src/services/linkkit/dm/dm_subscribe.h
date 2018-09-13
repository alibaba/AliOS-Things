/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _DM_SUBSCRIBE_H_
#define _DM_SUBSCRIBE_H_

typedef enum {
    DM_SUB_MULTI_TYPE_NEW,
    DM_SUB_MULTI_TYPE_RETRY
} dm_sub_multi_type_t;

int dm_sub_multi(_IN_ char **subscribe, _IN_ int count);

int dm_sub_multi_next(_IN_ dm_sub_multi_type_t type, _IN_ int devid, _IN_ int index);

int dm_sub_local_register(void);

#endif
