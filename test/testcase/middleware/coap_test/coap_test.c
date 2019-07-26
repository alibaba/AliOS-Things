/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "aos/kernel.h"
#include "ulog/ulog.h"

#include "yts/yunit.h"
#include "yts/yts.h"

#include "iot_import.h"
#include "iot_export.h"
#include "CoAPExport.h"

#define IOTX_PRODUCT_KEY         "vtkkbrpmxmF"
#define IOTX_DEVICE_NAME         "IoTxCoAPTestDev"
#define IOTX_DEVICE_SECRET       "Stk4IUErQUBc1tWRWEKWb5ACra4hFDYF"
#define IOTX_DEVICE_ID           "IoTxCoAPTestDev.1"

#define IOTX_PRE_DTLS_SERVER_URI "coaps://pre.iot-as-coap.cn-shanghai.aliyuncs.com:5684"
#define IOTX_PRE_NOSEC_SERVER_URI "coap://pre.iot-as-coap.cn-shanghai.aliyuncs.com:5683"

#define IOTX_ONLINE_DTLS_SERVER_URL "coaps://%s.iot-as-coap.cn-shanghai.aliyuncs.com:5684"

#if defined(COAP_ONLINE)
    #define COAP_URL  IOTX_ONLINE_DTLS_SERVER_URL
#else
    #ifdef COAP_DTLS_SUPPORT
    #define COAP_URL  IOTX_PRE_DTLS_SERVER_URI
    #else
    #define COAP_URL  IOTX_PRE_NOSEC_SERVER_URI
    #endif
#endif

#define TAG "coaptest"

char m_coap_client_running = 0;

static int iotx_set_devinfo(iotx_deviceinfo_t *p_devinfo)
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

static void iotx_response_handler(void * arg, void * p_response)
{
    int            len       = 0;
    unsigned char *p_payload = NULL;
    iotx_coap_resp_code_t resp_code;
    IOT_CoAP_GetMessageCode(p_response, &resp_code);
    IOT_CoAP_GetMessagePayload(p_response, &p_payload, &len);
    printf("[APPL]: Message response code: %d\r\n", resp_code);
    printf("[APPL]: Len: %d, Payload: %s, \r\n", len, p_payload);
}

static void iotx_post_data_to_server(void *param)
{
    int ret;
    char               path[IOTX_URI_MAX_LEN+1] = {0};
    iotx_message_t     message;
    iotx_deviceinfo_t  devinfo;
    message.p_payload = (unsigned char *)"{\"name\":\"hello world\"}";
    message.payload_len = strlen("{\"name\":\"hello world\"}");
    message.resp_callback = iotx_response_handler;
    message.msg_type = IOTX_MESSAGE_CON;
    message.content_type = IOTX_CONTENT_TYPE_JSON;
    iotx_coap_context_t *p_ctx = (iotx_coap_context_t *)param;

    iotx_set_devinfo(&devinfo);
    snprintf(path, IOTX_URI_MAX_LEN, "/topic/%s/%s/update/", (char *)devinfo.product_key,
                                            (char *)devinfo.device_name);

    ret = IOT_CoAP_SendMessage(p_ctx, path, &message);
//    YUNIT_ASSERT(0 == ret);
}

iotx_coap_context_t *p_ctx = NULL;

static void user_code_start()
{
    int ret;

    iotx_post_data_to_server((void *)p_ctx);
    //ret = IOT_CoAP_Yield(p_ctx);
    YUNIT_ASSERT(IOTX_SUCCESS == ret);
    if (m_coap_client_running) {
        aos_post_delayed_action(3000,user_code_start,NULL);
    } else {
        IOT_CoAP_Deinit(&p_ctx);
        YUNIT_ASSERT(NULL == p_ctx);
    }
}

int times = 0;
static void test_coap_start(void)
{
    int ret;
    iotx_coap_config_t config;
    iotx_deviceinfo_t deviceinfo;

    m_coap_client_running = 1;

    #ifdef COAP_ONLINE
        #ifdef COAP_DTLS_SUPPORT
            char url[256] = {0};
            snprintf(url, sizeof(url), IOTX_ONLINE_DTLS_SERVER_URL, IOTX_PRODUCT_KEY);
            config.p_url = url;
        #else
            printf("Online environment must access with DTLS\r\n");
            YUNIT_ASSERT(false);
        #endif
    #else
        #ifdef COAP_DTLS_SUPPORT
            config.p_url = IOTX_PRE_DTLS_SERVER_URI;
        #else
            config.p_url = IOTX_PRE_NOSEC_SERVER_URI;
        #endif
    #endif
    iotx_set_devinfo(&deviceinfo);
    config.p_devinfo = &deviceinfo;

    p_ctx = IOT_CoAP_Init(&config);
    YUNIT_ASSERT(NULL != p_ctx);
    if(NULL != p_ctx){
        ret = iotx_get_well_known(p_ctx);
        YUNIT_ASSERT(IOTX_SUCCESS == ret);
        ret = IOT_CoAP_DeviceNameAuth(p_ctx);
        YUNIT_ASSERT(IOTX_SUCCESS == ret);
        do{
            times ++;
            iotx_post_data_to_server((void *)p_ctx);
            //ret = IOT_CoAP_Yield(p_ctx);
            YUNIT_ASSERT(IOTX_SUCCESS == ret);
        }while(times<2);
        IOT_CoAP_Deinit(&p_ctx);
        YUNIT_ASSERT(NULL == p_ctx);
    }
    else{
        printf("IoTx CoAP init failed\r\n");
        YUNIT_ASSERT(NULL != p_ctx);
    }

}

static int init(void)
{
    return 0;
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

/* IOT_CoAP_Init: config.url非法coaps域名 */
static void ut_coap_init(void)
{
    iotx_coap_config_t  config;
    iotx_deviceinfo_t devinfo;
    void *p_ctx = NULL;

    iotx_set_devinfo(&devinfo);
//case1
    config.p_devinfo = &devinfo;
    config.p_url = "coaps://pre.iot-as-coap.cn-shanghai.aliyuncs.cn:5684";
    p_ctx = IOT_CoAP_Init(&config);
    if (p_ctx != NULL) {
        IOT_CoAP_Deinit(&p_ctx);
    }
    YUNIT_ASSERT(NULL == p_ctx);
//case2
    config.p_devinfo = NULL;
    config.p_url = "coaps://pre.iot-as-coap.cn-shanghai.aliyuncs.cn:5684";
    if (p_ctx != NULL) {
        IOT_CoAP_Deinit(&p_ctx);
    }
    YUNIT_ASSERT(NULL == p_ctx);
//case3
    config.p_devinfo = &devinfo;
    config.p_url = NULL;
    if (p_ctx != NULL) {
        IOT_CoAP_Deinit(&p_ctx);
    }
    YUNIT_ASSERT(NULL == p_ctx);
//case4
   p_ctx = IOT_CoAP_Init(NULL);
    if (p_ctx != NULL) {
        IOT_CoAP_Deinit(&p_ctx);
    }
    YUNIT_ASSERT(NULL == p_ctx);
}

static void ut_coap_deinit(void)
{
    void *p_ctx;
    iotx_coap_config_t config = {0};
    iotx_deviceinfo_t devinfo;
    iotx_set_devinfo(&devinfo);
    config.p_devinfo = &devinfo;

    config.p_url = COAP_URL;
    p_ctx = IOT_CoAP_Init(&config);
    YUNIT_ASSERT(NULL != p_ctx);

    IOT_CoAP_Deinit(&p_ctx);
    YUNIT_ASSERT(NULL == p_ctx);
}

/* IOT_CoAP_DeviceNameAuth: p_context=NULL */
static void ut_coap_auth(void)
{
    int ret = IOT_CoAP_DeviceNameAuth(NULL);
    YUNIT_ASSERT(ret < 0);
}

// IOT_CoAP_SendMessage: p_context=NULL
static void ut_coap_sendmsg(void)
{
    char uri[IOTX_URI_MAX_LEN+1] = {0};
    iotx_message_t    message;
    int ret = -1;

    message.p_payload     = "{\"name\":\"hello world\"}";
    message.payload_len   = strlen("{\"name\":\"hello world\"}");
    message.resp_callback = iotx_response_handler;
    message.msg_type      = IOTX_MESSAGE_NON;
    message.content_type  = IOTX_CONTENT_TYPE_JSON;

    ret = IOT_CoAP_SendMessage(NULL, uri, &message);
    YUNIT_ASSERT(ret < 0);
}

// IOT_CoAP_Yield(NULL)
// static void ut_coap_yield(void)
// {
//     int ret = IOT_CoAP_Yield(NULL);
//     YUNIT_ASSERT(ret<0);
// }

// IOT_CoAP_GetMessageCode(NULL,NULL)
static void ut_coap_getmsg(void)
{
    int ret = -1;

    ret = IOT_CoAP_GetMessageCode(NULL, NULL);
    ret = IOT_CoAP_GetMessagePayload(NULL, NULL, NULL);
    YUNIT_ASSERT(ret<0);
}

// (NULL,NULL)
static void ut_coap_nullcase(void)
{
    int ret = -1;
    CoAPMessage msg;
    iotx_event_notifyer(0, NULL);
    msg.user = NULL;
    iotx_event_notifyer(0, &msg);
    YUNIT_ASSERT(ret<0);
}


static yunit_test_case_t aos_coap_testcases[] = {
    { "init", ut_coap_init},
    { "deinit", ut_coap_deinit},
    { "auth", ut_coap_auth},
    { "sendmsg", ut_coap_sendmsg},
    { "getmsg", ut_coap_getmsg},
    // { "yield", ut_coap_yield},
    { "null_case", ut_coap_nullcase},
    { "coap_start", test_coap_start},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "coap", init, cleanup, setup, teardown, aos_coap_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_coap(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_coap);
