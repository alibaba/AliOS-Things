/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __BT_STATUS_CONV_H__
#define __BT_STATUS_CONV_H__
#include "bluetooth.h"
#include "bttypes.h"

extern const bt_status_t status_table[];
static inline bt_status_t bt_convert_status(BtStatus stack_status)
{
    if (stack_status <= BT_STATUS_LAST_CODE)
        return status_table[stack_status];
    else
        return BT_STS_LAST_CODE;
}

#define BT_STATUS(stack_status) bt_convert_status(stack_status)

#endif/*__BT_STATUS_CONV_H__*/
