/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#ifdef AOS_COMP_UND

#include "linkkit/mqtt_api.h"

#include "und_log.h"
#include "und_types.h"
#include "und_utils.h"
#include "und_config.h"
#include "und_adapter.h"
#include "und_platform.h"

#ifdef UND_CONFIG_USE_UAGENT
#include "uagent/uagent.h"
#include "linkkit/infra/infra_cjson.h"
#endif

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#define UND_CONFIG_INVALID  "please enable UND_CONFIG_USE_MQTT/UAGENT in menuconfig"

#ifdef UND_CONFIG_USE_UAGENT
static char und_uagent_regist = 0;
#endif
int und_conn_register_cb(char *topic, void *cb)
{
    UND_PTR_SANITY_CHECK(topic, UND_PARAM_ERR);
    UND_PTR_SANITY_CHECK(cb, UND_PARAM_ERR);

#if defined(UND_CONFIG_USE_MQTT)
    if (IOT_MQTT_Subscribe(NULL, topic, 0, (iotx_mqtt_event_handle_func_fpt)cb, NULL) < 0)
        return UND_ERR;
#endif
#if defined(UND_CONFIG_USE_UAGENT)
    if (und_uagent_regist == 0) {
        if (0 != uagent_register(UND_USE_UAGENT_MOD, UND_USE_UAGENT_MOD_NAME, UND_USE_UAGENT_MOD_VER,
                UND_USE_UAGENT_FUNC_VIEW, UND_USE_UAGENT_FUNC_VIEW_NAME, NULL, NULL)) {
            return UND_ERR;
        }
        und_uagent_regist = 1;
    }
#elif defined(UND_CONFIG_USE_MQTT)
#else
#error "please enable UND_CONFIG_USE_MQTT or UND_CONFIG_USE_UAGENT in menuconfig"
    return UND_ERR;
#endif
    return UND_SUCCESS;
}

int und_conn_unregister_cb(char *topic)
{
    UND_PTR_SANITY_CHECK(topic, UND_PARAM_ERR);

#if defined(UND_CONFIG_USE_MQTT)
    if (IOT_MQTT_Unsubscribe(NULL, topic) < 0)
        return UND_ERR;
#endif
#if defined(UND_CONFIG_USE_UAGENT)
    if (0 != uagent_unregister(UND_USE_UAGENT_MOD, UND_USE_UAGENT_FUNC_VIEW)) {
        return UND_ERR;
    }
#elif defined(UND_CONFIG_USE_MQTT)
#else
    return UND_ERR;
#endif
    return UND_SUCCESS;
}

int und_conn_send(char *topic, void *data, int len, int qos)
{
    int res;
    UND_PTR_SANITY_CHECK(topic, UND_PARAM_ERR);
    UND_PTR_SANITY_CHECK(data, UND_PARAM_ERR);
    UND_PARAM_RANGE_SANITY_CHECK(len, UND_REPORT_TARGET_BUF_LEN, 1, UND_PARAM_ERR);

#if defined(UND_CONFIG_USE_UAGENT)
    lite_cjson_t lite, lite_params;
    unsigned char policy;
    aos_memset(&lite, 0, sizeof(lite));
    if (lite_cjson_parse(data, len, &lite) != 0) {
        return UND_PARAM_ERR;
    }
    aos_memset(&lite_params, 0, sizeof(lite_params));
    if (lite_cjson_object_item(&lite, "params", aos_strlen("params"), &lite_params) != 0) {
        return UND_PARAM_ERR;
    }
    policy = UND_USE_UAGENT_TX_POLICY_OBJ;
    if (qos != 0)
        policy |= UND_USE_UAGENT_TX_QOS1;
    und_debug("uagent:%.*s\n", lite_params.value_length, lite_params.value);
    res = uagent_send(UND_USE_UAGENT_MOD, UND_USE_UAGENT_FUNC_VIEW,
            lite_params.value_length, lite_params.value, policy);
#elif defined(UND_CONFIG_USE_MQTT)
#else
    return UND_ERR;
#endif

#if defined(UND_CONFIG_USE_MQTT)
    res = IOT_MQTT_Publish_Simple(NULL, topic, qos, data, len);  /* IOTX_MQTT_QOS1 or IOTX_MQTT_QOS1 */
#endif
    return res;
}

int und_conn_is_connected()
{
#if defined(UND_CONFIG_USE_MQTT)
    return IOT_MQTT_CheckStateNormal(NULL);
#elif defined(UND_CONFIG_USE_UAGENT)
    return IOT_MQTT_CheckStateNormal(NULL);
#else
    return UND_ERR;
#endif
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif
