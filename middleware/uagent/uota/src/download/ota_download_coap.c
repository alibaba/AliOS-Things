/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "utils_md5.h"
#include "ota_service.h"
#include "ota_download.h"
#include "ota_util.h"
#include "ota_log.h"
#include "ota_manifest.h"
#include "ota_verify.h"
#include "ota_hal_os.h"
#include "iot_export.h"
#include "iot_export_coap.h"
#include "ota_hash.h"

#define OTA_COAP_BLOCK_SIZE 1024
#define COAP_OPTION_BLOCK2  23
#define MAX_RETRY           10

#define DOWNLOAD_PATH "/topic/ota/device/download/%s/%s"

static unsigned int block_cur_num       = 0;
static unsigned int block_more          = 1;
static unsigned int block_size          = OTA_COAP_BLOCK_SIZE;
static unsigned int total_size          = 0;
static int          coap_client_running = 0;
static int          retry_cnt;

static void *sem_send;
static void  iotx_response_block_handler(void *arg, void *p_response);
static void  iotx_req_block_from_server(char *path);

static ota_write_cb_t write_func = NULL;

static int ota_download_start(char *url, ota_write_cb_t func, void *cur_hash)
{
    if (!url || strlen(url) == 0 || func == NULL || cur_hash == NULL) {
        OTA_LOG_E("ota_download parms  error!\n");
        return OTA_DOWNLOAD_URL_FAIL;
    }
    int                  ret        = 0;
    uint32_t             breakpoint = 0;
    ota_hash_params      last_hash;
    ota_hash_ctx_params *hash_ctx                = NULL;
    char                 download_topic_buf[128] = { 0 };

    ota_service_manager *ctx = (ota_service_manager *)get_ota_service_manager();
    if (ctx == NULL) {
        OTA_LOG_E("ota download res is null.");
        return -1;
    }

    ret = ota_snprintf(download_topic_buf, sizeof(download_topic_buf),
                       DOWNLOAD_PATH, ctx->pk, ctx->dn);

    if (ret < 0) {
        OTA_LOG_E("ota_snprintf failed");
        return -1;
    }

    write_func = func;
    retry_cnt  = 0;

    hash_ctx = ota_get_global_hash_context();
    if (hash_ctx == NULL || hash_ctx->ctx_hash == NULL ||
        hash_ctx->ctx_size == 0) {
        OTA_LOG_E("ota get sign ctx fail\n ");
        ret = OTA_DOWNLOAD_FAIL;
        return ret;
    }

    breakpoint = ota_get_update_breakpoint();
    memset(&last_hash, 0x00, sizeof last_hash);
    ota_get_last_hash((char *)&last_hash);

    if (breakpoint && (ota_verify_hash_value(
                         last_hash, (*((ota_hash_params *)cur_hash))) == 0)) {
        OTA_LOG_I("----resume download breakpoint=%d------", breakpoint);
        ota_get_last_hash_context(hash_ctx);
        block_cur_num = breakpoint / OTA_COAP_BLOCK_SIZE;
    } else {
        breakpoint    = 0;
        block_cur_num = 0;
        if (ota_hash_init(hash_ctx->hash_method, hash_ctx->ctx_hash) != 0) {
            OTA_LOG_E("ota sign init fail\n ");
            ret = OTA_DOWNLOAD_SIGN_INIT_FAIL;
            return ret;
        }
    }
    ota_set_cur_hash((char *)cur_hash);

    coap_client_running = 1;
    sem_send            = ota_semaphore_init();

    while (coap_client_running && block_more) {
        iotx_req_block_from_server(download_topic_buf);
        ret = ota_semaphore_wait(&sem_send, 5000);
        if(ret < 0) {
            ret = OTA_DOWNLOAD_FAILED;
            OTA_LOG_E("download error %s", strerror(errno));
            break;
        }
    }
    if (block_more == 0 && block_cur_num) {
        OTA_LOG_I("----OTA_DOWNLOAD_FINISH------");
        ota_set_update_breakpoint(0);
        ret = OTA_DOWNLOAD_FINISH;
    } else {
        OTA_LOG_E("download read error %s", strerror(errno));
        ota_save_state(total_size, hash_ctx);
        ret = OTA_DOWNLOAD_FAILED;
    }

    return ret;
}

static void iotx_response_block_handler(void *arg, void *p_response)
{
    int          len = 0;
    unsigned int cur_num;
    unsigned int more;
    unsigned int size;

    unsigned char        *p_payload = NULL;
    iotx_coap_resp_code_t resp_code;
    ota_hash_ctx_params  *hash_ctx = NULL;
    hash_ctx                       = ota_get_global_hash_context();
    if (hash_ctx == NULL || hash_ctx->ctx_hash == NULL ||
        hash_ctx->ctx_size == 0) {
        OTA_LOG_E("ota get sign ctx fail\n ");
        return;
    }

    ota_IOT_CoAP_GetMessageCode(p_response, &resp_code);
    OTA_LOG_D("Message response code: %d\r\n", resp_code);
    if (resp_code == 0x45) {
        ota_IOT_CoAP_GetMessagePayload(p_response, &p_payload, &len);
        if (ota_IOT_CoAP_ParseOption_block(p_response, COAP_OPTION_BLOCK2,
                                           &cur_num, &more, &size)) {
            OTA_LOG_I("[block]: cur_num: %d, more: %d,size: %d \r\n", cur_num,
                      more, size);

            block_size = size;

            if (cur_num == block_cur_num) {
                if (more == 0) {
                    block_more = 0;
                } else {
                    block_cur_num++;
                }
                total_size += len;

                // OTA_LOG_I("size nbytes %d, %d", size, nbytes);
                if (OTA_CRYPTO_SUCCESS !=
                    ota_hash_update((const uint8_t *)p_payload, len,
                                    hash_ctx->ctx_hash)) {
                    OTA_LOG_E("ota hash update fail\n ");
                    ota_set_update_breakpoint(0);
                    return;
                }
                if (write_func != NULL) {
                    int ret = 0;
                    ret = write_func(OTA_COAP_BLOCK_SIZE, (uint8_t *)p_payload,
                                     len, 0);
                    if (ret < 0) {
                        OTA_LOG_I("write error:%d\n", ret);
                        return;
                    }
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

static void iotx_req_block_from_server(char *path)
{
    iotx_message_t message;
    message.p_payload     = NULL;
    message.payload_len   = 0;
    message.resp_callback = iotx_response_block_handler;
    message.msg_type      = IOTX_MESSAGE_CON;
    message.content_type  = IOTX_CONTENT_TYPE_JSON;

    ota_service_manager *ctx = (ota_service_manager *)get_ota_service_manager();
    if (ctx == NULL) {
        OTA_LOG_E("otacoap res is null");
        return;
    }
    iotx_coap_context_t *p_ctx = (iotx_coap_context_t *)ctx->h_coap;
    ota_IOT_CoAP_SendMessage_block(p_ctx, path, &message, COAP_OPTION_BLOCK2,
                                   block_cur_num, block_more, block_size);
}

static int ota_download_init(void)
{
    return 0;
}

static int ota_download_stop(void)
{
    return 0;
}

static ota_download dl_coap = {
    .init  = ota_download_init,
    .start = ota_download_start,
    .stop  = ota_download_stop,
};

const void *ota_get_download_coap(void)
{
    return &dl_coap;
}
