/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "linkkit/infra/infra_config.h"

#ifdef INFRA_PREAUTH

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "linkkit/infra/infra_types.h"
#include "linkkit/infra/infra_defs.h"
#include "linkkit/infra/infra_httpc.h"
#include "linkkit/infra/infra_preauth.h"
#include "linkkit/infra/infra_string.h"

#define PREAUTH_HTTP_REQ_LEN            300
#define PREAUTH_HTTP_RSP_LEN            300

#define PREAUTH_IOT_ID_MAXLEN           (64)
#define PREAUTH_IOT_TOKEN_MAXLEN        (65)
#define PREAUTH_IOT_HOST_MAXLEN         (64)

#ifndef CONFIG_GUIDER_AUTH_TIMEOUT
    #define CONFIG_GUIDER_AUTH_TIMEOUT      (10 * 1000)
#endif

#ifdef SUPPORT_TLS
    extern const char *iotx_ca_crt;
#endif

typedef struct {
    char *payload;
    int payload_len;
} preauth_http_response_t;

int _preauth_assemble_auth_req_string(const iotx_dev_meta_info_t *dev_meta, const char *sign,
                                      const char *device_id, char *request_buff, uint32_t buff_len)
{
    uint8_t i = 0;
    const char *kv[][2] = {
        { "productKey", NULL },
        { "deviceName", NULL },
        { "signmethod", "hmacsha256"},
        { "sign", NULL },
        { "version", "default" },
        { "clientId", NULL },
        { "timestamp", "2524608000000" },
        { "resources", "mqtt" }
    };

    if (dev_meta == NULL || sign == NULL || device_id == NULL || request_buff == NULL) {
        return FAIL_RETURN;
    }

    kv[0][1] = dev_meta->product_key;
    kv[1][1] = dev_meta->device_name;
    kv[3][1] = sign;
    kv[5][1] = device_id;

    for (i = 0; i < (sizeof(kv) / (sizeof(kv[0]))); i++) {
        if ((strlen(request_buff) + strlen(kv[i][0]) + strlen(kv[i][1]) + 2) >=
            buff_len) {
            return FAIL_RETURN;
        }

        memcpy(request_buff + strlen(request_buff), kv[i][0], strlen(kv[i][0]));
        memcpy(request_buff + strlen(request_buff), "=", 1);
        memcpy(request_buff + strlen(request_buff), kv[i][1], strlen(kv[i][1]));
        memcpy(request_buff + strlen(request_buff), "&", 1);
    }

    memset(request_buff + strlen(request_buff) - 1, '\0', 1);
    return SUCCESS_RETURN;
}

static int _preauth_get_string_value(char *p_string, char *value_buff, uint32_t buff_len)
{
    char *p = p_string;
    char *p_start = NULL;
    char *p_end = NULL;
    uint32_t len = 0;

    while (*(++p) != ',' || *p != '}') {
        if (*p == '\"') {
            if (p_start) {
                p_end = p;
                break;
            } else {
                p_start = p + 1;
            }
        }
    }

    if (p_start == NULL || p_end == NULL) {
        return FAIL_RETURN;
    }

    len = p_end - p_start;
    if (len > buff_len) {
        return FAIL_RETURN;
    }

    memcpy(value_buff, p_start, len);
    return SUCCESS_RETURN;
}

static int _preauth_parse_auth_rsp_string(char *json_string, uint32_t string_len, iotx_pre_auth_output_t *output)
{
    int res = FAIL_RETURN;
    char *p = json_string;
    char *p_start, *p_end, *pt;
    uint8_t len;
    int code = 0;

    while (p < (json_string + string_len)) {
        while (*(++p) != ':') {
            if (p >= (json_string + string_len)) {
                if (code != 200) {
                    return FAIL_RETURN;
                }
                else {
                    return SUCCESS_RETURN;
                }
            }
        }

        pt = p;
        p_start = p_end = NULL;
        while (--pt > json_string) {
            if (*pt == '\"') {
                if (p_end != NULL) {
                    p_start = pt + 1;
                    break;
                } else {
                    p_end = pt;
                }
            }
        }

        if (p_start == NULL || p_end == NULL) {
            return FAIL_RETURN;
        }
        len = p_end - p_start;

        if (strlen("code") == len && !memcmp(p_start, "code", len)) {
            infra_str2int(++p, &code);
            if (code != 200) {
                return FAIL_RETURN;
            }
        } else if (strlen("iotId") == len && !memcmp(p_start, "iotId", len)) {
            res = _preauth_get_string_value(p, output->username, PREAUTH_IOT_ID_MAXLEN);
            if (res < SUCCESS_RETURN) {
                return res;
            }
        } else if (strlen("iotToken") == len && !memcmp(p_start, "iotToken", len)) {
            res = _preauth_get_string_value(p, output->password, PREAUTH_IOT_TOKEN_MAXLEN);
            if (res < SUCCESS_RETURN) {
                return res;
            }
        } else if (strlen("host") == len && !memcmp(p_start, "host", len)) {
            res = _preauth_get_string_value(p, output->hostname, PREAUTH_IOT_HOST_MAXLEN);
            if (res < SUCCESS_RETURN) {
                return res;
            }
        } else if (strlen("port") == len && !memcmp(p_start, "port", len)) {
            int port_temp;
            infra_str2int(++p, &port_temp);
            output->port = port_temp;
        }
    }

    return SUCCESS_RETURN;
}

static int _preauth_recv_callback(char *ptr, int length, int total_length, void *userdata)
{
    preauth_http_response_t *response = (preauth_http_response_t *)userdata;
    if (strlen(response->payload) + length > response->payload_len) {
        return FAIL_RETURN;
    }
    memcpy(response->payload + strlen(response->payload), ptr, length);

    return length;
}

int preauth_get_connection_info(iotx_mqtt_region_types_t region, iotx_dev_meta_info_t *dev_meta,
                                const char *sign, const char *device_id, iotx_pre_auth_output_t *preauth_output)
{
    char http_url[128] = "http://";
    char http_url_frag[] = "/auth/devicename";
#ifdef SUPPORT_TLS
    int http_port = 443;
    char *pub_key = (char *)iotx_ca_crt;
#else
    int http_port = 80;
    char *pub_key = NULL;
#endif
    int res = FAIL_RETURN;
    void *http_handle = NULL;
    iotx_http_method_t http_method = IOTX_HTTP_POST;
    int http_timeout_ms = CONFIG_GUIDER_AUTH_TIMEOUT;
    preauth_http_response_t response; 
    char *http_header = "Accept: text/xml,text/javascript,text/html,application/json\r\n" \
                        "Content-Type: application/x-www-form-urlencoded;charset=utf-8\r\n";
    int http_recv_maxlen = PREAUTH_HTTP_RSP_LEN;
    char request_buff[PREAUTH_HTTP_REQ_LEN] = {0};
    char response_buff[PREAUTH_HTTP_RSP_LEN] = {0};

    if (g_infra_http_domain[region] == NULL) {
        return FAIL_RETURN;
    }

    memset(&response, 0, sizeof(preauth_http_response_t));
    memcpy(http_url + strlen(http_url), g_infra_http_domain[region], strlen(g_infra_http_domain[region]));
    memcpy(http_url + strlen(http_url), http_url_frag, sizeof(http_url_frag));

    _preauth_assemble_auth_req_string(dev_meta, sign, device_id, request_buff, sizeof(request_buff));

    response.payload = response_buff;
    response.payload_len = PREAUTH_HTTP_RSP_LEN;

    http_handle = wrapper_http_init();
    wrapper_http_setopt(http_handle, IOTX_HTTPOPT_URL, (void *)http_url);
    wrapper_http_setopt(http_handle, IOTX_HTTPOPT_PORT, (void *)&http_port);
    wrapper_http_setopt(http_handle, IOTX_HTTPOPT_METHOD, (void *)&http_method);
    wrapper_http_setopt(http_handle, IOTX_HTTPOPT_HEADER, (void *)http_header);
    wrapper_http_setopt(http_handle, IOTX_HTTPOPT_CERT, (void *)pub_key);
    wrapper_http_setopt(http_handle, IOTX_HTTPOPT_TIMEOUT, (void *)&http_timeout_ms);
    wrapper_http_setopt(http_handle, IOTX_HTTPOPT_RECVCALLBACK, (void *)_preauth_recv_callback);
    wrapper_http_setopt(http_handle, IOTX_HTTPOPT_RECVMAXLEN, (void *)&http_recv_maxlen);
    wrapper_http_setopt(http_handle, IOTX_HTTPOPT_RECVCONTEXT, (void *)&response);

    res = wrapper_http_perform(http_handle, request_buff, strlen(request_buff));
    wrapper_http_deinit(&http_handle);

    if (res < SUCCESS_RETURN) {
        return res;
    }

#ifdef INFRA_LOG_NETWORK_PAYLOAD
    preauth_info("Downstream Payload:");
    iotx_facility_json_print(response_buff, LOG_INFO_LEVEL, '<');
#endif
    res = _preauth_parse_auth_rsp_string(response_buff, strlen(response_buff), preauth_output);

    return res;
}

#endif /* #ifdef MQTT_PRE_AUTH */
