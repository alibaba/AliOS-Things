/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>

#include <aos/kernel.h>
#include <aos/cli.h>
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

#define CONFIG_HTTP_CONTINUE_TEST 0

enum {
    HTTP_AUTH,
    HTTP_OTA,
    HTTP_OTA_HEAD,
    HTTP_DYNAMIC_UP,
    HTTP_INVALID,
};

bool httpc_running = false;
int command = HTTP_INVALID;
httpc_connection_t settings;
static bool _ip_got_finished = false;

#if 1
#define PRODUCT_KEY             "a1cXH4Sgvdu"
#define PRODUCT_SECRET          "7PCG4aRnzdetZaIH"
#define DEVICE_NAME             "alios_net_test_1_1"
#define DEVICE_SECRET           "UgMue4eixCAKyGnhMjde51Bbs07c3tdW"
#else
#define PRODUCT_KEY             "a1MZxOdcBnO"
#define PRODUCT_SECRET          "h4I4dneEFp7EImTv"
#define DEVICE_NAME             "test_01"
#define DEVICE_SECRET           "t9GmMf2jb3LgWfXBaZD2r3aJrfVWBv56"
#endif

httpc_handle_t httpc_handle = 0;
#if CONFIG_HTTP_SECURE
char auth_server_name[CONFIG_HTTPC_SERVER_NAME_SIZE] = "https://iot-auth.cn-shanghai.aliyuncs.com/";
uint32_t auth_req_times = 0;
uint32_t auth_req_fail_times = 0;
uint32_t auth_rsp_times = 0;
#endif
static char ota_server_name[CONFIG_HTTPC_SERVER_NAME_SIZE] = "http://mjfile-test.smartmidea.net:80";
static char oss_server_name[CONFIG_HTTPC_SERVER_NAME_SIZE] = "http://aliosthings.oss-cn-hangzhou.aliyuncs.com/";
uint32_t ota_req_times = 0;
uint32_t ota_req_fail_times = 0;
uint32_t ota_rsp_times = 0;
bool ota_header_found = false;
int32_t ota_file_size = 0;
int32_t ota_rx_size = 0;

uint32_t ota_head_req_times = 0;
uint32_t ota_head_req_fail_times = 0;
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

#define RSP_BUF_SIZE 2048
uint8_t rsp_buf[RSP_BUF_SIZE];

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

#define HTTP_AUTH_HDR_SIZE 128
#define HTTP_AUTH_DATA_SIZE 512
#define HTTP_AUTH_SIGN_SIZE 66
#define HTTP_AUTH_TS_SIZE 16
static int32_t httpc_auth(const char *device_id, const char *product_key,
                       const char *device_name, const char *device_secret)
{
    int32_t ret;
    char hdr[HTTP_AUTH_HDR_SIZE] = {0};
    char data[HTTP_AUTH_DATA_SIZE] = {0};
    char sign[HTTP_AUTH_SIGN_SIZE] = {0};
    char timestamp[HTTP_AUTH_TS_SIZE] = {"2524608000000"};
    http_rsp_info_t rsp_info;

    ret = httpc_construct_header(hdr, HTTP_AUTH_HDR_SIZE, "Accept",
                                "text/xml,text/javascript,text/html,application/json");
    if (ret < 0) {
        LOG("http construct header fail\n");
        return -1;
    }

    calc_sign(sign, device_id, product_key, device_name, device_secret, timestamp);
    ret = snprintf(data, HTTP_AUTH_DATA_SIZE,
                   "productKey=%s&" "deviceName=%s&" "signmethod=%s&" "sign=%s&"
                   "version=default&" "clientId=%s&" "timestamp=%s&" "resources=mqtt",
                   product_key, device_name, "hmacsha1", sign, device_id, timestamp);

    if (ret < 0) {
        LOG("http construct data payload fail\n");
        return -1;
    }

    ret = httpc_send_request(httpc_handle, HTTP_POST, "/auth/devicename", hdr,
                       "application/x-www-form-urlencoded;charset=utf-8", data, ret);
    ++auth_req_times;
    if (ret != HTTP_SUCCESS) {
        ++auth_req_fail_times;
        goto exit;
    }

    memset(rsp_buf, 0, sizeof(rsp_buf));
    ret = httpc_recv_response(httpc_handle, rsp_buf, RSP_BUF_SIZE, &rsp_info, 10000);
    if (ret < 0) {
        ++auth_req_fail_times;
    } else {
        LOG("http session %x, buf size %d bytes, recv %d bytes data\n",
            httpc_handle, RSP_BUF_SIZE, rsp_info.rsp_len);
        if (rsp_info.rsp_len > 0) {
            LOG("%s", rsp_buf);
        }

        if (rsp_info.message_complete) {
            ++auth_rsp_times;
        }
    }

exit:
    LOG("auth_req_times %d, auth_rsp_times %d, auth_req_fail_times %d\r\n",
         auth_req_times, auth_rsp_times, auth_req_fail_times);
    close(settings.socket);
    httpc_deinit(httpc_handle);
#if CONFIG_HTTP_CONTINUE_TEST > 0
    httpc_running = true;
#endif
    httpc_handle = 0;
    return ret;
}
#endif

#define HTTP_OTA_HDR_SIZE 64
static int32_t httpc_ota(const char *uri)
{
    char hdr[HTTP_OTA_HDR_SIZE] = {0};
    int ret;
    http_rsp_info_t rsp_info;
    char *content;

    if (uri == NULL) {
        return HTTP_EARG;
    }

    ret = httpc_construct_header(hdr, HTTP_OTA_HDR_SIZE, "Accept", "*/*");
    if (ret < 0) {
        LOG("http construct header fail\n");
        return ret;
    }

    ret = httpc_send_request(httpc_handle, HTTP_GET, uri, hdr, NULL, NULL, 0);
    ++ota_req_times;
    if (ret != HTTP_SUCCESS) {
        ++ota_req_fail_times;
        goto exit;
    }

    while (ota_file_size == 0 || ota_rx_size < ota_file_size) {
        memset(rsp_buf, 0, sizeof(rsp_buf));
        ret = httpc_recv_response(httpc_handle, rsp_buf, RSP_BUF_SIZE, &rsp_info, 10000);
        if (ret < 0) {
            ++ota_req_fail_times;
            break;
        } else {
            if (rsp_info.body_present || rsp_info.message_complete) {
                LOG("http session %x, buf size %d bytes, recv %d bytes data\n",
                    httpc_handle, RSP_BUF_SIZE, rsp_info.rsp_len);
                if (ota_header_found == false) {
                    if (ota_file_size == 0) {
                        content = strstr(rsp_buf, "Content-Length");
                        if (content) {
                            ret = sscanf(content, "%*[^ ]%d", &ota_file_size);
                            if (ret < 0) {
                                LOG("http session fail to get ota header\r\n");
                                ++ota_req_fail_times;
                                break;
                            }
                            ota_header_found = true;
                            LOG("ota file size %d\r\n", ota_file_size);
                        } else {
                            continue;
                        }
                    }
                    content = strstr(rsp_buf, "\r\n\r\n");
                    if (content) {
                        content += 4;
                        ota_rx_size = rsp_info.rsp_len - ((uint8_t *)content - rsp_buf);
                        LOG("ota (%d/%d) \r\n", ota_rx_size, ota_file_size);
                    }
                    continue;
                }
                ota_rx_size += rsp_info.rsp_len;
                LOG("ota (%d/%d) \r\n", ota_rx_size, ota_file_size);
            }
        }
    }

    if (ota_rx_size >= ota_file_size && rsp_info.message_complete) {
        ++ota_rsp_times;
    }

exit:
    close(settings.socket);
    httpc_deinit(httpc_handle);
#if CONFIG_HTTP_CONTINUE_TEST > 0
    httpc_running = true;
#endif
    httpc_handle = 0;
    ota_header_found = false;
    ota_file_size = 0;
    ota_rx_size = 0;
    LOG("ota_req_times %d, ota_rsp_times %d, ota_req_fail_times %d\r\n",
         ota_req_times, ota_rsp_times, ota_req_fail_times);
    return ret;
}

static int32_t httpc_ota_head(const char *uri)
{
    char hdr[HTTP_OTA_HDR_SIZE] = {0};
    int ret;
    http_rsp_info_t rsp_info;
    char *content;

    if (uri == NULL) {
        return HTTP_EARG;
    }

    ret = httpc_construct_header(hdr, HTTP_OTA_HDR_SIZE, "Accept", "*/*");
    if (ret < 0) {
        LOG("http construct header fail\n");
        return ret;
    }

    ret = httpc_send_request(httpc_handle, HTTP_HEAD, uri, hdr, NULL, NULL, 0);
    ++ota_head_req_times;
    if (ret != HTTP_SUCCESS) {
        ++ota_head_req_fail_times;
        goto exit;
    }

    memset(rsp_buf, 0, sizeof(rsp_buf));
    ret = httpc_recv_response(httpc_handle, rsp_buf, RSP_BUF_SIZE, &rsp_info, 300000);
    if (ret < 0) {
        ++ota_head_req_fail_times;
    } else {
        LOG("http session %x, buf size %d bytes, recv %d bytes data\n",
            httpc_handle, RSP_BUF_SIZE, rsp_info.rsp_len);
        if (rsp_info.rsp_len > 0) {
            LOG("%s", rsp_buf);
        }

        if (rsp_info.headers_complete) {
            ++ota_head_rsp_times;
        }
    }

exit:
    LOG("ota_head_req_times %d, ota_head_rsp_times %d, ota_head_req_fail_times %d\r\n",
         ota_head_req_times, ota_head_rsp_times, ota_head_req_fail_times);
    close(settings.socket);
    httpc_deinit(httpc_handle);
#if CONFIG_HTTP_CONTINUE_TEST > 0
    httpc_running = true;
#endif
    httpc_handle = 0;
    return ret;
}

#define HTTP_UP_HDR_SIZE 64
static char bin_to_up[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
static char up_url[256];
static uint32_t up_req_times = 0;
static uint32_t up_rsp_times = 0;
static uint32_t up_req_fail_times = 0;

static int32_t httpc_up(char *uri)
{
    char hdr[HTTP_UP_HDR_SIZE] = { 0 };
    int32_t ret;
    http_rsp_info_t rsp_info;

    if (uri == NULL) {
        return HTTP_EARG;
    }

    ret = httpc_construct_header(hdr, HTTP_UP_HDR_SIZE, "Accept",
                                 "text/xml,text/javascript,text/html,application/json");
    if (ret < 0) {
        LOG("http construct header fail\n");
        return ret;
    }

    ret = httpc_send_request(httpc_handle, HTTP_PUT, uri, hdr, "", bin_to_up, sizeof(bin_to_up));
    ++up_req_times;
    if (ret != HTTP_SUCCESS) {
        ++up_req_fail_times;
        goto exit;
    }

    memset(rsp_buf, 0, sizeof(rsp_buf));
    ret = httpc_recv_response(httpc_handle, rsp_buf, RSP_BUF_SIZE, &rsp_info, 10000);
    if (ret < 0) {
        ++up_req_fail_times;
    } else {
        LOG("http session %x, buf size %d bytes, recv %d bytes data",
            httpc_handle, RSP_BUF_SIZE, rsp_info.rsp_len);
        if (rsp_info.rsp_len > 0) {
            LOG("%s", rsp_buf);
        }

        if (rsp_info.message_complete) {
            ++up_rsp_times;
        }
    }

exit:
    close(settings.socket);
    httpc_deinit(httpc_handle);
#if CONFIG_HTTP_CONTINUE_TEST > 0
    httpc_running = true;
#endif
    httpc_handle = 0;

    LOG("up_req_times %d, up_rsp_times %d, up_req_fail_times %d",
        up_req_times, up_rsp_times, up_req_fail_times);
    return ret;
}

#define REQ_BUF_SIZE 2048
uint8_t req_buf[REQ_BUF_SIZE];
static void httpc_delayed_action(void *arg)
{
#if CONFIG_HTTP_SECURE
    char device_id[64];
#endif
    int fd;

    if (httpc_running == false || _ip_got_finished == false) {
        goto exit;
    }

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        LOG("alloc socket fd fail\n");
        goto exit;
    }
    memset(&settings, 0, sizeof(settings));
    settings.socket = fd;
    //settings.keep_alive = true;
#if CONFIG_HTTP_SECURE
    if (command == HTTP_AUTH) {
        settings.server_name = auth_server_name;
    } else
#endif
    if (command == HTTP_OTA || command == HTTP_OTA_HEAD) {
        settings.server_name = ota_server_name;
    } else if (command == HTTP_DYNAMIC_UP) {
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
    httpc_handle = httpc_init(&settings);
    if (httpc_handle == 0) {
        LOG("http session init fail\n");
        close(fd);
        goto exit;
    }

    httpc_running = false;
    LOG("http session %x command %d at %d\n", httpc_handle, command, (uint32_t)aos_now_ms());
    switch (command) {
        case HTTP_AUTH:
#if CONFIG_HTTP_SECURE
            memset(device_id, 0, 64);
            snprintf(device_id, 64, "%s.%s", PRODUCT_KEY, DEVICE_NAME);
            httpc_auth(device_id, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET);
#endif
            break;
        case HTTP_OTA:
            httpc_ota("050509031881.bin");
            break;
        case HTTP_OTA_HEAD:
            httpc_ota_head("/050509031881.bin");
            break;
        case HTTP_DYNAMIC_UP:
            httpc_up(up_url);
            break;
        default:
            break;
    }

exit:
    aos_post_delayed_action(5000, httpc_delayed_action, (void *)(long)command);
}

static void httpc_cmd_handle(char *buf, int blen, int argc, char **argv)
{
    const char *type = argc > 1? argv[1]: "";

    if (strncmp(type, "stop", strlen("stop")) == 0) {
        httpc_running = false;
        return;
    }

    if (httpc_running == false) {
#if CONFIG_HTTP_SECURE
        if (strncmp(type, "auth", strlen(type)) == 0) {
            command = HTTP_AUTH;
            httpc_running = true;
        } else
#endif
        if (strncmp(type, "ota", strlen(type)) == 0) {
            command = HTTP_OTA;
            httpc_running = true;
        } else if (strncmp(type, "ota_head", strlen(type)) == 0) {
            command = HTTP_OTA_HEAD;
            httpc_running = true;
        } else if (strncmp(type, "up", strlen(type)) == 0) {
            if (argc > 2) {
                memset(up_url, 0, sizeof(up_url));
                strncpy(up_url, argv[2], sizeof(up_url) - 1);
                command = HTTP_DYNAMIC_UP;
                httpc_running = true;
            } else {
                LOG("miss url address\n");
            }
        } else {
            LOG("unknown command\n");
            return;
        }
    }
}

static struct cli_command httpc_cmd = {
    .name = "httpc",
    .help = "httpc auth | ota | ota_head | up | stop",
    .function = httpc_cmd_handle
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
    aos_cli_register_command(&httpc_cmd);
    http_client_initialize();
    aos_post_delayed_action(100, httpc_delayed_action, NULL);
    aos_loop_run();
    return 0;
}
