/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __GUIDER_INTERNAL_H__
#define __GUIDER_INTERNAL_H__
#define _GNU_SOURCE

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "iot_import.h"

#include "lite-log.h"
#include "lite-utils.h"
#include "utils_md5.h"
#include "utils_base64.h"
#include "utils_hmac.h"
#include "utils_httpc.h"
#include "ca.h"
//#include "tfs.h"
#include "guider.h"

#define GUIDER_IOT_ID_LEN           (256)
#define GUIDER_IOT_TOKEN_LEN        (512)
#define GUIDER_DEFAULT_TS_STR       "2524608000000"

#define CONN_SECMODE_LEN            (32)

#define GUIDER_SIGN_LEN             (256)
#define GUIDER_PID_LEN              (64)
#define GUIDER_TS_LEN               (16)
#define GUIDER_URL_LEN              (256)

#define GUIDER_DEVCODE_LEN          (256)
#define GUIDER_URLENCODE_LEN        (256)

#define GUIDER_DIRECT_DOMAIN        "iot-as-mqtt.cn-shanghai.aliyuncs.com"

#endif  /* __GUIDER_INTERNAL_H__ */
