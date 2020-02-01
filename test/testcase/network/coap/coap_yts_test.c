/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "aos/kernel.h"

#include "yunit/yunit.h"
#include "yunit/yts.h"

#include <coap.h>
#include "linkkit/wrappers/wrappers.h"
#include "linkkit/infra/infra_compat.h"

#include "ulog/ulog.h"

#define TAG "coap_yts"

/* online url */
#define IOTX_ONLINE_DTLS_SERVER_URL     "coaps://%s.coap.cn-shanghai.link.aliyuncs.com:5684"
#define IOTX_ONLINE_PSK_SERVER_URL      "coap-psk://%s.coap.cn-shanghai.link.aliyuncs.com:5682"

// for demo only
#define IOTX_PRODUCT_KEY      "a1X2bEnP82z"
#define IOTX_DEVICE_NAME      "test_coap_01"
#define IOTX_DEVICE_SECRET    "6DY0Kp6wzjEe5aU29mDifGDBJ5sxC2NL"

static int finish = 0;
static void iotx_response_handler(void *arg, void *p_response)
{
    int  ret = 0;        
    int  len = 0;
    unsigned char *p_payload = NULL;
    iotx_coap_resp_code_t resp_code;

    ret = IOT_CoAP_GetMessageCode(p_response, &resp_code);
    YUNIT_ASSERT( ret >= 0);

    ret = IOT_CoAP_GetMessagePayload(p_response, &p_payload, &len);
    YUNIT_ASSERT((ret >= 0 && len > 0) || (len == 0));

    LOGI(TAG, "[APPL]: Message response code: 0x%x", resp_code);
    if(p_payload != NULL)
    {
        LOGI(TAG, "[APPL]: Len: %d, Payload: %s", len, p_payload);
    }
    else
    {
        LOGW(TAG, "[APPL]: Len: %d, Payload: (null)", len);
    }

    finish = 1;
}

static void iotx_post_data_to_server(void *param)
{
    int ret = 0;
    char path[IOTX_URI_MAX_LEN + 1] = {0};
    iotx_coap_context_t *p_ctx = (iotx_coap_context_t *)param;
    iotx_message_t message;

    snprintf(path, IOTX_URI_MAX_LEN, "/topic/%s/%s/user/update/", IOTX_PRODUCT_KEY, IOTX_DEVICE_NAME);

    memset(&message, 0, sizeof(iotx_message_t));
    message.p_payload = (unsigned char *)"{\"name\":\"hello world\"}";
    message.payload_len = strlen("{\"name\":\"hello world\"}");
    message.resp_callback = iotx_response_handler;
    message.msg_type = IOTX_MESSAGE_CON;
    message.content_type = IOTX_CONTENT_TYPE_JSON;

    ret = IOT_CoAP_SendMessage(p_ctx, path, &message);
    YUNIT_ASSERT( ret >= 0);
}

static int iotx_get_devinfo(iotx_coap_device_info_t *p_devinfo)
{
    if (NULL == p_devinfo) {
        return IOTX_ERR_INVALID_PARAM;
    }

    memset(p_devinfo, 0x00, sizeof(iotx_coap_device_info_t));

    /* get device info */
    strncpy(p_devinfo->product_key, IOTX_PRODUCT_KEY, strlen(IOTX_PRODUCT_KEY));
    strncpy(p_devinfo->device_name, IOTX_DEVICE_NAME, strlen(IOTX_DEVICE_NAME));
    strncpy(p_devinfo->device_secret, IOTX_DEVICE_SECRET, strlen(IOTX_DEVICE_SECRET));
    memset(p_devinfo->device_id, 0, IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 2);
    snprintf(p_devinfo->device_id, IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 2,
                 "%s.%s", p_devinfo->product_key, p_devinfo->device_name);

    LOGD(TAG, "*****The Product Key  : %s *****", p_devinfo->product_key);
    LOGD(TAG, "*****The Device Name  : %s *****", p_devinfo->device_name);
    LOGD(TAG, "*****The Device Secret: %s *****", p_devinfo->device_secret);
    LOGD(TAG, "*****The Device ID    : %s *****", p_devinfo->device_id);
    return IOTX_SUCCESS;
}

static char url[256] = {0};
static iotx_coap_config_t      config;
static iotx_coap_device_info_t deviceinfo;
static iotx_coap_context_t *p_ctx = NULL;

static void coap_process(void)
{
    int ret = 0;
    int count = 0;
    finish = 0;

    aos_set_log_level(AOS_LL_DEBUG);

    p_ctx = IOT_CoAP_Init(&config);
    YUNIT_ASSERT( p_ctx != NULL );
    if (!p_ctx) {
        LOGE(TAG, "IoTx CoAP init failed");
        return;
    }

    ret = IOT_CoAP_DeviceNameAuth(p_ctx);
    YUNIT_ASSERT( ret >= 0 );

    while (!finish) {
        if (count == 0 || count == 11) {
           iotx_post_data_to_server((void *)p_ctx);
           count = 1;
        }

        ret = IOT_CoAP_Yield(p_ctx);
        YUNIT_ASSERT( ret >= 0 );
        count ++;
    }

    IOT_CoAP_Deinit(&p_ctx);
    YUNIT_ASSERT( p_ctx == NULL );
}

static void yts_coap_dtls(void)
{  
    memset(&config, 0x00, sizeof(iotx_coap_config_t));
    iotx_get_devinfo(&deviceinfo);

    snprintf(url, sizeof(url), IOTX_ONLINE_DTLS_SERVER_URL, IOTX_PRODUCT_KEY);
    config.p_url = url;
    config.p_devinfo = (iotx_coap_device_info_t *)&deviceinfo;
    config.wait_time_ms = 3000;

    coap_process(); 
}

static void yts_coap_psk(void)
{
    memset(&config, 0x00, sizeof(iotx_coap_config_t));

    iotx_get_devinfo(&deviceinfo);

    snprintf(url, sizeof(url), IOTX_ONLINE_PSK_SERVER_URL, IOTX_PRODUCT_KEY);
    config.p_url = url;
    config.p_devinfo = (iotx_coap_device_info_t *)&deviceinfo;
    config.wait_time_ms = 3000;

    coap_process();
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

static yunit_test_case_t aos_coap_testcases[] = {
    { "yts coap dtls",      yts_coap_dtls},
    { "yts coap psk",       yts_coap_psk},
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

