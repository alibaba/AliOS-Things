/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <aos/log.h>
#include <aos/aos.h>
#ifdef PLATFORM_NOT_LINUX
#include <aos/network.h>
#else
#include <arpa/inet.h>
#endif
#include <aos/kernel.h>
#include <netmgr.h>
#include <k_err.h>
#include "iot_import.h"
#include "iot_export.h"

#ifdef AOS_ATCMD
#include <atparser.h>
#endif


#define IOTX_PRE_DTLS_SERVER_URI "coaps://pre.iot-as-coap.cn-shanghai.aliyuncs.com:5684"
#define IOTX_PRE_NOSEC_SERVER_URI "coap://pre.iot-as-coap.cn-shanghai.aliyuncs.com:5683"

#define IOTX_ONLINE_DTLS_SERVER_URL "coaps://%s.iot-as-coap.cn-shanghai.aliyuncs.com:5684"

#define TAG "coapapp"

#define IOTX_PRODUCT_KEY         "fb6pcJ5Z5q5"
#define IOTX_DEVICE_NAME         "zt01"
#define IOTX_DEVICE_SECRET       "PyIjfWwCJZ8YpjjAI5sqATypExx66d4r"
#define IOTX_DEVICE_ID           "zt01.1"


typedef struct ota_device_info {
    const char *product_key;
    const char *device_name;
    void *h_coap;
} OTA_device_info;

OTA_device_info ota_device_info;

static void ota_init();
static void user_code_start();

static void coap_service_event(input_event_t *event, void *priv_data)
{

    if (event->type != EV_SYS) {
        return;
    }

    if (event->code != CODE_SYS_ON_COAP_AUTHED) {
        return;
    }

    ota_init();
    user_code_start();
}

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

static void iotx_response_handler(void *arg, void *p_response)
{
    int            len       = 0;
    unsigned char *p_payload = NULL;
    iotx_coap_resp_code_t resp_code;
    IOT_CoAP_GetMessageCode(p_response, &resp_code);
    IOT_CoAP_GetMessagePayload(p_response, &p_payload, &len);
    LOG("[APPL]: Message response code: %d\r\n", resp_code);

    if (p_payload) {
        LOG("[APPL]: Len: %d, Payload: %s, \r\n", len, p_payload);
    }
}

static void iotx_post_data_to_server(void *param)
{
    char               path[IOTX_URI_MAX_LEN + 1] = {0};
    iotx_message_t     message;
    iotx_deviceinfo_t  devinfo;
    message.p_payload = (unsigned char *)"{\"name\":\"hello world\"}";
    message.payload_len = strlen("{\"name\":\"hello world\"}");
    message.resp_callback = iotx_response_handler;
    message.msg_type = IOTX_MESSAGE_CON;
    message.content_type = IOTX_CONTENT_TYPE_JSON;
    iotx_coap_context_t *p_ctx = (iotx_coap_context_t *)param;
    LOG("send msg: Len: %d, Payload: %s, \r\n", message.payload_len, message.p_payload);
    iotx_set_devinfo(&devinfo);
    snprintf(path, IOTX_URI_MAX_LEN, "/topic/%s/%s/update/", (char *)devinfo.product_key,
             (char *)devinfo.device_name);

    IOT_CoAP_SendMessage(p_ctx, path, &message);
}

iotx_coap_context_t *p_ctx = NULL;

static void user_code_start()
{
    iotx_post_data_to_server((void *)p_ctx);

    //IOT_CoAP_Yield(p_ctx);
    if (m_coap_client_running) {
        aos_post_delayed_action(3000, user_code_start, NULL);
    } else {
        IOT_CoAP_Deinit(&p_ctx);
    }
}

static void coap_client_example()
{
    iotx_coap_config_t config;
    iotx_deviceinfo_t deviceinfo;

    iotx_set_devinfo(&deviceinfo);
#ifdef COAP_ONLINE
#ifdef COAP_DTLS_SUPPORT
    char url[256] = {0};
    snprintf(url, sizeof(url), IOTX_ONLINE_DTLS_SERVER_URL, deviceinfo.product_key);
    config.p_url = url;
#else
    LOG("Online environment must access with DTLS\r\n");
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

    p_ctx = IOT_CoAP_Init(&config);

    if (NULL != p_ctx) {
        m_coap_client_running = 1;
        aos_register_event_filter(EV_SYS,  coap_service_event, NULL);
        IOT_CoAP_DeviceNameAuth(p_ctx);
    } else {
        aos_post_delayed_action(3000, coap_client_example, NULL); //try again
        LOG("IoTx CoAP init failed\r\n");
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
#if AOS_ATCMD
    at.set_mode(ASYN);
    at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX, 
            AT_RECV_FAIL_POSTFIX, AT_SEND_DELIMITER, 1000);
#endif
    
#ifdef WITH_SAL
    sal_init();
#endif
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    aos_set_log_level(AOS_LL_INFO);
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
