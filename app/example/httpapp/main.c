/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>

#include <aos/kernel.h>
#include <aos/cli.h>
#include <aos/yloop.h>
#include <network/network.h>
#include "ulog/ulog.h"
#include "netmgr.h"
#include "http_api.h"

bool httpc_running = false;
int method;

httpc_handle_t httpc_handle = 0;
#if CONFIG_HTTP_SECURE
char server_name[CONFIG_HTTPC_SERVER_NAME_SIZE] = "https://iot-as-http.cn-shanghai.aliyuncs.com/";
#else
char server_name[CONFIG_HTTPC_SERVER_NAME_SIZE] = "http://httpie.org/";
#endif

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
                          int32_t data_len, bool is_final)
{
    LOG("http session %x, buf size %d bytes, recv %d bytes data, is_final %d\n",
         httpc, buf_size, data_len, is_final);
    return 0;
}

static void httpc_delayed_action(void *arg)
{
    if (httpc_handle == 0 || httpc_running == false) {
        httpc_running = false;
        goto exit;
    }

    LOG("http session %x method %d at %d\n", httpc_handle, method, (uint32_t)aos_now_ms());
    switch (method) {
        case HTTP_GET:
            httpc_send_request(httpc_handle, HTTP_GET, NULL, NULL, NULL, NULL, 0);
            httpc_running = false;
            break;
        default:
            break;
    }

exit:
    aos_post_delayed_action(100, httpc_delayed_action, (void *)(long)method);
}

#define RSP_BUF_SIZE 2000
uint8_t rsp_buf[RSP_BUF_SIZE];
static void httpc_cmd_handle(char *buf, int blen, int argc, char **argv)
{
    const char *type = argc > 1? argv[1]: "";
    httpc_connection_t settings;
    int fd;

    if (strncmp(type, "stop", strlen("stop")) == 0) {
        httpc_running = false;
        return;
    }

    if (httpc_running == false) {
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) {
            LOG("alloc socket fd fail\n");
            return;
        }
        memset(&settings, 0, sizeof(settings));
        settings.socket = fd;
        settings.recv_fn = httpc_recv_fun;
        settings.server_name = server_name;
#if CONFIG_HTTP_SECURE
        settings.ca_cert = ca_cert;
#endif
        settings.rsp_buf = rsp_buf;
        settings.rsp_buf_size = RSP_BUF_SIZE;
        httpc_handle = httpc_init(&settings);
        if (httpc_handle == 0) {
            LOG("http session init fail\n");
            return;
        }
    }

    if (strncmp(type, "GET", strlen("GET")) == 0) {
        method = HTTP_GET;
        httpc_running = true;
    }
}

static struct cli_command httpc_cmd = {
    .name = "httpc",
    .help = "httpc GET | stop",
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
