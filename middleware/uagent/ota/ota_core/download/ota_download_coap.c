/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "utils_md5.h"
#include "ota/ota_service.h"
#include "ota_log.h"
#include "ota_hal_os.h"
#include "ota_hal_plat.h"
#include "ota_verify.h"

#define OTA_COAP_BLOCK_SIZE 1024
#define COAP_OPTION_BLOCK2  23
#define MAX_RETRY           10

#define DOWNLOAD_PATH "/topic/ota/device/download/%s/%s"

static int block_cur_num = 0;
static int block_more = 1;
static int block_size = OTA_COAP_BLOCK_SIZE;
static int total_size = 0;
static int coap_client_running = 0;
static int retry_cnt = 5;

static void *sem_send;
static void  iotx_response_block_handler(void *arg, void *p_response);

static int ota_download_start(void *pctx)
{
    int ret = 0;
    int breakpoint = 0;
    char path[128] = {0};
    ota_service_t* ctx = pctx;
    if (!ctx || !(ctx->h_ch)) {
        return -1;
    }

    ret = ota_snprintf(path, sizeof(path), DOWNLOAD_PATH, ctx->pk, ctx->dn);
    if (ret < 0) {
        return -1;
    }

    retry_cnt  = 0;
    breakpoint = ota_get_break_point();
    if (breakpoint) {
        block_cur_num = breakpoint / OTA_COAP_BLOCK_SIZE;
    } else {
        breakpoint    = 0;
        block_cur_num = 0;
    }
    coap_client_running = 1;
    sem_send = ota_semaphore_create();
    while (coap_client_running && block_more) {
        iotx_message_t message;
        message.p_payload = NULL;
        message.payload_len = 0;
        message.resp_callback = iotx_response_block_handler;
        message.msg_type = COAP_MESSAGE_CON;
        message.content_type = COAP_CONTENT_TYPE_JSON;
        ota_coap_send_block(ctx->h_ch, path, &message, COAP_OPTION_BLOCK2, block_cur_num, block_more, block_size);
        ret = ota_semaphore_wait(&sem_send, 5000);
        if(ret < 0) {
            ret = OTA_DOWNLOAD_FAIL;
            OTA_LOG_E("download error %s", strerror(errno));
            break;
        }
    }
    if (block_more == 0 && block_cur_num) {
        OTA_LOG_I("download finish.");
        ota_set_break_point(0);
    } else {
        ota_set_break_point(breakpoint);
    }
    return ret;
}

static void iotx_response_block_handler(void *arg, void *p_response)
{
    int len = 0;
    int cur_num = 0;
    int more = 0;
    int size = 0;
    int breakpoint = 0;
    char *p_payload = NULL;
    iotx_coap_resp_code_t resp_code;
    ota_coap_get_code(p_response, &resp_code);
    if (resp_code == 0x45) {
        ota_coap_get_payload(p_response, (const char**)&p_payload, &len);
        if (ota_coap_parse_block(p_response, COAP_OPTION_BLOCK2, &cur_num, &more, &size)) {
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
                ret = ota_hal_write(&breakpoint, p_payload, len);
                if (ret < 0) {
                      OTA_LOG_E("write err:%d\n", ret);
                      return;
                }
            }
        } else {
            if (retry_cnt++ > MAX_RETRY) {
                coap_client_running = 0;
            }
        }
    } else {
        if (retry_cnt++ > MAX_RETRY) {
            coap_client_running = 0;
        }
    }
    ota_semaphore_post(&sem_send);
}

static int ota_download_stop(void)
{
    return 0;
}

static ota_download_t dl_coap = {
    .start = ota_download_start,
    .stop  = ota_download_stop,
};

ota_download_t *ota_get_download(void)
{
    return &dl_coap;
}
