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

#include "CoAPPlatform.h"
#include "CoAPExport.h"

#define COAP_INIT_TOKEN     (0x08080808)
#define COAP_SERV_MAX_PATH_LEN COAP_MSG_MAX_PATH_LEN

int g_status = 0;
int g_exit = 0;
NetworkAddr addr;


static unsigned int CoAPClientToken_get(unsigned char *p_encoded_data)
{
    static unsigned int value = COAP_INIT_TOKEN;
    p_encoded_data[0] = (unsigned char)((value & 0x00FF) >> 0);
    p_encoded_data[1] = (unsigned char)((value & 0xFF00) >> 8);
    p_encoded_data[2] = (unsigned char)((value & 0xFF0000) >> 16);
    p_encoded_data[3] = (unsigned char)((value & 0xFF000000) >> 24);
    value++;
    return sizeof(unsigned int);
}

static int CoAPClientPath_2_option(char *uri, CoAPMessage *message)
{
    char *ptr     = NULL;
    char *pstr    = NULL;
    char  path[COAP_MSG_MAX_PATH_LEN]  = {0};

    if (NULL == uri || NULL == message) {
        COAP_ERR("Invalid paramter p_path %p, p_message %p", uri, message);
        return COAP_ERROR_INVALID_PARAM;
    }
    if (COAP_SERV_MAX_PATH_LEN < strlen(uri)) {
        COAP_ERR("The uri length is too loog,len = %d", (int)strlen(uri));
        return COAP_ERROR_INVALID_LENGTH;
    }
    COAP_DEBUG("The uri is %s", uri);
    ptr = pstr = uri;
    while ('\0' != *ptr) {
        if ('/' == *ptr) {
            if (ptr != pstr) {
                memset(path, 0x00, sizeof(path));
                strncpy(path, pstr, ptr - pstr);
                COAP_DEBUG("path: %s,len=%d", path, (int)(ptr - pstr));
                CoAPStrOption_add(message, COAP_OPTION_URI_PATH,
                                  (unsigned char *)path, (int)strlen(path));
            }
            pstr = ptr + 1;

        }
        if ('\0' == *(ptr + 1) && '\0' != *pstr) {
            memset(path, 0x00, sizeof(path));
            strncpy(path, pstr, sizeof(path) - 1);
            COAP_DEBUG("path: %s,len=%d", path, (int)strlen(path));
            CoAPStrOption_add(message, COAP_OPTION_URI_PATH,
                              (unsigned char *)path, (int)strlen(path));
        }
        ptr ++;
    }
    return COAP_SUCCESS;
}

void  sendNotifyCallback(CoAPContext *context, CoAPReqResult result, void *userdata, NetworkAddr *remote, CoAPMessage *message)
{
       if(COAP_RECV_RESP_TIMEOUT == result){
            COAP_DEBUG("send message timeout, resend it");
           CoAPObsServer_notify(context, "/status", (unsigned char *)"{\"status\":\"1\"}", strlen("{\"status\":\"1\"}"), NULL);
       }
       else{
           COAP_DEBUG("recv response message");
       }
}

void  sendMulticastCallback(CoAPContext *context, CoAPReqResult result, void *userdata, NetworkAddr *remote, CoAPMessage *message)
{
       if(COAP_RECV_RESP_TIMEOUT == result){
            COAP_DEBUG("send multicast message timeout, resend it");
       }
       else{
           COAP_DEBUG("multicast recv response message");
       }
}

int CoAPClientObserve_send(CoAPContext *context, NetworkAddr *remote, const char *uri, unsigned char *buff, unsigned short len, CoAPSendMsgHandler callback)
{
    int ret = COAP_SUCCESS;
    CoAPMessage message;
    unsigned char tokenlen;
    unsigned char token[COAP_MSG_MAX_TOKEN_LEN] = {0};

    CoAPMessage_init(&message);
    CoAPMessageType_set(&message, COAP_MESSAGE_TYPE_CON);
    CoAPMessageCode_set(&message, COAP_MSG_CODE_GET);
    CoAPMessageId_set(&message, CoAPMessageId_gen(context));
    tokenlen = CoAPClientToken_get(token);
    CoAPMessageToken_set(&message, token, tokenlen);
    CoAPMessageHandler_set(&message, callback);

    CoAPUintOption_add(&message, COAP_OPTION_OBSERVE, 0);
    CoAPClientPath_2_option((char *)uri, &message);
    CoAPUintOption_add(&message, COAP_OPTION_CONTENT_FORMAT, COAP_CT_APP_JSON);
    CoAPMessagePayload_set(&message, buff, len);

    ret = CoAPMessage_send(context, remote, &message);

    CoAPMessage_destory(&message);

    return ret;
}

int CoAPClient_send(CoAPContext *context, NetworkAddr *remote, const char *uri, unsigned char *buff, unsigned short len, CoAPSendMsgHandler callback)
{
    int ret = COAP_SUCCESS;
    CoAPMessage message;
    unsigned char tokenlen;
    unsigned char token[COAP_MSG_MAX_TOKEN_LEN] = {0};

    CoAPMessage_init(&message);
    CoAPMessageType_set(&message, COAP_MESSAGE_TYPE_CON);
    CoAPMessageCode_set(&message, COAP_MSG_CODE_GET);
    CoAPMessageId_set(&message, CoAPMessageId_gen(context));
    tokenlen = CoAPClientToken_get(token);
    CoAPMessageToken_set(&message, token, tokenlen);
    CoAPMessageHandler_set(&message, callback);

    CoAPClientPath_2_option((char *)uri, &message);
    CoAPUintOption_add(&message, COAP_OPTION_CONTENT_FORMAT, COAP_CT_APP_JSON);
    CoAPMessagePayload_set(&message, buff, len);

    ret = CoAPMessage_send(context, remote, &message);

    CoAPMessage_destory(&message);

    return ret;
}



int CoAPClientMultiCast_send(CoAPContext *context, NetworkAddr *remote, const char *uri, unsigned char *buff, unsigned short len, CoAPSendMsgHandler callback, unsigned short *msgid)
{
    int ret = COAP_SUCCESS;
    CoAPMessage message;
    unsigned char tokenlen;
    unsigned char token[COAP_MSG_MAX_TOKEN_LEN] = {0};

    CoAPMessage_init(&message);
    CoAPMessageType_set(&message, COAP_MESSAGE_TYPE_NON);
    CoAPMessageCode_set(&message, COAP_MSG_CODE_GET);
    CoAPMessageId_set(&message, CoAPMessageId_gen(context));
    tokenlen = CoAPClientToken_get(token);
    CoAPMessageToken_set(&message, token, tokenlen);
    CoAPMessageHandler_set(&message, callback);

    CoAPClientPath_2_option((char *)uri, &message);
    CoAPUintOption_add(&message, COAP_OPTION_CONTENT_FORMAT, COAP_CT_APP_JSON);
    CoAPMessagePayload_set(&message, buff, len);

    ret = CoAPMessage_send(context, remote, &message);
    *msgid = message.header.msgid;
    CoAPMessage_destory(&message);

    return ret;
}


CoAPContext *CoAPClient_init()
{
    CoAPInitParam param;
    CoAPContext * context = NULL;

    param.appdata = NULL;
    param.group = "224.0.1.187";
    param.notifier = NULL;
    param.obs_maxcount = 16;
    param.res_maxcount = 32;
    param.port = 5684;
    param.send_maxcount = 16;
    param.waittime = 2000;

    context = CoAPContext_create(&param);

    return (CoAPContext *)context;
}

#ifndef COAP_WITH_YLOOP
void *CoAPClient_yield(void *param)
{
    CoAPContext *context = (CoAPContext *)param;
    COAP_DEBUG("Enter to CoAP client yield");
    while(1){
        CoAPMessage_cycle(context);
    }
}

void CoAPClient_loop(CoAPContext *context)
{
    void *thread_handle;
    int stack_used;
    HAL_ThreadCreate(&thread_handle, CoAPClient_yield, (void *)context, NULL, &stack_used);
}
#else
static void coap_send_period(void *arg)
{
    CoAPContext * context = (CoAPContext * )arg;
    if(!g_exit){
#ifdef COAP_USE_PLATFORM_MEMORY
        LITE_dump_malloc_free_stats(5);
#endif
        CoAPClient_send(context, &addr, (const char *)"/", NULL, 0, sendNotifyCallback);
    }else{
        CoAPContext_free(context);
    }
    aos_post_delayed_action(2000, coap_send_period, context);
}
#endif 


int coap_test(void)
{
    //unsigned short msgid = 0;
    CoAPContext * context = NULL;
#ifdef COAP_USE_PLATFORM_LOG
    LITE_openlog("CoAP");
    LITE_set_loglevel(5);
#endif

    context = CoAPClient_init();

#if 0
    addr.port = 5683;
    memset(addr.addr, 0x00, sizeof(addr.addr));
    strcpy(addr.addr, "224.0.1.187");
    CoAPClientMultiCast_send(context, &addr, (const char *)"/sys/dev/awss/deviceinfo", 
        (unsigned char *)"{\"device\":\"awss\"}", strlen("{\"device\":\"awss\"}"), sendMulticastCallback, &msgid);
#endif

    
#if 1
    addr.port = 5683;
    memset(addr.addr, 0x00, sizeof(addr.addr));
    strcpy(addr.addr, "139.196.187.107");

    CoAPClientObserve_send(context, &addr, (const char *)"/obs", NULL, 0, sendNotifyCallback);
#endif

#ifndef COAP_WITH_YLOOP
    CoAPClient_loop(context);
    while(!g_exit){
        aos_msleep(2000);
#ifdef COAP_USE_PLATFORM_MEMORY
        LITE_dump_malloc_free_stats(5);
#endif
       //CoAPMessageId_cancel(context, msgid);
       CoAPClient_send(context, &addr, (const char *)"/", NULL, 0, sendNotifyCallback);
    }
    CoAPContext_free(context);
#else
     aos_post_delayed_action(2000, coap_send_period, context);
#endif

    return COAP_SUCCESS;
}

int coap_client_init=0;
void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type == EV_WIFI && event->code == CODE_WIFI_ON_GOT_IP)
    {
		if (coap_client_init) {
			return;
		}
		coap_client_init = 1;
        
        coap_test();
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

