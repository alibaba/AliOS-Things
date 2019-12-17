/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include "aos/kernel.h"
#include "aos/yloop.h"
#include "aos/cli.h"
#include "http.h"

#include "ulog/ulog.h"

#define TAG "HTTPAPP"
#define BUF_SIZE 1024

char *geturl    = "https://iot-auth.cn-shanghai.aliyuncs.com/";
char *posturl   = "https://api.mediatek.com/mcs/v2/devices/D0n2yhrl/datapoints.csv";
char *puturl    = "https://ec2-52-76-74-57.ap-southeast-1.compute.amazonaws.com/mcs/test/ok/200";
char *deleteurl = "https://ec2-52-76-74-57.ap-southeast-1.compute.amazonaws.com/mcs/test/ok/200";
char *commonurl = "https://iot-auth.cn-shanghai.aliyuncs.com/";

/* httpc get method demo */
int httpc_get_demo(char* url)
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
    client_data.response_buf = buf;  //Sets a buffer to store the result.
    client_data.response_buf_len = BUF_SIZE;  //Sets the buffer size.
    ret = httpc_get(&client, url, &client_data);
    if( ret == 0 ) {
        LOGE(TAG, "Data received: %s", client_data.response_buf);
    }
    return ret;
}

/* httpc post method demo */
int httpc_post_demo(char* url)
{
    char *header = "deviceKey:FZoo0S07CpwUHcrt\r\n";
    char *content_type = "text/csv";
    char *post_data = "1,,I am string!";
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
    client_data.response_buf = buf;  //Sets a buffer to store the result.
    client_data.response_buf_len = BUF_SIZE;  //Sets the buffer size.
    httpc_set_custom_header(&client, header);  //Sets the custom header if needed.
    client_data.post_buf = post_data;  //Sets the user data to be posted.
    client_data.post_buf_len = strlen(post_data);  //Sets the post data length.
    client_data.post_content_type = content_type;  //Sets the content type.
    ret = httpc_post(&client, url, &client_data);
    if( ret == 0 ) {
        LOGE(TAG, "Data received: %s", client_data.response_buf);
    }
    return ret;
}

/* httpc put method demo */
int httpc_put_demo(char* url)
{
    char *content_type = "text/csv";
    char *put_data = "1,,I am string!";
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
    client_data.response_buf = buf;  //Sets a buffer to store the result.
    client_data.response_buf_len = BUF_SIZE;  //Sets the buffer size.
    client_data.post_buf = put_data;  //Sets the user data to be put.
    client_data.post_buf_len = strlen(put_data);  //Sets the put data length.
    client_data.post_content_type = content_type;  //Sets the content type.
    ret = httpc_put(&client, url, &client_data);
    if( ret == 0 ) {
        LOGD(TAG, "Data received: %s", client_data.response_buf);
    }
    return ret;
}

/* httpc put delete demo */
int httpc_delete_demo(char* url)
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
    client_data.response_buf = buf;  //Sets a buffer to store the result.
    client_data.response_buf_len = BUF_SIZE;  //Sets the buffer size.
    ret = httpc_delete(&client, url, &client_data);
    if( ret == 0 ) {
        LOGD(TAG, "Data received: %s", client_data.response_buf);
    }
    return ret;
}

/* httpc test demo */
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

#define HTTPS_HDR_SIZE 128
#define REQ_BUF_SIZE 1024
#define RSP_BUF_SIZE 2048
static uint8_t req_buf[REQ_BUF_SIZE] = {0};
static uint8_t rsp_buf[RSP_BUF_SIZE] = {0};
static char hdr[HTTPS_HDR_SIZE] = {0};
HTTPC_RESULT httpc_demo(httpclient_t *client, char *url)
{
    int ret = HTTP_ECONN;
    int fd;
    httpc_connection_t settings;
    httpc_handle_t httpc_handle;
    http_rsp_info_t rsp_info;
    char *content;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        LOGE(TAG, "Alloc socket fd fail");
        goto exit;
    }

    memset(&settings, 0, sizeof(settings));
    settings.socket = fd;
    settings.server_name = url;
#if CONFIG_HTTP_SECURE 
    settings.ca_cert = ca_cert;
#endif
    settings.req_buf = req_buf;
    settings.req_buf_size = REQ_BUF_SIZE;
    httpc_handle = httpc_init(&settings);
    if (httpc_handle == 0) {
        LOGE(TAG, "http session init fail");
        close(fd);
        goto exit;
    }

    ret = httpc_construct_header(hdr, HTTPS_HDR_SIZE, "Accept", "*/*");
    if (ret < 0) {
        LOGE(TAG, "http construct header fail ret=%d\n", ret);
        close(fd);
        httpc_deinit(httpc_handle);
        return ret;
    }

    ret = httpc_send_request(httpc_handle, HTTP_GET, NULL, hdr, NULL, NULL, 0);
    if (ret != HTTP_SUCCESS) {
        LOGE(TAG, "http send request fail ret=%d\n", ret);
        close(fd);
        httpc_deinit(httpc_handle);
        goto exit;
    }

    ret = httpc_recv_response(httpc_handle, rsp_buf, RSP_BUF_SIZE, &rsp_info, 10000);
    if (ret < 0) {
        LOGE(TAG, "http recv response fail ret=%d", ret);
    } else {
        LOGD(TAG, "http session %x, buf size %d bytes, recv %d bytes data\n",
            httpc_handle, RSP_BUF_SIZE, rsp_info.rsp_len);
        if (rsp_info.rsp_len > 0) {
            LOGD(TAG, "%s", rsp_buf);
        }

        if (rsp_info.message_complete) {
            LOGD(TAG, "message_complete!");
        }
    }

    close(fd);
    httpc_deinit(httpc_handle);

exit:
    LOGE(TAG, "httpclient_connect() result:%d, client:%p", ret, client);
    return (HTTPC_RESULT)ret;
}

int httpc_common_demo(char* commonurl)
{
    int ret;
    httpclient_t client = {0};

    LOGD(TAG, "%s https_connect", __func__);

    ret = httpc_demo(&client, commonurl);

    LOGD(TAG, "%s https_common_demo ret:%d", __func__, ret);

    return ret;
}

static void httpc_app()
{
    int ret;

    if(0 !=(ret = httpc_get_demo(geturl))) {
        LOGE(TAG, "httpc get demo run failed ret=%d", ret);
    }

    if(0 !=(ret = httpc_post_demo(posturl))) {
        LOGE(TAG, "httpc post demo run failed ret=%d", ret);
    }

    if(0 !=(ret = httpc_put_demo(puturl))) {
        LOGE(TAG, "httpc put demo run failed ret=%d", ret);
    }

    if(0 !=(ret = httpc_delete_demo(deleteurl))) {
        LOGE(TAG, "httpc delete demo run failed ret=%d", ret);
    }

    if(0 !=(ret = httpc_common_demo(commonurl))) {
        LOGE(TAG, "httpc common demo run failed ret=%d", ret);
    } 
}

static void httpc_app_task(void) {
    aos_task_new("httpc_app", httpc_app, NULL, 20 * 1024);
}

static struct cli_command httpc_app_commands[] = {
    {"httpc", "httpc", httpc_app_task},
};

int application_start(int argc, char *argv[])
{
    LOGD(TAG, "application started.");

    aos_cli_register_commands(httpc_app_commands,
            sizeof(httpc_app_commands) / sizeof(struct cli_command));
    aos_loop_run();

    return 0;
}


