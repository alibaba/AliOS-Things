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

#include <aos/kernel.h>
#include <cli.h>
#include <aos/yloop.h>
#include <network/network.h>
#include "http_config.h"
#include <http.h>
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
    HTTPAPP_POST,
    HTTPAPP_DELETE,
    HTTPAPP_PUT,
    HTTPAPP_INVALID,
};

/* @brief http app status */
bool httpapp_running = false;
/* @brief http app command */
int command = HTTPAPP_INVALID;
/* @brief http connection settings */
httpc_connection_t settings;
/* @brief ip got finished flag */
static bool _ip_got_finished = false;

/* @brief http app handle */
httpc_handle_t httpapp_handle = 0;
#if CONFIG_HTTP_SECURE
/* @brief auth server name */
char auth_server_name[CONFIG_HTTPC_SERVER_NAME_SIZE] = "https://iot-auth.cn-shanghai.aliyuncs.com/";
/* @brief times of sending auth request */
uint32_t auth_req_times = 0;
/* @brief times of reuqest failure      */
uint32_t auth_req_fail_times = 0;
/* @brief times of receiving auth response */
uint32_t auth_rsp_times = 0;
#endif
/* @brief ota server name */
static char ota_server_name[CONFIG_HTTPC_SERVER_NAME_SIZE] = "http://mjfile-test.smartmidea.net:80";
/* @brief oss server name */
static char oss_server_name[CONFIG_HTTPC_SERVER_NAME_SIZE] = "http://aliosthings.oss-cn-hangzhou.aliyuncs.com/";
/* @brief times of sending ota request */
uint32_t ota_req_times = 0;
/* @brief times of ota request failure */
uint32_t ota_req_fail_times = 0;
/* @brief times of receiving ota response */
uint32_t ota_rsp_times = 0;
/* @brief ota header found flag */
bool ota_header_found = false;
/* @brief ota file size */
int32_t ota_file_size = 0;
/* @brief ota receive data size */
int32_t ota_rx_size = 0;

/* @brief times of sending ota head request */
uint32_t ota_head_req_times = 0;
/* @brief times of ota head request failure */
uint32_t ota_head_req_fail_times = 0;
/* @brief times of receiving ota head response */
uint32_t ota_head_rsp_times = 0;

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

/* @brief http response buffer */
#define RSP_BUF_SIZE 2048
uint8_t rsp_buf[RSP_BUF_SIZE];

#define PRODUCT_KEY             "a1cXH4Sgvdu"
#define PRODUCT_SECRET          "7PCG4aRnzdetZaIH"
#define DEVICE_NAME             "alios_net_test_1_1"
#define DEVICE_SECRET           "UgMue4eixCAKyGnhMjde51Bbs07c3tdW"

#if CONFIG_HTTP_SECURE
static int8_t hb2hex(uint8_t hb)
{
    hb = hb & 0xf;
    return (int8_t)(hb < 10 ? '0' + hb : hb - 10 + 'a');
}

#define KEY_IOPAD_SIZE 64
#define SHA256_DIGEST_SIZE 32
static void hmac_sha256(char *sign, const char *msg, int msg_len,
                        const char *key, int key_len)
{
    mbedtls_sha256_context context;
    unsigned char k_ipad[KEY_IOPAD_SIZE];
    unsigned char k_opad[KEY_IOPAD_SIZE];
    unsigned char out[SHA256_DIGEST_SIZE];
    int32_t index;

    memset(k_ipad, 0, sizeof(k_ipad));
    memset(k_opad, 0, sizeof(k_opad));
    strncpy(k_ipad, key, key_len);
    strncpy(k_opad, key, key_len);

    for (index = 0; index < KEY_IOPAD_SIZE; index++) {
        k_ipad[index] ^= 0x36;
        k_opad[index] ^= 0x5c;
    }

    mbedtls_sha256_init(&context);
    mbedtls_sha256_starts(&context, 0);
    mbedtls_sha256_update(&context, k_ipad, KEY_IOPAD_SIZE);
    mbedtls_sha256_update(&context, (unsigned char *)msg, msg_len);
    mbedtls_sha256_finish(&context, out);

    mbedtls_sha256_init(&context);
    mbedtls_sha256_starts(&context, 0);
    mbedtls_sha256_update(&context, k_opad, KEY_IOPAD_SIZE);
    mbedtls_sha256_update(&context, (unsigned char *)msg, msg_len);
    mbedtls_sha256_finish(&context, out);

    for (index = 0; index < SHA256_DIGEST_SIZE; ++index) {
        sign[index * 2] = hb2hex(out[index] >> 4);
        sign[index * 2 + 1] = hb2hex(out[index]);
    }
}

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
#define HTTP_AUTH_HDR_SIZE 128
#define HTTP_AUTH_DATA_SIZE 512
#define HTTP_AUTH_SIGN_SIZE 66
#define HTTP_AUTH_TS_SIZE 16
static int32_t httpapp_auth(const char *device_id, const char *product_key,
                       const char *device_name, const char *device_secret)
{
    int32_t ret;
    char hdr[HTTP_AUTH_HDR_SIZE] = {0};
    char data[HTTP_AUTH_DATA_SIZE] = {0};
    char sign[HTTP_AUTH_SIGN_SIZE] = {0};
    char timestamp[HTTP_AUTH_TS_SIZE] = {"2524608000000"};
    http_rsp_info_t rsp_info;

    /* construct http header*/
    ret = httpc_construct_header(hdr, HTTP_AUTH_HDR_SIZE, "Accept",
                                "text/xml,text/javascript,text/html,application/json");
    if (ret < 0) {
        LOGE(TAG, "http construct header fail");
        return -1;
    }

    /* calculate signature */
    calc_sign(sign, device_id, product_key, device_name, device_secret, timestamp);
    ret = snprintf(data, HTTP_AUTH_DATA_SIZE,
                   "productKey=%s&" "deviceName=%s&" "signmethod=%s&" "sign=%s&"
                   "version=default&" "clientId=%s&" "timestamp=%s&" "resources=mqtt",
                   product_key, device_name, "hmacsha1", sign, device_id, timestamp);

    if (ret < 0) {
        LOGE(TAG, "http construct data payload fail");
        return -1;
    }

    /* sending the signature */
    ret = httpc_send_request(httpapp_handle, HTTP_POST, "/auth/devicename", hdr,
                       "application/x-www-form-urlencoded;charset=utf-8", data, ret);
    ++auth_req_times;
    if (ret != HTTP_SUCCESS) {
        ++auth_req_fail_times;
        goto exit;
    }

    memset(rsp_buf, 0, sizeof(rsp_buf));
    /* get the response */
    ret = httpc_recv_response(httpapp_handle, rsp_buf, RSP_BUF_SIZE, &rsp_info, 10000);
    if (ret < 0) {
        ++auth_req_fail_times;
    } else {
        LOGI(TAG, "http session %x, buf size %d bytes, recv %d bytes data",
            httpapp_handle, RSP_BUF_SIZE, rsp_info.rsp_len);
        if (rsp_info.rsp_len > 0) {
            LOGI(TAG, "%s", rsp_buf);
        }

        if (rsp_info.message_complete) {
            ++auth_rsp_times;
        }
    }

exit:
    LOGI(TAG, "auth_req_times %d, auth_rsp_times %d, auth_req_fail_times %d",
         auth_req_times, auth_rsp_times, auth_req_fail_times);
    close(settings.socket);
    httpc_deinit(httpapp_handle);
#if CONFIG_HTTP_CONTINUE_TEST > 0
    httpapp_running = true;
#endif
    httpapp_handle = 0;
    return ret;
}
#endif

/* @brief http ota example */
#define HTTP_OTA_HDR_SIZE 64
static int32_t httpapp_ota(const char *uri)
{
    char hdr[HTTP_OTA_HDR_SIZE] = {0};
    int ret;
    http_rsp_info_t rsp_info;
    char *content;

    if (uri == NULL) {
        return HTTP_EARG;
    }

    /* construct http header*/
    ret = httpc_construct_header(hdr, HTTP_OTA_HDR_SIZE, "Accept", "*/*");
    if (ret < 0) {
        LOGE(TAG, "http construct header fail");
        return ret;
    }

    /* sending the http header */
    ret = httpc_send_request(httpapp_handle, HTTP_GET, uri, hdr, NULL, NULL, 0);
    ++ota_req_times;
    if (ret != HTTP_SUCCESS) {
        ++ota_req_fail_times;
        goto exit;
    }

    /* receiving the payload in a loop */
    while (ota_file_size == 0 || ota_rx_size < ota_file_size) {
        memset(rsp_buf, 0, sizeof(rsp_buf));
        ret = httpc_recv_response(httpapp_handle, rsp_buf, RSP_BUF_SIZE, &rsp_info, 10000);
        if (ret < 0) {
            ++ota_req_fail_times;
            break;
        } else {
            if (rsp_info.body_present || rsp_info.message_complete) {
                LOGI(TAG, "http session %x, buf size %d bytes, recv %d bytes data",
                    httpapp_handle, RSP_BUF_SIZE, rsp_info.rsp_len);
                if (ota_header_found == false) {
                    if (ota_file_size == 0) {
                        content = strstr(rsp_buf, "Content-Length");
                        if (content) {
                            ret = sscanf(content, "%*[^ ]%d", &ota_file_size);
                            if (ret < 0) {
                                LOGE(TAG, "http session fail to get ota header");
                                ++ota_req_fail_times;
                                break;
                            }
                            ota_header_found = true;
                            LOGI(TAG, "ota file size %d", ota_file_size);
                        } else {
                            continue;
                        }
                    }
                    content = strstr(rsp_buf, "\r\n\r\n");
                    if (content) {
                        content += 4;
                        ota_rx_size = rsp_info.rsp_len - ((uint8_t *)content - rsp_buf);
                        LOGI(TAG, "ota (%d/%d)", ota_rx_size, ota_file_size);
                    }
                    continue;
                }
                ota_rx_size += rsp_info.rsp_len;
                LOGI(TAG, "ota (%d/%d)", ota_rx_size, ota_file_size);
            }
        }
    }

    if (ota_rx_size >= ota_file_size && rsp_info.message_complete) {
        ++ota_rsp_times;
    }

exit:
    close(settings.socket);
    httpc_deinit(httpapp_handle);
#if CONFIG_HTTP_CONTINUE_TEST > 0
    httpapp_running = true;
#endif
    httpapp_handle = 0;
    ota_header_found = false;
    ota_file_size = 0;
    ota_rx_size = 0;
    LOGI(TAG, "ota_req_times %d, ota_rsp_times %d, ota_req_fail_times %d",
         ota_req_times, ota_rsp_times, ota_req_fail_times);
    return ret;
}

/* @brief http ota head example */
static int32_t httpapp_ota_head(const char *uri)
{
    char hdr[HTTP_OTA_HDR_SIZE] = {0};
    int ret;
    http_rsp_info_t rsp_info;
    char *content;

    if (uri == NULL) {
        return HTTP_EARG;
    }
 
    /* construct http header */
    ret = httpc_construct_header(hdr, HTTP_OTA_HDR_SIZE, "Accept", "*/*");
    if (ret < 0) {
        LOGE(TAG, "http construct header fail");
        return ret;
    }

    /* sending http header */
    ret = httpc_send_request(httpapp_handle, HTTP_HEAD, uri, hdr, NULL, NULL, 0);
    ++ota_head_req_times;
    if (ret != HTTP_SUCCESS) {
        ++ota_head_req_fail_times;
        goto exit;
    }

    memset(rsp_buf, 0, sizeof(rsp_buf));
    /* receiving http response */
    ret = httpc_recv_response(httpapp_handle, rsp_buf, RSP_BUF_SIZE, &rsp_info, 300000);
    if (ret < 0) {
        ++ota_head_req_fail_times;
    } else {
        LOGI(TAG, "http session %x, buf size %d bytes, recv %d bytes data",
            httpapp_handle, RSP_BUF_SIZE, rsp_info.rsp_len);
        if (rsp_info.rsp_len > 0) {
            LOGI(TAG, "%s", rsp_buf);
        }

        if (rsp_info.headers_complete) {
            ++ota_head_rsp_times;
        }
    }

exit:
    LOGI(TAG, "ota_head_req_times %d, ota_head_rsp_times %d, ota_head_req_fail_times %d",
         ota_head_req_times, ota_head_rsp_times, ota_head_req_fail_times);
    close(settings.socket);
    httpc_deinit(httpapp_handle);
#if CONFIG_HTTP_CONTINUE_TEST > 0
    httpapp_running = true;
#endif
    httpapp_handle = 0;
    return ret;
}

/* http up example */
#define HTTP_UP_HDR_SIZE 64
static char bin_to_up[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
static char httpapp_url[256];
static char httpapp_data[256];
static uint32_t up_req_times = 0;
static uint32_t up_rsp_times = 0;
static uint32_t up_req_fail_times = 0;
static int32_t httpapp_up(char *uri)
{
    char hdr[HTTP_UP_HDR_SIZE] = { 0 };
    int32_t ret;
    http_rsp_info_t rsp_info;

    if (uri == NULL) {
        return HTTP_EARG;
    }

    /* construct http header */
    ret = httpc_construct_header(hdr, HTTP_UP_HDR_SIZE, "Accept",
                                 "text/xml,text/javascript,text/html,application/json");
    if (ret < 0) {
        LOGE(TAG, "http construct header fail");
        return ret;
    }

    /* sending http request */
    ret = httpc_send_request(httpapp_handle, HTTP_PUT, uri, hdr, "", bin_to_up, sizeof(bin_to_up));
    ++up_req_times;
    if (ret != HTTP_SUCCESS) {
        ++up_req_fail_times;
        goto exit;
    }

    memset(rsp_buf, 0, sizeof(rsp_buf));
    /* receiving http response */
    ret = httpc_recv_response(httpapp_handle, rsp_buf, RSP_BUF_SIZE, &rsp_info, 10000);
    if (ret < 0) {
        ++up_req_fail_times;
    } else {
        LOGI(TAG, "http session %x, buf size %d bytes, recv %d bytes data",
            httpapp_handle, RSP_BUF_SIZE, rsp_info.rsp_len);
        if (rsp_info.rsp_len > 0) {
            LOGI(TAG, "%s", rsp_buf);
        }

        if (rsp_info.message_complete) {
            ++up_rsp_times;
        }
    }

exit:
    close(settings.socket);
    httpc_deinit(httpapp_handle);
#if CONFIG_HTTP_CONTINUE_TEST > 0
    httpapp_running = true;
#endif
    httpapp_handle = 0;

    LOGI(TAG, "up_req_times %d, up_rsp_times %d, up_req_fail_times %d",
        up_req_times, up_rsp_times, up_req_fail_times);
    return ret;
}

/* @brief http get example */
#define BUF_SIZE 1024
static void httpapp_get(char* url)
{
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    char *buf = NULL;
    int ret;

    buf = malloc(BUF_SIZE);
    if (buf == NULL) {
        LOGE(TAG, "Malloc failed.");
        return -1;
    }
#if CONFIG_HTTP_SECURE
    client.client_cert = ca_cert;
    client.client_cert_len = strlen(ca_cert);
#endif
    memset(buf, 0, BUF_SIZE);
    client_data.response_buf = buf;
    client_data.response_buf_len = BUF_SIZE;
    ret = httpc_get(&client, url, &client_data);
    if( ret >= 0 ) {
        LOGI(TAG, "Data received: %s", client_data.response_buf);
    }
    return ret;

}

/* @brief http post example */
static void httpapp_post(char* url, char* post_data)
{
    char *header = "deviceKey:FZoo0S07CpwUHcrt\r\n";
    char *content_type = "text/csv";
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    char *buf = NULL;
    int ret;

    buf = malloc(BUF_SIZE);
    if (buf == NULL) {
        LOGE(TAG, "Malloc failed.\r\n");
        return -1;
    }
    memset(buf, 0, BUF_SIZE);
    client_data.response_buf = buf;
    client_data.response_buf_len = BUF_SIZE;
    httpc_set_custom_header(&client, header);
    client_data.post_buf = post_data;
    client_data.post_buf_len = strlen(post_data);
    client_data.post_content_type = content_type;
    ret = httpc_post(&client, url, &client_data);
    if( ret >= 0 ) {
        LOGI(TAG, "Data received: %s", client_data.response_buf);
    }
    return ret;
}

/* @brief http put example */
static void httpapp_put(char* url, char* put_data)
{
    char *content_type = "text/csv";
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    char *buf = NULL;
    int ret = 0;

    buf = malloc(BUF_SIZE);
    if (buf == NULL) {
        LOGE(TAG, "Malloc failed.");
        return -1;
    }
    memset(buf, 0, BUF_SIZE);
    client_data.response_buf = buf;
    client_data.response_buf_len = BUF_SIZE;
    client_data.post_buf = put_data;
    client_data.post_buf_len = strlen(put_data);
    client_data.post_content_type = content_type;
    ret = httpc_put(&client, url, &client_data);
    if( ret >= 0 ) {
        LOGI(TAG, "Data received: %s", client_data.response_buf);
    }
    return ret;
}

/* @brief http delete example */
static void httpapp_delete(char* url)
{
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    char *buf = NULL;
    int ret;

    buf = malloc(BUF_SIZE);
    if (buf == NULL) {
        LOGE(TAG, "Malloc failed.");
        return -1;
    }
    memset(buf, 0, BUF_SIZE);
    client_data.response_buf = buf;
    client_data.response_buf_len = BUF_SIZE;
    ret = httpc_delete(&client, url, &client_data);
    if( ret >= 0 ) {
        LOGI(TAG, "Data received: %s", client_data.response_buf);
    }
    return ret;
}

#define REQ_BUF_SIZE 2048
uint8_t req_buf[REQ_BUF_SIZE];
static void httpapp_delayed_action(void *arg)
{
#if CONFIG_HTTP_SECURE
    char device_id[64];
#endif
    int fd;

    if (httpapp_running == false || _ip_got_finished == false) {
        goto exit;
    }

    if((command == HTTPAPP_OTA) || (command == HTTPAPP_OTA_HEAD) ||
       (command == HTTPAPP_DYNAMIC_UP)) {
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) {
            LOGE(TAG, "alloc socket fd fail");
            goto exit;
        }
        memset(&settings, 0, sizeof(settings));
        settings.socket = fd;
        //settings.keep_alive = true;
#if CONFIG_HTTP_SECURE
        if (command == HTTPAPP_AUTH) {
            settings.server_name = auth_server_name;
        } else
#endif
        if (command == HTTPAPP_OTA || command == HTTPAPP_OTA_HEAD) {
            settings.server_name = ota_server_name;
        } else if (command == HTTPAPP_DYNAMIC_UP) {
            settings.server_name = oss_server_name;
        } else {
            close(fd);
            goto exit;
        }
#if CONFIG_HTTP_SECURE
        settings.ca_cert = ca_cert;
#endif
        settings.req_buf = req_buf;
        settings.req_buf_size = REQ_BUF_SIZE;
        httpapp_handle = httpc_init(&settings);
        if (httpapp_handle == 0) {
            LOGE(TAG, "http session init fail");
            close(fd);
            goto exit;
        }
        LOGI(TAG, "http session %x", httpapp_handle);
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
            httpapp_ota("050509031881.bin");
            break;
        case HTTPAPP_OTA_HEAD:
            httpapp_ota_head("/050509031881.bin");
            break;
        case HTTPAPP_DYNAMIC_UP:
            httpapp_up(httpapp_url);
            break;
        case HTTPAPP_GET:
            httpapp_get(httpapp_url);
        case HTTPAPP_POST:
            httpapp_post(httpapp_url, httpapp_data);
        case HTTPAPP_DELETE:
            httpapp_delete(httpapp_url);
        case HTTPAPP_PUT:
            httpapp_put(httpapp_url, httpapp_data);
        default:
            break;
    }

exit:
    aos_post_delayed_action(5000, httpapp_delayed_action, (void *)(long)command);
}

/*@brief http app help command */
void httpapp_help_command()
{
    LOGI(TAG, "Usage: httpc" );
    LOGI(TAG, "       httpc  [-h]" );
    LOGI(TAG, "       -a, [auth] Http auth request" );
    LOGI(TAG, "       -o, [ota] Ota download request" );
    LOGI(TAG, "       -e, [ota_head] head Ota download http head request" );
    LOGI(TAG, "       -u, [up] Dynamic ota request" );
    LOGI(TAG, "       -s, [stop] Stop httpapp running" );
    LOGI(TAG, "       -g, [get] Get http request" );
    LOGI(TAG, "       -p, [post] Post http request" );
    LOGI(TAG, "       -q, [put] Put http request" );
    LOGI(TAG, "       -d, [delete] Delete http request" );
    LOGI(TAG, "Example:" );
    LOGI(TAG, "httpc -a" );
    LOGI(TAG, "httpc auth" );
    LOGI(TAG, "httpc -o" );
    LOGI(TAG, "httpc ota" );
    LOGI(TAG, "httpc -e" );
    LOGI(TAG, "httpc ota_head" );
    LOGI(TAG, "httpc -u www.aliyun.com" );
    LOGI(TAG, "httpc up http://aliosthings.oss-cn-hangzhou.aliyuncs.com/ota3.bin" );
    LOGI(TAG, "httpc -s" );
    LOGI(TAG, "httpc stop" );
    LOGI(TAG, "httpc -g http://www.aliyun.com/" );
    LOGI(TAG, "httpc get http://www.aliyun.com/" );
    LOGI(TAG, "httpc -p http://www.aliyun.com/ 123456" );
    LOGI(TAG, "httpc post http://www.aliyun.com/ 123456" );
    LOGI(TAG, "httpc -q http://www.aliyun.com/ 123456" );
    LOGI(TAG, "httpc put http://www.aliyun.com/ 123456" );
    LOGI(TAG, "httpc -d http://www.aliyun.com/" );
    LOGI(TAG, "httpc delete http://www.aliyun.com/" );
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
            if (argc > 2) {
                memset(httpapp_url, 0, sizeof(httpapp_url));
                strncpy(httpapp_url, argv[2], sizeof(httpapp_url) - 1);
                command = HTTPAPP_DYNAMIC_UP;
                httpapp_running = true;
            } else {
                LOGW(TAG, "miss url address");
            }
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
        } else if ((strncmp(type, "-p", strlen(type)) == 0) ||
                   (strncmp(type, "post", strlen("post")) == 0)) {
            if (argc > 3) {
                memset(httpapp_url, 0, sizeof(httpapp_url));
                strncpy(httpapp_url, argv[2], sizeof(httpapp_url) - 1);
                memset(httpapp_data, 0, sizeof(httpapp_data));
                strncpy(httpapp_data, argv[3], sizeof(httpapp_data) - 1);
                command = HTTPAPP_POST;
                httpapp_running = true;
            } else {
                LOGW(TAG, "miss url address or payload");
            }
        } else if ((strncmp(type, "-q", strlen(type)) == 0) ||
                   (strncmp(type, "put", strlen("put")) == 0)) {
            if (argc > 3) {
                memset(httpapp_url, 0, sizeof(httpapp_url));
                strncpy(httpapp_url, argv[2], sizeof(httpapp_url) - 1);
                memset(httpapp_data, 0, sizeof(httpapp_data));
                strncpy(httpapp_data, argv[3], sizeof(httpapp_data) - 1);
                command = HTTPAPP_PUT;
                httpapp_running = true;
            } else {
                LOGW(TAG, "miss url address or payload");
            }
        } else if ((strncmp(type, "-d", strlen(type)) == 0) ||
                   (strncmp(type, "delete", strlen("delete")) == 0)) {
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
    .name = "httpc",
    .help = "httpc auth | ota | ota_head | up | stop | get | post | put | delete",
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
    http_client_initialize();
    aos_post_delayed_action(100, httpapp_delayed_action, NULL);
    aos_loop_run();
    return 0;
}
