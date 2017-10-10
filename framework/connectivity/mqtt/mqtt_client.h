/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _IOTX_MQTT_CLIENT_H_
#define _IOTX_MQTT_CLIENT_H_
#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iot_import.h"

/* maximum number of successful subscribe */
#define IOTX_MC_SUB_NUM_MAX                     (10)

/* maximum republish elements in list */
#define IOTX_MC_REPUB_NUM_MAX                   (20)

#if defined(__cplusplus)
}
#endif
#endif  /* #ifndef _IOTX_MQTT_CLIENT_H_ */
