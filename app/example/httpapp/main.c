/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>

#include <aos/kernel.h>
#include <aos/cli.h>
#include <aos/yloop.h>
#include <network/network.h>
#include "http_api.h"
#if CONFIG_HTTP_SECURE
#include "mbedtls/sha256.h"
#include "mbedtls/sha1.h"
#endif
#include "ulog/ulog.h"
#include "netmgr.h"

enum {
    HTTP_AUTH,
    HTTP_OTA,
    HTTP_INVALID,
};

bool httpc_running = false;
int command = HTTP_INVALID;
httpc_connection_t settings;

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
char ota_server_name[CONFIG_HTTPC_SERVER_NAME_SIZE] = "http://mjfile-test.smartmidea.net:80/";
uint32_t ota_req_times = 0;
uint32_t ota_req_fail_times = 0;
uint32_t ota_rsp_times = 0;
bool ota_header_found = false;
int32_t ota_file_size = 0;
int32_t ota_rx_size = 0;

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

static int httpc_recv_fun(httpc_handle_t httpc, uint8_t *buf, int32_t buf_size,
                          int32_t data_len, int32_t ret)
{
    char *content;

    if (command == HTTP_INVALID) {
        return 0;
    }

    if (command == HTTP_AUTH) {
#if CONFIG_HTTP_SECURE
        LOG("http session %x, buf size %d bytes, recv %d bytes data, ret %d\n",
            httpc, buf_size, data_len, ret);
        if (data_len > 0) {
            LOG("%s", buf);
        }

        if (ret <= 0 || ret == RX_FINAL) {
            close(settings.socket);
            httpc_deinit(httpc);
            httpc_running = true;
            httpc_handle = 0;
            if (ret == RX_FINAL) {
                ++auth_rsp_times;
            } else {
                ++auth_req_fail_times;
            }
            LOG("auth_req_times %d, auth_rsp_times %d, auth_req_fail_times %d\r\n",
                 auth_req_times, auth_rsp_times, auth_req_fail_times);
        }
#endif
    } else if (command == HTTP_OTA) {
        if (ret <= 0) {
            close(settings.socket);
            httpc_deinit(httpc);
            httpc_running = true;
            httpc_handle = 0;
            ++ota_req_fail_times;
            ota_header_found = false;
            ota_file_size = 0;
            ota_rx_size = 0;
            LOG("ota_req_times %d, ota_rsp_times %d, ota_req_fail_times %d\r\n",
                 ota_req_times, ota_rsp_times, ota_req_fail_times);
        }

        if (ret == RX_CONTINUE || ret == RX_FINAL) {
            LOG("http session %x, buf size %d bytes, recv %d bytes data, ret %d\n",
                httpc, buf_size, data_len, ret);
            if (ota_header_found == false) {
                if (ota_file_size == 0) {
                    content = strstr(buf, "Content-Length");
                    if (content) {
                        ret = sscanf(content, "%*[^ ]%d", &ota_file_size);
                        if (ret < 0) {
                            LOG("http session fail to get ota header\r\n");
                            return 0;
                        }
                        ota_header_found = true;
                        LOG("ota file size %d\r\n", ota_file_size);
                    } else {
                        return 0;
                    }
                }
                content = strstr(buf, "\r\n\r\n");
                if (content) {
                    content += 4;
                    ota_rx_size = data_len - ((uint8_t *)content - buf);
                    LOG("ota (%d/%d) \r\n", ota_rx_size, ota_file_size);
                }
                return 0;
            }
            ota_rx_size += data_len;
            LOG("ota (%d/%d) \r\n", ota_rx_size, ota_file_size);
            if (ota_rx_size >= ota_file_size && ret == RX_FINAL) {
                close(settings.socket);
                httpc_deinit(httpc);
                httpc_running = true;
                httpc_handle = 0;
                ++ota_rsp_times;
                ota_header_found = false;
                ota_file_size = 0;
                ota_rx_size = 0;
                LOG("ota_req_times %d, ota_rsp_times %d, ota_req_fail_times %d\r\n",
                     ota_req_times, ota_rsp_times, ota_req_fail_times);
            }
        }
    }
    return 0;
}

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
    return HTTPC_SUCCESS;
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
    if (ret == HTTPC_SUCCESS) {
        ++auth_req_times;
    } else {
        close(settings.socket);
        httpc_deinit(httpc_handle);
        httpc_running = true;
        httpc_handle = 0;
        ++auth_req_fail_times;
    }
    return ret;
}
#endif

#define HTTP_OTA_HDR_SIZE 64
static int32_t httpc_ota(const char *uri)
{
    char hdr[HTTP_OTA_HDR_SIZE] = {0};
    int ret;

    if (uri == NULL) {
        return HTTPC_FAIL;
    }

    ret = httpc_construct_header(hdr, HTTP_OTA_HDR_SIZE, "Accept", "*/*");
    if (ret < 0) {
        LOG("http construct header fail\n");
        return ret;
    }

    ret = httpc_send_request(httpc_handle, HTTP_GET, uri, hdr, NULL, NULL, 0);
    if (ret == HTTPC_SUCCESS) {
        ++ota_req_times;
    } else {
        close(settings.socket);
        httpc_deinit(httpc_handle);
        httpc_running = true;
        httpc_handle = 0;
        ++ota_req_fail_times;
    }
    return ret;
}

#define RSP_BUF_SIZE 2048
#define REQ_BUF_SIZE 1024
uint8_t rsp_buf[RSP_BUF_SIZE];
uint8_t req_buf[REQ_BUF_SIZE];
static void httpc_delayed_action(void *arg)
{
#if CONFIG_HTTP_SECURE
    char device_id[64];
#endif
    int fd;
    int32_t ret = HTTPC_FAIL;

    if (httpc_running == false) {
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
    settings.timeout = 10000;  // 10000ms for testing
    settings.recv_fn = httpc_recv_fun;
#if CONFIG_HTTP_SECURE
    if (command == HTTP_AUTH) {
        settings.server_name = auth_server_name;
    } else
#endif
    if (command == HTTP_OTA) {
        settings.server_name = ota_server_name;
    } else {
        close(fd);
        goto exit;
    }
#if CONFIG_HTTP_SECURE
    settings.ca_cert = ca_cert;
#endif
    settings.rsp_buf = rsp_buf;
    settings.rsp_buf_size = RSP_BUF_SIZE;
    settings.req_buf = req_buf;
    settings.req_buf_size = REQ_BUF_SIZE;
    httpc_handle = httpc_init(&settings);
    if (httpc_handle == 0) {
        LOG("http session init fail\n");
        close(fd);
        goto exit;
    }

    LOG("http session %x command %d at %d\n", httpc_handle, command, (uint32_t)aos_now_ms());
    switch (command) {
        case HTTP_AUTH:
#if CONFIG_HTTP_SECURE
            memset(device_id, 0, 64);
            snprintf(device_id, 64, "%s.%s", PRODUCT_KEY, DEVICE_NAME);
            ret = httpc_auth(device_id, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET);
#endif
            break;
        case HTTP_OTA:
            ret = httpc_ota("/050509031881.bin");
            break;
        default:
            break;
    }

exit:
    if (ret == HTTPC_SUCCESS) {
        httpc_running = false;
    }
    aos_post_delayed_action(500, httpc_delayed_action, (void *)(long)command);
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
        if (strncmp(type, "auth", strlen("auth")) == 0) {
            command = HTTP_AUTH;
            httpc_running = true;
        } else
#endif
        if (strncmp(type, "ota", strlen("ota")) == 0) {
            command = HTTP_OTA;
            httpc_running = true;
        } else {
            LOG("unknown command\n");
            return;
        }
    }
}

static struct cli_command httpc_cmd = {
    .name = "httpc",
    .help = "httpc auth | ota | stop",
    .function = httpc_cmd_handle
};

int application_start(int argc, char *argv[])
{
    aos_set_log_level(AOS_LL_DEBUG);
    netmgr_init();
    netmgr_start(false);
    aos_cli_register_command(&httpc_cmd);
    http_client_intialize();
    aos_post_delayed_action(100, httpc_delayed_action, NULL);
    aos_loop_run();
    return 0;
}
