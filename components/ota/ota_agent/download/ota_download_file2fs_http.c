/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "ota_log.h"
#include "ota_hal_os.h"
#include "ota_import.h"
#include "ota_hal_trans.h"
#include "httpclient.h"

int ota_get_upgrade_status(void);
void ota_set_upgrade_status(char is_upgrade);
int ota_download_extract_url(char *src_url, char *dest_url, unsigned int dest_buf_len);
int ota_httpc_request_send(httpclient_t *client, char *url, httpclient_data_t *client_data);
int ota_httpc_recv_data(httpclient_t *client, httpclient_data_t *client_data);
int ota_httpc_settings_init(httpclient_t *client, httpclient_data_t *client_data);
void ota_httpc_settings_destory(httpclient_t *client);
/**
 * ota_download_store_fs_start    OTA download file start and store in fs
 *
 * @param[in]              char *url  download url
 * @param[in]   unsigned int url_len  download url length
 * @param[in]       char *store_path  store file path and name eg:/root/test.bin
 * @param[in] report_func report_func report http downloading status function
 * @param[in]       void *user_param  user's param for report_func
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_DOWNLOAD_INIT_FAIL  OTA download init failed.
 * @return OTA_DOWNLOAD_CON_FAIL   OTA download connect failed.
 * @return OTA_DOWNLOAD_REQ_FAIL   OTA download request failed.
 * @return OTA_DOWNLOAD_RECV_FAIL  OTA download receive failed.
 */
int ota_download_store_fs_start(char *url, unsigned int url_len, char *store_path,
                                report_func report_func, void *user_param)
{
    int j = 0;
    int fd = -1;
    int  ret = OTA_DOWNLOAD_INIT_FAIL;
    unsigned int off_size = 0;
    int ota_rx_size = 0;
    int ota_file_size = 0;
    char tmp_header[64] = {0};
    unsigned char ota_header_found = false;
    httpclient_t client           = {0};
    httpclient_data_t client_data = {0};
    char *new_url                 = NULL;
    int tmp_url_len               = 0;
    int percent                   = 0;
    int divisor                   = 5;
    if ((store_path == NULL) || (url == NULL)) {
        ret = OTA_DOWNLOAD_INIT_FAIL;
        return ret;
    }
    if (ota_get_upgrade_status() == 1) {
        ret = OTA_DOWNLOAD_INIT_FAIL;
        return ret;
    }
    tmp_url_len = url_len + 1;
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
    fd = ota_fopen(store_path, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd < 0) {
        if (new_url != NULL) {
            ota_free(new_url);
        }
        ret = OTA_DOWNLOAD_INIT_FAIL;
        OTA_LOG_E("open %s failed\n", store_path);
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
        OTA_LOG_E("ota download begin....\n");
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
                        sscanf(client_data.header_buf + val_pos, "%d", &ota_file_size);
                    }
                }
                ret = ota_fwrite(fd, client_data.response_buf, client_data.content_block_len);
                if (ret < 0) {
                    ret = OTA_UPGRADE_WRITE_FAIL;
                    goto EXIT;
                }
                ota_rx_size += client_data.content_block_len;
                ota_msleep(5);
                off_size = ota_rx_size;
                if (ota_file_size) {
                    percent = ((long)(ota_rx_size >> 6) * 100) / (long)(ota_file_size >> 6);
                    if (percent / divisor) {
                        divisor += 5;
                        if (report_func != NULL) {
                            ret = report_func(user_param, percent);
                            if (ret < 0) {
                                OTA_LOG_E("report http download process failed, ret = %d", ret);
                            }
                        }
                        OTA_LOG_I(" in fs recv data(%d/%d) off:%d\r\n", ota_rx_size, ota_file_size, off_size);
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
    if (fd >= 0) {
        (void)ota_fclose(fd);
        fd = -1;
    }
    if (new_url != NULL) {
        ota_free(new_url);
        new_url = NULL;
    }
    OTA_LOG_I("in fs download complete:%d\n", ret);
    return ret;
}