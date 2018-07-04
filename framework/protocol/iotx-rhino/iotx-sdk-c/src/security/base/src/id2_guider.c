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
#ifdef MQTT_ID2_AUTH
#include "guider_internal.h"
#include "utils_epoch_time.h"
#include "tfs.h"
#include "sec_debug.h"

#ifndef MQTT_DIRECT
static int _is_non_symbol(char c)
{
    int         c_int = (int)c;

    if (c == '\0') {
        return 1;
    }
    return (c_int >= 48 && c_int <= 57) || (c_int >= 65 && c_int <= 90) || (c_int >= 97 && c_int <= 122);
}

static int _url_encode(const char *input, char *output)
{
    if (input == NULL || output == NULL) {
        sec_err("_url_encode: input param error!");
        return -1;
    }

    char        encoded[4] = {0};
    char        c;

    while (*input) {
        c = *input;

        if (c < 0) {
            input++;
        } else if (_is_non_symbol(c)) {
            *output++ = *input++;
        } else {
            HAL_Snprintf(encoded, 4, "%%%02x", c);
            *output++ = encoded[0];
            *output++ = encoded[1];
            *output++ = encoded[2];
            input++;
        }
    }

    *output++ = 0;

    return 0;
}
#endif /* #ifndef MQTT_DIRECT */

static int _calc_id2_signature(
            char *id2_sigbuf,
            const int sig_buflen,
            char *timestamp_str,
            char **id2_str,
            char **device_code_str)
{
    int                     rc = -1;
    uint8_t                 id2[TFS_ID2_LEN + 1] = {0};
    uint32_t                id2_len = TFS_ID2_LEN + 1;
    uint8_t                 dev_code[GUIDER_DEVCODE_LEN] = {0};
    uint32_t                dev_code_len = GUIDER_DEVCODE_LEN;
    uint32_t                sign_len = GUIDER_DEVCODE_LEN;
#ifndef MQTT_DIRECT
    char                    url_encode_buf[GUIDER_URLENCODE_LEN] = {0};
#endif
    iotx_device_info_pt     dev = NULL;
    char                   *digest_str = NULL;

    dev = iotx_device_info_get();
    LITE_ASSERT(dev);

    /* Get timestamp */
    sec_debug("timestamp_str: %s", timestamp_str);

    /* Get Device Code */
    rc = tfs_id2_get_timestamp_auth_code((uint8_t *)timestamp_str,
                                         NULL, 0, dev_code, &dev_code_len);

    LITE_ASSERT(!rc);
    *device_code_str = LITE_strdup((const char *)dev_code);

    /* Get ID2 */
    rc = tfs_get_ID2(id2, &id2_len);
    LITE_ASSERT(rc >= 0);
    *id2_str = LITE_strdup((const char *)id2);

    /* Get Signature */
    digest_str = LITE_format_string("clientId%s"
                                    "id2%s" "productKey%s" "timestamp%s",
                                    dev->device_id,
                                    *id2_str,
                                    dev->product_key,
                                    timestamp_str);
    rc = tfs_id2_get_timestamp_auth_code((uint8_t *)timestamp_str,
                                         (uint8_t *)digest_str, strlen(digest_str),
                                         (uint8_t *)id2_sigbuf, &sign_len);

    LITE_ASSERT(!rc);
    LITE_ASSERT(sign_len <= sig_buflen);
    LITE_free(digest_str);

#ifndef MQTT_DIRECT
    /* Get URL-Encoded Device Code */
    _url_encode((const char *)*device_code_str, url_encode_buf);
    LITE_free(*device_code_str);
    *device_code_str = LITE_strdup(url_encode_buf);

    /* Get URL-Encoded Signature */
    _url_encode((const char *)id2_sigbuf, url_encode_buf);
    memset(id2_sigbuf, 0, sig_buflen);
    strncpy(id2_sigbuf, url_encode_buf, sig_buflen);
#endif

    return 0;
}

static void id2_guider_get_timestamp_str(char *buf, int len)
{
    uint64_t ret = 0;
    int retry = 0;

    do {
        ret = utils_get_epoch_time_from_ntp(buf, len);
    } while (ret == 0 && ++retry < 10);

    if (retry > 1) {
        sec_err("utils_get_epoch_time_from_ntp() retry = %d.", retry);
    }

    if (ret == 0) {
        sec_err("utils_get_epoch_time_from_ntp() failed!");
    }

    return;
}

static void id2_guider_get_url(char *buf, int len)
{
#ifdef MQTT_DIRECT
    HAL_Snprintf(buf, len, "%s", "");
#else

    HAL_Snprintf(buf, len, "%s", "http://");

#if defined(TEST_ID2_DAILY)
    strcat(buf, "iot-auth.alibaba.net");
#elif defined(TEST_OTA_PRE)
    strcat(buf, "iot-auth-pre.cn-shanghai.aliyuncs.com");
#elif defined(TEST_ID2_PRE)
    strcat(buf, "iot-auth-pre.cn-shanghai.aliyuncs.com");
#elif defined(TEST_MQTT_DAILY)
    strcat(buf, "iot-auth.alibaba.net");
#else
    strcat(buf, "iot-auth.cn-shanghai.aliyuncs.com");
#endif

    strcat(buf, "/auth/id2");

#endif  /* MQTT_DIRECT */

    return;
}

static SECURE_MODE id2_guider_get_secure_mode(void)
{
    SECURE_MODE     rc = MODE_TLS_GUIDER;

#ifdef MQTT_DIRECT

#if defined(IOTX_WITHOUT_TLS) && defined(IOTX_WITHOUT_ITLS)
    rc = MODE_TCP_DIRECT_ID2_ENCRYPT;
#elif !defined(IOTX_WITHOUT_TLS)
#ifndef MQTT_ID2_CRYPTO
    rc = MODE_TLS_DIRECT_ID2_ENCRYPT;
#endif
#else
    rc = MODE_ITLS_DNS_ID2;
#endif  /* IOTX_WITHOUT_TLS */

#else   /* MQTT_DIRECT */

#ifdef IOTX_WITHOUT_TLS
    rc = MODE_TCP_GUIDER_PLAIN;
#else
#ifdef MQTT_ID2_CRYPTO
    rc = MODE_TCP_GUIDER_ID2_ENCRYPT;
#else
    rc = MODE_TLS_GUIDER;
#endif
#endif  /* IOTX_WITHOUT_TLS */

#endif  /* MQTT_DIRECT */

    return  rc;
}

#ifndef MQTT_DIRECT
static char *id2_guider_set_auth_req_str(char sign[], char ts[], char id2[]
#ifdef MQTT_ID2_CRYPTO
    , char dev_code[]
#endif
                                        )
{
#define AUTH_STRING_MAXLEN  (1024)

    char                   *ret = NULL;
    iotx_device_info_pt     dev = NULL;
    int                     rc = -1;
    int                     ext = 0;

    dev = iotx_device_info_get();
    LITE_ASSERT(dev);

    ret = LITE_malloc(AUTH_STRING_MAXLEN);
    LITE_ASSERT(ret);
    memset(ret, 0, AUTH_STRING_MAXLEN);

#ifdef SUPPORT_AUTH_ROUTER
    ext = 1;
#endif

    rc = sprintf(ret,
                 "id2=%s&" "sign=%s&"
#ifdef MQTT_ID2_CRYPTO
                 "deviceCode=%s&"
#endif
                 "productKey=%s&"
                 "timestamp=%s&" "version=default&" "clientId=%s&" "resources=mqtt&ext=%d,codec",
                 id2, sign,
#ifdef MQTT_ID2_CRYPTO
                 dev_code,
#endif
                 dev->product_key,
                 ts, dev->device_id,
                 ext);
    LITE_ASSERT(rc < AUTH_STRING_MAXLEN);

    return ret;
}

static int id2_guider_get_iotId_iotToken(
            const char *guider_addr,
            const char *request_string,
            char *iot_id,
            char *iot_token,
            char *host,
            uint16_t *pport)
{
#define PAYLOAD_STRING_MAXLEN  (1024)
    char*               iotx_payload = NULL;
    int                 iotx_port = 443;
    int                 ret = -1;
    iotx_conn_info_pt   usr = iotx_conn_info_get();
    int                 ret_code = 0;
    uint8_t            *b64_decode = NULL;
    uint8_t            *id2_decrypt = NULL;
    int                 id2_rc = -1;
    uint32_t            src_len = 0;
    uint32_t            dst_len = 0;
    uint32_t            dec_len = 512;
    int                 cipher_data = 0;
    const char         *pvalue;
    char                port_str[6];

    LITE_ASSERT(usr);

#ifdef IOTX_WITHOUT_TLS
    iotx_port = 80;
#endif

#if defined(TEST_OTA_PRE)
    iotx_port = 80;
#endif

#if defined(TEST_ID2_PRE)
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
    iotx_payload = LITE_malloc(PAYLOAD_STRING_MAXLEN);
    LITE_ASSERT(iotx_payload);
    memset(iotx_payload, 0, PAYLOAD_STRING_MAXLEN);
    _http_response(iotx_payload,
                   sizeof(iotx_payload),
                   request_string,
                   guider_addr,
                   iotx_port,
#if defined(TEST_OTA_PRE)
                   NULL
#elif defined(TEST_ID2_PRE)
                   NULL
#else
                   iotx_ca_get()
#endif
                  );
    sec_debug("http response: \r\n\r\n%s\r\n", iotx_payload);

    pvalue = LITE_json_value_of("code", iotx_payload);
    if (!pvalue) {
        goto do_exit;
    }

    ret_code = atoi(pvalue);
    LITE_free(pvalue);
    pvalue = NULL;

    if (200 != ret_code) {
        sec_err("++++");
        sec_err("ret_code = %d (!= 200), abort!", ret_code);
        sec_err("++++");
        goto do_exit;
    }

    pvalue = LITE_json_value_of("data.iotId", iotx_payload);
    if (NULL == pvalue) {
        cipher_data = 1;
        sec_debug("'data.iotId' NOT found, cipher_data = %d", cipher_data);
    } else {
        LITE_free(pvalue);
        pvalue = NULL;
        cipher_data = 0;
        sec_debug("'data.iotId' already found, cipher_data = %d", cipher_data);
    }

    if (cipher_data) {
        pvalue = LITE_json_value_of("data", iotx_payload);
        if (NULL == pvalue) {
            sec_err("'data' section not found in HTTP resp payload!");
            goto do_exit;
        }
        src_len = (uint32_t)strlen(pvalue);
        b64_decode = LITE_malloc(src_len);
        if (!b64_decode) {
            sec_err("allocate memory for b64_decode failed");
            goto do_exit;
        }
        id2_rc = utils_base64decode((const uint8_t *)pvalue,
                                    src_len,
                                    src_len,
                                    b64_decode,
                                    &dst_len);
        sec_debug("rc = utils_base64decode() = %d, %u Bytes => %u Bytes", id2_rc, src_len, dst_len);
        if (id2_rc) {
            goto do_exit;
        }
        LITE_free(pvalue);
        pvalue = NULL;

        id2_decrypt = LITE_malloc(dst_len);
        if (!id2_decrypt) {
            sec_err("allocate memory for id2_decrypt failed");
            goto do_exit;
        }

        id2_rc = tfs_id2_decrypt((const uint8_t *)b64_decode,
                                 dst_len,
                                 id2_decrypt,
                                 &dec_len);
        /* remove */
        /*sec_debug("rc = tfs_id2_decrypt() = %d, %u Bytes => %u Bytes", id2_rc, dst_len, dec_len);*/
        if (id2_rc != 0) {
            sec_err("decrypt cipher text with ID2 key error!");
            goto do_exit;
        }

        /* remove */
        /*sec_debug("id2_decrypt = %s", id2_decrypt);*/

        sprintf(iotx_payload, "{\"data\":");
        strcat(iotx_payload, (const char *)id2_decrypt);
        strcat(iotx_payload, "}");
        /* remove */
        /*sec_debug("iotx_payload = %s", iotx_payload);*/
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

    pvalue = LITE_json_value_of("data.resources.codec.key", iotx_payload);
    if (NULL == pvalue) {
        goto do_exit;
    }
    strcpy(usr->aeskey_str, pvalue);
    LITE_free(pvalue);

    src_len = (uint32_t)strlen(usr->aeskey_str);
    id2_rc = utils_base64decode((const uint8_t *)usr->aeskey_str,
                                src_len,
                                src_len,
                                usr->aeskey_hex,
                                &dst_len);
    sec_debug("rc = utils_base64decode() = %d, %u Bytes => %u Bytes", id2_rc, src_len, dst_len);
    LITE_ASSERT(!id2_rc);
    HEXDUMP_DEBUG(usr->aeskey_hex, dst_len);

    sec_debug("%10s: %s", "iotId", iot_id);

    /* remove */
    /*sec_debug("%10s: %s", "iotToken", iot_token);
    sec_debug("%10s: %s", "Host", host);
    sec_debug("%10s: %d", "Port", *pport);
    sec_debug("%10s: %s", "AES Key", usr->aeskey_str);*/

    ret = 0;

do_exit:
    if (iotx_payload) {
        LITE_free(iotx_payload);
        iotx_payload = NULL;
    }
    if (pvalue) {
        LITE_free(pvalue);
        pvalue = NULL;
    }
    if (b64_decode) {
        LITE_free(b64_decode);
        b64_decode = NULL;
    }
    if (id2_decrypt) {
        LITE_free(id2_decrypt);
        id2_decrypt = NULL;
    }

    return ret;
}
#endif  /* MQTT_DIRECT */

extern char* guider_get_domain();
int iotx_guider_id2_authenticate(void)
{
    char                partner_id[PID_STRLEN_MAX + 16] = {0};
    char                module_id[MID_STRLEN_MAX + 16] = {0};
    char                guider_url[GUIDER_URL_LEN] = {0};
    SECURE_MODE         secure_mode = MODE_TLS_GUIDER;
    char                guider_sign[GUIDER_SIGN_LEN] = {0};
    char                time_stamp_str[GUIDER_TS_LEN] = {0};
    char               *id2 = NULL;
    char               *device_code = NULL;

    iotx_device_info_pt dev  = iotx_device_info_get();
    iotx_conn_info_pt   conn = iotx_conn_info_get();
    char               *req_str = NULL;
    int                 gw = 0;
	int                 ext = 0;

    LITE_ASSERT(dev);
    LITE_ASSERT(conn);

    id2_guider_get_timestamp_str(time_stamp_str, sizeof(time_stamp_str));
    _ident_partner(partner_id, sizeof(partner_id));
    _ident_module(module_id, sizeof(module_id));
    id2_guider_get_url(guider_url, sizeof(guider_url));
    secure_mode = id2_guider_get_secure_mode();

    /* get ID2 Signature, deviceCode/ID2 fetched meanwhile */
    _calc_id2_signature(guider_sign,
                        sizeof(guider_sign),
                        time_stamp_str,
                        &id2,
                        &device_code);

    guider_print_dev_guider_info(dev, partner_id, module_id, guider_url, secure_mode,
                                 time_stamp_str, guider_sign,
                                 id2, device_code);

#ifndef MQTT_DIRECT
    char            iotx_conn_host[HOST_ADDRESS_LEN + 1] = {0};
    uint16_t        iotx_conn_port = 1883;
    char            iotx_id[GUIDER_IOT_ID_LEN + 1] = {0};
    char            iotx_token[GUIDER_IOT_TOKEN_LEN + 1] = {0};

    req_str = id2_guider_set_auth_req_str(guider_sign, time_stamp_str,
                                          id2
#ifdef MQTT_ID2_CRYPTO
                                          , device_code
#endif
                                         );

    LITE_ASSERT(req_str);
    sec_debug("req_str = '%s'", req_str);

    if (0 != id2_guider_get_iotId_iotToken(guider_url,
                                           req_str,
                                           iotx_id,
                                           iotx_token,
                                           iotx_conn_host,
                                           &iotx_conn_port)) {
        if (req_str) {
            HAL_Free(req_str);
        }

        sec_err("_iotId_iotToken_http() failed");
        LITE_free(id2);
        LITE_free(device_code);
        return -1;
    }
#endif

    /* Start Filling Connection Information */
    conn->pub_key = iotx_ca_get();

#ifdef MQTT_DIRECT

#if defined(TEST_ID2_DAILY)
    conn->port = 1885;
    _fill_conn_string(conn->host_name, sizeof(conn->host_name),
                      "%s",
                      "10.125.0.27");
#elif defined(TEST_ID2_PRE)
    conn->port = 80;
    _fill_conn_string(conn->host_name, sizeof(conn->host_name),
                      "%s",
                      "106.15.166.168");
#else
    conn->port = 1883;
    _fill_conn_string(conn->host_name, sizeof(conn->host_name),
                      "%s.%s",
                      dev->product_key,
                      "itls.cn-shanghai.aliyuncs.com");
#endif

    _fill_conn_string(conn->username, sizeof(conn->username),
#if defined(IOTX_WITHOUT_TLS) && defined(IOTX_WITHOUT_ITLS)
                      "%s&%s&%s",
                      id2,
                      device_code,
#else
                      "%s&%s",
#if !defined(IOTX_TEST)
                      id2,
#else
                      dev->device_name,
#endif
#endif
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

#ifdef RRPC_NEW
    ext = 1;
#endif

    _fill_conn_string(conn->client_id, sizeof(conn->client_id),
                      "%s"
                      "|securemode=%d"
                      ",timestamp=%s,signmethod=" SHA_METHOD ",gw=%d, ext=%d"
                      "%s"
#ifndef IOTX_WITHOUT_ITLS
					  ",authtype=id2"
#endif
					  "|"
                      , dev->device_id
                      , secure_mode
                      , time_stamp_str
					  , gw
					  , ext
                      , partner_id);

    guider_print_conn_info(conn);

    if (req_str) {
        HAL_Free(req_str);
    }

    LITE_free(id2);
    LITE_free(device_code);

    return 0;
}
#endif

