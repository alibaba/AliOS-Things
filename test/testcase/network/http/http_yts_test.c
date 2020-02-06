/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>

#include "yunit/yunit.h"
#include "yunit/yts.h"

#if CONFIG_HTTP_SECURE
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
#include "mbedtls/sha256.h"
#include "mbedtls/sha1.h"
#endif

#include "aos/kernel.h"
#include "aos/yloop.h"
#include "http.h"
#include "ulog/ulog.h"

#define TAG "http_yts"

#define BUF_SIZE 1024

char *geturl    = "https://iot-auth.cn-shanghai.aliyuncs.com/";
char *posturl   = "https://api.mediatek.com/mcs/v2/devices/D0n2yhrl/datapoints.csv";
char *puturl    = "https://ec2-52-76-74-57.ap-southeast-1.compute.amazonaws.com/mcs/test/ok/200";
char *deleteurl = "https://ec2-52-76-74-57.ap-southeast-1.compute.amazonaws.com/mcs/test/ok/200";
char *commonurl = "https://iot-auth.cn-shanghai.aliyuncs.com/";

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

/*@brief http get yts test */
static int yts_http_get()
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
    /* Sets a buffer to store the result */
    client_data.response_buf = buf;
    /* Sets the buffer size */
    client_data.response_buf_len = BUF_SIZE;
    ret = httpc_get(&client, geturl, &client_data);
    YUNIT_ASSERT( 0 <= ret );
    if( 0 <= ret ) {
        LOGD(TAG, "Data received: %s", client_data.response_buf);
    }
    else {
        LOGE(TAG, "http client get test failed and ret = %d errno=%d", ret, errno);
    }

    free(buf);
    return ret;
}

/*@brief http post yts test */
static int yts_http_post()
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
    /* Sets a buffer to store the result */
    client_data.response_buf = buf;
    /* Sets the buffer size */
    client_data.response_buf_len = BUF_SIZE;
    /* Sets the custom header if needed */
    httpc_set_custom_header(&client, header);
    /* Sets the user data to be posted */
    client_data.post_buf = post_data;
    /* Sets the post data length */
    client_data.post_buf_len = strlen(post_data);
    /* Sets the content type */
    client_data.post_content_type = content_type;
    ret = httpc_post(&client, posturl, &client_data);
    YUNIT_ASSERT( 0 <= ret );

    if( 0 <= ret ) {
        int code;
        char* data;
        int data_len;
        char content_len[8];

        LOGD(TAG, "Data received: %s", client_data.response_buf);
        code = httpc_get_response_code(&client);
        LOGD(TAG, "Response code: %d", ret);
        httpc_get_response_header_value(client_data.response_buf, "Content-Length", data, &data_len);
        memcpy(content_len, data, data_len);
        LOGD(TAG, "Content-Length is %s", content_len);
    }
    else {
        LOGE(TAG, "http client post test failed and ret=%d errno=%d", ret, errno);
    }


    free(buf);
    return ret;
}

/* @brief http put yts test */
static int yts_http_put()
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
    /* Sets a buffer to store the result */
    client_data.response_buf = buf;
    /* Sets the buffer size */
    client_data.response_buf_len = BUF_SIZE;
    /* Sets the user data to be put */
    client_data.post_buf = put_data;
    /* Sets the put data length */
    client_data.post_buf_len = strlen(put_data);
    /* Sets the content type */
    client_data.post_content_type = content_type;
    ret = httpc_put(&client, puturl, &client_data);
    YUNIT_ASSERT( 0 <= ret );
    if( 0 <= ret ) {
        int code;
        char* data;
        int data_len;
        char content_len[8];

        LOGD(TAG, "Data received: %s", client_data.response_buf);
        code = httpc_get_response_code(&client);
        LOGD(TAG, "Response code: %d", ret);
        httpclient_get_response_header_value(client_data.response_buf, "Content-Length", data, &data_len);
        memcpy(content_len, data, data_len);
        LOGD(TAG, "Content-Length is %s", content_len);
    }
    else {
        LOGE(TAG, "http client put test failed and ret=%d errno=%d", ret, errno);
    }
    free(buf);
    return ret;
}

/*@brief http delete yts test */
static int yts_http_delete()
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
    /* Sets a buffer to store the result */
    client_data.response_buf = buf;
    /* Sets the buffer size */
    client_data.response_buf_len = BUF_SIZE;
    ret = httpc_delete(&client, deleteurl, &client_data);
    YUNIT_ASSERT( 0 <= ret );
    if( 0 <= ret ) {
        LOGD(TAG, "Data received: %s", client_data.response_buf);
    }
    else {
        LOGE(TAG, "http client delete test failed and ret = %d errno=%d", ret, errno);
    }
    free(buf);
    return ret;
}

/* @brief httpc api yts test */
#define HTTPS_HDR_SIZE 128
#define REQ_BUF_SIZE 1024
#define RSP_BUF_SIZE 2048
static uint8_t req_buf[REQ_BUF_SIZE] = {0};
static uint8_t rsp_buf[RSP_BUF_SIZE] = {0};
static char hdr[HTTPS_HDR_SIZE] = {0};
static HTTPC_RESULT yts_http_test()
{
    int ret = HTTP_ECONN;
    int fd;
    httpc_connection_t settings;
    httpc_handle_t httpc_handle;
    http_rsp_info_t rsp_info;
    char *content;

    ret = http_client_initialize();
    YUNIT_ASSERT( 0 == ret );

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        LOGE(TAG, "Alloc socket fd fail");
        goto exit;
    }

    memset(&settings, 0, sizeof(settings));
    settings.socket = fd;
    settings.server_name = commonurl;
#if CONFIG_HTTP_SECURE 
    settings.ca_cert = ca_cert;
#endif
    settings.req_buf = req_buf;
    settings.req_buf_size = REQ_BUF_SIZE;
    httpc_handle = httpc_init(&settings);
    YUNIT_ASSERT( 0 != httpc_handle );
    if (httpc_handle == 0) {
        LOGE(TAG, "http session init fail");
        close(fd);
        goto exit;
    }

    ret = httpc_construct_header(hdr, HTTPS_HDR_SIZE, "Accept", "*/*");
    YUNIT_ASSERT( 0 <= ret );
    if (ret < 0) {
        LOGE(TAG, "http construct header fail ret=%d\n", ret);
        close(fd);
        httpc_deinit(httpc_handle);
        return ret;
    }

    ret = httpc_send_request(httpc_handle, HTTP_GET, NULL, hdr, NULL, NULL, 0);
    YUNIT_ASSERT( HTTP_SUCCESS == ret );
    if (ret != HTTP_SUCCESS) {
        LOGE(TAG, "http send request fail ret=%d\n", ret);
        close(fd);
        httpc_deinit(httpc_handle);
        goto exit;
    }

    ret = httpc_recv_response(httpc_handle, rsp_buf, RSP_BUF_SIZE, &rsp_info, 10000);
    YUNIT_ASSERT( 0 <= ret );
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
    ret = httpc_deinit(httpc_handle);
    YUNIT_ASSERT( 0 == ret );

exit:
    LOGE(TAG, "yts_http_test() result:%d errno=%d", ret, errno);
    return (HTTPC_RESULT)ret;
}

static void init(void)
{

}

static int cleanup(void)
{
    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

/* @brief Current http put/delete method url is abnormal */
static yunit_test_case_t aos_http_testcases[] = {
    { "yts http get",      yts_http_get},
    { "yts http post",     yts_http_post},
//  { "yts http put",      yts_http_put},
//  { "yts http delete",   yts_http_delete},
    { "yts http test",     yts_http_test},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "http", init, cleanup, setup, teardown, aos_http_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_http(void)
{
    yunit_add_test_suites(suites);
}

AOS_TESTCASE(test_http);

