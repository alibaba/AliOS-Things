/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __IOTX_MQTT_INTERNAL_H__
#define __IOTX_MQTT_INTERNAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "infra_state.h"
#include "infra_compat.h"
#include "infra_types.h"
#include "infra_defs.h"
#include "infra_string.h"
#include "infra_list.h"
#include "infra_log.h"
#include "infra_report.h"
#include "infra_net.h"
#include "infra_sha256.h"

#include "dev_sign_api.h"
#include "mqtt_wrapper.h"
#include "iotx_mqtt_config.h"
#include "iotx_mqtt_client.h"

#include "MQTTPacket.h"

#ifdef INFRA_LOG
    #include "infra_log.h"
    #define mqtt_emerg(...)             log_emerg("MQTT", __VA_ARGS__)
    #define mqtt_crit(...)              log_crit("MQTT", __VA_ARGS__)
    #define mqtt_err(...)               log_err("MQTT", __VA_ARGS__)
    #define mqtt_warning(...)           log_warning("MQTT", __VA_ARGS__)
    #define mqtt_info(...)              log_info("MQTT", __VA_ARGS__)
    #define mqtt_debug(...)             log_debug("MQTT", __VA_ARGS__)
#else
    #define mqtt_emerg(fmt, ...)        LOGF("MQTT", fmt, ##__VA_ARGS__)
    #define mqtt_crit(fmt, ...)         LOGF("MQTT", fmt, ##__VA_ARGS__)
    #define mqtt_err(fmt, ...)          LOGE("MQTT", fmt, ##__VA_ARGS__)
    #define mqtt_warning(fmt, ...)      LOGW("MQTT", fmt, ##__VA_ARGS__)
    #define mqtt_info(fmt, ...)         LOGI("MQTT", fmt, ##__VA_ARGS__)
    #define mqtt_debug(fmt, ...)        LOGD("MQTT", fmt, ##__VA_ARGS__)
#endif

#endif  /* __IOTX_MQTT_INTERNAL_H__ */

