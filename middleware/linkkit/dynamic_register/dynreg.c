/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include "string.h"
#include "infra_types.h"
#include "infra_defs.h"
#include "infra_string.h"
#include "infra_httpc.h"
#include "infra_sha256.h"
#include "dynreg_internal.h"
#include "dynreg_api.h"
#ifdef MQTT_DYNAMIC_REGISTER
    #include "dev_sign_api.h"
    #include "mqtt_api.h"
#endif

#define HTTP_RESPONSE_PAYLOAD_LEN           (256)

#define DYNREG_RANDOM_KEY_LENGTH            (15)
#define DYNREG_SIGN_LENGTH                  (65)
#define DYNREG_SIGN_METHOD_HMACSHA256       "hmacsha256"

typedef struct {
    char *payload;
    int payload_len;
} dynreg_http_response_t;

static int _parse_string_value(char *payload, int *pos, int *start, int *end)
{
    int idx = 0;

    for (idx = *pos + 1; idx < strlen(payload); idx++) {
        if (payload[idx] == '\"') {
            break;
        }
    }
    *start = *pos + 1;
    *end = idx - 1;
    *pos = idx;

    return 0;
}

static int _parse_dynreg_value(char *payload, char *key, int *pos, int *start, int *end)
{
    int idx = 0;
    /* printf("=====%s\n",&payload[*pos]); */

    if (memcmp(key, "code", strlen("code")) == 0) {
        for (idx = *pos; idx < strlen(payload); idx++) {
            if (payload[idx] < '0' || payload[idx] > '9') {
                break;
            }
        }
        *start = *pos;
        *end = idx - 1;
        *pos = *end;
        return 0;
    } else if (memcmp(key, "data", strlen("data")) == 0) {
        int bracket_cnt = 0;
        if (payload[*pos] != '{') {
            return -1;
        }
        for (idx = *pos; idx < strlen(payload); idx++) {
            if (payload[idx] == '{') {
                bracket_cnt++;
            } else if (payload[idx] == '}') {
                bracket_cnt--;
            }
            if (bracket_cnt == 0) {
                break;
            }
        }
        *start = *pos;
        *end = idx;
        *pos = *end;
        return 0;
    } else {
        if (payload[*pos] != '\"') {
            return -1;
        }
        return _parse_string_value(payload, pos, start, end);
    }

    /* return -1; */
}

static int _parse_dynreg_result(char *payload, char *key, int *start, int *end)
{
    int res = 0, idx = 0, pos = 0;

    for (idx = 0; idx < strlen(payload); idx++) {
        /* printf("loop start: %s\n",&payload[idx]); */
        if (payload[idx] == '\"') {
            for (pos = idx + 1; pos < strlen(payload); pos++) {
                if (payload[pos] == '\"') {
                    /* printf("key: %.*s\n",pos - idx - 1, &payload[idx+1]); */
                    break;
                }
            }

            if (pos == strlen(payload) || payload[pos + 1] != ':') {
                return -1;
            }

            pos += 2;
            res = _parse_dynreg_value(payload, key, &pos, start, end);
            if (res == 0 && memcmp(key, &payload[idx + 1], strlen(key)) == 0) {
                /* printf("value: %.*s\n",*end - *start + 1,&payload[*start]); */
                return 0;
            }

            idx = pos;
        }
    }

    printf("exit 4\n");
    return -1;
}

static int _calc_dynreg_sign(
            char product_key[IOTX_PRODUCT_KEY_LEN],
            char product_secret[IOTX_PRODUCT_SECRET_LEN],
            char device_name[IOTX_DEVICE_NAME_LEN],
            char random[DYNREG_RANDOM_KEY_LENGTH + 1],
            char sign[DYNREG_SIGN_LENGTH])
{
    int sign_source_len = 0;
    uint8_t signnum[32];
    uint8_t  *sign_source = NULL;
    const char *dynamic_register_sign_fmt = "deviceName%sproductKey%srandom%s";

    /* Start Dynamic Register */
    memcpy(random, "8Ygb7ULYh53B6OA", strlen("8Ygb7ULYh53B6OA"));
    dynreg_info("Random Key: %s", random);

    /* Calculate SHA256 Value */
    sign_source_len = strlen(dynamic_register_sign_fmt) + strlen(device_name) + strlen(product_key) + strlen(random) + 1;
    sign_source = dynreg_malloc(sign_source_len);
    if (sign_source == NULL) {
        dynreg_err("Memory Not Enough");
        return FAIL_RETURN;
    }
    memset(sign_source, 0, sign_source_len);
    HAL_Snprintf((char *)sign_source, sign_source_len, dynamic_register_sign_fmt, device_name, product_key, random);

    utils_hmac_sha256(sign_source, strlen((const char *)sign_source), (uint8_t *)product_secret, strlen(product_secret),
                      signnum);
    infra_hex2str(signnum, 32, sign);
    dynreg_free(sign_source);
    dynreg_info("Sign: %s", sign);

    return SUCCESS_RETURN;
}

static int _recv_callback(char *ptr, int length, int total_length, void *userdata)
{
    dynreg_http_response_t *response = (dynreg_http_response_t *)userdata;
    if (strlen(response->payload) + length > response->payload_len) {
        return FAIL_RETURN;
    }
    memcpy(response->payload + strlen(response->payload), ptr, length);

    return length;
}

static int _fetch_dynreg_http_resp(char *request_payload, char *response_payload,
                                   iotx_http_region_types_t region, char device_secret[IOTX_DEVICE_SECRET_LEN])
{
    int                 res = 0;
    const char         *domain = NULL;
    const char         *url_format = "http://%s/auth/register/device";
    char               *url = NULL;
    int                 url_len = 0;
    const char          *pub_key = NULL;
    void                *http_handle = NULL;
    int                  port = 443;
    iotx_http_method_t   method = IOTX_HTTP_POST;
    int                  timeout_ms = 10000;
    char                 *header = "Accept: text/xml,text/javascript,text/html,application/json\r\n" \
                                   "Content-Type: application/x-www-form-urlencoded\r\n";
    int                  http_recv_maxlen = HTTP_RESPONSE_PAYLOAD_LEN;
    dynreg_http_response_t      response;
    int                 start = 0, end = 0, data_start = 0, data_end = 0;

    domain = g_infra_http_domain[region];
    if (NULL == domain) {
        dynreg_err("Get domain failed");
        return FAIL_RETURN;
    }
    url_len = strlen(url_format) + strlen(domain) + 1;
    url = (char *)dynreg_malloc(url_len);
    if (NULL == url) {
        dynreg_err("Not Enough Memory");
        return FAIL_RETURN;
    }
    memset(url, 0, url_len);
    HAL_Snprintf(url, url_len, url_format, domain);

    memset(&response, 0, sizeof(dynreg_http_response_t));
    response.payload = response_payload;
    response.payload_len = HTTP_RESPONSE_PAYLOAD_LEN;
#ifdef SUPPORT_TLS
    {
        extern const char *iotx_ca_crt;
        pub_key = iotx_ca_crt;
    }
#endif

    http_handle = wrapper_http_init();
    wrapper_http_setopt(http_handle, IOTX_HTTPOPT_URL, (void *)url);
    wrapper_http_setopt(http_handle, IOTX_HTTPOPT_PORT, (void *)&port);
    wrapper_http_setopt(http_handle, IOTX_HTTPOPT_METHOD, (void *)&method);
    wrapper_http_setopt(http_handle, IOTX_HTTPOPT_HEADER, (void *)header);
    wrapper_http_setopt(http_handle, IOTX_HTTPOPT_CERT, (void *)pub_key);
    wrapper_http_setopt(http_handle, IOTX_HTTPOPT_TIMEOUT, (void *)&timeout_ms);
    wrapper_http_setopt(http_handle, IOTX_HTTPOPT_RECVCALLBACK, (void *)_recv_callback);
    wrapper_http_setopt(http_handle, IOTX_HTTPOPT_RECVMAXLEN, (void *)&http_recv_maxlen);
    wrapper_http_setopt(http_handle, IOTX_HTTPOPT_RECVCONTEXT, (void *)&response);

    res = wrapper_http_perform(http_handle, request_payload, strlen(request_payload));
    wrapper_http_deinit(&http_handle);

    if (res < SUCCESS_RETURN) {
        dynreg_err("Http Download Failed");
        dynreg_free(url);
        return FAIL_RETURN;
    }
    dynreg_free(url);
    dynreg_info("Http Response Payload: %s", response_payload);

    _parse_dynreg_result(response_payload, "code", &start, &end);
    dynreg_info("Dynamic Register Code: %.*s", end - start + 1, &response_payload[start]);

    if (memcmp(&response_payload[start], "200", strlen("200")) != 0) {
        return FAIL_RETURN;
    }

    _parse_dynreg_result(response_payload, "data", &data_start, &data_end);
    /*  dynreg_info("value: %.*s\n",data_end - data_start + 1,&response_payload[data_start]); */

    _parse_dynreg_result(&response_payload[data_start + 1], "deviceSecret", &start, &end);
    dynreg_info("Dynamic Register Device Secret: %.*s", end - start + 1, &response_payload[data_start + 1 + start]);

    if (end - start + 1 > IOTX_DEVICE_SECRET_LEN) {
        return FAIL_RETURN;
    }

    memcpy(device_secret, &response_payload[data_start + 1 + start], end - start + 1);

    return SUCCESS_RETURN;
}

int32_t _http_dynamic_register(iotx_http_region_types_t region, iotx_dev_meta_info_t *meta)
{
    int             res = 0, dynamic_register_request_len = 0;
    char            sign[DYNREG_SIGN_LENGTH] = {0};
    char            random[DYNREG_RANDOM_KEY_LENGTH + 1] = {0};
    const char     *dynamic_register_format = "productKey=%s&deviceName=%s&random=%s&sign=%s&signMethod=%s";
    char           *dynamic_register_request = NULL;
    char           *dynamic_register_response = NULL;

    if (strlen(meta->product_key) > IOTX_PRODUCT_KEY_LEN ||
        strlen(meta->product_secret) > IOTX_PRODUCT_SECRET_LEN ||
        strlen(meta->device_name) > IOTX_DEVICE_NAME_LEN) {
        return FAIL_RETURN;
    }

    /* Calcute Signature */
    res = _calc_dynreg_sign(meta->product_key, meta->product_secret, meta->device_name, random, sign);
    if (res != SUCCESS_RETURN) {
        dynreg_err("Calculate Sign Failed");
        return FAIL_RETURN;
    }

    /* Assemble Http Dynamic Register Request Payload */
    dynamic_register_request_len = strlen(dynamic_register_format) + strlen(meta->product_key) + strlen(meta->device_name) +
                                   strlen(random) + strlen(sign) + strlen(DYNREG_SIGN_METHOD_HMACSHA256) + 1;
    dynamic_register_request = dynreg_malloc(dynamic_register_request_len);
    if (dynamic_register_request == NULL) {
        dynreg_err("Not Enough Memory");
        return FAIL_RETURN;
    }
    memset(dynamic_register_request, 0, dynamic_register_request_len);
    HAL_Snprintf(dynamic_register_request, dynamic_register_request_len, dynamic_register_format,
                 meta->product_key, meta->device_name, random, sign, DYNREG_SIGN_METHOD_HMACSHA256);

    dynamic_register_response = dynreg_malloc(HTTP_RESPONSE_PAYLOAD_LEN);
    if (dynamic_register_response == NULL) {
        dynreg_err("Not Enough Memory");
        dynreg_free(dynamic_register_request);
        return FAIL_RETURN;
    }
    memset(dynamic_register_response, 0, HTTP_RESPONSE_PAYLOAD_LEN);

    /* Send Http Request For Getting Device Secret */
    res = _fetch_dynreg_http_resp(dynamic_register_request, dynamic_register_response, region, meta->device_secret);

#ifdef INFRA_LOG_NETWORK_PAYLOAD
    dynreg_dbg("Downstream Payload:");
    iotx_facility_json_print(dynamic_register_response, LOG_DEBUG_LEVEL, '<');
#endif

    dynreg_free(dynamic_register_request);
    dynreg_free(dynamic_register_response);
    if (res != SUCCESS_RETURN) {
        dynreg_err("Get Device Secret Failed");
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

#ifdef MQTT_DYNAMIC_REGISTER
static int _mqtt_dynreg_sign_password(iotx_dev_meta_info_t *meta_info, iotx_sign_mqtt_t *signout, char *rand)
{
    char signsource[DEV_SIGN_SOURCE_MAXLEN] = {0};
    uint16_t signsource_len = 0;
    const char sign_fmt[] = "deviceName%sproductKey%srandom%s";
    uint8_t sign_hex[32] = {0};

    signsource_len = strlen(sign_fmt) + strlen(meta_info->device_name) + 1 + strlen(meta_info->product_key) + strlen(
                                 rand) + 1;
    if (signsource_len >= DEV_SIGN_SOURCE_MAXLEN) {
        return ERROR_DEV_SIGN_SOURCE_TOO_SHORT;
    }
    memset(signsource, 0, signsource_len);
    memcpy(signsource, "deviceName", strlen("deviceName"));
    memcpy(signsource + strlen(signsource), meta_info->device_name, strlen(meta_info->device_name));
    memcpy(signsource + strlen(signsource), "productKey", strlen("productKey"));
    memcpy(signsource + strlen(signsource), meta_info->product_key, strlen(meta_info->product_key));
    memcpy(signsource + strlen(signsource), "random", strlen("random"));
    memcpy(signsource + strlen(signsource), rand, strlen(rand));

    utils_hmac_sha256((const uint8_t *)signsource, strlen(signsource), (uint8_t *)meta_info->product_secret,
                      strlen(meta_info->product_secret), sign_hex);
    infra_hex2str(sign_hex, 32, signout->password);

    return SUCCESS_RETURN;
}

static int32_t _mqtt_dynreg_sign_clientid(iotx_dev_meta_info_t *meta_info, iotx_sign_mqtt_t *signout, char *rand)
{
    const char *clientid1 = "|authType=register,random=";
    const char *clientid2 = ",signmethod=hmacsha256,securemode=2|";
    uint32_t clientid_len = 0;

    clientid_len = strlen(meta_info->product_key) + 1 + strlen(meta_info->device_name) +
                   strlen(clientid1) + strlen(rand) + strlen(clientid2) + 1;
    if (clientid_len >= DEV_SIGN_CLIENT_ID_MAXLEN) {
        return ERROR_DEV_SIGN_CLIENT_ID_TOO_SHORT;
    }
    memset(signout->clientid, 0, clientid_len);
    memcpy(signout->clientid, meta_info->product_key, strlen(meta_info->product_key));
    memcpy(signout->clientid + strlen(signout->clientid), ".", strlen("."));
    memcpy(signout->clientid + strlen(signout->clientid), meta_info->device_name, strlen(meta_info->device_name));
    memcpy(signout->clientid + strlen(signout->clientid), clientid1, strlen(clientid1));
    memcpy(signout->clientid + strlen(signout->clientid), rand, strlen(rand));
    memcpy(signout->clientid + strlen(signout->clientid), clientid2, strlen(clientid2));

    return SUCCESS_RETURN;
}

void _mqtt_dynreg_topic_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    int32_t res = 0;
    char *ds = (char *)pcontext;
    iotx_mqtt_topic_info_t     *topic_info = (iotx_mqtt_topic_info_pt) msg->msg;
    const char *asterisk = "**********************";

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED: {
            /* print topic name and topic message */
            char *device_secret = NULL;
            uint32_t device_secret_len = 0;

            /* parse secret */
            res = infra_json_value((char *)topic_info->payload, topic_info->payload_len, "deviceSecret", strlen("deviceSecret"),
                                   &device_secret, &device_secret_len);
            if (res == SUCCESS_RETURN) {
                memcpy(ds, device_secret + 1, device_secret_len - 2);
                memcpy(device_secret + 1 + 5, asterisk, strlen(asterisk));
                dynreg_info("Topic  : %.*s", topic_info->topic_len, topic_info->ptopic);
                dynreg_info("Payload: %.*s", topic_info->payload_len, topic_info->payload);
            }
        }
        break;
        default:
            break;
    }
}

int32_t _mqtt_dynamic_register(iotx_http_region_types_t region, iotx_dev_meta_info_t *meta)
{
    void *pClient = NULL;
    iotx_mqtt_param_t mqtt_params;
    int32_t res = 0;
    uint32_t length = 0, rand = 0;
    char rand_str[9] = {0};
    iotx_sign_mqtt_t signout;
    uint64_t timestart = 0, timenow = 0;
    char device_secret[IOTX_DEVICE_SECRET_LEN + 1] = {0};

    memset(&signout, 0, sizeof(iotx_sign_mqtt_t));

    /* setup hostname */
    if (IOTX_HTTP_REGION_CUSTOM == region) {
        if (g_infra_mqtt_domain[region] == NULL) {
            return ERROR_DEV_SIGN_CUSTOM_DOMAIN_IS_NULL;
        }

        length = strlen(g_infra_mqtt_domain[region]) + 1;
        if (length >= DEV_SIGN_HOSTNAME_MAXLEN) {
            return ERROR_DEV_SIGN_HOST_NAME_TOO_SHORT;
        }

        memset(signout.hostname, 0, DEV_SIGN_HOSTNAME_MAXLEN);
        memcpy(signout.hostname, g_infra_mqtt_domain[region], strlen(g_infra_mqtt_domain[region]));
    } else {
        length = strlen(meta->product_key) + strlen(g_infra_mqtt_domain[region]) + 2;
        if (length >= DEV_SIGN_HOSTNAME_MAXLEN) {
            return ERROR_DEV_SIGN_HOST_NAME_TOO_SHORT;
        }
        memset(signout.hostname, 0, DEV_SIGN_HOSTNAME_MAXLEN);
        memcpy(signout.hostname, meta->product_key, strlen(meta->product_key));
        memcpy(signout.hostname + strlen(signout.hostname), ".", strlen("."));
        memcpy(signout.hostname + strlen(signout.hostname), g_infra_mqtt_domain[region],
               strlen(g_infra_mqtt_domain[region]));
    }

    /* setup port */
    signout.port = 443;

    /* setup username */
    length = strlen(meta->device_name) + strlen(meta->product_key) + 2;
    if (length >= DEV_SIGN_USERNAME_MAXLEN) {
        return ERROR_DEV_SIGN_USERNAME_TOO_SHORT;
    }
    memset(signout.username, 0, DEV_SIGN_USERNAME_MAXLEN);
    memcpy(signout.username, meta->device_name, strlen(meta->device_name));
    memcpy(signout.username + strlen(signout.username), "&", strlen("&"));
    memcpy(signout.username + strlen(signout.username), meta->product_key, strlen(meta->product_key));

    /* password */
    rand = HAL_Random(0xffffffff);
    infra_hex2str((unsigned char *)&rand, 4, rand_str);
    res = _mqtt_dynreg_sign_password(meta, &signout, rand_str);
    if (res < 0) {
        return res;
    }

    /* client id */
    res = _mqtt_dynreg_sign_clientid(meta, &signout, rand_str);
    if (res < 0) {
        return res;
    }

    memset(&mqtt_params, 0, sizeof(iotx_mqtt_param_t));
    mqtt_params.host = signout.hostname;
    mqtt_params.port = signout.port;
    mqtt_params.username = signout.username;
    mqtt_params.password = signout.password;
    mqtt_params.client_id = signout.clientid;
    mqtt_params.request_timeout_ms = 5000;
    mqtt_params.clean_session = 1;
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.read_buf_size = 1000;
    mqtt_params.write_buf_size = 1000;

    pClient =  wrapper_mqtt_init(&mqtt_params);
    if (pClient == NULL) {
        return FAIL_RETURN;
    }

    res = wrapper_mqtt_connect(pClient);
    if (res < SUCCESS_RETURN) {
        wrapper_mqtt_release(&pClient);
        return FAIL_RETURN;
    }

    res = wrapper_mqtt_subscribe(pClient, "/ext/register", IOTX_MQTT_QOS1, _mqtt_dynreg_topic_handle, device_secret);
    if (res < 0) {
        wrapper_mqtt_release(&pClient);
        return FAIL_RETURN;
    }

    timestart = HAL_UptimeMs();
    while (1) {
        timenow = HAL_UptimeMs();
        if (timenow < timestart) {
            timestart = timenow;
        }

        if (timestart - timenow >= MQTT_DYNREG_TIMEOUT_MS) {
            break;
        }

        wrapper_mqtt_yield(pClient, 200);

        if (strlen(device_secret) > 0) {
            break;
        }
    }

    if (strlen(device_secret) > 0) {
        res = SUCCESS_RETURN;
    } else {
        res = FAIL_RETURN;
    }

    wrapper_mqtt_release(&pClient);

    memcpy(meta->device_secret, device_secret, strlen(device_secret));

    return res;
}
#endif

int32_t IOT_Dynamic_Register(iotx_http_region_types_t region, iotx_dev_meta_info_t *meta)
{
#ifdef MQTT_DYNAMIC_REGISTER
    return _mqtt_dynamic_register(region, meta);
#else
    return _http_dynamic_register(region, meta);
#endif
}

