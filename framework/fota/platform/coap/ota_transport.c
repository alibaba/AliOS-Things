/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "ota_transport.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <cJSON.h>
#include "iot_import.h"
#include "iot_export.h"

#include "ota_update_manifest.h"
#include "ota_log.h"
#include "ota_version.h"
#include "ota_util.h"
//#include "iot_import_ota.h"

#include "ota_log.h"

#define OSC_COAP_URI_MAX_LEN         (135)  /* IoTx CoAP uri maximal length */
#define MSG_REPORT_LEN  (256)
#define MSG_INFORM_LEN  (128)
#define FOTA_FETCH_PERCENTAGE_MIN 0
#define FOTA_FETCH_PERCENTAGE_MAX 100
#define OTA_VERSION_STR_LEN_MIN     (1)
#define OTA_VERSION_STR_LEN_MAX     (32)
#define OTA_CHECK_VER_DUARATION     (24*60*60*1000)

typedef enum {
    COAP_PROGRAMMING_FAILED = -4,
    COAP_CHECK_FAILED = -3,
    COAP_DOWNLOAD_FAILED = -2,
    COAP_UPGRADE_FAILED = -1,
} OTA_FAIL_E;
    
typedef struct ota_device_info {
    const char *product_key;
    const char *device_name;
    void *h_coap;
} OTA_device_info;

OTA_device_info g_ota_device_info;

aos_cloud_cb_t ota_update;

//check whether the progress state is valid or not
//return: true, valid progress state; false, invalid progress state.
static bool ota_check_progress(IOT_OTA_Progress_t progress)
{
    return ((progress >= FOTA_FETCH_PERCENTAGE_MIN)
            && (progress <= FOTA_FETCH_PERCENTAGE_MAX));
}

static void otacoap_response_handler(void *arg, void *p_response)
{
    int len = 0;
    unsigned char *p_payload = NULL;
    iotx_coap_resp_code_t resp_code;
    IOT_CoAP_GetMessageCode(p_response, &resp_code);
    IOT_CoAP_GetMessagePayload(p_response, &p_payload, &len);
    OTA_LOG_D("CoAP response code = %d", resp_code);
    OTA_LOG_D("[CoAP msg_len=%d, msg=%s\r\n", len, p_payload);

    if ((NULL != g_ota_device_info.h_coap) && (NULL != p_payload)) {
        ota_update((uint32_t)len, (const char *)p_payload);
    }
}

//report progress of OTA
static int otacoap_Publish(const char *topic_type, const char *msg)
{
    int ret;
    char uri[IOTX_URI_MAX_LEN + 1] = {0};
    iotx_message_t     message;
    message.p_payload = (unsigned char *)msg;
    message.payload_len = (unsigned short)strlen(msg);
    message.resp_callback = otacoap_response_handler;
    message.msg_type = IOTX_MESSAGE_CON;
    message.content_type = IOTX_CONTENT_TYPE_JSON;

    //topic name: /topic/ota/device/${topic_type}/${productKey}/${deviceName}
    ret = otacoap_GenTopicName(uri, OSC_COAP_URI_MAX_LEN, topic_type);
    if (ret < 0) {
        OTA_LOG_E("generate topic name failed");
        return -1;
    }

    if (IOTX_SUCCESS != (ret = IOT_CoAP_SendMessage(g_ota_device_info.h_coap, (char *)uri, &message))) {
        OTA_LOG_E("send CoAP msg failed%d", ret);
        return -1;
    }

    return 0;
}


//Generate topic name according to @ota_topic_type, @product_key, @device_name
//and then copy to @buf.
//0, successful; -1, failed
int otacoap_GenTopicName(char *buf, size_t buf_len, const char *ota_topic_type)
{
    int ret;

    ret = snprintf(buf,
                   buf_len,
                   "/topic/ota/device/%s/%s/%s",
                   ota_topic_type,
                   g_ota_device_info.product_key,
                   g_ota_device_info.device_name);
    //#ifdef IOTX_DEBUG
    //    OTA_ASSERT(ret < buf_len, "buffer should always enough");
    //#endif
    if (ret < 0) {
        OTA_LOG_E("snprintf failed");
        return -1;
    }

    return 0;
}


//Generate firmware information according to @id, @version
//and then copy to @buf.
//0, successful; -1, failed
int otalib_GenInfoMsg(char *buf, size_t buf_len, uint32_t id, const char *version)
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
int otalib_GenReportMsg(char *buf, size_t buf_len, uint32_t id, int progress, const char *msg_detail)
{
    int ret;
    if (NULL == msg_detail) {
        ret = snprintf(buf,
                       buf_len,
                       "{\"id\":%d,\"params\":\{\"step\": \"%d\"}}",
                       id,
                       progress);
    } else {
        ret = snprintf(buf,
                       buf_len,
                       "{\"id\":%d,\"params\":\{\"step\": \"%d\",\"desc\":\"%s\"}}",
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

int COAP_OTA_RequestVersion(const char *version)
{
    int ret, len;
    char msg_informed[MSG_INFORM_LEN] = {0};

    if ((NULL == g_ota_device_info.h_coap) || (NULL == version)) {
        OTA_LOG_E("one or more invalid parameter");
        return IOT_OTAE_INVALID_PARAM;
    }

    len = strlen(version);
    if ((len < OTA_VERSION_STR_LEN_MIN) || (len > OTA_VERSION_STR_LEN_MAX)) {
        OTA_LOG_E("version string is invalid: must be [1, 32] chars");
        return -1;
    }

    ret = otalib_GenInfoMsg(msg_informed, MSG_INFORM_LEN, 0, version);
    if (ret != 0) {
        OTA_LOG_E("generate inform message failed");
        return -1;
    }

    //request new firmware after initial state
    ret = otacoap_Publish("request", msg_informed);

    if (0 != ret) {
        OTA_LOG_E("Report version failed");
        return -1;
    }

    return 0;
}


static const char *to_capital_letter(char *value, int len)
{
    if (value == NULL && len <= 0) {
        return NULL;
    }
    for (int i = 0; i < len; i++) {
        if (*(value + i) >= 'a' && *(value + i) <= 'z') {
            *(value + i) -= 'a' - 'A';
        }
    }
    return value;
}

void platform_ota_init( void *signal)
{
    if (signal == NULL) {
        OTA_LOG_E("ota device info is null");
        return;
    }
    OTA_device_info *device_info = (OTA_device_info *)signal;
    OTA_LOG_D("device_info:%s,%s", device_info->product_key, device_info->device_name);
    memcpy(&g_ota_device_info, device_info , sizeof (OTA_device_info));
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
        strncpy(response_parmas->download_url, resourceUrl->valuestring,
                sizeof response_parmas->download_url);
        OTA_LOG_D(" response_parmas->download_url %s",
                  response_parmas->download_url);

        cJSON *md5 = cJSON_GetObjectItem(json_obj, "md5");
        if (!md5) {
            OTA_LOG_E("md5 back.");
            goto parse_failed;
        }

        strncpy(response_parmas->md5, md5->valuestring,
                sizeof response_parmas->md5);
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

int8_t platform_ota_cancel_upgrade(aos_cloud_cb_t msgCallback)
{
    return 0;
}

void otacoap_report_version()
{
    int ota_code = 0;
    do {
        ota_code = COAP_OTA_RequestVersion(ota_get_system_version());
        IOT_CoAP_Yield(g_ota_device_info.h_coap);
        HAL_SleepMs(2000);
    } while (0 != ota_code);

    aos_post_delayed_action(OTA_CHECK_VER_DUARATION, otacoap_report_version, NULL);
}

int8_t platform_ota_subscribe_upgrade(aos_cloud_cb_t msgCallback)
{
    ota_update = msgCallback;
    aos_post_delayed_action(OTA_CHECK_VER_DUARATION, otacoap_report_version, NULL);
    return 0;
}

int8_t platform_ota_publish_request(ota_request_params *request_parmas)
{
    return 0;
}

char *platform_ota_get_id()
{
    return 0;
}

int8_t platform_ota_status_post(int status, int progress)
{
    int ret = -1;
    char msg_reported[MSG_REPORT_LEN] = {0};

    if (!ota_check_progress(progress)) {
        OTA_LOG_E("progress is a invalid parameter");
        return ret;
    }

    if (status==OTA_CHECK_FAILED) {
        progress=COAP_CHECK_FAILED;
    }
    else if (status==OTA_DOWNLOAD_FAILED) {
        progress=COAP_DOWNLOAD_FAILED;
    }
    else if (status==OTA_DECOMPRESS_FAILED) {
        progress=COAP_PROGRAMMING_FAILED;
    }
    else if (status<0) {
        progress=COAP_UPGRADE_FAILED;
    }
    else if (status==OTA_INIT) {
        progress=0;
    }
    
    ret = otalib_GenReportMsg(msg_reported, MSG_REPORT_LEN, 0, progress, msg_reported);
    if (0 != ret) {
        OTA_LOG_E("generate reported message failed");
        return -1;
    }

    ret = otacoap_Publish("progress", msg_reported);
    if (0 != ret) {
        OTA_LOG_E("Report progress failed");
        return -1;
    }

    return ret;
}

int8_t platform_ota_result_post(void)
{
    int ret = -1;
    char msg_informed[MSG_INFORM_LEN] = {0};
    ret = otalib_GenInfoMsg(msg_informed, MSG_INFORM_LEN, 0,
                            (char *)ota_get_system_version());
    if (ret != 0) {
        OTA_LOG_E("generate inform message failed");
        return -1;
    }
    ret = otacoap_Publish("inform", msg_informed);
    if (0 != ret) {
        OTA_LOG_E("Report version failed");
        return -1;
    }
    return ret;
}
