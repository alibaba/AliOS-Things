/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _BT_HRS_H_
#define _BT_HRS_H_
#include "aos/list.h"

typedef struct _hrs_t {
    uint16_t conn_handle;
    uint16_t hrs_svc_handle;
    int16_t mea_ccc;
    uint8_t hrs_mea_flag;
    uint8_t hrs_mea_level;
    slist_t next;
} hrs_t;

typedef hrs_t *hrs_handle_t;

hrs_handle_t hrs_init(hrs_t *hrs);
int hrs_measure_level_update(hrs_handle_t handle, uint8_t *data, uint8_t length);

#endif

