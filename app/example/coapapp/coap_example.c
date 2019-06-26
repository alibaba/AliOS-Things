/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if !defined(_WIN32)
    #include <unistd.h>
#endif

#include "coap_api.h"
#include "wrappers.h"
#include "app_entry.h"
#include "infra_compat.h"

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
    HAL_Printf("[APPL]: Message response code: 0x%x\r\n", resp_code);
    HAL_Printf("[APPL]: Len: %d, Payload: %s\r\n", len, p_payload);
}

static void iotx_post_data_to_server(void *param)
{
    char path[IOTX_URI_MAX_LEN + 1] = {0};
    iotx_coap_context_t *p_ctx = (iotx_coap_context_t *)param;
    iotx_message_t message;

    HAL_Snprintf(path, IOTX_URI_MAX_LEN, "/topic/%s/%s/user/update/", IOTX_PRODUCT_KEY, IOTX_DEVICE_NAME);

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
    HAL_Snprintf(p_devinfo->device_id, IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 2,
                 "%s.%s", p_devinfo->product_key, p_devinfo->device_name);

    HAL_Printf("*****The Product Key  : %s *****\r\n", p_devinfo->product_key);
    HAL_Printf("*****The Device Name  : %s *****\r\n", p_devinfo->device_name);
    HAL_Printf("*****The Device Secret: %s *****\r\n", p_devinfo->device_secret);
    HAL_Printf("*****The Device ID    : %s *****\r\n", p_devinfo->device_id);
    return IOTX_SUCCESS;
}

static void show_usage()
{
    HAL_Printf("\r\nusage: coap-example [OPTION]...\r\n");
    HAL_Printf("\t-e pre|online|daily\t\tSet the cloud environment.\r\n");
    HAL_Printf("\t-s nosec|dtls|psk  \t\tSet the security setting.\r\n");
    HAL_Printf("\t-l                 \t\tSet the program run loop.\r\n");
    HAL_Printf("\t-r                 \t\tTesting the DTLS session ticket.\r\n");
    HAL_Printf("\t-h                 \t\tShow this usage.\r\n");
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

    HAL_Printf("[COAP-Client]: Enter Coap Client\r\n");
    memset(&config, 0x00, sizeof(iotx_coap_config_t));
    if (0 == strncmp(env, "pre", strlen("pre"))) {
        if (0 == strncmp(secur, "dtls", strlen("dtls"))) {
            config.p_url = IOTX_PRE_DTLS_SERVER_URI;
        } else if (0 == strncmp(secur, "psk", strlen("psk"))) {
            config.p_url = IOTX_PRE_PSK_SERVER_URI;
        } else {
            config.p_url = IOTX_PRE_NOSEC_SERVER_URI;
        }
    } else if (0 == strncmp(env, "online", strlen("online"))) {
        if (0 == strncmp(secur, "dtls", strlen("dtls"))) {
            char url[256] = {0};
            snprintf(url, sizeof(url), IOTX_ONLINE_DTLS_SERVER_URL, IOTX_PRODUCT_KEY);
            config.p_url = url;
        } else if (0 == strncmp(secur, "psk", strlen("psk"))) {
            char url[256] = {0};
            snprintf(url, sizeof(url), IOTX_ONLINE_PSK_SERVER_URL, IOTX_PRODUCT_KEY);
            config.p_url = url;
        } else {
            HAL_Printf("Online environment must access with DTLS/PSK\r\n");
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
        HAL_Printf("IoTx CoAP init failed\r\n");
    }
    if (m_coap_reconnect) {
        m_coap_reconnect = 0;
        goto reconnect;
    }

    IOT_SetLogLevel(IOT_LOG_NONE);
    HAL_Printf("[COAP-Client]: Exit Coap Client\r\n");
    return 0;
}

