/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _BT_BAS_H_
#define _BT_BAS_H_
#include "aos/list.h"

typedef struct _bas_t {
    uint16_t conn_handle;
    uint16_t bas_svc_handle;
    int16_t ccc;
    uint8_t battery_level;
    slist_t next;
} bas_t;

typedef bas_t *bas_handle_t;

bas_handle_t bas_init(bas_t *bas);
int bas_level_update(bas_handle_t handle, uint8_t level);

#endif
