/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __APP_CMD_H__
#define __APP_CMD_H__
#ifdef __cplusplus
extern "C"
{
#endif
#ifdef __PC_CMD_UART__

#include "hal_cmd.h"
typedef struct {
    void *param;
} APP_CMD_HANDLE;

void app_cmd_open(void);

void app_cmd_close(void);
#endif
#ifdef __cplusplus
}
#endif
#endif//__FMDEC_H__
