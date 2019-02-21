/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "iotx_system_internal.h"

#define SYS_GUIDER_MALLOC(size) LITE_malloc(size, MEM_MAGIC, "sys.guider")
#define SYS_GUIDER_FREE(ptr)    LITE_free(ptr)

#ifndef CONFIG_GUIDER_AUTH_TIMEOUT
    #define CONFIG_GUIDER_AUTH_TIMEOUT  (10 * 1000)
#endif

#ifndef CONFIG_GUIDER_DUMP_SECRET
    #define CONFIG_GUIDER_DUMP_SECRET   (0)
#endif

#define CUSTOME_DOMAIN_LEN_MAX          (60)

const char *secmode_str[] = {
    "TCP + Guider + Plain",
    "TCP + Guider + ID2-Crypto",
    "TLS + Direct",
    "TCP + Direct + Plain",
    "TCP + Direct + ID2-Crypto",
    "TLS + Guider + ID2-Authenticate",
    "",
    "TLS + Direct + ID2-Crypto",
    "ITLS + Direct + ID2-Authenticate"
};

#ifdef MQTT_DIRECT
const char *domain_mqtt_direct[] = {
    "iot-as-mqtt.cn-shanghai.aliyuncs.com",         /* Shanghai */
    "iot-as-mqtt.ap-southeast-1.aliyuncs.com",      /* Singapore */
    "iot-as-mqtt.ap-northeast-1.aliyuncs.com",      /* Japan */
    "iot-as-mqtt.us-west-1.aliyuncs.com",           /* America */
    "iot-as-mqtt.eu-central-1.aliyuncs.com"         /* Germany */
};
#endif

const char *domain_http_auth[] = {
    "iot-auth.cn-shanghai.aliyuncs.com",            /* Shanghai */
    "iot-auth.ap-southeast-1.aliyuncs.com",         /* Singapore */
    "iot-auth.ap-northeast-1.aliyuncs.com",         /* Japan */
    "iot-auth.us-west-1.aliyuncs.com",              /* America */
    "iot-auth.eu-central-1.aliyuncs.com",           /* Germany */
};

#ifdef ON_PRE
const char *domain_http_auth_pre[] = {
    "iot-auth-pre.cn-shanghai.aliyuncs.com",        /* Shanghai */
    "100.67.85.121",                                /* Singapore: iot-auth-pre.ap-southeast-1.aliyuncs.com */
    "iot-auth-pre.ap-northeast-1.aliyuncs.com",     /* Japan */
    "iot-auth-pre.us-west-1.aliyuncs.com",          /* America */
    "iot-auth-pre.eu-central-1.aliyuncs.com",       /* Germany */
};
#endif

/* ITLS direct domain */
#define GUIDER_DIRECT_DOMAIN_ITLS       "itls.cn-shanghai.aliyuncs.com"
#define GUIDER_DIRECT_DOMAIN_ITLS_DAILY  "10.125.7.82"
#define GUIDER_DIRECT_DOMAIN_ITLS_PRE    "106.15.166.168"

#define GUIDER_DIRECT_DOMAIN_DAILY       "10.125.3.189"
#define GUIDER_DIRECT_DOMAIN_PRE         "100.67.80.75"
static int iotx_guider_region = 0;
char iotx_domain_custom[GUIDER_DOMAIN_MAX][CUSTOME_DOMAIN_LEN_MAX] = {{0}};

int iotx_guider_set_region(int region_type)
{
    if (region_type >= GUIDER_REGION_MAX) {
        return FAIL_RETURN;
    }

    iotx_guider_region = region_type;
    return SUCCESS_RETURN;
}

int iotx_guider_get_region(void)
{
    return iotx_guider_region;
}

/* return domain of mqtt direct or http auth */
const char *iotx_guider_get_domain(int domain_type)
{
    if (iotx_guider_region == GUIDER_REGION_CUSTOM) {
        return iotx_domain_custom[domain_type];
    }

    if (domain_type == GUIDER_DOMAIN_MQTT) {
#ifdef MQTT_DIRECT
        return (char *)domain_mqtt_direct[iotx_guider_region];
#else
        return NULL;
#endif /* MQTT_DIRECT */
    } else if (domain_type == GUIDER_DOMAIN_HTTP) {
#if defined(ON_PRE)
        return (char *)domain_http_auth_pre[iotx_guider_region];
#elif defined(ON_DAILY)
        return "iot-auth.alibaba.net";
#else
        return (char *)domain_http_auth[iotx_guider_region];
#endif
    } else {
        return NULL;
    }
}

int iotx_guider_set_custom_domain(int domain_type, const char *domain)
{
    if ((domain_type >= GUIDER_DOMAIN_MAX) || (domain == NULL)) {
        return FAIL_RETURN;
    }

    int len = strlen(domain);
    if (len >= CUSTOME_DOMAIN_LEN_MAX) {
        return FAIL_RETURN;
    }

    memset(iotx_domain_custom[domain_type], 0, CUSTOME_DOMAIN_LEN_MAX);
    memcpy(iotx_domain_custom[domain_type], domain, len);

    return SUCCESS_RETURN;
}

/* sign used hmacSha256 default */
static int _calc_hmac_signature(
            char *hmac_sigbuf,
            const int hmac_buflen,
            const char *timestamp_str)
{
    char                    signature[65];
    char                    hmac_source[512];
    int                     rc = -1;
    iotx_device_info_t     dev;

    rc = iotx_device_info_get(&dev);
    if (rc < 0) {
        sys_err("get device info err");
        return rc;
    }

    memset(signature, 0, sizeof(signature));
    memset(hmac_source, 0, sizeof(hmac_source));
    rc = HAL_Snprintf(hmac_source,
                      sizeof(hmac_source),
                      "clientId%s" "deviceName%s" "productKey%s" "timestamp%s",
                      dev.device_id,
                      dev.device_name,
                      dev.product_key,
                      timestamp_str);
    LITE_ASSERT(rc < sizeof(hmac_source));

    utils_hmac_sha256(hmac_source, strlen(hmac_source),
                      signature,
                      dev.device_secret,
                      strlen(dev.device_secret));

    memcpy(hmac_sigbuf, signature, sizeof(signature));

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

    requ_payload = (char *)SYS_GUIDER_MALLOC(HTTP_POST_MAX_LEN);
    if (NULL == requ_payload) {
        sys_err("Allocate HTTP request buf failed!");
        return ERROR_MALLOC;
    }
    memset(requ_payload, 0, HTTP_POST_MAX_LEN);

    len = HAL_Snprintf(requ_payload,
                       HTTP_POST_MAX_LEN,
                       "%s",
                       request_string);
    LITE_ASSERT(len < HTTP_POST_MAX_LEN);
    sys_debug("requ_payload: \r\n\r\n%s\r\n", requ_payload);

    resp_payload = (char *)SYS_GUIDER_MALLOC(HTTP_RESP_MAX_LEN);
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
    sys_debug("PAYLOAD: %s", payload);

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

    /* sys_debug("dst(%d) = %s", rc, dst); */
    return 0;
}

void guider_print_conn_info(iotx_conn_info_t *conn)
{
    LITE_ASSERT(conn);
    sys_info("%s", "-----------------------------------------");
    sys_info("%16s : %-s", "Host", conn->host_name);
    sys_info("%16s : %d",  "Port", conn->port);
#if CONFIG_GUIDER_DUMP_SECRET
    sys_info("%16s : %-s", "UserName", conn->username);
    sys_info("%16s : %-s", "PassWord", conn->password);
#endif
    sys_info("%16s : %-s", "ClientID", conn->client_id);
    if (conn->pub_key) {
        sys_info("%16s : %p ('%.16s ...')", "TLS PubKey", conn->pub_key, conn->pub_key);
    }

    sys_info("%s", "-----------------------------------------");
}

void guider_print_dev_guider_info(iotx_device_info_t *dev,
                                  char *partner_id,
                                  char *module_id,
                                  char *guider_url,
                                  int secure_mode,
                                  char *time_stamp,
                                  char *guider_sign)
{
    sys_info("%s", "....................................................");
    sys_info("%20s : %-s", "ProductKey", dev->product_key);
    sys_info("%20s : %-s", "DeviceName", dev->device_name);
    sys_info("%20s : %-s", "DeviceID", dev->device_id);
#if CONFIG_GUIDER_DUMP_SECRET
    sys_info("%20s : %-s", "DeviceSecret", dev->device_secret);
#endif
    sys_info("%s", "....................................................");
    sys_info("%20s : %-s", "PartnerID Buf", partner_id);
    sys_info("%20s : %-s", "ModuleID Buf", module_id);
    sys_info("%20s : %s", "Guider URL", guider_url);
    if (secure_mode > 0) {
        sys_info("%20s : %d (%s)", "Guider SecMode", secure_mode, secmode_str[secure_mode]);
    }
    sys_info("%20s : %s", "Guider Timestamp", time_stamp);
#if CONFIG_GUIDER_DUMP_SECRET
    sys_info("%20s : %s", "Guider Sign", guider_sign);
#endif
    sys_info("%s", "....................................................");

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

#ifdef SUPPORT_ITLS
    rc = MODE_ITLS_DNS_ID2;
#else
#ifdef SUPPORT_TLS
    rc = MODE_TLS_DIRECT;
#else
    rc = MODE_TCP_DIRECT_PLAIN;
#endif
#endif /* SUPPORT_ITLS */

#else

#ifdef SUPPORT_TLS
    rc = MODE_TLS_GUIDER;
#else
    rc = MODE_TCP_GUIDER_PLAIN;
#endif  /* SUPPORT_TLS */

#endif  /* MQTT_DIRECT */

    return  rc;
}

#ifndef MQTT_DIRECT
static char *guider_set_auth_req_str(char sign[], char ts[])
{
#define AUTH_STRING_MAXLEN  (1024)

    char                   *ret = NULL;
    int                     rc = -1;
    iotx_device_info_t      dev;

    rc = iotx_device_info_get(&dev);
    if (rc < 0) {
        sys_err("get device info err");
        return NULL;
    }

    ret = SYS_GUIDER_MALLOC(AUTH_STRING_MAXLEN);
    LITE_ASSERT(ret);
    memset(ret, 0, AUTH_STRING_MAXLEN);

    rc = sprintf(ret,
                 "productKey=%s&" "deviceName=%s&" "signmethod=%s&" "sign=%s&"
                 "version=default&" "clientId=%s&" "timestamp=%s&" "resources=mqtt"
                 , dev.product_key
                 , dev.device_name
                 , SHA_METHOD
                 , sign
                 , dev.device_id
                 , ts
                );
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
#define PAYLOAD_STRING_MAXLEN  (1024)
    char               *iotx_payload = NULL;
    int                 iotx_port = 443;
    int                 ret = -1;
    int                 ret_code = 0;
    const char         *pvalue;
    char                port_str[6];

#ifndef SUPPORT_TLS
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
    iotx_payload = SYS_GUIDER_MALLOC(PAYLOAD_STRING_MAXLEN);
    LITE_ASSERT(iotx_payload);
    memset(iotx_payload, 0, PAYLOAD_STRING_MAXLEN);
    _http_response(iotx_payload,
                   PAYLOAD_STRING_MAXLEN,
                   request_string,
                   guider_addr,
                   iotx_port,
#if defined(TEST_OTA_PRE)
                   NULL
#else
                   iotx_ca_get()
#endif
                  );
#if defined(WITH_FAC_JSON_FLOW)
    sys_info("Downstream Payload:");
    iotx_facility_json_print(iotx_payload, LOG_INFO_LEVEL, '<');
#endif

    pvalue = LITE_json_value_of("code", iotx_payload, MEM_MAGIC, "sys.preauth");
    if (!pvalue) {
        goto do_exit;
    }

    ret_code = atoi(pvalue);
    LITE_free(pvalue);
    pvalue = NULL;

    if (200 != ret_code) {
        sys_err("++++");
        sys_err("ret_code = %d (!= 200), abort!", ret_code);
        sys_err("++++");
        goto do_exit;
    }

    pvalue = LITE_json_value_of("data.iotId", iotx_payload, MEM_MAGIC, "sys.preauth");
    if (NULL == pvalue) {
        goto do_exit;
    }
    strcpy(iot_id, pvalue);
    LITE_free(pvalue);
    pvalue = NULL;

    pvalue = LITE_json_value_of("data.iotToken", iotx_payload, MEM_MAGIC, "sys.preauth");
    if (NULL == pvalue) {
        goto do_exit;
    }
    strcpy(iot_token, pvalue);
    LITE_free(pvalue);
    pvalue = NULL;

    pvalue = LITE_json_value_of("data.resources.mqtt.host", iotx_payload, MEM_MAGIC, "sys.preauth");
    if (NULL == pvalue) {
        goto do_exit;
    }
    strcpy(host, pvalue);
    LITE_free(pvalue);
    pvalue = NULL;

    pvalue = LITE_json_value_of("data.resources.mqtt.port", iotx_payload, MEM_MAGIC, "sys.preauth");
    if (NULL == pvalue) {
        goto do_exit;
    }
    strcpy(port_str, pvalue);
    LITE_free(pvalue);
    pvalue = NULL;
    *pport = atoi(port_str);

    /*sys_debug("%10s: %s", "iotId", iot_id);*/
    /*sys_debug("%10s: %s", "iotToken", iot_token);*/
    sys_debug("%10s: %s", "Host", host);
    sys_debug("%10s: %d", "Port", *pport);

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

    return ret;
}
#endif  /* MQTT_DIRECT */

int iotx_guider_authenticate(iotx_conn_info_t *conn)
{
    char                partner_id[PID_STRLEN_MAX + 16] = {0};
    char                module_id[MID_STRLEN_MAX + 16] = {0};
    char                guider_url[GUIDER_URL_LEN] = {0};
    SECURE_MODE         secure_mode = MODE_TLS_GUIDER;
    char                guider_sign[GUIDER_SIGN_LEN] = {0};
    char                timestamp_str[GUIDER_TS_LEN] = {0};

    char               *req_str = NULL;
    int                 gw = 0;
    int                 ext = 0;
    iotx_device_info_t      dev;
    int len;
    int rc;
    rc = iotx_device_info_get(&dev);
    if (rc < 0) {
        sys_err("get device info err");
        return rc;
    }

    LITE_ASSERT(conn);
    if (conn->init != 0) {
        sys_warning("conn info already init!");
        return 0;
    }

    _ident_partner(partner_id, sizeof(partner_id));
    _ident_module(module_id, sizeof(module_id));

    secure_mode = guider_get_secure_mode();
    guider_get_timestamp_str(timestamp_str, sizeof(timestamp_str));

#ifdef MQTT_DIRECT
    /* setup the hostname and port */
#if defined(SUPPORT_ITLS)
#if defined(ON_DAILY)
    conn->port = 1885;
    conn->host_name = SYS_GUIDER_MALLOC(sizeof(GUIDER_DIRECT_DOMAIN_ITLS_DAILY));
    if (conn->host_name == NULL) {
        goto failed;
    }
    _fill_conn_string(conn->host_name, sizeof(GUIDER_DIRECT_DOMAIN_ITLS_DAILY),
                      GUIDER_DIRECT_DOMAIN_ITLS_DAILY);
#elif defined(ON_PRE)
    conn->port = 1885;
    conn->host_name = SYS_GUIDER_MALLOC(sizeof(GUIDER_DIRECT_DOMAIN_ITLS_PRE));
    if (conn->host_name == NULL) {
        goto failed;
    }
    _fill_conn_string(conn->host_name, sizeof(GUIDER_DIRECT_DOMAIN_ITLS_PRE),
                      GUIDER_DIRECT_DOMAIN_ITLS_PRE);
#else
    conn->port = 1883;
    len = strlen(dev.product_key) + 1 + sizeof(GUIDER_DIRECT_DOMAIN_ITLS);
    conn->host_name = SYS_GUIDER_MALLOC(len);
    if (conn->host_name == NULL) {
        goto failed;
    }
    _fill_conn_string(conn->host_name, len,
                      "%s.%s",
                      dev.product_key,
                      GUIDER_DIRECT_DOMAIN_ITLS);
#endif  /* defined(ON_DAILY) */
#else   /* defined(SUPPORT_ITLS) */
#if defined (ON_DAILY)
    conn->port = 1883;
    conn->host_name = SYS_GUIDER_MALLOC(sizeof(GUIDER_DIRECT_DOMAIN_DAILY));
    if (conn->host_name == NULL) {
        goto failed;
    }
    _fill_conn_string(conn->host_name, sizeof(GUIDER_DIRECT_DOMAIN_DAILY),
                      GUIDER_DIRECT_DOMAIN_DAILY);
#elif defined (ON_PRE)
    conn->port = 80;

    conn->host_name = SYS_GUIDER_MALLOC(sizeof(GUIDER_DIRECT_DOMAIN_PRE));
    if (conn->host_name == NULL) {
        goto failed;
    }
    _fill_conn_string(conn->host_name, sizeof(GUIDER_DIRECT_DOMAIN_PRE),
                      GUIDER_DIRECT_DOMAIN_PRE);
#else
    conn->port = 1883;
    len = strlen(dev.product_key) + 2 + strlen(iotx_guider_get_domain(GUIDER_DOMAIN_MQTT));
    conn->host_name = SYS_GUIDER_MALLOC(len);
    if (conn->host_name == NULL) {
        goto failed;
    }
    _fill_conn_string(conn->host_name, len,
                      "%s.%s",
                      dev.product_key,
                      iotx_guider_get_domain(GUIDER_DOMAIN_MQTT));
#endif  /* defined(ON_DAILY) */
#endif  /* defined(SUPPORT_ITLS) */

    /* calculate the sign */
    _calc_hmac_signature(guider_sign, sizeof(guider_sign), timestamp_str);

    guider_print_dev_guider_info(&dev, partner_id, module_id, guider_url, secure_mode,
                                 timestamp_str, guider_sign);

    len = strlen(dev.device_name) + strlen(dev.product_key) + 2;
    conn->username = SYS_GUIDER_MALLOC(len);
    if (conn->username == NULL) {
        goto failed;
    }

    /* fill up username and password */
    _fill_conn_string(conn->username, len,
                      "%s&%s",
                      dev.device_name,
                      dev.product_key);

    len = strlen(guider_sign) + 1;
    conn->password = SYS_GUIDER_MALLOC(len);
    if (conn->password == NULL) {
        goto failed;
    }
    _fill_conn_string(conn->password, len,
                      "%s",
                      guider_sign);
#else   /* MQTT_DIRECT */
    const char     *p_domain = NULL;
    char            iotx_conn_host[HOST_ADDRESS_LEN + 1] = {0};
    uint16_t        iotx_conn_port = 1883;
    char            iotx_id[GUIDER_IOT_ID_LEN + 1] = {0};
    char            iotx_token[GUIDER_IOT_TOKEN_LEN + 1] = {0};

    p_domain = iotx_guider_get_domain(GUIDER_DOMAIN_HTTP);
    HAL_Snprintf(guider_url, sizeof(guider_url), "http://%s/auth/devicename", p_domain);
    _calc_hmac_signature(guider_sign, sizeof(guider_sign), timestamp_str);

    guider_print_dev_guider_info(&dev, partner_id, module_id, guider_url, secure_mode,
                                 timestamp_str, guider_sign);

    req_str = guider_set_auth_req_str(guider_sign, timestamp_str);
    LITE_ASSERT(req_str);
    sys_debug("req_str = '%s'", req_str);

    /* http auth */
    if (0 != guider_get_iotId_iotToken(guider_url,
                                       req_str,
                                       iotx_id,
                                       iotx_token,
                                       iotx_conn_host,
                                       &iotx_conn_port)) {
        if (req_str) {
            LITE_free(req_str);
        }

        sys_err("_iotId_iotToken_http() failed");
        goto failed;
    }

    /* setup the hostname and port */
    conn->port = iotx_conn_port;
    len = strlen(iotx_conn_host) + 1;
    conn->host_name = SYS_GUIDER_MALLOC(len);
    if (conn->host_name == NULL) {
        goto failed;
    }
    _fill_conn_string(conn->host_name, len,
                      "%s",
                      iotx_conn_host);

    /* fill up username and password */
    len = strlen(iotx_id) + 1;
    conn->username = SYS_GUIDER_MALLOC(len);
    if (conn->username == NULL) {
        goto failed;
    }
    _fill_conn_string(conn->username, len, "%s", iotx_id);
    len = strlen(iotx_token) + 1;
    conn->password = SYS_GUIDER_MALLOC(len);
    if (conn->password == NULL) {
        goto failed;
    }
    _fill_conn_string(conn->password, len, "%s", iotx_token);
#endif /* MQTT_DIRECT */

    conn->pub_key = iotx_ca_get();

    len = CLIENT_ID_LEN;
    conn->client_id = SYS_GUIDER_MALLOC(len);
    if (conn->client_id == NULL) {
        goto failed;
    }
    _fill_conn_string(conn->client_id, len,
                      "%s"
                      "|securemode=%d"
                      ",timestamp=%s,signmethod=" SHA_METHOD ",gw=%d" ",ext=%d"
                      "%s"
                      "%s"
#ifdef SUPPORT_ITLS
                      ",authtype=id2"
#endif
#ifdef BUILD_AOS
                      ",a=aos-r-2.1.0"
#else
                      ",a=sdk-c-%s"
#endif
                      "|"
                      , dev.device_id
                      , secure_mode
                      , timestamp_str
                      , gw
                      , ext
                      , partner_id
                      , module_id
                      , LINKKIT_VERSION
                     );

    guider_print_conn_info(conn);

    if (req_str) {
        LITE_free(req_str);
    }
    conn->init = 1;
    return 0;
failed:
    if (conn->host_name != NULL) {
        LITE_free(conn->host_name);
        conn->host_name = NULL;
    }
    if (conn->username != NULL) {
        LITE_free(conn->username);
        conn->username = NULL;
    }
    if (conn->password != NULL) {
        LITE_free(conn->password);
        conn->password = NULL;
    }
    if (conn->client_id != NULL) {
        LITE_free(conn->client_id);
        conn->client_id = NULL;
    }
    return -1;

}

static iotx_conn_info_t     iotx_conn_info = {0, 0, NULL, NULL, NULL, NULL, NULL};

int iotx_device_info_get(iotx_device_info_t *device_info)
{
    if (device_info == NULL) {
        return -1;
    }
    memset(device_info, 0x0, sizeof(iotx_device_info_t));
    HAL_GetProductKey(device_info->product_key);
    HAL_GetDeviceName(device_info->device_name);
    HAL_GetDeviceSecret(device_info->device_secret);
    HAL_Snprintf(device_info->device_id, DEVICE_ID_LEN, "%s.%s", device_info->product_key, device_info->device_name);
    device_info->device_id[DEVICE_ID_LEN - 1] = '\0';
    return 0;
}

iotx_conn_info_pt iotx_conn_info_get(void)
{
    return &iotx_conn_info;
}

void iotx_conn_info_release(void)
{
    if (iotx_conn_info.host_name != NULL) {
        LITE_free(iotx_conn_info.host_name);
    }
    if (iotx_conn_info.username != NULL) {
        LITE_free(iotx_conn_info.username);
    }
    if (iotx_conn_info.password != NULL) {
        LITE_free(iotx_conn_info.password);
    }
    if (iotx_conn_info.client_id != NULL) {
        LITE_free(iotx_conn_info.client_id);
    }
    memset(&iotx_conn_info, 0, sizeof(iotx_conn_info));
}

iotx_conn_info_pt iotx_conn_info_reload(void)
{
    iotx_conn_info_release();
    if (iotx_guider_authenticate(&iotx_conn_info) < 0) {
        return NULL;
    }
    return &iotx_conn_info;
}
