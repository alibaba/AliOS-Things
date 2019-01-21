/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifdef MQTT_COMM_ENABLED
#ifdef MQTT_LOGPOST

#include "sdk-impl_internal.h"

#define LOG_PUBLISH_MSG_MAXLEN   (255)

#define IMPL_LOGPOST_MALLOC(size)    LITE_malloc(size, MEM_MAGIC, "impl.logpost")
#define IMPL_LOGPOST_FREE(ptr)       LITE_free(ptr)

static int iotx_mc_log_post(void *pclient, char *payload);

static const char THING_LOG_POST_PARAMS[] =
            "{\"id\":\"%d\",\"version\":\"1.0\",\"params\":[{\"timestamp\":%lld,\"logLevel\":\"%s\",\"module\":\"%s\",\"logContent\":\"%s";

static const char THING_LOG_POST_PARAMS_TAIL[] =
            "...\"}],\"method\":\"thing.log.post\"}";

/* Post log information to cloud through mqtt */
int IOT_MQTT_LogPost(void *pHandle, const char *level, const char *module, const char *msg)
{
    char *logbuf = NULL;
    int msgid = 123;
    int length = 0;
    int ret = 0;

    if (!pHandle || !module || !level || !msg) {
        return FAIL_RETURN;
    }

    logbuf = IMPL_LOGPOST_MALLOC(LOG_PUBLISH_MSG_MAXLEN + 1);
    if (logbuf == NULL) {
        return FAIL_RETURN;
    }
    memset(logbuf, 0, LOG_PUBLISH_MSG_MAXLEN + 1);

    /* generate log post json data */
    ret = HAL_Snprintf(logbuf, LOG_PUBLISH_MSG_MAXLEN, THING_LOG_POST_PARAMS, msgid, HAL_UTC_Get(), level, module, msg);
    if (ret < 0) {
        IMPL_LOGPOST_FREE(logbuf);
        return FAIL_RETURN;
    }

    length = strlen(logbuf);
    if (length >= (LOG_PUBLISH_MSG_MAXLEN - sizeof(THING_LOG_POST_PARAMS_TAIL))) {
        strcpy(logbuf + LOG_PUBLISH_MSG_MAXLEN - sizeof(THING_LOG_POST_PARAMS_TAIL) - 3, THING_LOG_POST_PARAMS_TAIL);
    } else {
        strcat(logbuf, THING_LOG_POST_PARAMS_TAIL + 3);
    }

    ret = iotx_mc_log_post(pHandle, logbuf);
    if (ret < 0) {
        log_info((char *)module, "log post to cloud fail, ret = %d\n", ret);
    } else {
        log_err((char *)module, "log post to cloud success");
    }

    IMPL_LOGPOST_FREE(logbuf);
    return ret;
}

static int iotx_mc_log_post(void *pclient, char *payload)
{
    int ret;
    char topic_name[IOTX_URI_MAX_LEN + 1] = {0};

    char product_key[PRODUCT_KEY_LEN + 1] = {0};
    char device_name[DEVICE_NAME_LEN + 1] = {0};
    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);

    iotx_mqtt_topic_info_t topic_info;


    if (!payload || !pclient) {
        return FAIL_RETURN;
    }

    /* log post topic name */
    ret = HAL_Snprintf(topic_name,
                       IOTX_URI_MAX_LEN,
                       "/sys/%s/%s/thing/log/post",
                       product_key,
                       device_name);
    if (ret <= 0) {
        log_err("logpost", "log topic generate err");
        return FAIL_RETURN;
    }

    /* print log post json data */
    log_debug("logpost", "log post data: %s", payload);

    topic_info.qos = IOTX_MQTT_QOS0;
    topic_info.payload = (void *)payload;
    topic_info.payload_len = strlen(payload);
    topic_info.retain = 0;
    topic_info.dup = 0;

    /* publish message */
    ret = iotx_mc_publish((iotx_mc_client_t *)pclient, topic_name, &topic_info);
    if (ret < 0) {
        log_err("logpost", "publish failed");
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

#endif  /* MQTT_LOGPOST */
#endif  /* MQTT_COMM_ENABLED */

