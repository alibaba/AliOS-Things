/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _IOTX_AUTH_H_
#define _IOTX_AUTH_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "iot_import.h"
#include "device.h"     // for deviceInfo{} + userInfo{}

int iotx_guider_authenticate(void);

#if defined(__cplusplus)
}
#endif
#endif
