/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <string.h>
#include <cJSON.h>
#include <stdlib.h>
#include "ota_log.h"
#include "ota_hal.h"
#include "ota_import.h"
#include "ota_hal_os.h"
#include "ota_hal_trans.h"
#include "ota_agent.h"
#include "aiot_mqtt_api.h"
/**
 * ota_sevice_parse_msg  OTA parse download info from cloud
 *
 * @param[in]  ota_service_t *ctx  service manager.
 *             char *json          transport message from Cloud.
 *
 * @return OTA_TRANSPORT_FAIL     transport errno.
 */
int ota_sevice_parse_msg(ota_service_t *ctx, const char *json)
{
    int ret            = 0;
    int is_get_module  = 0;
    cJSON *root        = NULL;
    ota_boot_param_t ota_param = {0};
    root = cJSON_Parse(json);
    if (NULL == root || NULL == ctx) {
        ret = OTA_TRANSPORT_PAR_FAIL;
        OTA_LOG_E("parse msg param is null");
        return ret;
    } else {
        /* recover the process type */
        cJSON *cmd = cJSON_GetObjectItem(root, "cmd");
        if (NULL == cmd) {
            ctx->ota_process = OTA_PROCESS_NORMAL;
        }
        cJSON *message = cJSON_GetObjectItem(root, "message");
        if (NULL == message) {
            ret = OTA_MESSAGE_PAR_FAIL;
            goto EXIT;
        }
        if ((strncmp(message->valuestring, "success", strlen("success")))) {
            ret = OTA_SUCCESS_PAR_FAIL;
            goto EXIT;
        }
        cJSON *json_obj = cJSON_GetObjectItem(root, "data");
        if (NULL == json_obj) {
            ret = OTA_DATA_PAR_FAIL;
            goto EXIT;
        }
        cJSON *url = cJSON_GetObjectItem(json_obj, "url");
        if (NULL == url) {
            ret = OTA_URL_PAR_FAIL;
            goto EXIT;
        }
        strncpy(ota_param.url, url->valuestring, OTA_URL_LEN - 1);
        ota_param.url[sizeof(ota_param.url) - 1] = 0;
        cJSON *submodule = cJSON_GetObjectItem(json_obj, "module");
        if (NULL != submodule) {
            is_get_module = 1;
            strncpy(ctx->module_name, submodule->valuestring, sizeof(ctx->module_name) - 1);
            OTA_LOG_I("submode = %s\r\n", submodule->valuestring);
        }
        cJSON *version = cJSON_GetObjectItem(json_obj, "version");
        if (NULL == version) {
            ret = OTA_VER_PAR_FAIL;
            goto EXIT;
        }
        strncpy(ota_param.ver, version->valuestring, sizeof(ota_param.ver));
        ota_param.ver[sizeof(ota_param.ver) - 1] = 0;
        cJSON *signMethod = cJSON_GetObjectItem(json_obj, "signMethod");
        if (signMethod != NULL) {
            memset(ota_param.hash, 0x00, OTA_HASH_LEN);
            ret = ota_to_capital(signMethod->valuestring, strlen(signMethod->valuestring));
            if (ret != 0) {
                ret = OTA_VER_PAR_FAIL;
                goto EXIT;
            }
            if (0 == strncmp(signMethod->valuestring, "MD5", strlen("MD5"))) {
                cJSON *md5 = cJSON_GetObjectItem(json_obj, "sign");
                if (NULL == md5) {
                    ret = OTA_MD5_PAR_FAIL;
                    goto EXIT;
                }
                ota_param.hash_type = OTA_MD5;
                strncpy(ota_param.hash, md5->valuestring, OTA_HASH_LEN - 1);
                ret = ota_to_capital(ota_param.hash, strlen(ota_param.hash));
                if (ret != 0) {
                    ret = OTA_VER_PAR_FAIL;
                    goto EXIT;
                }
            } else if (0 == strncmp(signMethod->valuestring, "SHA256", strlen("SHA256"))) {
                cJSON *sha256 = cJSON_GetObjectItem(json_obj, "sign");
                if (NULL == sha256) {
                    ret = OTA_SHA256_PAR_FAIL;
                    goto EXIT;
                }
                ota_param.hash_type = OTA_SHA256;
                strncpy(ota_param.hash, sha256->valuestring, OTA_HASH_LEN - 1);
                ret = ota_to_capital(ota_param.hash, strlen(ota_param.hash));
                if (ret != 0) {
                    ret = OTA_VER_PAR_FAIL;
                    goto EXIT;
                }
            } else {
                ret = OTA_HASH_PAR_FAIL;
                goto EXIT;
            }
        } else { /* old protocol*/
            memset(ota_param.hash, 0x00, OTA_HASH_LEN);
            cJSON *md5 = cJSON_GetObjectItem(json_obj, "md5");
            if (NULL == md5) {
                ret = OTA_MD5_PAR_FAIL;
                goto EXIT;
            }
            ota_param.hash_type = OTA_MD5;
            strncpy(ota_param.hash, md5->valuestring, OTA_HASH_LEN - 1);
            ret = ota_to_capital(ota_param.hash, strlen(ota_param.hash));
            if (ret != 0) {
                ret = OTA_VER_PAR_FAIL;
                goto EXIT;
            }
        }
        cJSON *size = cJSON_GetObjectItem(json_obj, "size");
        if (NULL == size) {
            ret = OTA_SIZE_PAR_FAIL;
            goto EXIT;
        }
        ota_param.len = size->valueint;
        cJSON *digestSign = cJSON_GetObjectItem(json_obj, "digestsign");
        if (digestSign != NULL) {
            unsigned int sign_len = OTA_SIGN_LEN;
            memset(ota_param.sign, 0x00, OTA_SIGN_LEN);
            if (ota_base64_decode((const unsigned char *)digestSign->valuestring, strlen(digestSign->valuestring), (unsigned char*)ota_param.sign, &sign_len) != 0) {
                ret = OTA_SIGN_PAR_FAIL;
                goto EXIT;
            }
        }
    }

EXIT:
    if (root != NULL) {
        cJSON_Delete(root);
        root = NULL;
    }
    OTA_LOG_I("Parse ota version:%s url:%s ret:%d\n", ota_param.ver, ota_param.url, ret);
    if (ret < 0) {
        ret = OTA_TRANSPORT_PAR_FAIL;
        if (ctx->report_func.report_status_cb != NULL) {
            ctx->report_func.report_status_cb(ctx->report_func.param, ret);
        }
    } else {
        if (ctx->report_func.report_status_cb != NULL) {
            ctx->report_func.report_status_cb(ctx->report_func.param, 1);
        }
        ota_param.upg_flag = 0x00;
        if (ctx->dev_type == 0) {
            if (is_get_module == 0) {
                strncpy(ctx->module_name, "default", 7);
            }
        }
        OTA_LOG_I("upg_flag = %04x", ota_param.upg_flag);
        ota_param.upg_status = OTA_TRANSPORT;
        ret = ota_update_parameter(&ota_param);
        if (ret != 0) {
            OTA_LOG_I("ota param err.\n");
            if (ctx->report_func.report_status_cb != NULL) {
                ctx->report_func.report_status_cb(ctx->report_func.param, OTA_TRANSPORT_PAR_FAIL);
            }
        }
        if (ctx->ota_triggered_func.triggered_ota_cb != NULL) {
            ctx->ota_triggered_func.triggered_ota_cb(ctx, ota_param.ver, ctx->module_name, ctx->ota_triggered_func.param);
        }
    }
    return ret;
}

/**
 * ota_mqtt_publish message to Cloud
 *
 * @param[in] mqtt_client      mqtt client ptr
 * @param[in] name             topic name
 * @param[in] msg              message content
 * @param[in] pk               product key
 * @param[in] dn               device name
 *
 * @return OTA_SUCCESS         OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
static int ota_mqtt_publish(void *mqtt_client, const char *name, char *msg, char *pk, char *dn)
{
    int ret = 0;
    char topic[OTA_MSG_LEN] = {0};
    if (name == NULL || msg == NULL || pk == NULL || dn == NULL) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    ret = ota_snprintf(topic, OTA_MSG_LEN - 1, "/ota/device/%s/%s/%s", name, pk, dn);
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    OTA_LOG_I("Public topic:%s msg:%s", topic, msg);
    ret = ota_hal_mqtt_publish(mqtt_client, topic, 1, (void *)msg, strlen(msg) + 1);
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    return ret;
}

/**
 * ota_mqtt_sub_cb  upgrade callback
 *
 * @param[in] pctx      ota context
 * @param[in] pclient   mqtt pclient
 * @param[in] msg       mqtt message
 *
 * @return void
 */
static void ota_mqtt_sub_cb(void *mqtt_client, void *msg, void *pctx)
{
    char *payload = NULL;
    ota_service_t *ctx = (ota_service_t *)pctx;
    aiot_mqtt_recv_t *mqtt_msg = (aiot_mqtt_recv_t *)msg;
    if ((msg == NULL) && (ctx == NULL)) {
        OTA_LOG_E("mqtt sub param err!");
        return;
    }
    if (mqtt_msg->type == AIOT_MQTTRECV_PUB) {
        payload = (char *)mqtt_msg->data.pub.payload;
        OTA_LOG_I("pub, qos: %d, topic: %.*s\n", mqtt_msg->data.pub.qos, mqtt_msg->data.pub.topic_len, mqtt_msg->data.pub.topic);
        if (payload != NULL) {
            OTA_LOG_I("mqtt cb evt:%d %s", mqtt_msg->type, payload);
            ota_sevice_parse_msg(ctx, payload);
        }
    } else {
        OTA_LOG_I("mqtt type = %d", mqtt_msg->type);
        payload = (char *)mqtt_msg->data.pub.payload;
        // OTA_LOG_I("pub, qos: %d, topic: %.*s\n", mqtt_msg->data.pub.qos, mqtt_msg->data.pub.topic_len, mqtt_msg->data.pub.topic);
        OTA_LOG_E("mqtt receive err msg type!");
    }
}

/**
 * ota_transport_inform  OTA inform version to cloud.
 *
 * @param[in]   void *mqttclient   mqtt client ptr
 * @param[in]           char *pk   product key value
 * @param[in]           char *dn   device name
 * @param[in]  char *module_name   want to report module name, when module_name == NULL, report default module ver
 * @param[in]          char *ver   version string
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_transport_inform(void *mqttclient, char *pk, char *dn, char *module_name, char *ver)
{
    int  ret = 0;
    char msg[OTA_MSG_LEN] = {0};
    if ((mqttclient == NULL) || (ver == NULL) || (pk == NULL) ||
        (dn == NULL) || (module_name == NULL)) {
        OTA_LOG_E("inform version input param err!");
        goto OTA_TRANS_INFOR_OVER;
    }
    if (strncmp(module_name, "default", strlen("default")) == 0) {
        ret = ota_snprintf(msg, OTA_MSG_LEN - 1, "{\"id\":%d,\"params\":{\"version\":\"%s\"}}", 0, ver);
    } else {
       ret = ota_snprintf(msg, OTA_MSG_LEN - 1, "{\"id\":%d,\"params\":{\"version\":\"%s\",\"module\":\"%s\"}}", 0, ver, module_name);
    }
    if (ret < 0) {
        OTA_LOG_E("inform version pack msg err!");
        goto OTA_TRANS_INFOR_OVER;
    }
    ret = ota_mqtt_publish(mqttclient, "inform", msg, pk, dn);
OTA_TRANS_INFOR_OVER:
    if (ret < 0) {
        OTA_LOG_E("inform version failed!");
    }
    return ret;
}

int ota_transport_inform_otaver(ota_service_t *ctx)
{
    int ret = 0;
    char topic[OTA_MSG_LEN] = {0};
    char msg[OTA_MSG_LEN] = {0};
    char keyver[20] = "SYS_OTA_ID";
    char attver[64] = {0};
    if (ctx == NULL) {
        return OTA_TRANSPORT_PAR_FAIL;
    }
    ret = ota_snprintf(topic, OTA_MSG_LEN - 1, "/sys/%s/%s/thing/deviceinfo/update", ctx->pk, ctx->dn);
    if (ret < 0) {
        return OTA_TRANSPORT_PAR_FAIL;
    }
    ret = ota_snprintf(attver, sizeof(attver) - 1, "HOTA-%s-%s-%s-%s", OTA_VERSION, OTA_TRANSTYPE, OTA_OS_TYPE, OTA_BOARD_TYPE);
    if (ret < 0) {
        return OTA_TRANSPORT_PAR_FAIL;
    }
    OTA_LOG_I("Public topic:%s", topic);
    ret = ota_snprintf(msg, OTA_MSG_LEN - 1, "{\"id\":\"%d\",\"version\":\"1.0\",\"params\":[{\"attrKey\":\"%s\",\"attrValue\":\"%s\"}],\"method\":\"thing.deviceinfo.update\"}",
          0, keyver, attver);
    if (ret < 0) {
        return OTA_TRANSPORT_PAR_FAIL;
    }
    OTA_LOG_I("Public msg:%s", msg);
    ret = ota_hal_mqtt_publish(ctx->mqtt_client, topic, 1, (void *)msg, strlen(msg) + 1);
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    return ret;
}
/**
 * ota_transport_upgrade  OTA subcribe message from Cloud.
 *
 * @param[in] ota_service_t *ctx ctx  ota service context
 *
 * @return OTA_SUCCESS         OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_transport_upgrade(ota_service_t *ctx)
{
    int  ret                = OTA_TRANSPORT_INT_FAIL;
    char topic[OTA_MSG_LEN] = {0};

    if (ctx == NULL) {
        return ret;
    }
    ret = ota_snprintf(topic, OTA_MSG_LEN - 1, "/ota/device/%s/%s/%s", "upgrade", ctx->pk, ctx->dn);
    if (ret < 0) {
        ret = OTA_TRANSPORT_INT_FAIL;
    } else {
        ret = ota_hal_mqtt_subscribe(ctx->mqtt_client, topic, ota_mqtt_sub_cb, ctx);
    }
    return ret;
}

/**
 * ota_transport_status  OTA report status to Cloud
 *
 * @param[in] void *param param  ota service context
 * @param[in] status      [1-100] percent, [<0] error no.
 *
 * @return OTA_SUCCESS         OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_transport_status(void *param, int status)
{
    int  ret                = OTA_TRANSPORT_INT_FAIL;
    ota_service_t *ctx      = NULL;
    char msg[OTA_MSG_LEN]   = {0};
    char *err_str           = "";
    int tmp_status          = 0;
    if (param == NULL) {
        OTA_LOG_E("report status param is null!");
        return ret;
    }
    ctx = (ota_service_t *)param;
    tmp_status = status;
    if (status < 0) {
        switch (status) {
            case OTA_INIT_FAIL:
                err_str = "OTA init failed";
                tmp_status = OTA_REPORT_UPGRADE_ERR;
                break;
            case OTA_TRANSPORT_INT_FAIL:
                err_str = "OTA transport init failed";
                tmp_status = OTA_REPORT_UPGRADE_ERR;
                break;
            case OTA_TRANSPORT_VER_FAIL:
                err_str = "OTA transport verion is too old";
                tmp_status = OTA_REPORT_UPGRADE_ERR;
                break;
            case OTA_TRANSPORT_PAR_FAIL:
                err_str = "OTA transport parse failed";
                tmp_status = OTA_REPORT_UPGRADE_ERR;
                break;
            case OTA_DOWNLOAD_INIT_FAIL:
                err_str = "OTA download init failed";
                tmp_status = OTA_REPORT_DOWNLOAD_ERR;
                break;
            case OTA_DOWNLOAD_HEAD_FAIL:
                err_str = "OTA download header failed";
                tmp_status = OTA_REPORT_DOWNLOAD_ERR;
                break;
            case OTA_DOWNLOAD_CON_FAIL:
                err_str = "OTA download connect failed";
                tmp_status = OTA_REPORT_DOWNLOAD_ERR;
                break;
            case OTA_DOWNLOAD_REQ_FAIL:
                err_str = "OTA download request failed";
                tmp_status = OTA_REPORT_DOWNLOAD_ERR;
                break;
            case OTA_DOWNLOAD_RECV_FAIL:
                err_str = "OTA download receive failed";
                tmp_status = OTA_REPORT_DOWNLOAD_ERR;
                break;
            case OTA_VERIFY_MD5_FAIL:
                err_str = "OTA verfiy MD5 failed";
                tmp_status = OTA_REPORT_VERIFY_ERR;
                break;
            case OTA_VERIFY_SHA2_FAIL:
                err_str = "OTA verfiy SHA256 failed";
                tmp_status = OTA_REPORT_VERIFY_ERR;
                break;
            case OTA_VERIFY_RSA_FAIL:
                err_str = "OTA verfiy RSA failed";
                tmp_status = OTA_REPORT_VERIFY_ERR;
                break;
            case OTA_VERIFY_IMAGE_FAIL:
                err_str = "OTA verfiy image failed";
                tmp_status = OTA_REPORT_VERIFY_ERR;
                break;
            case OTA_UPGRADE_WRITE_FAIL:
                err_str = "OTA upgrade write failed";
                tmp_status = OTA_REPORT_BURN_ERR;
                break;
            case OTA_UPGRADE_FW_SIZE_FAIL:
                err_str = "OTA upgrade FW too big";
                tmp_status = OTA_REPORT_BURN_ERR;
                break;
            case OTA_CUSTOM_CALLBAK_FAIL:
                err_str = "OTA register callback failed";
                tmp_status = OTA_REPORT_BURN_ERR;
                break;
            case OTA_MCU_INIT_FAIL:
                err_str = "OTA MCU init failed";
                tmp_status = OTA_REPORT_BURN_ERR;
                break;
            case OTA_MCU_VERSION_FAIL:
                err_str = "OTA MCU init failed";
                tmp_status = OTA_REPORT_BURN_ERR;
                break;
            case OTA_MCU_NOT_READY:
                err_str = "OTA MCU not ready";
                tmp_status = OTA_REPORT_BURN_ERR;
                break;
            case OTA_MCU_REBOOT_FAIL:
                err_str = "OTA MCU fail to reboot";
                tmp_status = OTA_REPORT_BURN_ERR;
                break;
            case OTA_MCU_HEADER_FAIL:
                err_str = "OTA MCU header error";
                tmp_status = OTA_REPORT_BURN_ERR;
                break;
            case OTA_MCU_UPGRADE_FAIL:
                err_str = "OTA MCU upgrade fail";
                tmp_status = OTA_REPORT_BURN_ERR;
                break;
            case OTA_INVALID_PARAMETER:
                err_str = "OTA invalid parameter";
                tmp_status = OTA_REPORT_BURN_ERR;
                break;
            default:
                err_str = "OTA undefined failed";
                tmp_status = OTA_REPORT_UPGRADE_ERR;
                break;
        }
    }
    if (strncmp(ctx->module_name, "default", strlen("default")) == 0) {
        ret = ota_snprintf(msg, OTA_MSG_LEN - 1, "{\"id\":%d,\"params\":{\"step\": \"%d\",\"desc\":\"%s\"}}", 1, tmp_status, err_str);
    } else {
        ret = ota_snprintf(msg, OTA_MSG_LEN - 1, "{\"id\":%d,\"params\":{\"step\": \"%d\",\"desc\":\"%s\",\"module\":\"%s\"}}", 1, tmp_status, err_str, ctx->module_name);
    }
    if (ret < 0) {
        return OTA_TRANSPORT_INT_FAIL;
    }
    ret = ota_mqtt_publish(ctx->mqtt_client, "progress", msg, ctx->pk, ctx->dn);
    if (ret < 0) {
        OTA_LOG_E("report status failed!");
        return OTA_TRANSPORT_INT_FAIL;
    }
    return ret;
}

