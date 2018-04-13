/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "guider_internal.h"
#include "utils_epoch_time.h"

const char *secmode_str[] = {
    "TCP + Guider + Plain",
    "TCP + Guider + ID2-Crypto",
    "TLS + Direct",
    "TCP + Direct + Plain",
    "TCP + Direct + ID2-Crypto",
    "TLS + Guider + ID2-Authenticate",
    "",
    "TLS + Direct + ID2-Crypto",
    ""
};


int g_domain_type = 0;


void guider_set_domain_type(int domain_type)
{
    g_domain_type = domain_type;
}

char* guider_get_domain()
{
    if (0 == g_domain_type)
        return GUIDER_DIRECT_DOMAIN;

    return NULL;
}


static int _calc_hmac_signature(
            char *hmac_sigbuf,
            const int hmac_buflen,
            const char *timestamp_str)
{
    char                    signature[64];
    char                    hmac_source[512];
    int                     rc = -1;
    iotx_device_info_pt     dev;

    dev = iotx_device_info_get();
    LITE_ASSERT(dev);

    memset(signature, 0, sizeof(signature));
    memset(hmac_source, 0, sizeof(hmac_source));
    rc = HAL_Snprintf(hmac_source,
                      sizeof(hmac_source),
                      "clientId%s" "deviceName%s" "productKey%s" "timestamp%s",
                      dev->device_id,
                      dev->device_name,
                      dev->product_key,
                      timestamp_str);
    LITE_ASSERT(rc < sizeof(hmac_source));

#if USING_SHA1_IN_HMAC
    utils_hmac_sha1(hmac_source, strlen(hmac_source),
                    signature,
                    dev->device_secret,
                    strlen(dev->device_secret));
#else
    utils_hmac_md5(hmac_source, strlen(hmac_source),
                   signature,
                   dev->device_secret,
                   strlen(dev->device_secret));
#endif

    memcpy(hmac_sigbuf, signature, hmac_buflen);
    return 0;
}

#ifndef MQTT_DIRECT
int _http_response(char *payload,
                   const int payload_len,
                   const char *request_string,
                   const char *url,
                   const int port_num,
                   const char *pkey
                  )
{
#define HTTP_POST_MAX_LEN   (1024)
#define HTTP_RESP_MAX_LEN   (1024)

    int                     len = 0;
    int                     ret = -1;
    char                   *requ_payload = NULL;
    char                   *resp_payload = NULL;

    httpclient_t            httpc;
    httpclient_data_t       httpc_data;

    memset(&httpc, 0, sizeof(httpclient_t));
    memset(&httpc_data, 0, sizeof(httpclient_data_t));

    httpc.header = "Accept: text/xml,text/javascript,text/html,application/json\r\n";

    requ_payload = (char *)LITE_malloc(HTTP_POST_MAX_LEN);
    if (NULL == requ_payload) {
        log_err("Allocate HTTP request buf failed!");
        return ERROR_MALLOC;
    }
    memset(requ_payload, 0, HTTP_POST_MAX_LEN);

    len = HAL_Snprintf(requ_payload,
                       HTTP_POST_MAX_LEN,
                       "%s",
                       request_string);
    LITE_ASSERT(len < HTTP_POST_MAX_LEN);
    log_debug("requ_payload: \r\n\r\n%s\r\n", requ_payload);

    resp_payload = (char *)LITE_malloc(HTTP_RESP_MAX_LEN);
    if (!resp_payload) {
        goto RETURN;
    }
    LITE_ASSERT(resp_payload);
    memset(resp_payload, 0, HTTP_RESP_MAX_LEN);

    httpc_data.post_content_type = "application/x-www-form-urlencoded;charset=utf-8";
    httpc_data.post_buf = requ_payload;
    httpc_data.post_buf_len = strlen(requ_payload);
    httpc_data.response_buf = resp_payload;
    httpc_data.response_buf_len = HTTP_RESP_MAX_LEN;

    ret = httpclient_common(&httpc,
                            url,
                            port_num,
                            pkey,
                            HTTPCLIENT_POST,
                            CONFIG_GUIDER_AUTH_TIMEOUT,
                            &httpc_data);
    if (ret != 0) {
        goto RETURN;
    }

    memcpy(payload, httpc_data.response_buf, payload_len);
    log_debug("PAYLOAD: %s", payload);

RETURN:
    if (requ_payload) {
        LITE_free(requ_payload);
        requ_payload = NULL;
    }
    if (resp_payload) {
        LITE_free(resp_payload);
        resp_payload = NULL;
    }

    return 0;
}

#endif  /* #ifndef MQTT_DIRECT */

void _ident_partner(char *buf, int len)
{
    char                tmp[PID_STRLEN_MAX] = {0};

    memset(tmp, 0, sizeof(tmp));
    HAL_GetPartnerID(tmp);
    if (strlen(tmp)) {
        HAL_Snprintf(buf, len, ",partner_id=%s", tmp);
    } else {
        strcpy(buf, "");
    }

    return;
}

void _ident_module(char *buf, int len)
{
    char                tmp[MID_STRLEN_MAX] = {0};

    memset(tmp, 0, sizeof(tmp));
    HAL_GetModuleID(tmp);
    if (strlen(tmp)) {
        HAL_Snprintf(buf, len, ",module_id=%s", tmp);
    } else {
        strcpy(buf, "");
    }

    return;
}

int _fill_conn_string(char *dst, int len, const char *fmt, ...)
{
    int                     rc = -1;
    va_list                 ap;
    char                   *ptr = NULL;

    va_start(ap, fmt);
    rc = HAL_Vsnprintf(dst, len, fmt, ap);
    va_end(ap);
    LITE_ASSERT(rc <= len);

    ptr = strstr(dst, "||");
    if (ptr) {
        *ptr = '\0';
    }

    /* log_debug("dst(%d) = %s", rc, dst); */
    return 0;
}

void guider_print_conn_info(iotx_conn_info_pt conn)
{
    log_debug("%s", "-----------------------------------------");
    log_debug("%16s : %-s", "Host", conn->host_name);
    log_debug("%16s : %d",  "Port", conn->port);
    /*log_debug("%16s : %-s", "UserName", conn->username);
    log_debug("%16s : %-s", "PassWord", conn->password);*/  /* remove */
    log_debug("%16s : %-s", "ClientID", conn->client_id);
    if (conn->pub_key) {
        log_debug("%16s : %p ('%.16s ...')", "TLS PubKey", conn->pub_key, conn->pub_key);
    }

    log_debug("%s", "-----------------------------------------");
}

void guider_print_dev_guider_info(iotx_device_info_pt dev,
                                  char *partner_id,
                                  char *module_id,
                                  char *guider_url,
                                  int secure_mode,
                                  char *time_stamp,
                                  char *guider_sign,
                                  char *id2,
                                  char *dev_code)
{
    log_debug("%s", "....................................................");
    log_debug("%20s : %-s", "ProductKey", dev->product_key);
    log_debug("%20s : %-s", "DeviceName", dev->device_name);
    log_debug("%20s : %-s", "DeviceID", dev->device_id);
    /*log_debug("%20s : %-s", "DeviceSecret", dev->device_secret);*/  /* remove */
    log_debug("%s", "....................................................");
    log_debug("%20s : %-s", "PartnerID Buf", partner_id);
    log_debug("%20s : %-s", "ModuleID Buf", module_id);
    log_debug("%20s : %s", "Guider URL", guider_url);
    if (secure_mode > 0) {
        log_debug("%20s : %d (%s)", "Guider SecMode", secure_mode, secmode_str[secure_mode]);
    }
    log_debug("%20s : %s", "Guider Timestamp", time_stamp);
    log_debug("%s", "....................................................");
    /*log_debug("%20s : %s", "Guider Sign", guider_sign);*/ /* remove */
    if (id2 != NULL) {
        log_debug("%20s : %s", "Guider ID2", id2);
        log_debug("%20s : %s", "Guider DeviceCode", dev_code);
    }
    log_debug("%s", "....................................................");

    return;
}

static void guider_get_url(char *buf, int len)
{
#ifdef MQTT_DIRECT
    HAL_Snprintf(buf, len, "%s", "");
#else

    HAL_Snprintf(buf, len, "%s", "http://");

#if defined(TEST_OTA_PRE)
    strcat(buf, "iot-auth-pre.cn-shanghai.aliyuncs.com");
#elif defined(TEST_MQTT_DAILY)
    strcat(buf, "iot-auth.alibaba.net");
#else
    strcat(buf, "iot-auth.cn-shanghai.aliyuncs.com");
#endif

    strcat(buf, "/auth/devicename");

#endif  /* MQTT_DIRECT */

    return;
}

static void guider_get_timestamp_str(char *buf, int len)
{
    HAL_Snprintf(buf, len, "%s", GUIDER_DEFAULT_TS_STR);

    return;
}

static SECURE_MODE guider_get_secure_mode(void)
{
    SECURE_MODE     rc = MODE_TLS_GUIDER;

#ifdef MQTT_DIRECT

#ifdef IOTX_WITHOUT_TLS
    rc = MODE_TCP_DIRECT_PLAIN;
#else
    rc = MODE_TLS_DIRECT;
#endif  /* IOTX_WITHOUT_TLS */

#else   /* MQTT_DIRECT */

#ifdef IOTX_WITHOUT_TLS
    rc = MODE_TCP_GUIDER_PLAIN;
#else
    rc = MODE_TLS_GUIDER;
#endif  /* IOTX_WITHOUT_TLS */

#endif  /* MQTT_DIRECT */

    return  rc;
}

#ifndef MQTT_DIRECT
static char *guider_set_auth_req_str(char sign[], char ts[])
{
#define AUTH_STRING_MAXLEN  (1024)

    char                   *ret = NULL;
    iotx_device_info_pt     dev = NULL;
    int                     rc = -1;

    dev = iotx_device_info_get();
    LITE_ASSERT(dev);

    ret = HAL_Malloc(AUTH_STRING_MAXLEN);
    LITE_ASSERT(ret);
    memset(ret, 0, AUTH_STRING_MAXLEN);

    rc = sprintf(ret,
                 "productKey=%s&" "deviceName=%s&" "signmethod=%s&" "sign=%s&"
                 "version=default&" "clientId=%s&" "timestamp=%s&" "resources=mqtt"
                 , dev->product_key
                 , dev->device_name
#if USING_SHA1_IN_HMAC
                 , SHA_METHOD
#else
                 , MD5_METHOD
#endif
                 , sign
                 , dev->device_id
                 , ts);
    LITE_ASSERT(rc < AUTH_STRING_MAXLEN);

    return ret;
}

static int guider_get_iotId_iotToken(
            const char *guider_addr,
            const char *request_string,
            char *iot_id,
            char *iot_token,
            char *host,
            uint16_t *pport)
{
    char                iotx_payload[1024] = {0};
    int                 iotx_port = 443;
    int                 ret = -1;
    iotx_conn_info_pt   usr = iotx_conn_info_get();
    int                 ret_code = 0;
    const char         *pvalue;
    char                port_str[6];

    LITE_ASSERT(usr);

#ifdef IOTX_WITHOUT_TLS
    iotx_port = 80;
#endif

#if defined(TEST_OTA_PRE)
    iotx_port = 80;
#endif

    /*
        {
            "code": 200,
            "data": {
                "iotId":"030VCbn30334364bb36997f44cMYTBAR",
                "iotToken":"e96d15a4d4734a73b13040b1878009bc",
                "resources": {
                    "mqtt": {
                            "host":"iot-as-mqtt.cn-shanghai.aliyuncs.com",
                            "port":1883
                        }
                    }
            },
            "message":"success"
        }
    */
    _http_response(iotx_payload,
                   sizeof(iotx_payload),
                   request_string,
                   guider_addr,
                   iotx_port,
#if defined(TEST_OTA_PRE)
                   NULL
#elif defined(MQTT_ID2_AUTH) && defined(TEST_ID2_PRE)
                   NULL
#else
                   iotx_ca_get()
#endif
                  );
    log_debug("http response: \r\n\r\n%s\r\n", iotx_payload);

    pvalue = LITE_json_value_of("code", iotx_payload);
    if (!pvalue) {
        goto do_exit;
    }

    ret_code = atoi(pvalue);
    LITE_free(pvalue);
    pvalue = NULL;

    if (200 != ret_code) {
        log_err("++++");
        log_err("ret_code = %d (!= 200), abort!", ret_code);
        log_err("++++");
        goto do_exit;
    }

    pvalue = LITE_json_value_of("data.iotId", iotx_payload);
    if (NULL == pvalue) {
        goto do_exit;
    }
    strcpy(iot_id, pvalue);
    LITE_free(pvalue);
    pvalue = NULL;

    pvalue = LITE_json_value_of("data.iotToken", iotx_payload);
    if (NULL == pvalue) {
        goto do_exit;
    }
    strcpy(iot_token, pvalue);
    LITE_free(pvalue);
    pvalue = NULL;

    pvalue = LITE_json_value_of("data.resources.mqtt.host", iotx_payload);
    if (NULL == pvalue) {
        goto do_exit;
    }
    strcpy(host, pvalue);
    LITE_free(pvalue);
    pvalue = NULL;

    pvalue = LITE_json_value_of("data.resources.mqtt.port", iotx_payload);
    if (NULL == pvalue) {
        goto do_exit;
    }
    strcpy(port_str, pvalue);
    LITE_free(pvalue);
    pvalue = NULL;
    *pport = atoi(port_str);

    log_debug("%10s: %s", "iotId", iot_id);
    log_debug("%10s: %s", "iotToken", iot_token);
    log_debug("%10s: %s", "Host", host);
    log_debug("%10s: %d", "Port", *pport);

    ret = 0;

do_exit:
    if (pvalue) {
        LITE_free(pvalue);
        pvalue = NULL;
    }

    return ret;
}
#endif  /* MQTT_DIRECT */

int iotx_guider_authenticate(void)
{
    char                partner_id[PID_STRLEN_MAX + 16] = {0};
    char                module_id[MID_STRLEN_MAX + 16] = {0};
    char                guider_url[GUIDER_URL_LEN] = {0};
    SECURE_MODE         secure_mode = MODE_TLS_GUIDER;
    char                guider_sign[GUIDER_SIGN_LEN] = {0};
    char                timestamp_str[GUIDER_TS_LEN] = {0};

    iotx_device_info_pt dev = iotx_device_info_get();
    iotx_conn_info_pt   conn = iotx_conn_info_get();
    char               *req_str = NULL;
    int                 gw = 0;

    LITE_ASSERT(dev);
    LITE_ASSERT(conn);

    _ident_partner(partner_id, sizeof(partner_id));
    _ident_module(module_id, sizeof(module_id));
    guider_get_url(guider_url, sizeof(guider_url));
    secure_mode = guider_get_secure_mode();
    guider_get_timestamp_str(timestamp_str, sizeof(timestamp_str));
    _calc_hmac_signature(guider_sign, sizeof(guider_sign), timestamp_str);

    guider_print_dev_guider_info(dev, partner_id, module_id, guider_url, secure_mode,
                                 timestamp_str, guider_sign, NULL, NULL);

#ifndef MQTT_DIRECT
    char            iotx_conn_host[HOST_ADDRESS_LEN + 1] = {0};
    uint16_t        iotx_conn_port = 1883;
    char            iotx_id[GUIDER_IOT_ID_LEN + 1] = {0};
    char            iotx_token[GUIDER_IOT_TOKEN_LEN + 1] = {0};


    req_str = guider_set_auth_req_str(guider_sign, timestamp_str);
    LITE_ASSERT(req_str);
    log_debug("req_str = '%s'", req_str);

    if (0 != guider_get_iotId_iotToken(guider_url,
                                       req_str,
                                       iotx_id,
                                       iotx_token,
                                       iotx_conn_host,
                                       &iotx_conn_port)) {
        if (req_str) {
            HAL_Free(req_str);
        }

        log_err("_iotId_iotToken_http() failed");
        return -1;
    }
#endif

    /* Start Filling Connection Information */
    conn->pub_key = iotx_ca_get();

#ifdef MQTT_DIRECT
#if defined (ON_DAILY)   /* daily*/
    conn->port = 1883;
    _fill_conn_string(conn->host_name, sizeof(conn->host_name),
                      "10.125.0.27");
#elif defined (ON_PRE) || defined (ON_PRE3)    /* pre */
    conn->port = 80;
    _fill_conn_string(conn->host_name, sizeof(conn->host_name),
                      "100.67.80.75");
#else /* online */
    conn->port = 1883;
    _fill_conn_string(conn->host_name, sizeof(conn->host_name),
                      "%s.%s",
                      dev->product_key,
                      guider_get_domain());
#endif
    _fill_conn_string(conn->username, sizeof(conn->username),
                      "%s&%s",
                      dev->device_name,
                      dev->product_key);
    _fill_conn_string(conn->password, sizeof(conn->password),
                      "%s",
                      guider_sign);

#else   /* MQTT_DIRECT */

    conn->port = iotx_conn_port;
    _fill_conn_string(conn->host_name, sizeof(conn->host_name),
                      "%s",
                      iotx_conn_host);
    _fill_conn_string(conn->username, sizeof(conn->username), "%s", iotx_id);
    _fill_conn_string(conn->password, sizeof(conn->password), "%s", iotx_token);

#endif  /* MQTT_DIRECT */

#ifdef GATEWAY_SUPPORT
    gw = 1;
#endif

    _fill_conn_string(conn->client_id, sizeof(conn->client_id),
                      "%s"
                      "|securemode=%d"
#if USING_SHA1_IN_HMAC
                      ",timestamp=%s,signmethod=" SHA_METHOD ",gw=%d"
#else
                      ",timestamp=%s,signmethod=" MD5_METHOD ",gw=%d"
#endif
                      "%s"
                      "%s"
                      "|"
                      , dev->device_id
                      , secure_mode
                      , timestamp_str
                      , gw
                      , partner_id
                      , module_id
                     );

    guider_print_conn_info(conn);

    if (req_str) {
        HAL_Free(req_str);
    }

    return 0;

}

