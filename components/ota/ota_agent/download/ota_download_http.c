/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "ota_log.h"
#include "ota_hal_os.h"
#include "ota_import.h"
#include "ota_hal_trans.h"
#include "httpclient.h"

static int ota_upgrading = 0;
static unsigned char dl_buf[OTA_DOWNLOAD_BLOCK_SIZE] = {0};
static unsigned char head_buf[OTA_DOWNLOAD_BLOCK_SIZE] = {0};
#if defined OTA_CONFIG_SECURE_DL_MODE
static const char *ca_cert = \
{   \
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

void ota_set_upgrade_status(char is_upgrade)
{
    ota_upgrading = is_upgrade;
}

int ota_get_upgrade_status()
{
    return ota_upgrading;
}

/**
 * ota_httpc_settings_init       init httpc settings
 *
 * @param[in] httpclient_t       *client        http client handle
 * @param[in] httpclient_data_t  *client_data   httpc data
 *
 * @return 0  success
 * @return -1 fail
 */
int ota_httpc_settings_init(httpclient_t *client, httpclient_data_t *client_data)
{
    int ret = -1;
    if ((client != NULL) && (client_data != NULL)) {
        ret = 0;
#if defined OTA_CONFIG_SECURE_DL_MODE
        OTA_LOG_I("init https ota.\n");
        client->server_cert = ca_cert;
        client->server_cert_len = strlen(ca_cert) + 1;
#else
        OTA_LOG_I("init http.\n");
#endif
        memset(head_buf, 0, sizeof(head_buf));
        client_data->header_buf = (char *)head_buf;
        client_data->header_buf_len = sizeof(head_buf);

        memset(dl_buf, 0, sizeof(dl_buf));
        client_data->response_buf = (char *)dl_buf;
        client_data->response_buf_len = sizeof(dl_buf);
    }
    return ret;
}

/**
 * ota_httpc_settings_destory  destory httpc settings
 *
 * @param[in] httpc_connection_t *settings  httpc settings
 *
 * @return void
 */
void ota_httpc_settings_destory(httpclient_t *client)
{
    httpclient_clse(client);
}

/**
 * ota_httpc_request_send     OTA send request to server
 *
 * @param[in] httpclient_t *client           httpc client
 * @param[in] char *url                      host url
 * @param[in] httpclient_data_t *client_data ttpc response infomation
 *
 * @return  0  success.
 * @return -1  failed.
 */
int ota_httpc_request_send(httpclient_t *client, char *url, httpclient_data_t *client_data)
{
    int ret = -1;
    if (client == NULL || url == NULL || client_data == NULL) {
        OTA_LOG_E("http recv fuc input parameter err\n");
    } else {
        ret = httpclient_send(client, url, HTTP_GET, client_data);
        if (ret < 0) {
            OTA_LOG_E("send data ret:%d\n", ret);
        }
    }
    return ret;
}

/**
 * ota_httpc_recv_data        OTA receive data from httpc
 *
 * @param[in] httpclient_t      *client           httpc handle
 * @param[in] httpclient_data_t *client_data      httpc response infomation
 *
 * @return  0  success.
 * @return -1  failed.
 */
int ota_httpc_recv_data(httpclient_t *client, httpclient_data_t *client_data)
{
   int ret = -1;
   if (client == NULL || client_data == NULL) {
        OTA_LOG_E("http recv fuc input parameter err\n");
    } else {
        ret = httpclient_recv(client, client_data);
        if ((ret == HTTP_ERECV) || (ret == HTTP_ETIMEOUT)) {
            OTA_LOG_E("recv ret:%d\n", ret);
        }
    }
    return ret;
}

/**
 * ota_download_extract_url   OTA extrack http url from https url
 *
 * @param[in] char *src_url       src https url.
 * @param[in] char *dest_url      new url store buf.
 * @param[in] char *dest_buf_len  new url store buf len.
 *
 * @return 0 success, others fail
 */
int ota_download_extract_url(char *src_url, char *dest_url, unsigned int dest_buf_len)
{
    int ret = 0;
    unsigned int len = 0;
    if ((src_url == NULL) || (strlen(src_url) == 0) || (dest_url == NULL)) {
        OTA_LOG_E("url parms error!");
        return OTA_DOWNLOAD_INIT_FAIL;
    }
    len = strlen(src_url);
    if (len > dest_buf_len) {
        OTA_LOG_E("url too long!");
        return OTA_DOWNLOAD_INIT_FAIL;
    }
    OTA_LOG_I("cpy len = %d\r\n", len);
    strncpy(dest_url, src_url, dest_buf_len);
    dest_url[dest_buf_len - 1] = 0;
 #ifndef OTA_CONFIG_SECURE_DL_MODE
   if (dest_url[4] == 's') {
        int i = 0;
        for (i = 4; i < len; i++) {
            dest_url[i] = dest_url[i + 1];
        }
        dest_url[i] = 0;
    }
#endif
    OTA_LOG_I("http_uri:%s\n", dest_url);
    return ret;
}

/**
 * ota_download_image_header  OTA download image header
 *
 * @param[in]   ota_service_t *ctx ota service context
 * @param[in]            char *url download url
 * @param[in] unsigned int url_len download url length
 * @param[in]    unsigned int size image size
 *
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_DOWNLOAD_INIT_FAIL  OTA download init failed.
 * @return OTA_DOWNLOAD_CON_FAIL   OTA download connect failed.
 * @return OTA_DOWNLOAD_REQ_FAIL   OTA download request failed.
 * @return OTA_DOWNLOAD_RECV_FAIL  OTA download receive failed.
 */
int ota_download_image_header(ota_service_t *ctx, char *url, unsigned int url_len, unsigned int size)
{
    int  ret = OTA_DOWNLOAD_INIT_FAIL;
    unsigned int off_size = 0;
    char *content = NULL;
    char tmp_header[64] = {0};
    int retry_tm = 0;
    int ota_rx_size = 0;
    int want_download_size = 0;
    unsigned char ota_header_found = false;
    httpclient_t client = { 0 };
    httpclient_data_t client_data = {0};
    char *new_url = NULL;
    int tmp_url_len = 0;
    #if defined(BOARD_HAAS200)
    char tmp[100] = {0};
    #endif

#ifdef OTA_CONFIG_LOCAL_RSA
    char *sign_info_ptr         = NULL;
#endif
    char *image_info_ptr        = NULL;
    if ((ctx == NULL) || (url == NULL) || (ota_get_upgrade_status() == 1)) {
        ret = OTA_DOWNLOAD_INIT_FAIL;
        return ret;
    }
#ifdef OTA_CONFIG_LOCAL_RSA
    sign_info_ptr = (char *)ctx->header.sign_info;
    if (sign_info_ptr == NULL) {
        ret = OTA_DOWNLOAD_INIT_FAIL;
        return ret;
    }
#endif
    image_info_ptr = (char *)ctx->header.image_info;
    if (image_info_ptr == NULL) {
        ret = OTA_DOWNLOAD_INIT_FAIL;
        return ret;
    }
    tmp_url_len = url_len + 1;
    OTA_LOG_I("URl len = %d\r\n", url_len);
    new_url = ota_malloc(tmp_url_len);
    if (new_url == NULL) {
        ret = OTA_DOWNLOAD_INIT_FAIL;
        return ret;
    }
    memset(new_url, 0, tmp_url_len);
    if (ota_download_extract_url(url, new_url, tmp_url_len) < 0) {
        if (new_url != NULL) {
            ota_free(new_url);
        }
        ret = OTA_DOWNLOAD_INIT_FAIL;
        return ret;
    }
#ifdef OTA_CONFIG_LOCAL_RSA
    off_size = size - sizeof(ota_image_info_t) - sizeof(ota_sign_info_t);
#else
    off_size = size - sizeof(ota_image_info_t);
#endif
    for (retry_tm = OTA_DOWNLOAD_RETRY_CNT; (retry_tm > 0) && (ret < 0); retry_tm--) {
        if (retry_tm < OTA_DOWNLOAD_RETRY_CNT) {
            OTA_LOG_I("retry count.\n");
            ota_msleep(6000);
        }
        ret = ota_httpc_settings_init(&client, &client_data);
        if (ret < 0) {
            ret = OTA_DOWNLOAD_INIT_FAIL;
            goto OVER;
        }
        memset(tmp_header, 0, sizeof(tmp_header));
        OTA_LOG_I("retry:%d ret:%d rx_size:%d\n", retry_tm, ret, off_size);
        ota_snprintf(tmp_header, sizeof(tmp_header), "Range: bytes=%d-\r\n", off_size);
        client.header = tmp_header;
        OTA_LOG_I("http conn.\n");
        ret = httpclient_conn(&client, new_url);
        if (ret < 0) {
            ret = OTA_DOWNLOAD_INIT_FAIL;
            goto OVER;
        }
        OTA_LOG_I("http request send.\n");
        ret = ota_httpc_request_send(&client, new_url, &client_data);
        if (ret < 0) {
            ret = OTA_DOWNLOAD_REQ_FAIL;
            goto OVER;
        }
        ota_set_upgrade_status(1);
        OTA_LOG_I("http begin download.\n");
        while (want_download_size == 0 || ota_rx_size < want_download_size) {
            ret = ota_httpc_recv_data(&client, &client_data);
            if (ret < 0) {
                ret = OTA_DOWNLOAD_RECV_FAIL;
                break;
            } else {
                int tmp_size = client_data.content_block_len;
                content = (char *)client_data.response_buf;
                if (ota_header_found == false) {
                    ota_header_found = true;
                    int val_pos, val_len;
                    if (0 == httpclient_get_response_header_value(client_data.header_buf, "Content-Length", (int *)&val_pos, (int *)&val_len)) {
                        #if defined(BOARD_HAAS200)
                        //Content-Length will not exceed 100 bytes
                        memset(tmp, 0 ,sizeof(tmp));
                        memcpy(tmp, client_data.header_buf+val_pos, val_len);
                        want_download_size = strtol(tmp, NULL, 0);
                        #else
                        sscanf(client_data.header_buf + val_pos, "%d", &want_download_size);
                        #endif
                    }
                }
                if (ota_rx_size + tmp_size <= off_size) {
#ifdef OTA_CONFIG_LOCAL_RSA
                    if (ota_rx_size  + tmp_size <= sizeof(ota_sign_info_t)) {
                        memcpy(sign_info_ptr, content, tmp_size);
                        sign_info_ptr += tmp_size;
                        ota_rx_size += tmp_size;
                    } else {
                        if (ota_rx_size <= sizeof(ota_sign_info_t)) {
                            unsigned int remain_len = sizeof(ota_sign_info_t) - ota_rx_size;
                            memcpy(sign_info_ptr, content, remain_len);
                            sign_info_ptr += remain_len;
                            content += remain_len;
                            memcpy(image_info_ptr, content, tmp_size - remain_len);
                            image_info_ptr += tmp_size - remain_len;
                            ota_rx_size += tmp_size;
                        } else {
                            memcpy(image_info_ptr, content, tmp_size);
                            image_info_ptr += tmp_size;
                            ota_rx_size += tmp_size;
                        }
                    }
#else
                    memcpy(image_info_ptr, content, tmp_size);
                    image_info_ptr += tmp_size;
                    ota_rx_size += tmp_size;
#endif
                } else {
                    OTA_LOG_E("image head buf full");
                    ret = OTA_DOWNLOAD_RECV_FAIL;
                    retry_tm = 0;
                    break;
                }
            }
        }
OVER:
        ota_set_upgrade_status(0);
        ota_httpc_settings_destory(&client);
        ota_header_found = false;
        want_download_size = 0;
        ota_rx_size = 0;
    }
    if (new_url != NULL) {
        ota_free(new_url);
        new_url = NULL;
    }
    OTA_LOG_I("parse image info:%d\n", ret);
    return ret;
}

/**
 * ota_download_start    OTA download start
 *
 * @param[in]              char *url  download url
 * @param[in]   unsigned int url_len  download url length
 * @param[in] report_func repot_func  report http downloading status function
 * @param[in]       void *user_param  user's param for repot_func
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_DOWNLOAD_INIT_FAIL  OTA download init failed.
 * @return OTA_DOWNLOAD_CON_FAIL   OTA download connect failed.
 * @return OTA_DOWNLOAD_REQ_FAIL   OTA download request failed.
 * @return OTA_DOWNLOAD_RECV_FAIL  OTA download receive failed.
 */
int ota_download_start(char *url, unsigned int url_len, report_func repot_func, void *user_param)
{
    int  ret = OTA_DOWNLOAD_INIT_FAIL;
    unsigned int offset   = 0;
    unsigned int off_size = 0;
    int j = 0;
    int ota_rx_size   = 0;
    int ota_file_size = 0;
    char tmp_header[64] = {0};
    unsigned char ota_header_found = false;
    httpclient_t client           = {0};
    httpclient_data_t client_data = {0};
    char *new_url                 = NULL;
    int tmp_url_len               = 0;
    int percent                   = 0;
    int divisor                   = 5;
    if (url == NULL) {
        ret = OTA_DOWNLOAD_INIT_FAIL;
        return ret;
    }
    if (ota_get_upgrade_status() == 1) {
        ret = OTA_DOWNLOAD_INIT_FAIL;
        return ret;
    }
    tmp_url_len = url_len + 1;
    OTA_LOG_I("URl len = %d\r\n", url_len);
    new_url = ota_malloc(tmp_url_len);
    if (new_url == NULL) {
        ret = OTA_DOWNLOAD_INIT_FAIL;
        return ret;
    }
    memset(new_url, 0, tmp_url_len);
    if (ota_download_extract_url(url, new_url, tmp_url_len) < 0) {
        if (new_url != NULL) {
            ota_free(new_url);
        }
        ret = OTA_DOWNLOAD_INIT_FAIL;
        return ret;
    }
    for (j = OTA_DOWNLOAD_RETRY_CNT; (j > 0) && (ret < 0); j--) {
        ret = ota_httpc_settings_init(&client, &client_data);
        if (ret < 0) {
            ret = OTA_DOWNLOAD_INIT_FAIL;
            goto EXIT;
        }
        memset(tmp_header, 0, sizeof(tmp_header));
        if (j >= OTA_DOWNLOAD_RETRY_CNT) {
            strncpy(tmp_header, "Accept: */*\r\n", sizeof(tmp_header));
            tmp_header[sizeof(tmp_header) - 1] = 0;
        } else {
            ota_msleep(6000);
            OTA_LOG_I("reconnect retry:%d ret:%d rx_size:%d\n", j, ret, off_size);
            ota_snprintf(tmp_header, sizeof(tmp_header), "Range: bytes=%d-\r\n", off_size);
        }
        client.header = tmp_header;
        ret = httpclient_conn(&client, new_url);
        if (ret < 0) {
            ret = OTA_DOWNLOAD_CON_FAIL;
            goto EXIT;
        }
        ret = ota_httpc_request_send(&client, new_url, &client_data);
        if (ret < 0) {
            ret = OTA_DOWNLOAD_REQ_FAIL;
            goto EXIT;
        }
        ota_set_upgrade_status(1);
        while (ota_file_size == 0 || ota_rx_size < ota_file_size) {
            ret = ota_httpc_recv_data(&client, &client_data);
            if (ota_get_upgrade_status() == 0) {
                OTA_LOG_E("download stop.\n");
                ret = OTA_DOWNLOAD_RECV_FAIL;
                break;
            } else if (ret < 0) {
                ret = OTA_DOWNLOAD_RECV_FAIL;
                break;
            } else {
                if (ota_header_found == false) {
                    int val_pos, val_len;
                    ota_header_found = true;
                    if (0 == httpclient_get_response_header_value(client_data.header_buf, "Content-Length", (int *)&val_pos, (int *)&val_len)) {
                        #if defined(BOARD_HAAS200)
                        ota_file_size = strtol(client_data.header_buf+val_pos , NULL, 0);
                        #else
                        sscanf(client_data.header_buf + val_pos, "%d", &ota_file_size);
                        #endif
                    }
                }
                ret = ota_write(&offset, client_data.response_buf, client_data.content_block_len);
                if (ret < 0) {
                    ret = OTA_UPGRADE_WRITE_FAIL;
                    goto EXIT;
                }
                ota_rx_size += client_data.content_block_len;
                ota_msleep(5);
                off_size = ota_rx_size;
                OTA_LOG_I("recv size = %d, has recv size = %d\r\n", client_data.content_block_len, off_size);
                if (ota_file_size) {
                    percent = ((long)(ota_rx_size >> 6) * 100) / (long)(ota_file_size >> 6);
                    if (percent / divisor) {
                        divisor += 5;
                        if (repot_func != NULL) {
                            ret = repot_func(user_param, percent);
                            if (ret != 0) {
                                OTA_LOG_E("report http download process failed ret %d\r\n", ret);
                            }
                        }
                        OTA_LOG_I("ota recv data(%d/%d) off:%d \r\n", ota_rx_size, ota_file_size, off_size);
                    }
                }
            }
        }

EXIT:
        ota_set_upgrade_status(0);
        ota_httpc_settings_destory(&client);
        ota_header_found = false;
        ota_file_size = 0;
        ota_rx_size = 0;
    }
    if (new_url != NULL) {
        ota_free(new_url);
        new_url = NULL;
    }
    OTA_LOG_I("download complete:%d\n", ret);
    return ret;
}
