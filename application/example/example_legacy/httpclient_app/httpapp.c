/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
/**
 * @file httpapp.c
 *
 * This file includes sample code of how to do http communication.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <aos/kernel.h>
#include <aos/cli.h>
#include <aos/yloop.h>
#include <httpclient.h>
#if CONFIG_HTTP_SECURE
#include "mbedtls/sha256.h"
#include "mbedtls/sha1.h"
#endif
#include "ulog/ulog.h"
#include "netmgr.h"

#define TAG "HTTPAPP"

#define CONFIG_HTTP_CONTINUE_TEST 0

/* @brief http app command */
enum httpapp_command{
    HTTPAPP_AUTH,
    HTTPAPP_OTA,
    HTTPAPP_OTA_HEAD,
    HTTPAPP_DYNAMIC_UP,
    HTTPAPP_GET,
    HTTPAPP_INVALID,
};

/* @brief http app status */
bool httpapp_running = false;
/* @brief http app command */
int command = HTTPAPP_INVALID;
/* @brief ip got finished flag */
static bool _ip_got_finished = false;


#if CONFIG_HTTP_SECURE
/* @brief auth server name */
static char *auth_server_name = "https://iot-auth.cn-shanghai.aliyuncs.com/auth/devicename";
#endif
/* @brief ota server name */
static char *ota_url = "http://mjfile-test.smartmidea.net:80/050509031881.bin";
/* @brief oss server name */
static char *oss_url = "http://aliosthings.oss-cn-hangzhou.aliyuncs.com/ota4.bin";

#if CONFIG_HTTP_SECURE
static const char *ca_cert = \
{
    \
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\r\n" \
    "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\r\n" \
    "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\r\n" \
    "MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\r\n" \
    "YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\r\n" \
    "aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\r\n" \
    "jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\r\n" \
    "xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\r\n" \
    "1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\r\n" \
    "snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\r\n" \
    "U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\r\n" \
    "9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\r\n" \
    "BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\r\n" \
    "AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\r\n" \
    "yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\r\n" \
    "38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\r\n" \
    "AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\r\n" \
    "DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\r\n" \
    "HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\r\n" \
    "-----END CERTIFICATE-----"
};
#endif

/* @brief http request buffer */
#define REQ_BUF_SIZE 2048
static char req_buf[REQ_BUF_SIZE];

/* @brief http response buffer */
#define RSP_BUF_SIZE 2048
static char rsp_buf[RSP_BUF_SIZE];

static char httpapp_url[256];

#define PRODUCT_KEY             "a1FxISeKbq9"
#define PRODUCT_SECRET          "ThNbP5iNUQ1lQe2Q"
#define DEVICE_NAME             "alen-activate-test"
#define DEVICE_SECRET           "jcumDL5AJRgU7zRNcCcnHRiQmtii0vDn"

#if CONFIG_HTTP_SECURE
static int8_t hb2hex(uint8_t hb)
{
    hb = hb & 0xf;
    return (int8_t)(hb < 10 ? '0' + hb : hb - 10 + 'a');
}

#define KEY_IOPAD_SIZE 64
#define SHA1_DIGEST_SIZE 20
static void hmac_sha1(char *sign, const char *msg, int msg_len,
                      const char *key, int key_len)
{
    mbedtls_sha1_context context;
    unsigned char k_ipad[KEY_IOPAD_SIZE];
    unsigned char k_opad[KEY_IOPAD_SIZE];
    unsigned char out[SHA1_DIGEST_SIZE];
    int index;

    memset(k_ipad, 0, sizeof(k_ipad));
    memset(k_opad, 0, sizeof(k_opad));
    memcpy(k_ipad, key, key_len);
    memcpy(k_opad, key, key_len);
    memset(out, 0, SHA1_DIGEST_SIZE);

    for (index = 0; index < KEY_IOPAD_SIZE; index++) {
        k_ipad[index] ^= 0x36;
        k_opad[index] ^= 0x5c;
    }

    mbedtls_sha1_init(&context);
    mbedtls_sha1_starts(&context);
    mbedtls_sha1_update(&context, k_ipad, KEY_IOPAD_SIZE);
    mbedtls_sha1_update(&context, (unsigned char *)msg, msg_len);
    mbedtls_sha1_finish(&context, out);

    mbedtls_sha1_init(&context);
    mbedtls_sha1_starts(&context);
    mbedtls_sha1_update(&context, k_opad, KEY_IOPAD_SIZE);
    mbedtls_sha1_update(&context, out, SHA1_DIGEST_SIZE);
    mbedtls_sha1_finish(&context, out);

    for (index = 0; index < SHA1_DIGEST_SIZE; ++index) {
        sign[index * 2] = hb2hex(out[index] >> 4);
        sign[index * 2 + 1] = hb2hex(out[index]);
    }
}

static int32_t calc_sign(char *sign, const char *device_id, const char *product_key,
                         const char *device_name, const char *device_secret,
                         const char *timestamp)
{
    char hmac_source[512] = {0};

    snprintf(hmac_source, sizeof(hmac_source),
             "clientId%s" "deviceName%s" "productKey%s" "timestamp%s",
             device_id, device_name, product_key, timestamp);

    hmac_sha1(sign, hmac_source, strlen(hmac_source),
              device_secret, strlen(device_secret));
    return HTTP_SUCCESS;
}

/* @brief http sending http auth example */
#define HTTP_AUTH_DATA_SIZE 512
#define HTTP_AUTH_SIGN_SIZE 66
#define HTTP_AUTH_TS_SIZE 16
static int32_t httpapp_auth(const char *device_id, const char *product_key,
                            const char *device_name, const char *device_secret)
{
    int32_t ret;
    char post_data[HTTP_AUTH_DATA_SIZE] = {0};
    char sign[HTTP_AUTH_SIGN_SIZE] = {0};
    char timestamp[HTTP_AUTH_TS_SIZE] = {"2524608000000"};
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    char * customer_header = "Accept: text/xml,text/javascript,text/html,application/json\r\n";
    char * content_type = "application/x-www-form-urlencoded;charset=utf-8";

    memset(req_buf, 0, sizeof(req_buf));
    client_data.header_buf = req_buf;
    client_data.header_buf_len = sizeof(req_buf);

    memset(rsp_buf, 0, sizeof(rsp_buf));
    client_data.response_buf = rsp_buf;
    client_data.response_buf_len = sizeof(rsp_buf);

    /* calculate signature */
    calc_sign(sign, device_id, product_key, device_name, device_secret, timestamp);
    ret = snprintf(post_data, HTTP_AUTH_DATA_SIZE,
                   "productKey=%s&" "deviceName=%s&" "signmethod=%s&" "sign=%s&"
                   "version=default&" "clientId=%s&" "timestamp=%s&" "resources=mqtt",
                   product_key, device_name, "hmacsha1", sign, device_id, timestamp);

    if (ret < 0) {
        LOGE(TAG, "http construct data payload fail");
        return -1;
    }

    /* set server cert and cert len */
    client.server_cert = ca_cert;
    client.server_cert_len = strlen(ca_cert) + 1;

    httpclient_set_custom_header(&client, customer_header);
    client_data.post_buf = post_data;
    client_data.post_buf_len = strlen(post_data);
    client_data.post_content_type = content_type;
    ret = httpclient_post(&client, auth_server_name, &client_data);
    if( ret >= 0 ) {
        LOGI(TAG, "Data received: %s", client_data.response_buf);
    } else {
        LOGE(TAG, "Auth failed");
    }

    return 0;
}
#endif

static int32_t httpapp_ota(char *url)
{
    httpclient_t client = { 0 };
    httpclient_data_t client_data = {0};
    int ret, total_len = 0;
    int recv_len = 0, recv_total_len = 0;
    char * customer_header = "Accept: */*\r\n";
    long long start = aos_now_ms();

    memset(req_buf, 0, sizeof(req_buf));
    client_data.header_buf = req_buf;
    client_data.header_buf_len = sizeof(req_buf);

    memset(rsp_buf, 0, sizeof(rsp_buf));
    client_data.response_buf = rsp_buf;
    client_data.response_buf_len = sizeof(rsp_buf);

    ret = httpclient_connect(&client, url);
    if(HTTP_SUCCESS != ret ) {
        LOGE(TAG, "http connect failed");
        return -1;
    }
    
    httpclient_set_custom_header(&client, customer_header);

    ret = httpclient_send_request(&client, url, HTTP_GET, &client_data);
    if(HTTP_SUCCESS != ret) {
        LOGE(TAG, "http send request failed");
        return -1;
    }

    while (total_len == 0 || recv_total_len < total_len) {
        ret = httpclient_recv_response(&client, &client_data);
        if (ret == HTTP_SUCCESS || ret == HTTP_EAGAIN) {
            recv_len = client_data.content_block_len;
            /*
             * Fetch data from client_data.response_buf
             * according to recv_len
             */
        } else {
            recv_len = 0;
        }

        if (ret < 0) {
            break;
        }
        
        /* first time */
        if (total_len <= 0) {
            int val_pos, val_len;
            if(0 == httpclient_get_response_header_value(client_data.header_buf, "Content-Length", (int *)&val_pos, (int *)&val_len)) {
                sscanf(client_data.header_buf + val_pos, "%d", &total_len);
            }
        }

        recv_total_len += recv_len;

        LOGI(TAG, "Total recv %d / %d \n", recv_total_len, total_len);
    }

    LOGI(TAG,"Finished. Duration %d ms\n", (int)(aos_now_ms() - start));

    httpclient_close(&client);
    return 0;
}

/* @brief http ota head example */
static int32_t httpapp_ota_head(const char *url)
{
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    int ret;
    char * customer_header = "Accept: */*\r\n";

    memset(req_buf, 0, sizeof(req_buf));
    client_data.header_buf = req_buf;
    client_data.header_buf_len = sizeof(req_buf);

    memset(rsp_buf, 0, sizeof(rsp_buf));
    client_data.response_buf = rsp_buf;
    client_data.response_buf_len = sizeof(rsp_buf);

    httpclient_set_custom_header(&client, customer_header);
    ret = httpclient_head(&client, url, &client_data);  
    if (ret == HTTP_ECLSD) {
        LOGI(TAG, "Response header:\n%s\n", client_data.header_buf);
    }
   
    return ret;
}

/* http up example */
static char bin_to_up[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
static int32_t httpapp_up(char *url)
{
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    int ret;
    char * customer_header = "Accept: text/xml,text/javascript,text/html,application/json\r\n";
    char * content_type = "application/octet-stream";

    memset(req_buf, 0, sizeof(req_buf));
    client_data.header_buf = req_buf;
    client_data.header_buf_len = sizeof(req_buf);

    memset(rsp_buf, 0, sizeof(rsp_buf));
    client_data.response_buf = rsp_buf;
    client_data.response_buf_len = sizeof(rsp_buf);

    httpclient_set_custom_header(&client, customer_header);

    client_data.post_buf = bin_to_up;
    client_data.post_buf_len = sizeof(bin_to_up);
    client_data.post_content_type = content_type;
    ret = httpclient_put(&client, url, &client_data);
    if( ret >= 0 ) {
        LOGI(TAG, "Data received: %s", client_data.response_buf);
    }

    return 0;
}

static int32_t httpapp_get(char* url)
{
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    int ret;
    char * customer_header = "Accept: */*\r\n";

    memset(req_buf, 0, sizeof(req_buf));
    client_data.header_buf = req_buf;
    client_data.header_buf_len = sizeof(req_buf);

    memset(rsp_buf, 0, sizeof(rsp_buf));
    client_data.response_buf = rsp_buf;
    client_data.response_buf_len = sizeof(rsp_buf);

    httpclient_set_custom_header(&client, customer_header);
    ret = httpclient_get(&client, url, &client_data);
    if( ret >= 0 ) {
        LOGI(TAG, "Data received: %s", client_data.response_buf);
    }
    return ret;
}
  
static void httpapp_delayed_action(void *arg)
{
#if CONFIG_HTTP_SECURE
    char device_id[64];
#endif

    if (httpapp_running == false || _ip_got_finished == false) {
        goto exit;
    }

    LOGI(TAG, "http command %d at %d", command, (uint32_t)aos_now_ms());

    httpapp_running = false;
    switch (command) {
        case HTTPAPP_AUTH:
#if CONFIG_HTTP_SECURE
            memset(device_id, 0, 64);
            snprintf(device_id, 64, "%s.%s", PRODUCT_KEY, DEVICE_NAME);
            httpapp_auth(device_id, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET);
#endif
            break;
        case HTTPAPP_OTA:
            httpapp_ota(ota_url);
            break;
        case HTTPAPP_OTA_HEAD:
            httpapp_ota_head(ota_url);
            break;
        case HTTPAPP_DYNAMIC_UP:
            httpapp_up(oss_url);
            break;
        case HTTPAPP_GET:
            httpapp_get(httpapp_url);
            break;
        default:
            break;
    }

exit:
    aos_post_delayed_action(5000, httpapp_delayed_action, (void *)(long)command);
}

/*@brief http app help command */
void httpapp_help_command()
{
    LOGI(TAG, "Usage: http" );
    LOGI(TAG, "       http  [-h]" );
    LOGI(TAG, "       -a, [auth] Http auth request" );
    LOGI(TAG, "       -o, [ota] Ota download request" );
    LOGI(TAG, "       -e, [ota_head] head Ota download http head request" );
    LOGI(TAG, "       -u, [up] Dynamic ota request" );
    LOGI(TAG, "       -h, [get] Http get request" );
    LOGI(TAG, "       -s, [stop] Stop httpapp running" );
    LOGI(TAG, "Example:" );
    LOGI(TAG, "http -a" );
    LOGI(TAG, "http auth" );
    LOGI(TAG, "http -o" );
    LOGI(TAG, "http ota" );
    LOGI(TAG, "http -e" );
    LOGI(TAG, "http ota_head" );
    LOGI(TAG, "http -u" );
    LOGI(TAG, "http up" );
    LOGI(TAG, "http -g  https://www.aliyun.com/" );
    LOGI(TAG, "http get https://www.aliyun.com/" );
    LOGI(TAG, "http -s" );
    LOGI(TAG, "http stop" );
}

/* @brief httpapp command handle */
static void httpapp_cmd_handle(char *buf, int blen, int argc, char **argv)
{
    const char *type = argc > 1? argv[1]: "";

    if ((strncmp(type, "-s", strlen("-s")) == 0) ||
       (strncmp(type, "stop", strlen("stop")) == 0)) {
        LOGI(TAG, "set httpapp exit" );
        httpapp_running = false;
        return;
    }

    if (httpapp_running == false) {
#if CONFIG_HTTP_SECURE
        if ((strncmp(type, "-a", strlen(type)) == 0) ||
            (strncmp(type, "auth", strlen("auth")) == 0)) {
            command = HTTPAPP_AUTH;
            httpapp_running = true;
        } else
#endif
        if ((strncmp(type, "-e", strlen(type)) == 0) ||
            (strncmp(type, "ota_head", strlen("ota_head")) == 0)) {
            command = HTTPAPP_OTA_HEAD;
            httpapp_running = true;
        }
        else if ((strncmp(type, "-o", strlen(type)) == 0) ||
            (strncmp(type, "ota", strlen("ota")) == 0)) {
            command = HTTPAPP_OTA;
            httpapp_running = true;
        } else if ((strncmp(type, "-u", strlen(type)) == 0) ||
            (strncmp(type, "up", strlen("up")) == 0)) {
                command = HTTPAPP_DYNAMIC_UP;
                httpapp_running = true;
        } else if ((strncmp(type, "-g", strlen(type)) == 0) ||
                   (strncmp(type, "get", strlen("get")) == 0)) {
            if (argc > 2) {
                memset(httpapp_url, 0, sizeof(httpapp_url));
                strncpy(httpapp_url, argv[2], sizeof(httpapp_url) - 1);
                command = HTTPAPP_GET;
                httpapp_running = true;
            } else {
                LOGW(TAG, "miss url address");
            }
        } else if (strncmp(type, "-h", strlen(type)) == 0) {
            httpapp_help_command();
        } else {
            LOGE(TAG, "unknown command, please use help command: httpc -h");
        }
    }
}

/* @brief http app command */
static struct cli_command httpapp_cmd = {
    .name = "http",
    .help = "http auth | ota | ota_head | up | get | stop",
    .function = httpapp_cmd_handle
};

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }
    
    _ip_got_finished = true;
}

int application_start(int argc, char *argv[])
{
    aos_set_log_level(AOS_LL_DEBUG);
    netmgr_init();
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    aos_cli_register_command(&httpapp_cmd);
    aos_post_delayed_action(100, httpapp_delayed_action, NULL);
    aos_loop_run();
    return 0;
}
