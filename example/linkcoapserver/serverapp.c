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

#include "CoAPPlatform.h"
#include "CoAPExport.h"

int g_status = 0;
int g_exit = 0;
NetworkAddr addr;

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

void AwssInfoCallback(CoAPContext *context, const char *paths, NetworkAddr *remote, CoAPMessage *request)
{
    int ret = COAP_SUCCESS;
    unsigned char payload[256] = {0};
    CoAPMessage response;
    unsigned int observe = 0;

    CoAPMessage_init(&response);
    CoAPMessageType_set(&response, COAP_MESSAGE_TYPE_NON);
    CoAPMessageCode_set(&response, COAP_MSG_CODE_205_CONTENT);
    CoAPMessageId_set(&response, request->header.msgid);
    CoAPMessageToken_set(&response, request->token, request->header.tokenlen);

    ret = CoAPUintOption_get(request, COAP_OPTION_OBSERVE, &observe);
    COAP_DEBUG("observe %d", observe);
    if(COAP_SUCCESS == ret && 0 == observe){
        CoAPObsServer_add(context, paths, remote, request);
        CoAPUintOption_add(&response, COAP_OPTION_OBSERVE, 0);
    }

    CoAPUintOption_add(&response, COAP_OPTION_CONTENT_FORMAT, COAP_CT_APP_JSON);
    snprintf((char *)payload, sizeof(payload), "{\"status\":\"%d\"}", g_status++);
    CoAPMessagePayload_set(&response, payload, strlen((char *)payload));

    COAP_DEBUG("Send a response message");
    CoAPMessage_send(context, remote, &response);
    CoAPMessage_destory(&response);

}

void CoAPServerExitCallback(CoAPContext *context, const char *paths, NetworkAddr *remote, CoAPMessage *request)
{
    COAP_INFO("CoAPServerExitCallback！！！！");
    g_exit = 1;

}

void CoAPServerObserve(CoAPContext *context, const char *paths, NetworkAddr *remote, CoAPMessage *request)
{
    CoAPObsServer_notify(context, "/sys/dev/awss/deviceinfo", (unsigned char *)"observe notify", strlen("observe notify"), NULL);
}


int CoAPMessageSendMultiCast(CoAPContext * context, NetworkAddr * remote)
{
    CoAPMessage message;
    CoAPMessage_init(&message);
    CoAPMessageType_set(&message, COAP_MESSAGE_TYPE_NON);
    CoAPMessageCode_set(&message, COAP_MSG_CODE_GET);
    CoAPMessageId_set(&message, CoAPMessageId_gen(context));
    CoAPMessageHandler_set(&message, sendNotifyCallback);

    CoAPStrOption_add(&message, COAP_OPTION_URI_PATH,
                      (unsigned char *)"hello", (int)strlen("hello"));

    CoAPUintOption_add(&message, COAP_OPTION_CONTENT_FORMAT, COAP_CT_APP_JSON);

    CoAPMessage_send(context, remote, &message);

    CoAPMessage_destory(&message);
    return COAP_SUCCESS;
}


#ifdef COAP_WITH_YLOOP
static void coap_send_period(void *arg)
{
    CoAPContext * context = (CoAPContext * )arg;
    if(!g_exit){
#ifdef COAP_USE_PLATFORM_MEMORY
        LITE_dump_malloc_free_stats(5);
#endif
        int id;
        CoAPServerMultiCast_send(context, &addr, (const char *)"/sys/dev/awss/deviceinfo", (unsigned char *)"{\"device\":\"awss\"}", strlen("{\"device\":\"awss\"}"), sendNotifyCallback,&id);
    }else{
        CoAPContext_free(context);
    }
    aos_post_delayed_action(2000, coap_send_period, context);
}
#endif 
int server_test(void)
{
    CoAPContext * context = NULL;
#ifdef COAP_USE_PLATFORM_LOG
    LITE_openlog("CoAP");
    LITE_set_loglevel(5);
#endif
    context = CoAPServer_init();
    CoAPServer_register(context, "/sys/dev/awss/deviceinfo", AwssInfoCallback);
    CoAPServer_register(context, "/sys/dev/exit", CoAPServerExitCallback);
    CoAPServer_register(context, "/sys/dev/observe", CoAPServerObserve);

    addr.port = 5684;
    memset(addr.addr, 0x00, sizeof(addr.addr));
    strcpy(addr.addr, "224.0.1.187");
    
#ifndef COAP_WITH_YLOOP
    CoAPServer_loop(context);
    while(!g_exit){
        aos_msleep(2000);
#ifdef COAP_USE_PLATFORM_MEMORY
        LITE_dump_malloc_free_stats(5);
#endif
        int id=0;
        CoAPServerMultiCast_send(context, &addr, (const char *)"/sys/dev/awss/deviceinfo", (unsigned char *)"{\"device\":\"awss\"}", strlen("{\"device\":\"awss\"}"), sendNotifyCallback,&id);
    }
    CoAPServer_deinit(context);
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
        server_test();
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


