/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _OTA_IMPORT_H_
#define _OTA_IMPORT_H_

#include "iot_import.h"
#include "iot_export_mqtt.h"
#include "lite-log.h"
#include "json_parser.h"
#include "utils_md5.h"
#include "utils_debug.h"
#include "utils_httpc.h"


#define OTA_MALLOC          HAL_Malloc
#define OTA_FREE            HAL_Free
#define OTA_LOG_DEBUG       log_debug
#define OTA_LOG_INFO        log_info
#define OTA_LOG_ERROR       log_err
#define OTA_ASSERT          IOTX_ASSERT


#endif /* _OTA_IMPORT_H_ */
