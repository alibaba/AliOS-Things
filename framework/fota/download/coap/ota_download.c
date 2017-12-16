/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include <aos/network.h>

#include "iot_import.h"
#include "iot_export.h"

#include "md5.h"
#include "ota_download.h"
#include "ota_util.h"
#include "ota_log.h"
#include "ota_update_manifest.h"


#define  OTA_COAP_BLOCK_SIZE   1024
#define  COAP_OPTION_BLOCK2    23
#define  MAX_RETRY             10

#define DOWNLOAD_PATH   "/topic/ota/device/download/%s/%s"

MD5_CTX  g_ctx;

typedef struct ota_device_info {
    const char *product_key;
    const char *device_name;
    void *h_coap;
} OTA_device_info; 

extern OTA_device_info g_ota_device_info;

static unsigned int block_cur_num=0;
static unsigned int block_more=1;
static unsigned int block_size=OTA_COAP_BLOCK_SIZE;
static unsigned int total_size=0;

static int coap_client_running=0;
static int retry_cnt;

static aos_sem_t sem_send;
//static aos_sem_t sem_rec;  

static void iotx_response_block_handler(void * arg, void * p_response);
static void iotx_req_block_from_server( char  *path);
static void task_rec(void *para);

static write_flash_cb_t write_func=NULL;



int ota_download(char *url, write_flash_cb_t func, char *md5)
{
    //url="/large";   
    if (!url || strlen(url) == 0 || func == NULL || md5 == NULL) {
        OTA_LOG_E("ota_download parms  error!\n");
        return OTA_DOWNLOAD_URL_FAIL;
    }
    int ret = 0;
    uint32_t breakpoint = 0;
    char last_md5[33] = {0};
    char download_topic_buf[128]={0};

    ret = snprintf(download_topic_buf,
                   sizeof(download_topic_buf),
                   DOWNLOAD_PATH,
                   g_ota_device_info.product_key,
                   g_ota_device_info.device_name);

    if (ret < 0) {
        LOG("snprintf failed");
        return -1;
    }

    write_func=func;
    retry_cnt=0;

    breakpoint = ota_get_update_breakpoint();
    ota_get_last_MD5(last_md5);

    if (breakpoint && !strncmp(last_md5, md5, 32)) {
        OTA_LOG_I("----resume download breakpoint=%d------",breakpoint);
        ota_get_last_MD5_context(&g_ctx);
        block_cur_num=breakpoint/OTA_COAP_BLOCK_SIZE;
    } else {
        breakpoint = 0;
        block_cur_num=0;
        MD5_Init(&g_ctx);
    }
    ota_set_cur_MD5(md5);

    coap_client_running=1;
    aos_sem_new(&sem_send, 0);
    //aos_sem_new(&sem_rec, 0);

    //aos_task_new("coap rec", task_rec, 0, 4096);
    while(coap_client_running&&block_more){
        iotx_req_block_from_server(download_topic_buf);
        //aos_sem_signal(&sem_rec);
        aos_sem_wait(&sem_send, 5000);
    }
    //aos_sem_signal(&sem_rec);

    if (block_more == 0&&block_cur_num) {
        OTA_LOG_I("----OTA_DOWNLOAD_FINISH------");
        ota_set_update_breakpoint(0);
        ret = OTA_DOWNLOAD_FINISH;
    } else {
        OTA_LOG_E("download read error %s" , strerror(errno));
        save_state(total_size, &g_ctx);
        ret = OTA_DOWNLOAD_FAILED;
    }  

    return ret;
}

static void iotx_response_block_handler(void * arg, void * p_response)
{
    int            len       = 0;
    unsigned int cur_num;
    unsigned int more;
    unsigned int size;

    unsigned char *p_payload = NULL;
    iotx_coap_resp_code_t resp_code;
    
    IOT_CoAP_GetMessageCode(p_response, &resp_code);
    OTA_LOG_D("Message response code: %d\r\n", resp_code);
    if(resp_code==0x45){
        IOT_CoAP_GetMessagePayload(p_response, &p_payload, &len);

        if(IOT_CoAP_ParseOption_block(p_response,COAP_OPTION_BLOCK2,&cur_num,&more,&size)){
            OTA_LOG_I("[block]: cur_num: %d, more: %d,size: %d \r\n",cur_num,more,size);

            block_size=size;

            if(cur_num==block_cur_num){
                if(more==0){
                    block_more=0;
                }
                else{
                    block_cur_num++;
                }
                total_size += len;

                //OTA_LOG_I("size nbytes %d, %d", size, nbytes);
                MD5_Update(&g_ctx, (const uint8_t *) p_payload, len);
                if(write_func!=NULL){
                    write_func(OTA_COAP_BLOCK_SIZE, (uint8_t *) p_payload, len, 0);
                }
                
            }
        }else{
            if(retry_cnt++>MAX_RETRY){
                coap_client_running=0; 
            }    
        }
    } else {
        if(retry_cnt++>MAX_RETRY){
            coap_client_running=0; 
        }        
    }
    aos_sem_signal(&sem_send);
}

static void iotx_req_block_from_server( char * path)
{

    iotx_message_t     message;

    message.p_payload =NULL;
    message.payload_len =0;
    message.resp_callback = iotx_response_block_handler;
    message.msg_type = IOTX_MESSAGE_CON;
    message.content_type = IOTX_CONTENT_TYPE_JSON;

    iotx_coap_context_t *p_ctx = (iotx_coap_context_t *)g_ota_device_info.h_coap;

    IOT_CoAP_SendMessage_block(p_ctx, path, &message,
                               COAP_OPTION_BLOCK2,block_cur_num,block_more,block_size);    
}


// static void task_rec(void *para)

// {
//     while(coap_client_running&&block_more){
//         aos_sem_wait(&sem_rec, AOS_WAIT_FOREVER);
//       IOT_CoAP_Yield(g_ota_device_info.h_coap);
//     }   
// }

int check_md5(const char *buffer, const int32_t len)
{
    uint8_t digest[16] = {0};
    char digest_str[33] = {0};
    int i = 0;
    OTA_LOG_D("digest=%s", buffer);
    MD5_Final((uint8_t *)digest, &g_ctx);
    for (; i < 16 ; i++) {
        snprintf(digest_str + i * 2, 2 + 1, "%02X", digest[i]);
    }
    OTA_LOG_I("url md5=%s", buffer);
    OTA_LOG_I("digestMD5=%s", digest_str);
    if (strncmp(digest_str, buffer, 32)) {
        OTA_LOG_E("update_packet md5 check FAIL!");
        return -1;
    }
    return 0;
}
