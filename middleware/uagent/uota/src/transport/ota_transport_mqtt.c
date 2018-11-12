/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <string.h>
#include <stdlib.h>
#include <cJSON.h>

#include "ota_log.h"
#include "ota_service.h"
#include "ota_hal_os.h"
#include "ota_hal_plat.h"
#include "ota_verify.h"
#include "ota_rsa_verify.h"
#include "ota_base64.h"

static int ota_gen_info_msg(char *buf, int len, int id, const char *ver)
{
    int ret = 0;
    ret = ota_snprintf(buf, len, "{\"id\":%d,\"params\":{\"version\":\"%s\"}}", id, ver);
    if (ret < 0) {
        OTA_LOG_E("mqtt info failed");
        return -1;
    }
    return 0;
}

// Generate report information according to @id, @msg
// and then copy to @buf.
// 0, successful; -1, failed
static int ota_gen_report_msg(char *buf, int len, int id, int progress, const char *msg)
{
    int ret = 0;
    ret = ota_snprintf(buf, len, "{\"id\":%d,\"params\":{\"step\": \"%d\",\"desc\":\"%s\"}}", id, progress, msg?msg:NULL);
    if (ret < 0) {
        OTA_LOG_E("mqtt report failed");
        return -1;
    }
    return 0;
}

// Generate topic name according to @ota_topic_type, @product_key, @device_name
// and then copy to @buf.
// 0, successful; -1, failed
static int ota_mqtt_gen_topic_name(char *buf, int len, const char *topic, char *pk, char *dn)
{
    int ret = 0;
    ret = ota_snprintf(buf, len, "/ota/device/%s/%s/%s", topic, pk, dn);
    if (ret < 0) {
        OTA_LOG_E("mqtt topic failed");
        return -1;
    }
    return 0;
}

static int ota_mqtt_publish(const char *topic, const char *msg, char *pk, char *dn)
{
    int ret = 0;
    char name[OTA_MQTT_TOPIC_LEN] = {0};
    if (topic == NULL || msg == NULL || pk == NULL || dn == NULL) {
        OTA_LOG_E("mqtt pub is null");
        return -1;
    }
    ret = ota_mqtt_gen_topic_name(name, OTA_MQTT_TOPIC_LEN, topic, pk, dn);
    if (ret < 0) {
        OTA_LOG_E("generate topic name of info failed");
        return -1;
    }
    OTA_LOG_I("mqtt pub name:%s msg:%s",name,msg);
    ret = ota_hal_mqtt_publish(name, 1, (void *)msg, strlen(msg) + 1);
    if (ret < 0) {
        OTA_LOG_E("publish failed");
        return -1;
    }
    return 0;
}

static void ota_mqtt_sub_cb(void *pcontext, void *pclient, void* msg)
{
    char *payload = NULL;
    if (msg == NULL) {
        OTA_LOG_E("mqtt is null");
        return;
    }
    
    ota_mqtt_msg_t *mqtt_msg = (ota_mqtt_msg_t*)msg;
    switch (mqtt_msg->event) {
        case OTA_MQTT_EVENT_PUB_RECEIVED:
            payload = (char *)mqtt_msg->topic->payload;
            break;
        default:
            return;
    }
    if(payload == NULL) {
        OTA_LOG_E("payload is null");
        return;
    }

    OTA_LOG_E("mqtt cb evt:%d %s", mqtt_msg->event, payload);
    ota_cloud_cb_t ota_update = (ota_cloud_cb_t)pcontext;
    if (!ota_update) {
        OTA_LOG_E("mqtt cb is null.");
        return;
    }
    ota_update(payload);
}

static int ota_trans_inform(const char* ver, char* pk, char* dn)
{
    int  ret = 0;
    char msg[OTA_MSG_INFORM_LEN] = {0};
    ret = ota_gen_info_msg(msg, OTA_MSG_INFORM_LEN, 0, ver);
    if (ret != 0) {
        OTA_LOG_E("generate inform message failed");
        return -1;
    }
    ret = ota_mqtt_publish("inform", msg, pk, dn);
    if (0 != ret) {
        OTA_LOG_E("mqtt inform failed");
        return -1;
    }
    return ret;
}

static int ota_trans_response(const char *json)
{
    char *url = ota_get_transport_mqtt()->url;
    char *hash = ota_get_transport_mqtt()->hash;
    unsigned char *sign = (unsigned char *)ota_get_transport_mqtt()->sign;
    cJSON *root = cJSON_Parse(json);
    if (!root) {
        OTA_LOG_E("Error before: [%s]\n", cJSON_GetErrorPtr());
        goto parse_failed;
    } else {
        cJSON *message = cJSON_GetObjectItem(root, "message");
        if (NULL == message) {
            OTA_LOG_E("invalid json doc.");
            goto parse_failed;
        }
        if ((strncmp(message->valuestring, "success", strlen("success")))) {
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
        ota_set_ota_version(version->valuestring);
        strncpy(url, resourceUrl->valuestring, OTA_URL_LEN-1);
        OTA_LOG_I("ver:%s url:%s", version->valuestring, resourceUrl->valuestring);
        cJSON *signMethod = cJSON_GetObjectItem(json_obj, "signMethod");
        if (signMethod) { // new protocol
            memset(hash, 0x00, OTA_HASH_LEN);
            if (0 == strncmp(signMethod->valuestring, "Md5", strlen("Md5"))) {
                cJSON *md5 = cJSON_GetObjectItem(json_obj, "sign");
                if (!md5) {
                    OTA_LOG_E("no sign(md5) found");
                    goto parse_failed;
                }
                ota_get_transport_mqtt()->hash_type = MD5;
                strncpy(hash, md5->valuestring, strlen(md5->valuestring)+1);
                hash[strlen(md5->valuestring)] = '\0';
                ota_to_capital(hash, strlen(hash));
            } else if (0 == strncmp(signMethod->valuestring, "SHA256", strlen("SHA256"))) {
                cJSON *sha256 = cJSON_GetObjectItem(json_obj, "sign");
                if (!sha256) {
                    OTA_LOG_E("no sign(sha256) found");
                    goto parse_failed;
                }
                ota_get_transport_mqtt()->hash_type = SHA256;
                strncpy(hash, sha256->valuestring, strlen(sha256->valuestring)+1);
                hash[strlen(sha256->valuestring)] = '\0';
                ota_to_capital(hash, strlen(hash));
            } else {
                OTA_LOG_E("get signMethod failed.");
                goto parse_failed;
            }
        } else { // old protocol
            memset(hash, 0x00, OTA_HASH_LEN);
            cJSON *md5 = cJSON_GetObjectItem(json_obj, "md5");
            if (!md5) {
                OTA_LOG_E("no md5 found");
                goto parse_failed;
            }
            ota_get_transport_mqtt()->hash_type = MD5;
            strncpy(hash, md5->valuestring, strlen(md5->valuestring)+1);
            hash[strlen(md5->valuestring)] = '\0';
            ota_to_capital(hash, strlen(hash));
        }
        cJSON *size = cJSON_GetObjectItem(json_obj, "size");
        if (!size) {
            OTA_LOG_E("size back.");
            goto parse_failed;
        }
        ota_set_firm_size(size->valueint);
        ota_set_upg_flag(OTA_RAW);
        OTA_LOG_I("download file size:%d", size->valueint);
        cJSON *diff = cJSON_GetObjectItem(json_obj, "isDiff");
        if (diff) {
            int is_diff = diff->valueint;
            if (is_diff) {
                ota_set_upg_flag(OTA_DIFF);
                cJSON *splictSize = cJSON_GetObjectItem(json_obj, "splictSize");
                if (splictSize) {
                    int splict_size = splictSize->valueint;
                    ota_set_splict_size(splict_size);
                }
            }
        }
        cJSON *digestSign = cJSON_GetObjectItem(json_obj, "digestsign");
        if (digestSign) {
           memset(sign, 0x00, OTA_SIGN_LEN);
           ota_get_transport_mqtt()->sign_en = OTA_SIGN_ON;
           ota_get_transport_mqtt()->sign_len = strlen(digestSign->valuestring);
           OTA_LOG_I("sign value = %s", digestSign->valuestring);
           if(ota_base64_decode(sign, (unsigned int*)&ota_get_transport_mqtt()->sign_len, (const unsigned char*)digestSign->valuestring, strlen(digestSign->valuestring)) != 0 ) {
                OTA_LOG_E("decode base64 failed");
                return -1;
            }
        } else {
           ota_get_transport_mqtt()->sign_en = OTA_SIGN_OFF;
        }
    }
    OTA_LOG_I("mqtt parse success, sign_en:%d hash_type:%d \n",ota_get_transport_mqtt()->sign_en,ota_get_transport_mqtt()->hash_type);
    goto parse_success;

parse_failed:
    OTA_LOG_E("mqtt parse failed.");
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

static int ota_trans_upgrade(ota_cloud_cb_t msg_cb, char* pk, char* dn)
{
    int   ret = 0;
    char  name[OTA_MQTT_TOPIC_LEN] = {0};
    if (pk == NULL || dn == NULL) {
        OTA_LOG_E("upgrade is null");
        return -1;
    }
    ret = ota_mqtt_gen_topic_name(name, OTA_MQTT_TOPIC_LEN, "upgrade", pk, dn);
    if (ret < 0) {
        OTA_LOG_E("mqtt topic upgrade failed");
        return -1;
    }
    OTA_LOG_I("upgrade:%s",name);
    ret = ota_hal_mqtt_subscribe(name, ota_mqtt_sub_cb,(void *)msg_cb);
    if (ret < 0) {
        OTA_LOG_E("mqtt subscribe failed \n");
        return -1;
    }
    return ret;
}

static int ota_trans_status(int progress, char* pk, char* dn)
{
    int  ret = -1;
    int  status = ota_get_status();
    char msg[OTA_MSG_REPORT_LEN] = {0};
    char err[32];
    memset(err, 0x00, sizeof(err));
    if (status < 0) {
        progress = status;
        switch (status) {
            case OTA_INIT_FAIL:
                sprintf(err, "%s", "ota init failed");
                break;
            case OTA_INIT_VER_FAIL:
                sprintf(err, "%s", "ota version not match");
                break;
            case OTA_DOWNLOAD_FAIL:
                sprintf(err, "%s", "ota download failed");
                break;
            case OTA_DOWNLOAD_URL_FAIL:
                sprintf(err, "%s", "ota download url failed");
                break;
            case OTA_DOWNLOAD_IP_FAIL:
                sprintf(err, "%s", "ota download ip failed");
                break;
            case OTA_DOWNLOAD_CON_FAIL:
                sprintf(err, "%s", "ota download connect failed");
                break;
            case OTA_DOWNLOAD_READ_FAIL:
                sprintf(err, "%s", "ota download read failed");
                break;
            case OTA_DOWNLOAD_WRITE_FAIL:
                sprintf(err, "%s", "ota download write failed");
                break;
            case OTA_VERIFY_FAIL:
                sprintf(err, "%s", "ota verify failed");
                break;
            case OTA_UPGRADE_FAIL:
                sprintf(err, "%s", "ota upgrade failed");
                break;
            case OTA_REBOOT_FAIL:
                sprintf(err, "%s", "ota reboot failed");
                break;
            case OTA_VERIFY_RSA_FAIL:
                sprintf(err, "%s", "ota verify rsa failed");
                break;
            case OTA_VERIFY_HASH_FAIL:
                sprintf(err, "%s", "ota verify hash failed");
                break;
            case OTA_UPGRADE_DIFF_FAIL:
                sprintf(err, "%s", "ota diff failed");
                break;
            default:
                sprintf(err, "%s", "ota undefined failed");
                break;
        }
    }
    ret = ota_gen_report_msg(msg, OTA_MSG_REPORT_LEN, 0, progress, err);
    if (0 != ret) {
        OTA_LOG_E("generate message failed");
        return -1;
    }
    ret = ota_mqtt_publish("progress", msg, pk, dn);
    if (0 != ret) {
        OTA_LOG_E("Report progress failed");
        return -1;
    }
    return ret;
}

static int ota_trans_deinit(void)
{
    if(ota_get_transport_mqtt()->url){
        ota_free(ota_get_transport_mqtt()->url);
        ota_get_transport_mqtt()->url = NULL;
    }
    if(ota_get_transport_mqtt()->hash){
        ota_free(ota_get_transport_mqtt()->hash);
        ota_get_transport_mqtt()->hash = NULL;
    }
    if(ota_get_transport_mqtt()->sign){
        ota_free(ota_get_transport_mqtt()->sign);
        ota_get_transport_mqtt()->sign = NULL;
    }
    return ota_hal_mqtt_deinit();
}

static int ota_trans_init(void)
{
    int ret = 0;
    ret = ota_hal_mqtt_init();
    if (ret < 0) {
        OTA_LOG_E("mqtt_init failed\n");
        return -1;
    }
    ota_get_transport_mqtt()->url = ota_malloc(OTA_URL_LEN);
    if(NULL == ota_get_transport_mqtt()->url){
        OTA_LOG_E("mqtt url failed\n");
        return -1;
    }
    memset(ota_get_transport_mqtt()->url, 0, OTA_URL_LEN);
    ota_get_transport_mqtt()->hash = ota_malloc(OTA_HASH_LEN);
    if(NULL == ota_get_transport_mqtt()->hash){
        OTA_LOG_E("malloc hash failed\n");
        return -1;
    }
    memset(ota_get_transport_mqtt()->hash, 0, OTA_HASH_LEN);
    ota_get_transport_mqtt()->sign = ota_malloc(OTA_SIGN_LEN);
    if(NULL == ota_get_transport_mqtt()->sign){
        OTA_LOG_E("malloc sign failed\n");
        return -1;
    }
    memset(ota_get_transport_mqtt()->sign, 0, OTA_SIGN_LEN);
    return 0;
}

static ota_transport_t trans_mqtt = {
    .init             = ota_trans_init,
    .inform           = ota_trans_inform,
    .response         = ota_trans_response,
    .upgrade          = ota_trans_upgrade,
    .status           = ota_trans_status,
    .deinit           = ota_trans_deinit,
};

ota_transport_t *ota_get_transport_mqtt(void)
{
    return &trans_mqtt;
}
