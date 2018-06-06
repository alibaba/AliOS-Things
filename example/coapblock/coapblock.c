/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#ifdef PLATFORM_NOT_LINUX
#include <lwip/inet.h>
#else
#include <arpa/inet.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <aos/log.h>
#include <aos/aos.h>
#include <netmgr.h>
#include <k_err.h>
#include "iot_import.h"
#include "iot_export.h"

#define IOTX_PRE_DTLS_SERVER_URI "coaps://pre.iot-as-coap.cn-shanghai.aliyuncs.com:5684"
#define IOTX_PRE_NOSEC_SERVER_URI  "coap://iot-as-coap.alibaba.net:5683" // "coap://pre.iot-as-coap.cn-shanghai.aliyuncs.com:5683"//

#define IOTX_ONLINE_DTLS_SERVER_URL "coaps://%s.iot-as-coap.cn-shanghai.aliyuncs.com:5684"


#define TAG "coapapp"

#define IOTX_PRODUCT_KEY         "fb6pcJ5Z5q5"
#define IOTX_DEVICE_NAME         "zt01"
#define IOTX_DEVICE_SECRET       "PyIjfWwCJZ8YpjjAI5sqATypExx66d4r"
#define IOTX_DEVICE_ID           "zt01.1"

#define OPTION_BLOCK2            23

typedef struct ota_device_info {
    const char *product_key;
    const char *device_name;
    void *h_coap;
} OTA_device_info;

OTA_device_info ota_device_info;

unsigned int g_cur_num = 0;
unsigned int g_more = 1;
unsigned int g_size = 512;
unsigned char g_option_val[3] = {0};

aos_sem_t sem_send;
//aos_sem_t sem_rec;
static void ota_init();

int iotx_set_devinfo(iotx_deviceinfo_t *p_devinfo)
{
    if (NULL == p_devinfo) {
        return IOTX_ERR_INVALID_PARAM;
    }

    memset(p_devinfo, 0x00, sizeof(iotx_deviceinfo_t));
    strncpy(p_devinfo->device_id,    IOTX_DEVICE_ID,   IOTX_DEVICE_ID_LEN);
    strncpy(p_devinfo->product_key,  IOTX_PRODUCT_KEY, IOTX_PRODUCT_KEY_LEN);
    strncpy(p_devinfo->device_secret, IOTX_DEVICE_SECRET, IOTX_DEVICE_SECRET_LEN);
    strncpy(p_devinfo->device_name,  IOTX_DEVICE_NAME, IOTX_DEVICE_NAME_LEN);

    return IOTX_SUCCESS;
}

char m_coap_client_running = 0;
int coap_client_init = 0;

static void iotx_response_block_handler(void *arg, void *p_response)
{
    int            len       = 0;
    unsigned int cur_num;
    unsigned int more;
    unsigned int size;

    unsigned char *p_payload = NULL;
    iotx_coap_resp_code_t resp_code;

    IOT_CoAP_GetMessageCode(p_response, &resp_code);
    IOT_CoAP_GetMessagePayload(p_response, &p_payload, &len);
    printf("[APPL]: Message response code: %d\r\n", resp_code);
    if (IOT_CoAP_ParseOption_block(p_response, OPTION_BLOCK2, &cur_num, &more, &size)) {
        printf("[block]: cur_num: %d, more: %d,size: %d \r\n", cur_num, more, size);
        if (cur_num == g_cur_num) {
            g_cur_num++;
        }
        g_size = size;
        if (more == 0) {
            g_more = 0;
        }
        aos_sem_signal(&sem_send);
    }
    //printf("[APPL]: Len: %d, Payload: %s, \r\n", len, p_payload);
}


static void iotx_req_block_from_server(void *param)
{

    iotx_message_t     message;

    message.p_payload = NULL;
    message.payload_len = 0;
    message.resp_callback = iotx_response_block_handler;
    message.msg_type = IOTX_MESSAGE_CON;
    message.content_type = IOTX_CONTENT_TYPE_JSON;

    iotx_coap_context_t *p_ctx = (iotx_coap_context_t *)param;

    IOT_CoAP_SendMessage_block(p_ctx, "/large", &message,
                               OPTION_BLOCK2, g_cur_num, g_more, g_size);
}

iotx_coap_context_t *p_ctx = NULL;

static void user_code_start()
{
    iotx_post_data_to_server((void *)p_ctx);
    // IOT_CoAP_Yield(p_ctx);
    if (m_coap_client_running) {
        aos_post_delayed_action(3000, user_code_start, NULL);
    } else {
        IOT_CoAP_Deinit(&p_ctx);
    }
}


static void block_test_start()
{
    //IOT_CoAP_Yield(p_ctx);
    if (m_coap_client_running && g_more) {
        iotx_req_block_from_server((void *)p_ctx);
        //aos_sem_signal(&sem_rec);
        aos_sem_wait(&sem_send, 2000);
        aos_post_delayed_action(10, block_test_start, NULL);

    } else {
        LOG("=======IOT_CoAP_Deinit========");
        IOT_CoAP_Deinit(&p_ctx);
        aos_sem_free(&sem_send);
        // aos_sem_free(&sem_rec);
    }
}

// void task_rec(void *para){
//     while(m_coap_client_running&&g_more){
//         aos_sem_wait(&sem_rec, AOS_WAIT_FOREVER);
//       IOT_CoAP_Yield(p_ctx);
//     }
// }


static void coap_client_example()
{
    iotx_coap_config_t config;
    iotx_deviceinfo_t deviceinfo;

    m_coap_client_running = 1;

    iotx_set_devinfo(&deviceinfo);
#ifdef COAP_ONLINE
#ifdef COAP_DTLS_SUPPORT
    char url[256] = {0};
    snprintf(url, sizeof(url), IOTX_ONLINE_DTLS_SERVER_URL, deviceinfo.product_key);
    config.p_url = url;
#else
    printf("Online environment must access with DTLS\r\n");
    return -1;
#endif
#else
#ifdef COAP_DTLS_SUPPORT
    config.p_url = IOTX_PRE_DTLS_SERVER_URI;
#else
    config.p_url = IOTX_PRE_NOSEC_SERVER_URI;
#endif
#endif

    config.p_devinfo = &deviceinfo;

    aos_sem_new(&sem_send, 0);

    //aos_sem_new(&sem_rec, 0);
    p_ctx = IOT_CoAP_Init(&config);
    if (NULL != p_ctx) {
        IOT_CoAP_DeviceNameAuth(p_ctx);
        block_test_start();
        //  aos_task_new("coap rec", task_rec, 0, 2048);
        //  ota_init();
    } else {
        printf("IoTx CoAP init failed\r\n");
    }
}

void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type == EV_WIFI && event->code == CODE_WIFI_ON_GOT_IP) {
        if (coap_client_init) {
            return;
        }
        coap_client_init = 1;
        coap_client_example();
    }
}

int application_start(void)
{
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    aos_set_log_level(AOS_LL_DEBUG);
    netmgr_init();
    netmgr_start(true);

    aos_loop_run();

    /* never return */
    return 0;
}

static void ota_init()
{
    ota_device_info.product_key = IOTX_PRODUCT_KEY;
    ota_device_info.device_name = IOTX_DEVICE_NAME;
    ota_device_info.h_coap = p_ctx;
    aos_post_event(EV_SYS, CODE_SYS_ON_START_FOTA, (long unsigned int)&ota_device_info);
}
