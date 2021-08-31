/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __BT_STATUS_CONV_H__
#define __BT_STATUS_CONV_H__
#include "bluetooth.h"
#include "btlib_type.h"
#include "hal_trace.h"

extern const bt_status_t status_table[];

#if defined(__cplusplus)
extern "C" {
#endif
char *btifstatus2str(bt_status_t status);
char *btstatus2str(BtStatus status);
#if defined(__cplusplus)
}
#endif

//static inline bt_status_t bt_convert_status(BtStatus stack_status)
//{
//    // TRACE("%s:input=%d%s,output=%d%s", __func__, stack_status, btstatus2str(stack_status), status_table[stack_status], btifstatus2str(status_table[stack_status]));
//
//    if (stack_status <= BT_STATUS_LAST_CODE)
//        return status_table[stack_status];
//    else
//        return BT_STS_LAST_CODE;
//}
//
//#define BT_STATUS(stack_status) bt_convert_status(stack_status)

#endif/*__BT_STATUS_CONV_H__*/
