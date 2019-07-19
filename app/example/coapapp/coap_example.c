/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if !defined(_WIN32)
    #include <unistd.h>
#endif
#include "network/coap.h"
#include "wrappers.h"
#include "app_entry.h"
#include "infra_compat.h"

#include "ulog/ulog.h"

#define TAG "COAPAPP"

#define IOTX_PRE_DTLS_SERVER_URI        "coaps://pre.coap.cn-shanghai.link.aliyuncs.com:5684"
#define IOTX_PRE_NOSEC_SERVER_URI       "coap://pre.coap.cn-shanghai.link.aliyuncs.com:5683"
#define IOTX_PRE_PSK_SERVER_URI         "coap-psk://pre.coap.cn-shanghai.link.aliyuncs.com:5683"

/* online url */
#define IOTX_ONLINE_DTLS_SERVER_URL     "coaps://%s.coap.cn-shanghai.link.aliyuncs.com:5684"
#define IOTX_ONLINE_PSK_SERVER_URL      "coap-psk://%s.coap.cn-shanghai.link.aliyuncs.com:5682"

// for demo only
#define IOTX_PRODUCT_KEY      "a1X2bEnP82z"
#define IOTX_DEVICE_NAME      "test_06"
#define IOTX_DEVICE_SECRET    "wQ1xOzFH3kLdjCTLfi8Xbw4otRz0lHoq"


char m_coap_client_running = 0;
char m_coap_reconnect = 0;

static void iotx_response_handler(void *arg, void *p_response)
{
    int            len       = 0;
    unsigned char *p_payload = NULL;
    iotx_coap_resp_code_t resp_code;
    IOT_CoAP_GetMessageCode(p_response, &resp_code);
    IOT_CoAP_GetMessagePayload(p_response, &p_payload, &len);
    LOGI(TAG, "[APPL]: Message response code: 0x%x", resp_code);
    if(p_payload != NULL)
    {
        LOGI(TAG, "[APPL]: Len: %d, Payload: %s", len, p_payload);
    }
    else
    {
        LOGW(TAG, "[APPL]: Len: %d, Payload: (null)", len);
    }
}

static void iotx_post_data_to_server(void *param)
{
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

    IOT_CoAP_SendMessage(p_ctx, path, &message);
}

static int iotx_get_devinfo(iotx_coap_device_info_t *p_devinfo)
{
    if (NULL == p_devinfo) {
        return IOTX_ERR_INVALID_PARAM;
    }

    memset(p_devinfo, 0x00, sizeof(iotx_coap_device_info_t));

    /* get device info */
    HAL_GetProductKey(p_devinfo->product_key);
    HAL_GetDeviceName(p_devinfo->device_name);
    HAL_GetDeviceSecret(p_devinfo->device_secret);
    memset(p_devinfo->device_id, 0, IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 2);
    snprintf(p_devinfo->device_id, IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 2,
                 "%s.%s", p_devinfo->product_key, p_devinfo->device_name);

    LOGD(TAG, "*****The Product Key  : %s *****", p_devinfo->product_key);
    LOGD(TAG, "*****The Device Name  : %s *****", p_devinfo->device_name);
    LOGD(TAG, "*****The Device Secret: %s *****", p_devinfo->device_secret);
    LOGD(TAG, "*****The Device ID    : %s *****", p_devinfo->device_id);
    return IOTX_SUCCESS;
}

static void show_usage()
{
    LOGI(TAG, "\r\nusage: coap-example [OPTION]...");
    LOGI(TAG, "\t-e pre|online|daily\t\tSet the cloud environment.");
    LOGI(TAG, "\t-s nosec|dtls|psk  \t\tSet the security setting.");
    LOGI(TAG, "\t-l                 \t\tSet the program run loop.");
    LOGI(TAG, "\t-r                 \t\tTesting the DTLS session ticket.");
    LOGI(TAG, "\t-h                 \t\tShow this usage.");
}

int linkkit_main(void *paras)
{

    int argc = 0;
    char **argv = NULL;

    int                     count = 0;
    char                    secur[32] = {0};
    char                    env[32] = {0};
    int                     opt;
    iotx_coap_config_t      config;
    iotx_coap_device_info_t deviceinfo;
    char url[256] = {0};

    if (paras != NULL) {
        app_main_paras_t *p = (app_main_paras_t *)paras;
        argc = p->argc;
        argv = p->argv;
    }

        /* set device info use HAL function */
    HAL_SetProductKey(IOTX_PRODUCT_KEY);
    HAL_SetDeviceName(IOTX_DEVICE_NAME);
    HAL_SetDeviceSecret(IOTX_DEVICE_SECRET);

    IOT_SetLogLevel(IOT_LOG_DEBUG);

#if !defined(_WIN32) && !defined(BUILD_AOS)
    while ((opt = getopt(argc, argv, "e:s:lhr")) != -1) {
        switch (opt) {
            case 's': {
                if (strlen(optarg) > 31) {
                    memcpy(secur, optarg, 31);
                } else {
                    memcpy(secur, optarg, strlen(optarg));
                }
            }
            break;
            case 'e': {
                if (strlen(optarg) > 31) {
                    memcpy(env, optarg, 31);
                } else {
                    memcpy(env, optarg, strlen(optarg));
                }
            }
            break;
            case 'l':
                m_coap_client_running = 1;
                break;
            case 'r':
                m_coap_reconnect = 1;
                break;
            case 'h':
                show_usage();
                return 0;
            default:
                break;
        }
    }
#else
    /* Just use psk security mode, online environment */
    (void)argc;
    (void)argv;
    (void)opt;
    memcpy(secur, "psk", 4);
    memcpy(env, "online", 7);
    m_coap_client_running = 1;
    m_coap_reconnect = 1;
#endif

    LOGD(TAG, "[COAP-Client]: Enter Coap Client");
    memset(&config, 0x00, sizeof(iotx_coap_config_t));
    if (0 == strncmp(env, "pre", strlen("pre"))) {
        if (0 == strncmp(secur, "dtls", strlen("dtls"))) {
            config.p_url = IOTX_PRE_DTLS_SERVER_URI;
        } else if (0 == strncmp(secur, "psk", strlen("psk"))) {
            config.p_url = IOTX_PRE_PSK_SERVER_URI;
        } else {
            config.p_url = IOTX_PRE_NOSEC_SERVER_URI;
        }
        (void)url;
    } else if (0 == strncmp(env, "online", strlen("online"))) {
        if (0 == strncmp(secur, "dtls", strlen("dtls"))) {
            snprintf(url, sizeof(url), IOTX_ONLINE_DTLS_SERVER_URL, IOTX_PRODUCT_KEY);
            config.p_url = url;
        } else if (0 == strncmp(secur, "psk", strlen("psk"))) {
            snprintf(url, sizeof(url), IOTX_ONLINE_PSK_SERVER_URL, IOTX_PRODUCT_KEY);
            config.p_url = url;
        } else {
            LOGE(TAG, "Online environment must access with DTLS/PSK\r\n");
            IOT_SetLogLevel(IOT_LOG_NONE);
            return -1;
        }
    }

    iotx_get_devinfo(&deviceinfo);
    config.p_devinfo = (iotx_coap_device_info_t *)&deviceinfo;
    config.wait_time_ms = 3000;

    iotx_coap_context_t *p_ctx = NULL;

reconnect:
    p_ctx = IOT_CoAP_Init(&config);
    if (NULL != p_ctx) {
        IOT_CoAP_DeviceNameAuth(p_ctx);
        do {
            if (count == 11 || 0 == count) {
                iotx_post_data_to_server((void *)p_ctx);
                count = 1;
            }
            count ++;
            IOT_CoAP_Yield(p_ctx);
        } while (m_coap_client_running);

        IOT_CoAP_Deinit(&p_ctx);
    } else {
        LOGE(TAG, "IoTx CoAP init failed");
    }
    if (m_coap_reconnect) {
        m_coap_reconnect = 0;
        goto reconnect;
    }

    IOT_SetLogLevel(IOT_LOG_NONE);
    LOGD("[COAP-Client]: Exit Coap Client");
    return 0;
}

