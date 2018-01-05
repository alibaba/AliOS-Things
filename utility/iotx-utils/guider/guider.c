/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "guider_internal.h"

extern int httpclient_common(
            httpclient_t *client,
            const char *url,
            int port,
            const char *ca_crt,
            int method,
            uint32_t timeout_ms,
            httpclient_data_t *client_data);
extern uint64_t utils_get_epoch_time(char copy[], int len);

#define SHA_METHOD              "hmacsha1"
#define MD5_METHOD              "hmacmd5"

/* By default we use hmac-sha1 algorithm for hmac in PK/DN/DS case */
#define USING_SHA1_IN_HMAC      (1)

typedef enum _SECURE_MODE {
    MODE_TLS_GUIDER             = -1,
    MODE_TCP_GUIDER_PLAIN       = 0,
    MODE_TCP_GUIDER_ID2_ENCRYPT = 1,
    MODE_TLS_DIRECT             = 2,
    MODE_TCP_DIRECT_PLAIN       = 3,
    MODE_TCP_DIRECT_ID2_ENCRYPT = 4,
    MODE_TLS_GUIDER_ID2_ENCRYPT = 5,
} SECURE_MODE;

const char *secmode_str[] = {
    "TCP + Guider + Plain",
    "TCP + Guider + ID2-Crypto",
    "TLS + Direct",
    "TCP + Direct + Plain",
    "TCP + Direct + ID2-Crypto",
    "TLS + Guider + ID2-Authenticate",
    ""
};

#ifdef MQTT_ID2_AUTH
static uint64_t _timestamp_num(char *str)
{
    char        backup;
    uint64_t    cnv = 0;
    uint64_t    res = 0;

    backup = str[10];
    str[10] = '\0';
    cnv = atoi(str);
    res = cnv * 1000;
    str[10] = backup;
    cnv = atoi(str + 10);
    res += cnv;

    return res;
}

static bool _is_non_symbol(char c)
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
        log_err("_url_encode: input param error!");
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
            snprintf(encoded, 4, "%%%02x", c);
            *output++ = encoded[0];
            *output++ = encoded[1];
            *output++ = encoded[2];
            input++;
        }
    }

    *output++ = 0;

    return 0;
}

static int _calc_id2_signature(
            char *id2_sigbuf,
            const int sig_buflen,
            char *timestamp_str,
            char **id2_str,
            char **device_code_str
)
{
    int                     rc = -1;
    uint8_t                 id2[TFS_ID2_LEN + 1] = {0};
    uint32_t                id2_len = TFS_ID2_LEN + 1;
    uint8_t                 dev_code[GUIDER_DEVCODE_LEN] = {0};
    uint32_t                dev_code_len = 0;
    uint32_t                sign_len = 0;
    char                    url_encode_buf[GUIDER_URLENCODE_LEN] = {0};
    iotx_device_info_pt     dev = NULL;
    char                   *digest_str = NULL;
    uint64_t                ts_val = 0;

    dev = iotx_device_info_get();
    assert(dev);

    /* Get timestamp */
    ts_val = _timestamp_num(timestamp_str);
    log_debug("ts_val: %ld", ts_val);

    /* Get Device Code */
    rc = tfs_id2_get_auth_code(ts_val, dev_code, &dev_code_len);
    assert(!rc);
    *device_code_str = LITE_strdup((const char *)dev_code);
    log_debug("deviceCode[%d] = '%s'", dev_code_len, *device_code_str);

    /* Get ID2 */
    rc = tfs_get_ID2(id2, &id2_len);
    assert(rc >= 0);
    *id2_str = LITE_strdup((const char *)id2);
    log_debug("id2[%d] = '%s'", id2_len, *id2_str);

    /* Get Signature */
    digest_str = LITE_format_string("clientId%s" "id2%s" "timestamp%s",
                                    dev->device_id,
                                    *id2_str,
                                    timestamp_str);
    log_debug("digest_str = %s", digest_str);
    rc = tfs_id2_get_digest_auth_code(ts_val,
                                      (uint8_t *)digest_str,
                                      strlen(digest_str),
                                      (uint8_t *)id2_sigbuf,
                                      &sign_len);
    assert(!rc);
    assert(sign_len <= sig_buflen);
    LITE_free(digest_str);
    log_debug("id2_sigbuf[%d] = '%s'", sign_len, id2_sigbuf);

    /* Get URL-Encoded Device Code */
    _url_encode((const char *)*device_code_str, url_encode_buf);
    LITE_free(*device_code_str);
    *device_code_str = LITE_strdup(url_encode_buf);
    log_debug("Encoded deviceCode[%d] = '%s'", (int)strlen(*device_code_str), *device_code_str);

    /* Get URL-Encoded Signature */
    _url_encode((const char *)id2_sigbuf, url_encode_buf);
    memset(id2_sigbuf, 0, sig_buflen);
    strncpy(id2_sigbuf, url_encode_buf, sig_buflen);
    log_debug("Encoded id2_sigbuf[%d] = '%s'", (int)strlen(id2_sigbuf), id2_sigbuf);

    return 0;
}
#endif  /* #ifdef MQTT_ID2_AUTH */

#ifndef MQTT_ID2_AUTH
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
    assert(dev);

    memset(signature, 0, sizeof(signature));
    memset(hmac_source, 0, sizeof(hmac_source));
    rc = snprintf(hmac_source,
                  sizeof(hmac_source),
                  "clientId%s" "deviceName%s" "productKey%s" "timestamp%s",
                  dev->device_id,
                  dev->device_name,
                  dev->product_key,
                  timestamp_str);
    assert(rc < sizeof(hmac_source));
    log_debug("| source: %s (%d)", hmac_source, (int)strlen(hmac_source));
    log_debug("| secret: %s (%d)", dev->device_secret, (int)strlen(dev->device_secret));

#if USING_SHA1_IN_HMAC
    log_debug("| method: %s", SHA_METHOD);
    utils_hmac_sha1(hmac_source, strlen(hmac_source),
                    signature,
                    dev->device_secret,
                    strlen(dev->device_secret));
#else
    log_debug("| method: %s", MD5_METHOD);
    utils_hmac_md5(hmac_source, strlen(hmac_source),
                   signature,
                   dev->device_secret,
                   strlen(dev->device_secret));
#endif

    log_debug("| signature: %s (%d)", signature, (int)strlen(signature));

    memcpy(hmac_sigbuf, signature, hmac_buflen);
    return 0;
}
#endif  /* #ifndef MQTT_ID2_AUTH */

#ifndef MQTT_DIRECT
static int _http_response(char *payload,
                          const int payload_len,
                          const char *request_string,
                          const char *url,
                          const int port_num,
                          httpclient_t *phttpc,
                          const char *pkey
                         )
{
#define HTTP_POST_MAX_LEN   (1024)
#define HTTP_RESP_MAX_LEN   (1024)

    int                     len = 0;
    int                     ret = -1;
    char                   *requ_payload = NULL;
    char                   *resp_payload = NULL;

    httpclient_data_t       httpc_data;

    memset(phttpc, 0, sizeof(httpclient_t));
    memset(&httpc_data, 0, sizeof(httpclient_data_t));

    phttpc->header = "Accept: text/xml,text/javascript,text/html,application/json\r\n";

    requ_payload = (char *)LITE_malloc(HTTP_POST_MAX_LEN);
    if (NULL == requ_payload) {
        log_err("Allocate HTTP request buf failed!");
        return ERROR_MALLOC;
    }
    memset(requ_payload, 0, HTTP_POST_MAX_LEN);

    len = snprintf(requ_payload,
                   HTTP_POST_MAX_LEN,
                   "%s",
                   request_string);
    assert(len < HTTP_POST_MAX_LEN);
    log_debug("requ_payload: \r\n\r\n%s\r\n", requ_payload);

    resp_payload = (char *)LITE_malloc(HTTP_RESP_MAX_LEN);
    if (!resp_payload) {
        goto RETURN;
    }
    assert(resp_payload);
    memset(resp_payload, 0, HTTP_RESP_MAX_LEN);

    httpc_data.post_content_type = "application/x-www-form-urlencoded;charset=utf-8";
    httpc_data.post_buf = requ_payload;
    httpc_data.post_buf_len = strlen(requ_payload);
    httpc_data.response_buf = resp_payload;
    httpc_data.response_buf_len = HTTP_RESP_MAX_LEN;

    ret = httpclient_common(phttpc,
                            url,
                            port_num,
                            pkey,
                            HTTPCLIENT_POST,
                            10000,
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

static int _iotId_iotToken_http(
            const char *guider_addr,
            const char *request_string,
            char *iot_id,
            char *iot_token,
            char *host,
            httpclient_t *phttpc,
            uint16_t *pport)
{
    char                iotx_payload[512] = {0};
    int                 iotx_port = 443;
    int                 ret = -1;
    iotx_conn_info_pt   usr = iotx_conn_info_get();
    int                 ret_code = 0;
#if defined(MQTT_ID2_AUTH)
    uint8_t            *b64_decode = NULL;
    uint8_t            *id2_decrypt = NULL;
    int                 id2_rc = -1;
    uint32_t            src_len = 0;
    uint32_t            dst_len = 0;
    uint32_t            dec_len = 0;
    int                 cipher_data = 0;
#endif
    const char         *pvalue;
    char                port_str[6];

    assert(usr);

#ifdef IOTX_WITHOUT_TLS
    iotx_port = 80;
#endif

#if defined(MQTT_ID2_AUTH) && defined(TEST_ID2_DAILY)
    iotx_port = 80;
#endif

#if defined(TEST_OTA_PRE)
    iotx_port = 80;
#endif

#if defined(TEST_OTA_DAILY)
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
                   phttpc,
#if defined(MQTT_ID2_AUTH) && defined(TEST_ID2_DAILY)
                   NULL
#elif defined(TEST_OTA_PRE)
                   NULL
#elif defined(TEST_OTA_DAILY)
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

#if defined(MQTT_ID2_AUTH)
    pvalue = LITE_json_value_of("data.iotId", iotx_payload);
    if (NULL == pvalue) {
        cipher_data = 1;
        log_debug("'data.iotId' NOT found, cipher_data = %d", cipher_data);
    } else {
        LITE_free(pvalue);
        pvalue = NULL;
        cipher_data = 0;
        log_debug("'data.iotId' already found, cipher_data = %d", cipher_data);
    }

    if (cipher_data) {
        pvalue = LITE_json_value_of("data", iotx_payload);
        if (NULL == pvalue) {
            goto do_exit;
        }
        src_len = (uint32_t)strlen(pvalue);
        b64_decode = LITE_malloc(src_len);
        if (!b64_decode) {
            log_err("malloc memory for b64_decode.");
            goto do_exit;
        }
        id2_rc = utils_base64decode((const uint8_t *)pvalue,
                                    src_len,
                                    src_len,
                                    b64_decode,
                                    &dst_len);
        log_debug("rc = utils_base64decode() = %d, %u Bytes => %u Bytes", id2_rc, src_len, dst_len);
        if (!id2_rc) {
            goto do_exit;
        }
        LITE_free(pvalue);
        pvalue = NULL;

        id2_decrypt = LITE_malloc(dst_len);
        if (!id2_decrypt) {
            log_err("malloc memory for id2_decrypt error");
            goto do_exit;
        }

        id2_rc = tfs_id2_decrypt((const uint8_t *)b64_decode,
                                 dst_len,
                                 id2_decrypt,
                                 &dec_len);
        log_debug("rc = tfs_id2_decrypt() = %d, %u Bytes => %u Bytes", id2_rc, dst_len, dec_len);
        if (id2_rc != 0) {
            log_err("decrypt cipher text with ID2 key error!");
            goto do_exit;
        }

        log_debug("id2_decrypt = %s", id2_decrypt);

        sprintf(iotx_payload, "{\"data\":");
        strcat(iotx_payload, (const char *)id2_decrypt);
        strcat(iotx_payload, "}");
        log_debug("iotx_payload = %s", iotx_payload);
    }
#endif

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

#ifdef MQTT_ID2_AUTH
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
    log_debug("rc = utils_base64decode() = %d, %u Bytes => %u Bytes", id2_rc, src_len, dst_len);
    assert(!id2_rc);
    HEXDUMP_DEBUG(usr->aeskey_hex, dst_len);

#endif

    log_debug("%10s: %s", "iotId", iot_id);
    log_debug("%10s: %s", "iotToken", iot_token);
    log_debug("%10s: %s", "Host", host);
    log_debug("%10s: %d", "Port", *pport);
#ifdef MQTT_ID2_AUTH
    log_debug("%10s: %s", "AES Key", usr->aeskey_str);
#endif

    ret = 0;

do_exit:
    if (pvalue) {
        LITE_free(pvalue);
        pvalue = NULL;
    }
#if defined(MQTT_ID2_AUTH)
    if (b64_decode) {
        LITE_free(b64_decode);
        b64_decode = NULL;
    }
    if (id2_decrypt) {
        LITE_free(id2_decrypt);
        id2_decrypt = NULL;
    }
#endif

    return ret;
}
#endif  /* #ifndef MQTT_DIRECT */

static void _timestamp_string(char *buf, int len)
{
#ifdef MQTT_ID2_AUTH
    utils_get_epoch_time(buf, len);
#else
    snprintf(buf, len, "%s", GUIDER_DEFAULT_TS_STR);
#endif
    return;
}

static SECURE_MODE _secure_mode_num(void)
{
    SECURE_MODE rc = MODE_TLS_GUIDER;

#ifdef MQTT_DIRECT

    #ifdef IOTX_WITHOUT_TLS
    rc = MODE_TCP_DIRECT_PLAIN;
    #else
    rc = MODE_TLS_DIRECT;
    #endif

#else   // MQTT_DIRECT

    #ifdef IOTX_WITHOUT_TLS
    rc = MODE_TCP_GUIDER_PLAIN;
    #else

        #ifdef MQTT_ID2_AUTH
            #ifdef MQTT_ID2_CRYPTO
            rc = MODE_TCP_GUIDER_ID2_ENCRYPT;
            #else
            rc = -1;
            #endif
        #else   // MQTT_ID2_AUTH
        rc = MODE_TLS_GUIDER;
        #endif  // MQTT_ID2_AUTH
    #endif  // IOTX_WITHOUT_TLS

#endif  // MQTT_DIRECT

    return  rc;
}

static void _secure_mode_str(char *buf, int len)
{
    memset(buf, 0, len);
    snprintf(buf, len, "securemode=%d", _secure_mode_num());
    return;
}

static void _ident_partner(char *buf, int len)
{
    char                tmp[GUIDER_PID_LEN] = {0};

    memset(tmp, 0, sizeof(tmp));
    HAL_GetPartnerID(tmp);
    if (strlen(tmp)) {
        snprintf(buf, len, ",partner_id=%s", tmp);
    } else {
        strcpy(buf, "");
    }

    return;
}

static void _authenticate_http_url(char *buf, int len)
{
#ifdef MQTT_DIRECT
    snprintf(buf, len, "%s", "");
#else

    snprintf(buf, len, "%s", "http://");

#if defined(MQTT_ID2_AUTH) && defined(TEST_ID2_DAILY)
    strcat(buf, "iot-auth.alibaba.net");
#elif defined(TEST_OTA_PRE)
    strcat(buf, "iot-auth-pre.cn-shanghai.aliyuncs.com");
#elif defined(TEST_OTA_DAILY)
    strcat(buf, "iot-auth.alibaba.net");
#else
    strcat(buf, "iot-auth.cn-shanghai.aliyuncs.com");
#endif

#ifdef MQTT_ID2_AUTH
    strcat(buf, "/auth/id2");
#else
    strcat(buf, "/auth/devicename");
#endif

#endif  // MQTT_DIRECT

    return;
}

#ifndef MQTT_DIRECT
static char *_authenticate_string(char sign[], char ts[]
#ifdef MQTT_ID2_AUTH
    , char id2[]
    #ifdef IOTX_WITHOUT_TLS
        , char dev_code[]
    #endif
#endif
                                 )
{
    char                   buffer[1024];
    char                   *ret = NULL;
    iotx_device_info_pt     dev = NULL;
    int                     rc = -1;

    dev = iotx_device_info_get();
    assert(dev);

#ifdef MQTT_ID2_AUTH
    rc = sprintf(buffer,
                  "id2=%s&" "sign=%s&"
#ifdef IOTX_WITHOUT_TLS
                  "deviceCode=%s&"
#endif
                  "timestamp=%s&" "version=default&" "clientId=%s&" "resources=mqtt,codec",
                  id2, sign,
#ifdef IOTX_WITHOUT_TLS
                  dev_code,
#endif
                  ts, dev->device_id);
#else
    rc = sprintf(buffer,
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
#endif
    assert(rc < 1024);
    ret = (char *)malloc(rc + 1);
    assert(ret != NULL);
    memcpy(ret, buffer, rc);
    ret[rc] = 0;

    return ret;
}
#endif  // MQTT_DIRECT

static int _fill_conn_string(char *dst, int len, const char *fmt, ...)
{
    int                     rc = -1;
    va_list                 ap;
    char                   *ptr = NULL;

    va_start(ap, fmt);
    rc = vsnprintf(dst, len, fmt, ap);
    va_end(ap);
    assert(rc <= len);

    ptr = strstr(dst, "||");
    if (ptr) {
        *ptr = '\0';
    }

    // log_debug("dst(%d) = %s", rc, dst);
    return 0;
}

/*
    struct {
        char            host_name[HOST_ADDRESS_LEN + 1];
        uint16_t        port;
        char            user_name[USER_NAME_LEN + 1];
        char            password[PASSWORD_LEN + 1];
        char            client_id[CLIENT_ID_LEN + 1];
        const char *    pubKey;
    }
*/
int iotx_guider_authenticate(void)
{
    char                guider_pid_buf[GUIDER_PID_LEN + 16] = {0};
    char                guider_url[GUIDER_URL_LEN] = {0};
    SECURE_MODE         guider_secmode_num = MODE_TCP_GUIDER_PLAIN;
    char                guider_secmode_str[CONN_SECMODE_LEN] = {0};
    char                guider_sign[GUIDER_SIGN_LEN] = {0};
    char                guider_timestamp_str[GUIDER_TS_LEN] = {0};
#ifdef MQTT_ID2_AUTH
    char               *guider_id2 = NULL;
    char               *guider_device_code = NULL;
#endif

    iotx_device_info_pt dev = iotx_device_info_get();
    iotx_conn_info_pt   usr = iotx_conn_info_get();
    char               *req_str = NULL;
    httpclient_t       httpc;

    assert(dev);
    assert(usr);

    _secure_mode_str(guider_secmode_str, sizeof(guider_secmode_str));

    _timestamp_string(guider_timestamp_str, sizeof(guider_timestamp_str));
    _ident_partner(guider_pid_buf, sizeof(guider_pid_buf));
    _authenticate_http_url(guider_url, sizeof(guider_url));
    guider_secmode_num = _secure_mode_num();

#ifdef MQTT_ID2_AUTH
    // get ID2 Signature, deviceCode/ID2 fetched meanwhile
    _calc_id2_signature(guider_sign,
                        sizeof(guider_sign),
                        guider_timestamp_str,
                        &guider_id2,
                        &guider_device_code);
#else
    _calc_hmac_signature(guider_sign, sizeof(guider_sign),
                         guider_timestamp_str);
#endif

    log_debug("%s", "....................................................");
    log_debug("%20s : %-s", "ProductKey", dev->product_key);
    log_debug("%20s : %-s", "DeviceName", dev->device_name);
    log_debug("%20s : %-s", "DeviceID", dev->device_id);
    log_debug("%20s : %-s", "DeviceSecret", dev->device_secret);
    log_debug("%s", "....................................................");
    log_debug("%20s : %-s", "PartnerID Buf", guider_pid_buf);
    log_debug("%20s : %s", "Guider URL", guider_url);
    if (guider_secmode_num > 0) {
        log_debug("%20s : %d (%s)", "Guider SecMode", guider_secmode_num, secmode_str[guider_secmode_num]);
    }
    log_debug("%20s : %s", "Guider Timestamp", guider_timestamp_str);
    log_debug("%s", "....................................................");
    log_debug("%20s : %s", "Guider Sign", guider_sign);
#ifdef MQTT_ID2_AUTH
    log_debug("%20s : %s", "Guider ID2", guider_id2);
    log_debug("%20s : %s", "Guider DeviceCode", guider_device_code);
#endif
    log_debug("%s", "....................................................");

#ifndef MQTT_DIRECT
    char            iotx_conn_host[HOST_ADDRESS_LEN + 1] = {0};
    uint16_t        iotx_conn_port = 1883;
    char            iotx_id[GUIDER_IOT_ID_LEN + 1] = {0};
    char            iotx_token[GUIDER_IOT_TOKEN_LEN + 1] = {0};

#ifdef MQTT_ID2_AUTH
    req_str = _authenticate_string(guider_sign, guider_timestamp_str,
                                   guider_id2
#ifdef IOTX_WITHOUT_TLS
                                   , guider_device_code
#endif
                                  );
#else
    req_str = _authenticate_string(guider_sign, guider_timestamp_str);
#endif
    assert(req_str);
    log_debug("req_str = '%s'", req_str);

    if (0 != _iotId_iotToken_http(guider_url,
                                  req_str,
                                  iotx_id,
                                  iotx_token,
                                  iotx_conn_host,
                                  &httpc,
                                  &iotx_conn_port)) {
        if (req_str) {
            free(req_str);
        }

        log_err("_iotId_iotToken_http() failed");
        httpclient_close(&httpc);
#ifdef MQTT_ID2_AUTH
        LITE_free(guider_id2);
        LITE_free(guider_device_code);
#endif
        return -1;
    }
#endif

    /* Start Filling Connection Information */
    usr->pub_key = iotx_ca_get();
#ifdef MQTT_DIRECT

    usr->port = 1883;
    _fill_conn_string(usr->host_name, sizeof(usr->host_name),
                      "%s.%s",
                      dev->product_key,
                      GUIDER_DIRECT_DOMAIN);
    _fill_conn_string(usr->username, sizeof(usr->username), "%s&%s",
                      dev->device_name,
                      dev->product_key);
    _fill_conn_string(usr->password, sizeof(usr->password),
                      "%s",
                      guider_sign);

#else   /* MQTT_DIRECT */

    usr->port = iotx_conn_port;
    _fill_conn_string(usr->host_name, sizeof(usr->host_name),
                      "%s",
                      iotx_conn_host);
    _fill_conn_string(usr->username, sizeof(usr->username), "%s", iotx_id);
    _fill_conn_string(usr->password, sizeof(usr->password), "%s", iotx_token);

#endif  /* MQTT_DIRECT */

#if 1
    _fill_conn_string(usr->client_id, sizeof(usr->client_id),
                      "%s"
                      "|%s"
#if USING_SHA1_IN_HMAC
                      ",timestamp=%s,signmethod=" SHA_METHOD ",gw=0"
#else
                      ",timestamp=%s,signmethod=" MD5_METHOD ",gw=0"
#endif
                      "%s|"
                      , dev->device_id
                      , guider_secmode_str
                      , guider_timestamp_str
                      , guider_pid_buf);
#else
    _fill_conn_string(usr->client_id, sizeof(usr->client_id),
                      "%s"
                      , dev->device_id
                     );
#endif

    log_debug("%s", "-----------------------------------------");
    log_debug("%16s : %-s", "Host", usr->host_name);
    log_debug("%16s : %d",  "Port", usr->port);
    log_debug("%16s : %-s", "UserName", usr->username);
    log_debug("%16s : %-s", "PassWord", usr->password);
    log_debug("%16s : %-s", "ClientID", usr->client_id);
    if (usr->pub_key) {
        log_debug("%16s : %p ('%.16s ...')", "TLS PubKey", usr->pub_key, usr->pub_key);
    }
#ifdef MQTT_ID2_AUTH
    char            tmp_str[40] = {0};

    LITE_hexbuf_convert(usr->aeskey_hex, tmp_str, 128 / 8, 1);
    log_debug("%16s : %-s", "AES Key", tmp_str);
#endif
    log_debug("%s", "-----------------------------------------");

    if (req_str) {
        free(req_str);
    }

#ifdef MQTT_ID2_AUTH
    LITE_free(guider_id2);
    LITE_free(guider_device_code);
#endif

    return 0;
}
