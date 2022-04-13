/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "amp_system.h"
#include "amp_config.h"
#include "amp_defines.h"

#include "dev_sign_api.h"
#include "mqtt_api.h"
#include "wrappers_defs.h"
#include "infra_compat.h"
#include "app_ntp.h"
#include "app_mgr.h"
#include "app_ota_service.h"
#include "das.h"
#include "cJSON.h"
#include "ulog/ulog.h"

#define MOD_STR "APP_OTA"
extern void *IOT_MQTT_Construct_Interal(iotx_mqtt_param_t *pInitParams, iotx_dev_meta_info_t meta_info);
extern int apppack_upgrade(char *url);
int upgrading = 0;
char file_url[300] = {0};

int64_t g_ntp_time = 0;
int64_t g_up_time = 0;
uint8_t device_token_verify_rejected = 0;

static void       *session = NULL;
static const char *sub_topic = NULL;

int32_t update_file(const char *payload)
{
    int32_t ret = -1;
    cJSON *root = NULL, *params = NULL, *url = NULL;

    root = cJSON_Parse(payload);
    if (root == NULL || !cJSON_IsObject(root))
    {
        amp_debug(MOD_STR, "JSON Parse Error");
        return ret;
    }
    params = cJSON_GetObjectItem(root, "params");
    if (params == NULL || !cJSON_IsObject(params))
    {
        amp_debug(MOD_STR, "JSON Parse Error");
        cJSON_Delete(root);
        return ret;
    }
    do
    {
        url = cJSON_GetObjectItem(params, "url");
        if (url == NULL || !cJSON_IsString(url))
        {
            amp_debug(MOD_STR, "JSON Parse Error");
            cJSON_Delete(root);
            return ret;
        }

        strcpy(file_url, url->valuestring);
        // amp_debug(MOD_STR, "get url: %s", url->valuestring);
        if (upgrading)
        {
            amp_debug(MOD_STR, "upgrading.........");
            return -1;
        }
        apppack_upgrade(file_url);
    } while (0);

    cJSON_Delete(root);

    return 0;
}

void putfile_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_t *topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type)
    {
    case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
        /* print topic name and topic message */
        amp_debug(MOD_STR, "Putfile Message Arrived:");
        amp_debug(MOD_STR, "Topic  : %d %s", topic_info->topic_len, topic_info->ptopic);
        amp_debug(MOD_STR, "Payload: %d %s", topic_info->payload_len, topic_info->payload);
        amp_debug(MOD_STR, "\n");
        update_file(topic_info->payload);
        break;
    default:
        break;
    }
}

int32_t token_verify(const char *payload)
{
    int32_t ret = -1;
    char token_flag[AMP_DEVICE_TOKEN_LENGTH] = {0};
    int token_len = AMP_DEVICE_TOKEN_LENGTH;
    cJSON *root = NULL, *params = NULL, *token = NULL;

    root = cJSON_Parse(payload);
    if (root == NULL || !cJSON_IsObject(root))
    {
        amp_debug(MOD_STR, "JSON Parse Error");
        return ret;
    }
    params = cJSON_GetObjectItem(root, "params");
    if (params == NULL || !cJSON_IsObject(params))
    {
        amp_debug(MOD_STR, "JSON Parse Error");
        cJSON_Delete(root);
        return ret;
    }

    token = cJSON_GetObjectItem(params, "token");
    if (token == NULL || !cJSON_IsString(token))
    {
        amp_debug(MOD_STR, "JSON Parse Error");
        cJSON_Delete(root);
        return ret;
    }
    // strcpy(file_url, token->valuestring);
    amp_debug(MOD_STR, "get token flag is: %s", token->valuestring);
    HAL_Kv_Set(AMP_DEVICE_TOKEN_VERIFY_FLAG, token->valuestring, AMP_DEVICE_TOKEN_LENGTH, 1);
    if (strcmp(token->valuestring, "rejected") == 0) {
        amp_debug(MOD_STR, "token verify failed, destroy pclient");
        device_token_verify_rejected = 1;
    }

    cJSON_Delete(root);

    return 0;
}

void verify_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_t *topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type)
    {
    case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
        /* print topic name and topic message */
        amp_debug(MOD_STR, "Verify Message Arrived:");
        amp_debug(MOD_STR, "Topic  : %d %s", topic_info->topic_len, topic_info->ptopic);
        amp_debug(MOD_STR, "Payload: %d %s", topic_info->payload_len, topic_info->payload);
        amp_debug(MOD_STR, "\n");
        token_verify(topic_info->payload);
        break;
    default:
        break;
    }
}

int topic_publish(void *handle, const char *fmt, char *payload)
{
    int             res = 0;
    char           *topic = NULL;
    int             topic_len = 0;
    char            amp_internal_productkey[IOTX_PRODUCT_KEY_LEN] = {0};
    int             productkey_len = IOTX_PRODUCT_KEY_LEN;

    HAL_Kv_Get(AMP_INTERNAL_PRODUCTKEY, amp_internal_productkey, &productkey_len);

    topic_len = strlen(fmt) + strlen(amp_internal_productkey) + strlen(AMP_INTERNAL_DEVICENAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        amp_debug(MOD_STR, "memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, amp_internal_productkey, AMP_INTERNAL_DEVICENAME);

    res = IOT_MQTT_Publish_Simple(handle, topic, IOTX_MQTT_QOS0, payload, strlen(payload));
    if (res < 0) {
        amp_debug(MOD_STR, "publish failed, res = %d", res);
        HAL_Free(topic);
        return -1;
    }

    HAL_Free(topic);
    return 0;
}

int topic_subscribe(void *handle, const char *fmt, void *callback)
{
    int res = 0;
    int topic_len = 0;
    char *topic = NULL;
    char amp_internal_productkey[IOTX_PRODUCT_KEY_LEN] = {0};
    int  productkey_len = IOTX_PRODUCT_KEY_LEN;

    HAL_Kv_Get(AMP_INTERNAL_PRODUCTKEY, amp_internal_productkey, &productkey_len);
    topic_len = strlen(fmt) + strlen(amp_internal_productkey) + strlen(AMP_INTERNAL_DEVICENAME) + 1;
    topic = amp_malloc(topic_len);
    if (topic == NULL) {
        amp_debug(MOD_STR, "memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, amp_internal_productkey, AMP_INTERNAL_DEVICENAME);

    res = IOT_MQTT_Subscribe(handle, topic, IOTX_MQTT_QOS0, callback, NULL);
    if (res < 0)
    {
        amp_debug(MOD_STR, "subscribe failed");
        amp_free(topic);
        return -1;
    }

    amp_free(topic);
    return 0;

}

int internal_service_subscribe(void *handle)
{
    int res = 0;
    const char *update_fmt = "/sys/%s/%s/thing/service/putfile";
    const char *verify_fmt = "/sys/%s/%s/thing/service/verify";

    res = topic_subscribe(handle, update_fmt, putfile_message_arrive);
    if (res < 0) {
        amp_debug(MOD_STR, "subscribe update topic failed");
        return -1;
    }

    res = topic_subscribe(handle, verify_fmt, verify_message_arrive);
    if (res < 0) {
        amp_debug(MOD_STR, "subscribe update topic failed");
        return -1;
    }
}

#define PROP_POST_FORMAT_TOKEN "{\"params\":{\"token\":\"%s\"}}"
int internal_service_publish(void *handle)
{
    int res = 0;
    char token_content[AMP_DEVICE_TOKEN_LENGTH] = {0};
    int token_len = AMP_DEVICE_TOKEN_LENGTH;
    const char *verify_fmt = "/sys/%s/%s/thing/event/property/post";
    char *payload = NULL;

    payload = amp_malloc(128);
    if (payload == NULL) {
        amp_debug(MOD_STR, "memory not enough");
        return -1;
    }
    memset(payload, 0, 128);
    HAL_Kv_Get(AMP_DEVICE_TOKEN, token_content, &token_len);
    HAL_Snprintf(payload, 128, PROP_POST_FORMAT_TOKEN, token_content);

    res = topic_publish(handle, verify_fmt, payload);
    if (res < 0) {
        amp_debug(MOD_STR, "publish failed");
        amp_free(payload);
        return -1;
    }

    amp_free(payload);
    return res;
}

void example_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    // amp_debug(MOD_STR, "msg->event_type : %d", msg->event_type);
}

static int on_publish(const char *topic, const uint8_t *msg, size_t size, void *channel)
{
    int ret = 0;
    int retry = 0;
    iotx_mqtt_topic_info_t topic_msg;

    topic_msg.qos = IOTX_MQTT_QOS1;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)msg;
    topic_msg.payload_len = size;

    do {
        ret = IOT_MQTT_Publish(channel, topic, &topic_msg);
        if (ret < 0) {
            if (ret == MQTT_STATE_ERROR || ret == MQTT_NETWORK_ERROR) {
                HAL_SleepMs(200);
            } else {
                amp_debug(MOD_STR, "error occur when publish, %d", ret);
                break;
            }
        }
    } while(ret < 0 && retry++ < 10);

    amp_debug(MOD_STR, "\n publish message: \n topic: %s\n payload len: \%d\n ret = %d\n",
                  topic, topic_msg.payload_len, ret);

    if (ret < 0) {
        ret = DAS_ERROR_GENERIC;
    } else {
        ret = DAS_SUCCESS;
    }

    return ret;
}

static void on_message(void *handle, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    /* print topic name and topic message */
    amp_debug(MOD_STR, "----");
    amp_debug(MOD_STR, "packetId: %d", ptopic_info->packet_id);
    amp_debug(MOD_STR, "Topic: '%.*s' (Length: %d)",
                  ptopic_info->topic_len,
                  ptopic_info->ptopic,
                  ptopic_info->topic_len);
    amp_debug(MOD_STR, "Payload: '%.*s' (Length: %d)",
                  ptopic_info->payload_len,
                  ptopic_info->payload,
                  ptopic_info->payload_len);
    amp_debug(MOD_STR, "----");

    if (!memcmp(sub_topic, ptopic_info->ptopic, ptopic_info->topic_len)) {
        das_on_message(handle,
                       (uint8_t *)ptopic_info->payload,
                       (size_t)ptopic_info->payload_len);
    }

    return;
}

static void ntp_reply(const char *ntp_offset_time_ms)
{
    g_ntp_time = atoll(ntp_offset_time_ms); 
    g_up_time = HAL_UptimeMs();
}

void *app_ota_service_task(void *paras)
{
    void *pclient = NULL;
    int res = 0;
    int loop_cnt = 0;
    char token_content[AMP_DEVICE_TOKEN_LENGTH] = {0};
    char token_flag[AMP_DEVICE_TOKEN_LENGTH] = {0};
    int token_len = AMP_DEVICE_TOKEN_LENGTH;
    int productkey_len = IOTX_PRODUCT_KEY_LEN;
    int productsecret_len = IOTX_PRODUCT_SECRET_LEN;
    iotx_mqtt_param_t mqtt_params;
    iotx_dev_meta_info_t meta_info;
    memset(&meta_info, 0x0, sizeof(meta_info));

    HAL_Kv_Get(AMP_INTERNAL_PRODUCTKEY, meta_info.product_key, &productkey_len);
    HAL_Kv_Get(AMP_INTERNAL_PRODUCTSECRET, meta_info.product_secret, &productsecret_len);
    memcpy(meta_info.device_name, AMP_INTERNAL_DEVICENAME, IOTX_DEVICE_NAME_LEN);

    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.handle_event.h_fp = example_event_handle;

    pclient = IOT_MQTT_Construct_Interal(&mqtt_params, meta_info);
    if (NULL == pclient)
    {
        amp_debug(MOD_STR, "MQTT construct failed");
        HAL_ThreadDelete(NULL);
        return NULL;
    }

    session = das_init(meta_info.product_key, meta_info.device_name);
    if (session == NULL) {
        IOT_MQTT_Destroy(&pclient);
        amp_debug(MOD_STR, "das_init() failed");
        HAL_ThreadDelete(NULL);
        return NULL;
    }

    das_connection(session, on_publish, pclient);
    das_on_connected(session);

    sub_topic = das_sub_topic(session, NULL);
    if (sub_topic == NULL) {
        IOT_MQTT_Destroy(&pclient);
        amp_debug(MOD_STR, "das_get_sub_topic() failed");
        HAL_ThreadDelete(NULL);
        return NULL;
    }

    res = IOT_MQTT_Subscribe(pclient,
              sub_topic, IOTX_MQTT_QOS1, on_message, session);
    if (res < 0) {
        amp_debug(MOD_STR, "IOT_MQTT_Subscribe() failed, %d", res);
    }

    res = internal_service_subscribe(pclient);
    if (res < 0)
    {
        IOT_MQTT_Destroy(&pclient);
        HAL_ThreadDelete(NULL);
        return NULL;
    }

    HAL_Kv_Get(AMP_DEVICE_TOKEN_VERIFY_FLAG, token_flag, &token_len);
    amp_debug(MOD_STR, "token flag is: %s", token_flag);
    if (strlen(token_flag) == 0 || strcmp(token_flag, "rejected") == 0)
    {
        HAL_Kv_Get(AMP_DEVICE_TOKEN, token_content, &token_len);
        if (strlen(token_content) == 0) {
            IOT_MQTT_Destroy(&pclient);
            HAL_ThreadDelete(NULL);
            return NULL;
        }

        res = internal_service_publish(pclient);
        if (res < 0)
        {
            IOT_MQTT_Destroy(&pclient);
            HAL_ThreadDelete(NULL);
            return NULL;
        }
    }

    res = app_ntp_time_request(pclient, meta_info, ntp_reply);
    if (res < 0)
    {
        amp_warn(MOD_STR, "ntp failed");
        // IOT_MQTT_Destroy(&pclient);
        // return;
    }
    
    while (1)
    {
        uint64_t time_ms = HAL_UptimeMs();

        res = das_stepping(session, time_ms);
        if (res != DAS_SUCCESS) {
            amp_debug(MOD_STR, "das_stepping() failed, %d", res);
            res = -1;
        }

        IOT_MQTT_Yield(pclient, 200);

        if (device_token_verify_rejected == 1) {
            break;
        }
    }

    das_on_disconnected(session);

    das_final(session);
    
    IOT_MQTT_Destroy(&pclient);
    HAL_ThreadDelete(NULL);

    return NULL;
}

int app_ota_service_init(void)
{
    void *ota_task;
    int ota_stack_used;
    amp_os_thread_param_t ota_task_params = {0};

    ota_task_params.priority = HAL_GetDefaultTaskPriority();
    ota_task_params.stack_size = 1024 * 8;
    ota_task_params.name = "amp_ota";

    if (HAL_ThreadCreate(&ota_task, app_ota_service_task, NULL, &ota_task_params, &ota_stack_used) != 0) {
        amp_debug(MOD_STR, "ota service task create failed!");
        return -1;
    }
    amp_debug(MOD_STR, "app ota service start");
    HAL_SleepMs(10);
}
