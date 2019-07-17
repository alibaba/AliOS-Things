/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "mqtt_http.h"
#include "be_jse_task.h"
#include "be_port_osal.h"
#include "be_utils.h"
#include "cJSON.h"

#include "ca.h"  /* iotx_ca_get */
#include "utils_hmac.h"
#include "utils_httpc.h"

#ifndef CONFIG_MQTT_HTTP_STACK_SIZE
#define CONFIG_MQTT_HTTP_STACK_SIZE 4096
#endif

#define CM_READ_ONLY
static const char string_SHA_METHOD[] CM_READ_ONLY = "hmacsha1";
static const char string_MD5_METHOD[] CM_READ_ONLY = "hmacmd5";
static const char string_TIMESTAMP[] CM_READ_ONLY  = "2524608000000";

static const char string_AUTH_URL[] CM_READ_ONLY =
    "https://iot-auth.cn-shanghai.aliyuncs.com/auth/register/device";

static const char string_AUTH_CONTENT_TYPE[] CM_READ_ONLY =
    "application/x-www-form-urlencoded";
static const char string_hmac_format[] CM_READ_ONLY =
    "deviceName%s"
    "productKey%s"
    "random%s";
static const char string_auth_req_format[] CM_READ_ONLY =
    "productKey=%s&"
    "deviceName=%s&"
    "signMethod=%s&"
    "sign=%s&"
    "version=default&"
    "clientId=%s&"
    "random=%s&"
    "resources=mqtt";
static const int int_random_length = 15;

#define CM_AUTH_TIMEOUT (10 * 1000)

static void *mqttHttpTskHandle = NULL;

extern uint32_t HAL_Random(uint32_t region);
static char *genRandomString(int length) {
    int flag, i;
    char *str;
    if ((str = (char *)malloc(length)) == NULL) {
        return NULL;
    }

    for (i = 0; i < length - 1; i++) {
        flag = rand() % 3;
        switch (flag) {
            case 0:
                str[i] = 'A' + HAL_Random(26);
                break;
            case 1:
                str[i] = 'a' + HAL_Random(26);
                break;
            case 2:
                str[i] = '0' + HAL_Random(10);
                break;
            default:
                str[i] = 'x';
                break;
        }
    }
    str[length - 1] = '\0';
    return str;
}

int _http_response_2(char *payload, const int payload_len,
                     const char *request_string, const char *url,
                     const int port_num, const char *pkey) {
#define HTTP_POST_MAX_LEN (1024)
#define HTTP_RESP_MAX_LEN (1024)

    int ret            = SUCCESS_RETURN;
    char *requ_payload = NULL;
    char *resp_payload = NULL;
    httpclient_t httpc;
    httpclient_data_t httpc_data;

    memset(&httpc, 0, sizeof(httpclient_t));
    memset(&httpc_data, 0, sizeof(httpclient_data_t));

    /* todo */
    httpc.header =
        "Accept: text/xml,text/javascript,text/html,application/json\r\n";

    requ_payload = (char *)malloc(HTTP_POST_MAX_LEN);
    if (NULL == requ_payload) {
        return ERROR_MALLOC;
    }
    memset(requ_payload, 0, HTTP_POST_MAX_LEN);

    snprintf(requ_payload, HTTP_POST_MAX_LEN, "%s", request_string);

    resp_payload = (char *)malloc(HTTP_RESP_MAX_LEN);
    if (!resp_payload) {
        ret = FAIL_RETURN;
        goto RETURN;
    }

    memset(resp_payload, 0, HTTP_RESP_MAX_LEN);

    httpc_data.post_content_type = (char *)string_AUTH_CONTENT_TYPE;
    httpc_data.post_buf          = requ_payload;
    httpc_data.post_buf_len      = strlen(requ_payload);
    httpc_data.response_buf      = resp_payload;
    httpc_data.response_buf_len  = HTTP_RESP_MAX_LEN;

    ret = httpclient_common(&httpc, url, port_num, pkey, HTTPCLIENT_POST,
                            CM_AUTH_TIMEOUT, &httpc_data);
    if (ret != 0) {
        ret = FAIL_RETURN;
        goto RETURN;
    }

    memcpy(payload, httpc_data.response_buf, payload_len);

RETURN:
    if (requ_payload) {
        free(requ_payload);
        requ_payload = NULL;
    }
    if (resp_payload) {
        free(resp_payload);
        resp_payload = NULL;
    }

    return ret;
}

static char *_set_auth_req_str(const char *product_key, const char *device_name,
                               const char *client_id, const char *sign,
                               const char *ts) {
#define AUTH_STRING_MAXLEN (1024)

    char *req = NULL;

    req = malloc(AUTH_STRING_MAXLEN);
    memset(req, 0, AUTH_STRING_MAXLEN);

    snprintf(req, AUTH_STRING_MAXLEN, string_auth_req_format, product_key,
             device_name, string_SHA_METHOD, sign, client_id, ts);
    return req;
}

static char *_get_device_secret(const char *product_key,
                                const char *device_name, const char *client_id,
                                const char *guider_addr,
                                const char *request_string) {
    char payload[512]  = {0};
    char *deviceSecret = NULL;

    strncpy(payload, request_string, strlen(request_string));

    /* printf("[%s][%d] request_string=%s \n", __FUNCTION__, __LINE__,
       request_string); printf("[%s][%d] guider_addr=%s \n", __FUNCTION__,
       __LINE__, guider_addr); */

    _http_response_2(payload, sizeof(payload), request_string, guider_addr, 443,
                     iotx_ca_get());
    /* CM_INFO(cm_log_info_auth_rsp, payload); */

    printf("payload = %s \n", payload);
    /* payload =
       {"code":200,"data":{"deviceName":"123456","deviceSecret":"JkwXalKfOtaP9f9IJGtdqbQaaGr8ghHf","productKey":"a1jbUxTP2VU"},"message":"success"} */

    cJSON *root = NULL;
    cJSON *item;

    root = cJSON_Parse(payload);
    if (root == NULL) {
        goto do_exit;
    }

    item = cJSON_GetObjectItem(root, "code");
    if (cJSON_IsNumber(item)) {
        if (item->valuedouble != 200) {
            goto do_exit;
        }

    } else if (cJSON_IsString(item)) {
        if (strcmp(item->valuestring, "200") != 0) {
            goto do_exit;
        }
    } else {
        goto do_exit;
    }

    item = cJSON_GetObjectItem(root, "data");
    if (item == NULL) goto do_exit;

    cJSON *deviceSecretItem = cJSON_GetObjectItem(item, "deviceSecret");
    if (cJSON_IsString(deviceSecretItem)) {
        /* 获取 deviceSecret */
        deviceSecret = strdup(deviceSecretItem->valuestring);
        printf("获取deviceSecret成功, %s\n", deviceSecret);
    }

do_exit:
    cJSON_Delete(root);
    return deviceSecret;
}

static int _calc_hmac_signature(const char *product_key,
                                const char *product_secret,
                                const char *device_name, char *hmac_sigbuf,
                                const int hmac_buflen, const char *random) {
    char signature[64];
    char hmac_source[512];
    int ret = FAIL_RETURN;

    memset(signature, 0, sizeof(signature));
    memset(hmac_source, 0, sizeof(hmac_source));

    ret = snprintf(hmac_source, sizeof(hmac_source), string_hmac_format,
                   device_name, product_key, random);

    utils_hmac_sha1(hmac_source, strlen(hmac_source), signature, product_secret,
                    strlen(product_secret));

    memcpy(hmac_sigbuf, signature, hmac_buflen);
    return ret;
}

char *iot_get_deviceSecret(const char *product_key, const char *product_secret,
                           const char *device_name, const char *client_id) {
    char *req_str        = NULL;
    char guider_sign[48] = {0};
    char *s_random       = NULL;
    char *deviceSecret   = NULL;

    s_random = strdup("012345678901234");
    /* s_random = genRandomString(int_random_length);
       todo   string_TIMESTAMP -> random */
    _calc_hmac_signature(product_key, product_secret, device_name, guider_sign,
                         sizeof(guider_sign), s_random);

    req_str = _set_auth_req_str(product_key, device_name, client_id,
                                guider_sign, s_random);

    deviceSecret = _get_device_secret(product_key, device_name, client_id,
                                      string_AUTH_URL, req_str);
    if (req_str) free(req_str);
    if (s_random) free(s_random);
    return deviceSecret;
}

static void http_task_post_jse(void *arg) {
    IOT_DEVICESECRET_s *iotDeviceSecret = (IOT_DEVICESECRET_s *)arg;
    int ret;
    be_jse_symbol_t **params;

    if (iotDeviceSecret->deviceSecret[0]) {
        params = (be_jse_symbol_t **)calloc(1, sizeof(be_jse_symbol_t *) * 2);
        params[0] = new_symbol(BE_SYM_NULL);
        params[1] = new_str_symbol(iotDeviceSecret->deviceSecret);
        be_jse_post_async(iotDeviceSecret->func, params, 2);

    } else {
        params = (be_jse_symbol_t **)calloc(1, sizeof(be_jse_symbol_t *) * 1);
        params[0] = new_str_symbol("something error.");
        be_jse_post_async(iotDeviceSecret->func, params, 1);
    }

    free(iotDeviceSecret);
}

static void mqtt_network_task(void *arg) {
    /* MQTT 一型一密 注册
       ProductKey: a1jbUxTP2VU
       ProductSecret: VtuTmAMhR8QXfMa5 */
    /* auth */
    IOT_DEVICESECRET_s *iotDeviceSecret = (IOT_DEVICESECRET_s *)arg;
    char *deviceSecret                  = NULL;
    /* only test ...
       deviceSecret = iot_get_deviceSecret("a1jbUxTP2VU", "VtuTmAMhR8QXfMa5",
                                           "123456", "a1jbUxTP2VU.123456"); */
    char client_id[128];
    snprintf(client_id, 127, "%s.%s", iotDeviceSecret->productKey,
             iotDeviceSecret->deviceName);
    deviceSecret = iot_get_deviceSecret(iotDeviceSecret->productKey,
                                        iotDeviceSecret->productSecret,
                                        iotDeviceSecret->deviceName, client_id);
    if (deviceSecret)
        strcpy(iotDeviceSecret->deviceSecret, deviceSecret);
    else
        iotDeviceSecret->deviceSecret[0] = 0;

    be_jse_task_schedule_call(http_task_post_jse, iotDeviceSecret);

    free(deviceSecret);

    printf("[%s][%d] ~ Byte ~ \n", __FUNCTION__, __LINE__);

    void *handle      = mqttHttpTskHandle;
    mqttHttpTskHandle = NULL;
    be_osal_delete_task(handle);
}

void *mqtt_http_get_instance() { return mqttHttpTskHandle; }

void mqtt_http_start(void *iotDeviceSecret) {
    if (mqttHttpTskHandle == NULL) {
        be_osal_create_task("mqttHttpTsk", mqtt_network_task, iotDeviceSecret,
                            1024 * 7, MQTTHTTP_TSK_PRIORITY,
                            &mqttHttpTskHandle);
    }
}
