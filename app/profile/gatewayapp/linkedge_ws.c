#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <unistd.h>
#include "aos/kernel.h"
#include <time.h>

#include "libwebsockets.h"

#include "iot_import.h"

#include "json_parser.h"

#include "aos/hal/uart.h"

#include <cJson.h>
#include "device.h"



/* Websockets local link*/
static struct lws_context *context = NULL;
static struct lws_context_creation_info info;
static struct lws *wsi = NULL;
static struct lws_protocols protocol;

static int destroy_flag = 0;
static int connection_flag = 0;
static int writeable_flag = 0;
static unsigned int msg_id = 0;

static int wait_reply = 0;

/* Device sync status */
static int dev_reg = 0;
static int dev_online = 0;

/*Report status */
static volatile int report_ongoing = 0;
static volatile int report_len = 0;

/*LinkEdge WS Protocol*/
#define FMT_REGISTER            "{\"method\":\"registerDevice\",\"msgId\":%d,\"deviceId\":\"%s\",\"productKey\":\"%s\",\"payload\":{}}"
#define FMT_ONLINE              "{\"method\":\"onlineDevice\",\"msgId\":%d,\"deviceId\":\"%s\",\"productKey\":\"%s\",\"payload\":{}}"
#define FMT_OFFLINE             "{\"method\":\"offlineDevice\",\"msgId\":%d,\"deviceId\":\"%s\",\"productKey\":\"%s\",\"payload\":{}}"
#define FMT_REPORT_PROPERTY     "{\"method\":\"reportProperty\",\"msgId\":%d,\"deviceId\":\"%s\",\"productKey\":\"%s\",\"payload\":{\"%s\":%s}}"
#define FMT_REPORT_PROPERTY_STRING  "{\"method\":\"reportProperty\",\"msgId\":%d,\"deviceId\":\"%s\",\"productKey\":\"%s\",\"payload\":{\"%s\":\"%s\"}}"
#define FMT_REPORT_EVENT        "{\"method\":\"reportEvent\",\"msgId\":%d,\"deviceId\":\"%s\",\"productKey\":\"%s\",\"payload\":{\"name\":\"%s\",\"params\":{%s}}}"


#define LE_PROTO_MAX 1024

static char linkEdgeProtoBuf[LWS_SEND_BUFFER_PRE_PADDING+LE_PROTO_MAX+LWS_SEND_BUFFER_POST_PADDING];
#define linkEdgeProto (linkEdgeProtoBuf+LWS_SEND_BUFFER_PRE_PADDING)

#define WS_PRINTF printf

#ifndef LINKEDGE_SERVER
#define LINKEDGE_SERVER "192.168.1.100"
#endif

static inline unsigned int _get_msg_id()
{
    return ++msg_id;
}

static int ws_service_callback(
                         struct lws *wsi,
                         enum lws_callback_reasons reason, void *user,
                         void *in, size_t len)
{
    
    switch (reason) {
 
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            WS_PRINTF("[LinkEdge WS] Connect with server success.\n");
            connection_flag = 1;
            lws_callback_on_writable(wsi);
            break;
 
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            WS_PRINTF("[LinkEdge WS] Connect with server error.\n");
            destroy_flag = 1;
            connection_flag = 0;
            break;
 
        case LWS_CALLBACK_CLOSED:
            WS_PRINTF("[LinkEdge WS] LWS_CALLBACK_CLOSED\n");
            destroy_flag = 1;
            connection_flag = 0;
            break;
        case LWS_CALLBACK_CLIENT_RECEIVE:
            WS_PRINTF("[LinkEdge WS] Client recvived:%s\n", (char *)in);
            wait_reply = 0;
            lws_callback_on_writable(wsi);
            if(report_ongoing == 2) report_ongoing = 0;
            break;

        case LWS_CALLBACK_CLIENT_WRITEABLE :
        {
            int olen;
            if(wait_reply) break;
            
            if(!dev_reg){
                WS_PRINTF("[LinkEdge WS] Register device.\n");
                wait_reply = 1;
                olen = snprintf(linkEdgeProto, LE_PROTO_MAX, FMT_REGISTER,
                    _get_msg_id(), DEVICE_NAME, PRODUCT_KEY);

                lws_write(wsi, linkEdgeProto, olen, LWS_WRITE_TEXT);
                dev_reg = 1;
            } else if(!dev_online){
                WS_PRINTF("[LinkEdge WS] Online device.\n");
                wait_reply = 1;
                olen = snprintf(linkEdgeProto, LE_PROTO_MAX, FMT_ONLINE,
                    _get_msg_id(), DEVICE_NAME, PRODUCT_KEY);

                lws_write(wsi, linkEdgeProto, olen, LWS_WRITE_TEXT);
                dev_online = 1;
            } else if(report_len){
                //WS_PRINTF("[LinkEdge WS] scanner event param %d: %s.\n", elen, param);

                wait_reply = 1;

                lws_write(wsi, linkEdgeProto, report_len, LWS_WRITE_TEXT);

                report_len = 0;
                report_ongoing = 0;
            }

            writeable_flag = 1;
            break;
        }
        default:
            break;
    }
 
    return 0;
}


struct session_data {
    int fd;
};

static void lws_p(int l, const char * line){
    WS_PRINTF("\n---------LWS---------\n\t\t");
    WS_PRINTF(line);
}

void websocket_loop(){
    lws_service(context, 50);
}

int websocket_local_offline(){
    return !dev_online;
}

int websocket_report_event(const char * event, const char * param){
    if(report_ongoing){
        int wait = 3;
        while(report_ongoing && wait--){
            aos_msleep(10);
        }
        if(report_ongoing) return -1;
    }
    report_ongoing = 1;

    report_len = snprintf(linkEdgeProto, LE_PROTO_MAX, FMT_REPORT_EVENT,
        _get_msg_id(), DEVICE_NAME, PRODUCT_KEY, event, param);

    lws_write(wsi, linkEdgeProto, report_len, LWS_WRITE_TEXT);

    report_len = 0;
    report_ongoing = 0;
    
    return 0;
}

int websocket_report_prop(const char * prop, const char * value){
    if(report_ongoing){
        int wait = 3;
        while(report_ongoing && wait--){
            aos_msleep(10);
        }
        if(report_ongoing) return -1;
    }
    report_ongoing = 1;

    report_len = snprintf(linkEdgeProto, LE_PROTO_MAX, FMT_REPORT_PROPERTY,
        _get_msg_id(), DEVICE_NAME, PRODUCT_KEY, prop, value);

    //printf("Report prop: %s\n", linkEdgeProto);
    lws_write(wsi, linkEdgeProto, report_len, LWS_WRITE_TEXT);

    report_len = 0;
    report_ongoing = 0;

    return 0;
}

void websocket_local_link(){

     cJSON_Hooks hook = {
        malloc,
        free,
    };

    cJSON_InitHooks(&hook);

    memset(&info, 0, sizeof info);
    info.port = CONTEXT_PORT_NO_LISTEN;
    info.iface = NULL;
    info.protocols = &protocol;
    info.ssl_cert_filepath = NULL;
    info.ssl_private_key_filepath = NULL;
    info.extensions = 0;
    info.gid = -1;
    info.uid = -1;
    info.options = 0;
    info.timeout_secs = 2;
 
    protocol.name  = NULL;
    protocol.callback = &ws_service_callback;
    protocol.per_session_data_size = sizeof(struct session_data);
    protocol.rx_buffer_size = 0;
    protocol.id = 0;
    protocol.user = NULL;

    lws_set_log_level(0x3, lws_p);
 
    context = lws_create_context(&info);

    if (context == NULL) {
        WS_PRINTF("[Main] context is NULL.\n");
        return;
    }

    WS_PRINTF("Linkedge Server: %s\n", LINKEDGE_SERVER);
    wsi = lws_client_connect(context, LINKEDGE_SERVER, 7682, 0,"/", 
        LINKEDGE_SERVER":7682", NULL, protocol.name, -1);
    if (wsi == NULL) {
        WS_PRINTF("[Main] wsi create error.\n");
        return -1;
    }

    WS_PRINTF("[Main] wsi create success.\n"); 
}

