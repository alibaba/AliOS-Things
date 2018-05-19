/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <cJSON.h>

#include "iot_import.h"
#include "iot_export.h"
#include "ota_log.h"
#include "ota_update_manifest.h"
#include "ota_util.h"
#include "ota_version.h"
#include "iot_export_mqtt.h"
#include "mqtt_instance.h"

#define OTA_MQTT_TOPIC_LEN        (64)
#define POTA_FETCH_PERCENTAGE_MIN (0)
#define POTA_FETCH_PERCENTAGE_MAX (100)

#define MSG_REPORT_LEN  (256)
#define MSG_INFORM_LEN  (128)

typedef struct ota_device_info {
    char product_key[PRODUCT_KEY_MAXLEN + 1];
    char device_name[DEVICE_NAME_MAXLEN + 1];
} OTA_device_info;

typedef enum {
    ALIOT_OTA_PROGRAMMING_FAILED = -4,
    ALIOT_OTA_CHECK_FAILED = -3,
    ALIOT_OTA_DOWNLOAD_FAILED = -2,
    ALIOT_OTA_UPGRADE_FAILED = -1,
} ALIOT_OTA_FAIL_E;

OTA_device_info g_ota_device_info;

static char *g_upgrad_topic;

extern int version_report();

static const char *to_capital_letter(char *value, int len);
static int  ota_mqtt_gen_topic_name(char *buf, size_t buf_len, const char *ota_topic_type, const char *product_key,
                                    const char *device_name);
static void ota_mqtt_sub_callback(char *topic, int topic_len, void *payload, int payload_len, void *ctx);
static int  ota_mqtt_publish(const char *topic_type, const char *msg);
static int  ota_gen_info_msg(char *buf, size_t buf_len, uint32_t id, const char *version);
static int  ota_gen_report_msg(char *buf, size_t buf_len, uint32_t id, int progress, const char *msg_detail);
static bool ota_check_progress(int progress);

static const char *to_capital_letter(char *value, int len)
{
    if (value == NULL && len <= 0) {
        return NULL;
    }
    int i =0;
    for (; i < len; i++) {
        if (*(value + i) >= 'a' && *(value + i) <= 'z') {
            *(value + i) -= 'a' - 'A';
        }
    }
    return value;
}

//Generate topic name according to @ota_topic_type, @product_key, @device_name
//and then copy to @buf.
//0, successful; -1, failed
static int ota_mqtt_gen_topic_name(char *buf, size_t buf_len, const char *ota_topic_type, const char *product_key,
                                   const char *device_name)
{
    int ret;
    ret = snprintf(buf, buf_len, "/ota/device/%s/%s/%s", ota_topic_type, product_key, device_name);

    if (ret < 0) {
        OTA_LOG_E("snprintf failed");
        return -1;
    }

    return 0;
}

static int ota_mqtt_publish(const char *topic_type, const char *msg)
{
    int ret;
    char topic_name[OTA_MQTT_TOPIC_LEN] = {0};
    // iotx_mqtt_topic_info_t topic_msg;

    // memset(&topic_msg, 0, sizeof(iotx_mqtt_topic_info_t));

    // topic_msg.qos = IOTX_MQTT_QOS1;
    // topic_msg.retain = 0;
    // topic_msg.dup = 0;
    // topic_msg.payload = (void *) msg;
    // topic_msg.payload_len = strlen(msg);

    //inform OTA to topic: "/ota/device/progress/$(product_key)/$(device_name)"
    ret = ota_mqtt_gen_topic_name(topic_name, OTA_MQTT_TOPIC_LEN, topic_type, g_ota_device_info.product_key,
                                  g_ota_device_info.device_name);
    if (ret < 0) {
        OTA_LOG_E("generate topic name of info failed");
        return -1;
    }
    OTA_LOG_I("public topic=%s ,payload=%s\n", topic_name, msg);
    ret =  mqtt_publish(topic_name,1,msg,strlen(msg)+1);
    //ret = IOT_MQTT_Publish(g_ota_device_info.pclient, topic_name, &topic_msg);
    if (ret < 0) {
        OTA_LOG_E("publish failed");
        return -1;
    }

    return 0;
}

static void ota_mqtt_sub_callback(char *topic, int topic_len, void *payload, int payload_len, void *ctx)
{
    OTA_LOG_I("----------------------ota_mqtt_sub_callback--------------------,payload=%s\n", payload);
    aos_cloud_cb_t ota_update = (aos_cloud_cb_t )ctx;
    if (!ota_update) {
        OTA_LOG_E("aliot_mqtt_ota_callback  pcontext null");
        return;
    }

    ota_update(UPGRADE_DEVICE ,payload);
}
// void aliot_mqtt_ota_callback(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
// {
//     aos_cloud_cb_t ota_update = (aos_cloud_cb_t )pcontext;
//     if (!ota_update) {
//         OTA_LOG_E("aliot_mqtt_ota_callback  pcontext null");
//         return;
//     }

//     iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;

//     OTA_LOG_D("Topic: '%.*s' (Length: %d)",
//               ptopic_info->topic_len,
//               ptopic_info->ptopic,
//               ptopic_info->topic_len);
//     OTA_LOG_D("Payload: '%.*s' (Length: %d)",
//               ptopic_info->payload_len,
//               ptopic_info->payload,
//               ptopic_info->payload_len);

//     ota_update(UPGRADE_DEVICE , ptopic_info->payload);
// }

//Generate firmware information according to @id, @version
//and then copy to @buf.
//0, successful; -1, failed
static int ota_gen_info_msg(char *buf, size_t buf_len, uint32_t id, const char *version)
{
    int ret;
    ret = snprintf(buf,
                   buf_len,
                   "{\"id\":%d,\"params\":{\"version\":\"%s\"}}",
                   id,
                   version);

    if (ret < 0) {
        OTA_LOG_E("snprintf failed");
        return -1;
    }

    return 0;
}

//Generate report information according to @id, @msg
//and then copy to @buf.
//0, successful; -1, failed
static int ota_gen_report_msg(char *buf, size_t buf_len, uint32_t id, int progress, const char *msg_detail)
{
    int ret;
    if (NULL == msg_detail) {
        ret = snprintf(buf,
                       buf_len,
                       "{\"id\":%d,\"params\":{\"step\": \"%d\",\"desc\":\"%d%%\"}}",
                       id,
                       progress,
                       progress);
    } else {
        ret = snprintf(buf,
                       buf_len,
                       "{\"id\":%d,\"params\":{\"step\": \"%d\",\"desc\":\"%s\"}}",
                       id,
                       progress,
                       msg_detail);
    }

    if (ret < 0) {
        OTA_LOG_E("snprintf failed");
        return -1;
    } else if (ret >= buf_len) {
        OTA_LOG_E("msg is too long");
        return -1;
    }

    return 0;
}

//check whether the progress state is valid or not
//return: true, valid progress state; false, invalid progress state.
static bool ota_check_progress(int progress)
{
    return ((progress >= POTA_FETCH_PERCENTAGE_MIN)
            && (progress <= POTA_FETCH_PERCENTAGE_MAX));
}

int8_t platform_ota_parse_requset(const char *request, int *buf_len, ota_request_params *request_parmas)
{
    return 0;
}

int8_t platform_ota_parse_response(const char *response, int buf_len, ota_response_params *response_parmas)
{
    cJSON *root = cJSON_Parse(response);
    if (!root) {
        OTA_LOG_E("Error before: [%s]\n", cJSON_GetErrorPtr());
        goto parse_failed;
    } else {
        // char *info = cJSON_Print(root);
        // OTA_LOG_D("root is %s", info);
        // free(info);

        cJSON *message =  cJSON_GetObjectItem(root, "message");

        if (NULL == message) {
            OTA_LOG_E("invalid json doc of OTA ");
            goto parse_failed;
        }

        //check whether is positive message
        if ((strncmp(message->valuestring, "success", strlen("success")) )) {
            OTA_LOG_E("fail state of json doc of OTA");
            goto parse_failed;
        }

        cJSON *json_obj = cJSON_GetObjectItem(root, "data");
        if (!json_obj) {
            OTA_LOG_E("data back.");
            goto parse_failed;
        }

        cJSON *resourceUrl = cJSON_GetObjectItem(json_obj, "url");
        if (!resourceUrl) {
            OTA_LOG_E("resourceUrl back.");
            goto parse_failed;
        }

        cJSON *version = cJSON_GetObjectItem(json_obj, "version");
        if (!version) {
            OTA_LOG_E("version back.");
            goto parse_failed;
        }
        ota_set_version(version->valuestring);

#ifdef  MULTI_BINS        
        char *upgrade_version = strtok(version->valuestring, "_");
        if (!upgrade_version) {
            strncpy(response_parmas->primary_version, version->valuestring,
                    (sizeof response_parmas->primary_version)-1);
        } else {
            strncpy(response_parmas->primary_version, upgrade_version,
                    (sizeof response_parmas->primary_version)-1);
            upgrade_version = strtok(NULL, "_");
            if (upgrade_version) {
                strncpy(response_parmas->secondary_version, upgrade_version,
                        (sizeof response_parmas->secondary_version)-1);
            }
            OTA_LOG_I("response primary_version = %s, secondary_version = %s",
                      response_parmas->primary_version, response_parmas->secondary_version);
        }
#else
        strncpy(response_parmas->primary_version, version->valuestring,
                (sizeof response_parmas->primary_version)-1);

#endif
        strncpy(response_parmas->download_url, resourceUrl->valuestring,
                (sizeof response_parmas->download_url)-1);
        OTA_LOG_D(" response_parmas->download_url %s",
                  response_parmas->download_url);

        cJSON *md5 = cJSON_GetObjectItem(json_obj, "md5");
        if (!md5) {
            OTA_LOG_E("md5 back.");
            goto parse_failed;
        }

        strncpy(response_parmas->md5, md5->valuestring,
                sizeof response_parmas->md5);
        response_parmas->md5[(sizeof response_parmas->md5) - 1] = '\0';
        to_capital_letter(response_parmas->md5, sizeof response_parmas->md5);
        cJSON *size = cJSON_GetObjectItem(json_obj, "size");
        if (!md5) {
            OTA_LOG_E("size back.");
            goto parse_failed;
        }

        response_parmas->frimware_size = size->valueint;

    }

    OTA_LOG_D("parse_json success");
    goto parse_success;

parse_failed:
    if (root) {
        cJSON_Delete(root);
    }
    return -1;

parse_success:
    if (root) {
        cJSON_Delete(root);
    }
    return 0;
}
int8_t platform_ota_parse_cancel_responce(const char *response, int buf_len, ota_response_params *response_parmas)
{
    return 0;
}

int8_t platform_ota_publish_request(ota_request_params *request_parmas)
{
    return 0;
}

int8_t platform_ota_subscribe_upgrade(aos_cloud_cb_t msgCallback)
{
    g_upgrad_topic =  aos_zalloc(OTA_MQTT_TOPIC_LEN);

    if (!g_upgrad_topic) {
        OTA_LOG_E("generate topic name of upgrade malloc fail");
        return -1;
    }

    int ret = ota_mqtt_gen_topic_name(g_upgrad_topic, OTA_MQTT_TOPIC_LEN, "upgrade", g_ota_device_info.product_key,
                                      g_ota_device_info.device_name);
    if (ret < 0) {
        OTA_LOG_E("generate topic name of upgrade failed");
        goto do_exit;
    }
    //subscribe the OTA topic: "/ota/device/upgrade/$(product_key)/$(device_name)"
    // ret = IOT_MQTT_Subscribe(g_ota_device_info.pclient, g_upgrad_topic, IOTX_MQTT_QOS1,
    //                          aliot_mqtt_ota_callback, (void*)msgCallback);

    ret = mqtt_subscribe(g_upgrad_topic, ota_mqtt_sub_callback, (void*)msgCallback);

    if (ret < 0) {
        OTA_LOG_E("mqtt subscribe failed");
        goto do_exit;
    }

    return ret;
do_exit:
    if (NULL != g_upgrad_topic) {
        aos_free(g_upgrad_topic);
        g_upgrad_topic = NULL;
    }

    return -1;
}

int8_t platform_ota_status_post(int status, int progress)
{
    int ret = -1;
    char msg_reported[MSG_REPORT_LEN] = {0};
    if (!ota_check_progress(progress)) {
        OTA_LOG_E("progress is a invalid parameter");
        return ret;
    }

    if (status == OTA_CHECK_FAILED) {
        progress = ALIOT_OTA_CHECK_FAILED;
    } else if (status == OTA_DOWNLOAD_FAILED) {
        progress = ALIOT_OTA_DOWNLOAD_FAILED;
    } else if (status == OTA_DECOMPRESS_FAILED) {
        progress = ALIOT_OTA_PROGRAMMING_FAILED;
    } else if (status < 0) {
        progress = ALIOT_OTA_UPGRADE_FAILED;
    } else if (status == OTA_INIT) {
        progress = 0;
    }
    ret = ota_gen_report_msg(msg_reported, MSG_REPORT_LEN, 0, progress, NULL);

    if (0 != ret) {
        OTA_LOG_E("generate reported message failed");
        return -1;
    }

    ret = ota_mqtt_publish("progress", msg_reported);
    if (0 != ret) {
        OTA_LOG_E("Report progress failed");
        return -1;
    }
#ifdef STM32_USE_SPI_WIFI
    IOT_MQTT_Yield(g_ota_device_info.pclient, 1000);
#endif
    ret = 0;
    return ret;
}


int8_t platform_ota_result_post(void)
{
    int ret = -1;
    char msg_informed[MSG_INFORM_LEN] = {0};
    ret = ota_gen_info_msg(msg_informed, MSG_INFORM_LEN, 0,
                           ota_get_system_version());
    if (ret != 0) {
        OTA_LOG_E("generate inform message failed");
        return -1;
    }
    ret = ota_mqtt_publish("inform", msg_informed);
    if (0 != ret) {
        OTA_LOG_E("Report version failed");
        return -1;
    }

#ifdef VCALL_RHINO
    ret = version_report();
    if (0 != ret) {
        OTA_LOG_E("Report detail version failed");
        return -1;
    }
#endif

    return ret;
}

int8_t platform_ota_cancel_upgrade(aos_cloud_cb_t msgCallback)
{
    return 0;
}

const char *platform_ota_get_id(void)
{
    return NULL;
}

//deinitialize OTA module
int OTA_Deinit(void *handle)
{
    if (g_upgrad_topic) {
        aos_free(g_upgrad_topic);
        g_upgrad_topic = NULL;
    }
    return 0;
}
#ifdef WITH_CM  
extern int work_queue_stop(void);
extern int IOT_CM_Deinit(void *p);
#endif 
int8_t platform_destroy_connect(void){
#ifdef WITH_CM   
    work_queue_stop();
    return IOT_CM_Deinit(NULL);
#else    
    return mqtt_deinit_instance();
#endif    
}

void platform_ota_init( void *signal)
{
    HAL_GetProductKey(g_ota_device_info.product_key);
    HAL_GetDeviceName(g_ota_device_info.device_name);
}

