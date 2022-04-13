/**
 * @file aiot_state_api.c
 * @brief 状态码模块实现
 * @date 2019-12-27
 *
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 *
 */

#include "core_stdinc.h"
#include "aiot_state_api.h"

aiot_state_logcb_t g_logcb_handler = NULL;

int32_t aiot_state_set_logcb(aiot_state_logcb_t handler)
{
    g_logcb_handler = handler;
    return 0;
}

