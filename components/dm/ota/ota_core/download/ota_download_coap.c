/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include "ota_log.h"
#include "ota_import.h"
#include "ota_crypto.h"
#include "ota_hal_common.h"

static int block_cur_num       = 0;
static int block_more          = 1;
static int block_size          = OTA_DOWNLOAD_BLOCK_SIZE;
static int total_size          = 0;
static int ota_upgrading = 0;
static void *sem_send = NULL;

static void ota_coap_block_cb(void *arg, void *p_response)
{
    int len         = 0;
    int cur_num     = 0;
    int more        = 0;
    int size        = 0;
    char *p_payload = NULL;
    int retry_cnt   = OTA_DOWNLOAD_RETRY_CNT;
    iotx_coap_resp_code_t resp_code;
    ota_coap_get_code(p_response, &resp_code);
    if (resp_code == 0x45) {
        ota_coap_get_payload(p_response, (const char **)&p_payload, &len);
        if (ota_coap_parse_block(p_response, OTA_COAP_OPTION_BLOCK, &cur_num, &more, &size)) {
            OTA_LOG_I("num: %d, more: %d, size: %d", cur_num, more, size);
            block_size = size;
            if (cur_num == block_cur_num) {
                if (more == 0) {
                    block_more = 0;
                } else {
                    block_cur_num++;
                }
                total_size += len;
                int ret = 0;
                ret = ota_write(NULL, p_payload, len);
                if (ret < 0) {
                      OTA_LOG_E("write err:%d", ret);
                      return;
                }
            }
        } else {
            if (retry_cnt++ > 0) {
                ota_upgrading = 0;
            }
        }
    } else {
        if (retry_cnt++ > 0) {
            ota_upgrading = 0;
        }
    }
    ota_semaphore_post(&sem_send);
}

/**
 * ota_download_start  OTA download start
 *
 * @param[in] char *url download url
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_DOWNLOAD_INIT_FAIL  OTA download init failed.
 * @return OTA_DOWNLOAD_CON_FAIL   OTA download connect failed.
 * @return OTA_DOWNLOAD_REQ_FAIL   OTA download request failed.
 * @return OTA_DOWNLOAD_RECV_FAIL  OTA download receive failed.
 */
int ota_download_start(const char *url)
{
    int ret        = 0;

    block_cur_num = 0;
    ota_upgrading = 1;
    sem_send = ota_semaphore_create();
    while (ota_upgrading && block_more) {
        iotx_message_t message;
        message.p_payload = NULL;
        message.payload_len = 0;
        message.resp_callback = ota_coap_block_cb;
        message.msg_type = COAP_MESSAGE_CON;
        message.content_type = COAP_CONTENT_TYPE_JSON;
        ota_coap_send_block(NULL, url, &message, OTA_COAP_OPTION_BLOCK, block_cur_num, block_more, block_size);
        ret = ota_semaphore_wait(&sem_send, OTA_DOWNLOAD_TIMEOUT);
        if(ret < 0) {
            ret = OTA_DOWNLOAD_FAIL;
            OTA_LOG_E("download error %s", strerror(errno));
            break;
        }
    }
    if (block_more == 0 && block_cur_num) {
        OTA_LOG_I("download finish.");
    }
    return ret;
}

/**
 * ota_download_stop  OTA download stop
 *
 * @param[in] void
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_DOWNLOAD_INIT_FAIL  OTA download init failed.
 * @return OTA_DOWNLOAD_CON_FAIL   OTA download connect failed.
 * @return OTA_DOWNLOAD_REQ_FAIL   OTA download request failed.
 * @return OTA_DOWNLOAD_RECV_FAIL  OTA download receive failed.
 */
int ota_download_stop(void)
{
    int ret = 0;
    ota_upgrading = 0;
    return ret;
}

/**
 * ota_download_init  OTA download init
 *
 * @param[in] void
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_DOWNLOAD_INIT_FAIL  OTA download init failed.
 * @return OTA_DOWNLOAD_CON_FAIL   OTA download connect failed.
 * @return OTA_DOWNLOAD_REQ_FAIL   OTA download request failed.
 * @return OTA_DOWNLOAD_RECV_FAIL  OTA download receive failed.
 */
int ota_download_init(void)
{
    int ret = 0;
    return ret;
}

/**
 * ota_download_deinit  OTA download deinit
 *
 * @param[in] void
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_DOWNLOAD_INIT_FAIL  OTA download init failed.
 * @return OTA_DOWNLOAD_CON_FAIL   OTA download connect failed.
 * @return OTA_DOWNLOAD_REQ_FAIL   OTA download request failed.
 * @return OTA_DOWNLOAD_RECV_FAIL  OTA download receive failed.
 */
int ota_download_deinit(void)
{
    int ret = 0;
    return ret;
}
