/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "CoAPPlatform.h"
#include "CoAPExport.h"
#include "CoAPServer.h"
#include <aos/aos.h>

#define COAP_INIT_TOKEN     (0x01020304)
#define COAP_SERV_MAX_PATH_LEN COAP_MSG_MAX_PATH_LEN

static unsigned int g_coap_running = 0;
static void *g_coap_thread = NULL;

static unsigned int CoAPServerToken_get(unsigned char *p_encoded_data)
{
    static unsigned int value = COAP_INIT_TOKEN;
    p_encoded_data[0] = (unsigned char)((value & 0x00FF) >> 0);
    p_encoded_data[1] = (unsigned char)((value & 0xFF00) >> 8);
    p_encoded_data[2] = (unsigned char)((value & 0xFF0000) >> 16);
    p_encoded_data[3] = (unsigned char)((value & 0xFF000000) >> 24);
    value++;
    return sizeof(unsigned int);
}

static int CoAPServerPath_2_option(char *uri, CoAPMessage *message)
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


CoAPContext *CoAPServer_init()
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

#ifdef COAP_USE_PLATFORM_LOG
    LITE_openlog("CoAP");
    LITE_set_loglevel(5);
#endif

    context = CoAPContext_create(&param);

    return (CoAPContext *)context;
}

#ifndef COAP_WITH_YLOOP
static void *CoAPServer_yield(void *param)
{
    CoAPContext *context = (CoAPContext *)param;
    COAP_DEBUG("Enter to CoAP daemon task");
    while(g_coap_running){
        CoAPMessage_cycle(context);
    }

    if(NULL != context){
        CoAPContext_free(context);
        context = NULL;
    }

    HAL_ThreadDelete(NULL);
    g_coap_thread = NULL;
    COAP_INFO("Exit the CoAP daemon task");
    return NULL;
}

void CoAPServer_loop(CoAPContext *context)
{
    int stack_used;
    g_coap_running = 1;
    HAL_ThreadCreate(&g_coap_thread, CoAPServer_yield, (void *)context, NULL, &stack_used);
}
#endif

void CoAPServer_deinit(CoAPContext *context)
{
    COAP_INFO("CoAP Server deinit");
    g_coap_running = 0;
    HAL_SleepMs(1000);
}

int CoAPServer_register(CoAPContext *context, const char *uri, CoAPRecvMsgHandler callback)
{

    return CoAPResource_register(context, uri, COAP_PERM_GET, COAP_CT_APP_JSON, 60, callback);
}

int CoAPServerMultiCast_send(CoAPContext *context, NetworkAddr *remote, const char *uri, unsigned char *buff, unsigned short len, CoAPSendMsgHandler callback, unsigned short *msgid)
{
    int ret = COAP_SUCCESS;
    CoAPMessage message;
    unsigned char tokenlen;
    unsigned char token[COAP_MSG_MAX_TOKEN_LEN] = {0};
    COAP_INFO("CoAPServerMultiCast_send");
    CoAPMessage_init(&message);
    CoAPMessageType_set(&message, COAP_MESSAGE_TYPE_NON);
    CoAPMessageCode_set(&message, COAP_MSG_CODE_POST);
    CoAPMessageId_set(&message, CoAPMessageId_gen(context));
    tokenlen = CoAPServerToken_get(token);
    CoAPMessageToken_set(&message, token, tokenlen);
    CoAPMessageHandler_set(&message, callback);

    CoAPServerPath_2_option((char *)uri, &message);
    CoAPUintOption_add(&message, COAP_OPTION_CONTENT_FORMAT, COAP_CT_APP_JSON);
    CoAPMessagePayload_set(&message, buff, len);
    *msgid = message.header.msgid;
    ret = CoAPMessage_send(context, remote, &message);

    CoAPMessage_destory(&message);

    return ret;
}

int CoAPServerResp_send(CoAPContext *context, NetworkAddr *remote, unsigned char *buff, unsigned short len, void *req, const char *paths)
{
    int ret = COAP_SUCCESS;
    CoAPMessage response;
    unsigned int observe = 0;
    CoAPMessage *request = (CoAPMessage *)req;

    CoAPMessage_init(&response);
    CoAPMessageType_set(&response, COAP_MESSAGE_TYPE_NON);
    CoAPMessageCode_set(&response, COAP_MSG_CODE_205_CONTENT);
    CoAPMessageId_set(&response, request->header.msgid);
    CoAPMessageToken_set(&response, request->token, request->header.tokenlen);

    ret = CoAPUintOption_get(request, COAP_OPTION_OBSERVE, &observe);
    if(COAP_SUCCESS == ret && 0 == observe){
        CoAPObsServer_add(context, paths, remote, request);
        CoAPUintOption_add(&response, COAP_OPTION_OBSERVE, 0);
    }

    CoAPUintOption_add(&response, COAP_OPTION_CONTENT_FORMAT, COAP_CT_APP_JSON);
    CoAPMessagePayload_set(&response, buff, len);

    COAP_DEBUG("Send a response message");
    ret = CoAPMessage_send(context, remote, &response);
    CoAPMessage_destory(&response);

    return ret;
}


