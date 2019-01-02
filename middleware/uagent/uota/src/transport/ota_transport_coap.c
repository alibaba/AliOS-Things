/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <cJSON.h>

#include "iot_export.h"
#include "iot_export_coap.h"
#include "ota_log.h"
#include "ota_transport.h"
#include "ota_service.h"
#include "ota_util.h"
#include "ota_version.h"
#include "ota_verify.h"
#include "ota_hal_os.h"
#include "ota_base64.h"

static ota_cloud_cb_t ota_update;
static int            otacoap_GenTopicName(char *buf, size_t buf_len,
                                           const char *ota_topic_type);
static void           otacoap_response_handler(void *arg, void *p_response);
static int            otacoap_Publish(const char *topic_type, const char *msg);
static int            otalib_GenInfoMsg(char *buf, size_t buf_len, uint32_t id,
                                        const char *version);
static int         otalib_GenReportMsg(char *buf, size_t buf_len, uint32_t id,
                                       int progress, const char *msg_detail);
static int         otacoap_report_version(const char *version);
static const char *to_capital_letter(char *value, int len);

static void otacoap_response_handler(void *arg, void *p_response)
{
    int                   len       = 0;
    unsigned char        *p_payload = NULL;
    iotx_coap_resp_code_t resp_code;
    ota_service_manager *ctx = (ota_service_manager *)get_ota_service_manager();
    ota_IOT_CoAP_GetMessageCode(p_response, &resp_code);
    ota_IOT_CoAP_GetMessagePayload(p_response, &p_payload, &len);
    OTA_LOG_D("CoAP response code = %d, [CoAP msg_len=%d, msg=%s", resp_code,
              len, p_payload ? p_payload : NULL);
    if ((NULL != ctx->h_coap) && (NULL != p_payload)) {
        ota_update((uint32_t)len, (const char *)p_payload);
    }
}

// report progress of OTA
static int otacoap_Publish(const char *topic_type, const char *msg)
{
    int                  ret;
    char                 uri[OTA_MQTT_TOPIC_LEN + 1] = { 0 };
    iotx_message_t       message;
    ota_service_manager *ctx = (ota_service_manager *)get_ota_service_manager();
    message.p_payload        = (unsigned char *)msg;
    message.payload_len      = (unsigned short)strlen(msg);
    message.resp_callback    = otacoap_response_handler;
    message.msg_type         = IOTX_MESSAGE_CON;
    message.content_type     = IOTX_CONTENT_TYPE_JSON;

    ret = otacoap_GenTopicName(uri, OSC_COAP_URI_MAX_LEN, topic_type);
    if (ret < 0) {
        OTA_LOG_E("generate topic name failed");
        return -1;
    }

    OTA_LOG_E("Publich:%s msg:%s\n", uri, msg);
    if (IOTX_SUCCESS !=
        (ret = ota_IOT_CoAP_SendMessage(ctx->h_coap, (char *)uri, &message))) {
        OTA_LOG_E("send CoAP msg failed:%d", ret);
        return -1;
    }

    return 0;
}


// Generate topic name according to @ota_topic_type, @product_key, @device_name
// and then copy to @buf.
// 0, successful; -1, failed
static int otacoap_GenTopicName(char *buf, size_t buf_len,
                                const char *ota_topic_type)
{
    int                  ret;
    ota_service_manager *ctx = (ota_service_manager *)get_ota_service_manager();
    if (buf == NULL || ota_topic_type == NULL) {
        OTA_LOG_E("coap gen topic is null");
        return -1;
    }
    ret = ota_snprintf(buf, buf_len, "/topic/ota/device/%s/%s/%s",
                       ota_topic_type, ctx->pk, ctx->dn);
    if (ret < 0) {
        OTA_LOG_E("ota_snprintf failed");
        return -1;
    }

    return 0;
}

// Generate firmware information according to @id, @version
// and then copy to @buf.
// 0, successful; -1, failed
static int otalib_GenReqMsg(char *buf, size_t buf_len, uint32_t id,
                            const char *version)
{
    int ret = 0;
    ret     = ota_snprintf(
      buf, buf_len,
      "{\"id\":%d,\"params\":{\"mode\":\"coap\",\"version\":\"%s\"}}", id,
      version ? version : NULL);
    if (ret < 0) {
        OTA_LOG_E("ota_snprintf failed");
        return -1;
    }

    return 0;
}
// Generate firmware information according to @id, @version
// and then copy to @buf.
// 0, successful; -1, failed
static int otalib_GenInfoMsg(char *buf, size_t buf_len, uint32_t id,
                             const char *version)
{
    int ret = 0;
    ret =
      ota_snprintf(buf, buf_len, "{\"id\":%d,\"params\":{\"version\":\"%s\"}}",
                   id, version ? version : NULL);

    if (ret < 0) {
        OTA_LOG_E("ota_snprintf failed");
        return -1;
    }

    return 0;
}

// Generate report information according to @id, @msg
// and then copy to @buf.
// 0, successful; -1, failed
static int otalib_GenReportMsg(char *buf, size_t buf_len, uint32_t id,
                               int progress, const char *msg_detail)
{
    int ret;
    if (NULL == msg_detail) {
        ret = ota_snprintf(
          buf, buf_len,
          "{\"id\":%d,\"params\":{\"step\": \"%d\",\"desc\":\"%d%%\"}}", id,
          progress, progress);
    } else {
        ret = ota_snprintf(
          buf, buf_len,
          "{\"id\":%d,\"params\":{\"step\": \"%d\",\"desc\":\"%s\"}}", id,
          progress, msg_detail);
    }

    if (ret < 0) {
        OTA_LOG_E("ota_snprintf failed");
        return -1;
    } else if (ret >= buf_len) {
        OTA_LOG_E("msg is too long");
        return -1;
    }

    return 0;
}

static int otacoap_report_version(const char *version)
{
    int                  ret, len;
    char                 msg_informed[MSG_INFORM_LEN] = { 0 };
    ota_service_manager *ctx = (ota_service_manager *)get_ota_service_manager();
    if ((NULL == ctx->h_coap) || (NULL == version)) {
        OTA_LOG_E("one or more invalid parameter");
        return -1;
    }

    len = strlen(version);
    if ((len < OTA_VERSION_STR_LEN_MIN) || (len > OTA_VERSION_STR_LEN_MAX)) {
        OTA_LOG_E("version string is invalid: must be [1, 32] chars");
        return -1;
    }

    if (ctx->dl_protcol == OTA_PROTCOL_COAP) {
        ret = otalib_GenReqMsg(msg_informed, MSG_INFORM_LEN, 0, version);
    } else {
        ret = otalib_GenInfoMsg(msg_informed, MSG_INFORM_LEN, 0, version);
    }
    if (ret != 0) {
        OTA_LOG_E("generate inform message failed");
        return -1;
    }

    // request new firmware after initial state
    ret = otacoap_Publish("request", msg_informed);
    if (0 != ret) {
        OTA_LOG_E("Report version failed");
        return -1;
    }

    return 0;
}


static const char *to_capital_letter(char *value, int len)
{
    int i = 0;

    if (value == NULL || len <= 0) {
        return NULL;
    }
    for (i = 0; i < len; i++) {
        if (*(value + i) >= 'a' && *(value + i) <= 'z') {
            *(value + i) -= 'a' - 'A';
        }
    }
    return value;
}

#define IOTX_ONLINE_DTLS_SERVER_URL \
    "coaps://%s.iot-as-coap.cn-shanghai.aliyuncs.com:5684"

int ota_transport_init(void)
{
    int ret = 0;

    iotx_coap_config_t   config;
    iotx_device_info_t   dev;
    ota_service_manager *ctx = (ota_service_manager *)get_ota_service_manager();

    memset(&config, 0, sizeof(config));
    memset(&dev, 0, sizeof(dev));

    strncpy(dev.device_id, ctx->ps, sizeof(dev.device_id)-1);
    strncpy(dev.product_key, ctx->pk, sizeof(dev.product_key)-1);
    strncpy(dev.device_name, ctx->dn, sizeof(dev.device_name)-1);
    strncpy(dev.device_secret, ctx->ds, sizeof(dev.device_secret)-1);
    config.p_devinfo = &dev;

    char url[256] = { 0 };
    ota_snprintf(url, sizeof(url), IOTX_ONLINE_DTLS_SERVER_URL,
                ctx->pk);
    config.p_url = url;
    ctx->h_coap = (void *)ota_IOT_CoAP_Init(&config);
    if (ctx->h_coap) {
        ret = ota_IOT_CoAP_DeviceNameAuth(ctx->h_coap);
        if (ret < 0) {
            OTA_LOG_E("COAP error");
            return ret;
        }
        OTA_LOG_D("IOT_CoAP_DeviceNameAuth. success.");
    }
    otacoap_report_version(ota_get_system_version());
    return 0;
}

int8_t ota_parse_request(const char *request, int *buf_len,
                         ota_request_params *request_parmas)
{
    return 0;
}

int8_t ota_parse_response(const char *response, int buf_len,
                          ota_response_params *response_parmas)
{
    cJSON *root = cJSON_Parse(response);
    if (!root) {
        const char *err = cJSON_GetErrorPtr();
        OTA_LOG_E("Error before: [%s]\n", err ? err : NULL);
        goto parse_failed;
    } else {
        cJSON *message = cJSON_GetObjectItem(root, "message");
        if (NULL == message) {
            OTA_LOG_E("invalid json doc of OTA ");
            goto parse_failed;
        }

        // check whether is positive message
        if ((strncasecmp(message->valuestring, "success", strlen("success")))) {
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
                (sizeof response_parmas->download_url) - 1);

        OTA_LOG_D(" response_parmas->download_url %s",
                  response_parmas->download_url);

        cJSON *resourceVer = cJSON_GetObjectItem(json_obj, "version");
        if (!resourceVer) {
            OTA_LOG_E("resourceVer back.");
            goto parse_failed;
        }

        OTA_LOG_D(" response version %s",
                  resourceVer->valuestring ? resourceVer->valuestring : NULL);
        char *upgrade_version = strtok(resourceVer->valuestring, "_");
        if (!upgrade_version) {
            strncpy(response_parmas->primary_version, resourceVer->valuestring,
                    (sizeof response_parmas->primary_version) - 1);
        } else {
            strncpy(response_parmas->primary_version, upgrade_version,
                    (sizeof response_parmas->primary_version) - 1);

            upgrade_version = strtok(NULL, "_");
            if (upgrade_version) {
                strncpy(response_parmas->secondary_version, upgrade_version,
                        (sizeof response_parmas->secondary_version) - 1);
            }
            OTA_LOG_I("response primary_version = %s, secondary_version = %s",
                      response_parmas->primary_version
                        ? response_parmas->primary_version
                        : NULL,
                      response_parmas->secondary_version
                        ? response_parmas->secondary_version
                        : NULL);
        }

        cJSON *signMethod = cJSON_GetObjectItem(json_obj, "signMethod");
        if (signMethod) { // new protocol
            if (0 ==
                strncasecmp(signMethod->valuestring, "Md5", strlen("Md5"))) {
                cJSON *md5 = cJSON_GetObjectItem(json_obj, "sign");
                if (!md5) {
                    OTA_LOG_E("no sign(md5) found");
                    goto parse_failed;
                }
                response_parmas->hash_method = MD5;
                strncpy(response_parmas->hash_value, md5->valuestring,
                        OTA_MD5_LEN);
                response_parmas->hash_value[OTA_MD5_LEN] = '\0';
                to_capital_letter(response_parmas->hash_value, OTA_MD5_LEN);
            } else if (0 == strncasecmp(signMethod->valuestring, "Sha256",
                                        strlen("Sha256"))) {
                cJSON *sha256 = cJSON_GetObjectItem(json_obj, "sign");
                if (!sha256) {
                    OTA_LOG_E("no sign(sha256) found");
                    goto parse_failed;
                }

                response_parmas->hash_method = SHA256;
                strncpy(response_parmas->hash_value, sha256->valuestring,
                        OTA_SHA256_LEN);
                response_parmas->hash_value[OTA_SHA256_LEN] = '\0';
                to_capital_letter(response_parmas->hash_value, OTA_SHA256_LEN);
            } else {
                goto parse_failed;
            }

        } else {

            cJSON *md5 = cJSON_GetObjectItem(json_obj, "md5");
            if (!md5) {
                OTA_LOG_E("no md5 found");
                goto parse_failed;
            }
            response_parmas->hash_method = MD5;
            strncpy(response_parmas->hash_value, md5->valuestring, OTA_MD5_LEN);
            response_parmas->hash_value[OTA_MD5_LEN] = '\0';
            to_capital_letter(response_parmas->hash_value, OTA_MD5_LEN);
        }

        cJSON *size = cJSON_GetObjectItem(json_obj, "size");
        if (!size) {
            OTA_LOG_E("size back.");
            goto parse_failed;
        }
        ota_service_manager *ctx =
          (ota_service_manager *)get_ota_service_manager();
        ctx->firm_size                 = size->valueint;
        response_parmas->frimware_size = size->valueint;

        cJSON *digestSign = cJSON_GetObjectItem(json_obj, "digestsign");
        if (digestSign) {
           response_parmas->sign_enable = OTA_SIGN_ON;
           response_parmas->sign_value_len = sizeof(response_parmas->sign_value);
           if(ota_base64_decode(response_parmas->sign_value, &response_parmas->sign_value_len, (const unsigned char*)digestSign->valuestring, strlen(digestSign->valuestring)) != 0 ) {
                OTA_LOG_E("decode base64 failed");
                return -1;
            }
        }
        else {
           response_parmas->sign_enable = OTA_SIGN_OFF;
        }
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

static int8_t ota_parse_cancel_response(const char *response, int buf_len,
                                        ota_response_params *response_parmas)
{
    return 0;
}

static int8_t ota_cancel_upgrade(ota_cloud_cb_t msgCallback)
{
    return 0;
}

static int8_t ota_subscribe_upgrade(ota_cloud_cb_t msgCallback)
{
    ota_update = msgCallback;
    return 0;
}

static int8_t ota_publish_request(ota_request_params *request_parmas)
{
    return 0;
}

static const char *ota_get_uuid()
{
    ota_service_manager *ctx = (ota_service_manager *)get_ota_service_manager();
    return (const char *)ctx->uuid;
}

static int8_t ota_ustatus_post(int status, int progress)
{
    int  ret                          = -1;
    char msg_reported[MSG_REPORT_LEN] = { 0 };
    char error_desc[32];
    memset(error_desc, 0x00, sizeof(error_desc));
    if (status < 0) {
        progress = status;
        switch (status) {
            case OTA_INIT_FAILED:
                sprintf(error_desc, "%s", "ota init failed");
                break;
            case OTA_DOWNLOAD_FAILED:
                sprintf(error_desc, "%s", "ota download failed");
                break;
            case OTA_DECOMPRESS_FAILED:
                sprintf(error_desc, "%s", "ota decompress failed");
                break;
            case OTA_CHECK_FAILED:
                sprintf(error_desc, "%s", "ota check hash failed");
                break;
            case OTA_UPGRADE_FAILED:
                sprintf(error_desc, "%s", "ota upgrade failed");
                break;
            case OTA_REBOOT_FAILED:
                sprintf(error_desc, "%s", "ota reboot failed");
                break;
            case OTA_RSA_VERIFY_FAILED:
                sprintf(error_desc, "%s", "ota rsa verify failed");
                break;
            case OTA_CHECK_IMAGE_FAILED:
                sprintf(error_desc, "%s", "ota check image failed");
                break;
            case OTA_DIFF_FAILED:
                sprintf(error_desc, "%s", "ota diff failed");
                break;
            default:
                sprintf(error_desc, "%s", "ota failed");
                break;
        }
    }
    ret = otalib_GenReportMsg(msg_reported, MSG_REPORT_LEN, 0, progress, error_desc);
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

static int8_t ota_uresult_post(void)
{
    int  ret                          = -1;
    char msg_informed[MSG_INFORM_LEN] = { 0 };
    ret = otalib_GenInfoMsg(msg_informed, MSG_INFORM_LEN, 0,
                            (char *)ota_get_system_version());
    if (ret != 0) {
        OTA_LOG_E("generate inform message failed");
        return -1;
    }

    ret = otacoap_Publish("inform", msg_informed);
    // OTA_LOG_I("ota_result_post =%s",msg_informed);
    if (0 != ret) {
        OTA_LOG_E("Report version failed");
        return -1;
    }
    return ret;
}

static int ota_transport_deinit(void)
{
    ota_service_manager *ctx = (ota_service_manager *)get_ota_service_manager();
    if (ctx && ctx->h_coap)
        return ota_IOT_CoAP_Deinit(&(ctx->h_coap));
    else
        return -1;
}


static ota_transport trans_coap = {
    .init                  = ota_transport_init,
    .parse_request         = ota_parse_request,
    .parse_response        = ota_parse_response,
    .parse_cancel_response = ota_parse_cancel_response,
    .subscribe_upgrade     = ota_subscribe_upgrade,
    .cancel_upgrade        = ota_cancel_upgrade,
    .publish_request       = ota_publish_request,
    .status_post           = ota_ustatus_post,
    .result_post           = ota_uresult_post,
    .get_uuid              = ota_get_uuid,
    .deinit                = ota_transport_deinit,
};

const void *ota_get_transport_coap(void)
{
    return &trans_coap;
}
