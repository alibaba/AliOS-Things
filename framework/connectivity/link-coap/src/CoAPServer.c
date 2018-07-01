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
#include <string.h>
#include "CoAPPlatform.h"
#include "CoAPExport.h"
#include "CoAPServer.h"

#define COAP_INIT_TOKEN             (0x01020304)
#ifndef COAP_SERV_SENDLIST_MAXCOUNT
#define COAP_SERV_SENDLIST_MAXCOUNT COAP_DEFAULT_SENDLIST_MAXCOUNT
#endif
#ifndef COAP_SERV_RES_MAXCOUNT
#define COAP_SERV_RES_MAXCOUNT      COAP_DEFAULT_RES_MAXCOUNT
#endif
#ifndef COAP_SERV_OBS_MAXCOUNT
#define COAP_SERV_OBS_MAXCOUNT      COAP_DEFAULT_OBS_MAXCOUNT
#endif
#ifndef COAP_SERV_WAIT_TIME_MS
#define COAP_SERV_WAIT_TIME_MS      COAP_DEFAULT_WAIT_TIME_MS
#endif

static unsigned int g_coap_running = 0;
#ifdef COAP_SERV_MULTITHREAD
static void *g_coap_thread = NULL;
static void *g_semphore    = NULL;
#endif
static CoAPContext *g_context = NULL;
static void        *g_coap_serv_mutex = NULL;
static int          g_call_num = 0;

#ifdef HAL_ASYNC_API
static void        *g_retrans_timer = NULL;
static void        *g_deinit_timer = NULL;
#endif

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
    if (COAP_MSG_MAX_PATH_LEN <= strlen(uri)) {
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
                CoAPStrOption_add(message, COAP_OPTION_URI_PATH,
                                  (unsigned char *)path, (int)strlen(path));
            }
            pstr = ptr + 1;

        }
        if ('\0' == *(ptr + 1) && '\0' != *pstr) {
            memset(path, 0x00, sizeof(path));
            strncpy(path, pstr, sizeof(path) - 1);
            CoAPStrOption_add(message, COAP_OPTION_URI_PATH,
                              (unsigned char *)path, (int)strlen(path));
        }
        ptr ++;
    }
    return COAP_SUCCESS;
}


#ifdef HAL_ASYNC_API
void CoAPServer_retransmit(void *data)
{
    CoAPContext *context = (CoAPContext *)data;

    if (NULL == context) {
        return;
    }

    CoAPMessage_retransmit(context);

    HAL_Timer_Stop(g_retrans_timer);
    HAL_Timer_Start(g_retrans_timer, COAP_SERV_WAIT_TIME_MS);
}

void CoAPServer_recv(intptr_t fd, void *data)
{
    CoAPContext *context  = NULL;

    if (NULL == data) {
        return;
    }

    COAP_DEBUG("Fd %d is ready to read %p", fd, data);
    context  = (CoAPContext *)data;
    CoAPMessage_process(context, 1);
}

#endif

CoAPContext *CoAPServer_init()
{
#ifdef HAL_ASYNC_API
    intptr_t  fd = -1;
#endif
#ifdef COAP_SERV_MULTITHREAD
    int stack_used;
#endif
    CoAPInitParam param;

    memset(&param, 0x00, sizeof(CoAPInitParam));
    if (NULL == g_context) {
        param.appdata = NULL;
        param.group = "224.0.1.187";
        param.notifier = NULL;
        param.obs_maxcount = COAP_SERV_OBS_MAXCOUNT;
        param.res_maxcount = COAP_SERV_RES_MAXCOUNT;
        param.port = 5683;
        param.send_maxcount = COAP_SERV_SENDLIST_MAXCOUNT;
        param.waittime = COAP_SERV_WAIT_TIME_MS;

        g_coap_serv_mutex = HAL_MutexCreate();
        if (NULL == g_coap_serv_mutex) {
            COAP_ERR("Mutex Create failed");
            return NULL;
        }

        /* Mutil-thread support */
#ifdef COAP_SERV_MULTITHREAD
        g_semphore  = HAL_SemaphoreCreate();
        if (NULL == g_semphore) {
            if (NULL != g_coap_serv_mutex) {
                HAL_MutexDestroy(g_coap_serv_mutex);
                g_coap_serv_mutex = NULL;
            }
            COAP_ERR("Semaphore Create failed");
            return NULL;
        }
#endif


        g_context = CoAPContext_create(&param);
        if (NULL == g_context) {
            COAP_ERR("CoAP Context Create failed");
#ifdef COAP_SERV_MULTITHREAD
            HAL_SemaphoreDestroy(g_semphore);
            g_semphore = NULL;
#endif
            if (NULL != g_coap_serv_mutex) {
                HAL_MutexDestroy(g_coap_serv_mutex);
                g_coap_serv_mutex = NULL;
            }
            return NULL;
        }

#ifdef COAP_SERV_MULTITHREAD
        g_coap_running = 1;
        HAL_ThreadCreate(&g_coap_thread, CoAPServer_yield, (void *)g_context, NULL, &stack_used);
#endif

#ifdef HAL_ASYNC_API
        fd = CoAPContextFd_get(g_context);
        HAL_Register_Recv_Callback(fd, CoAPServer_recv, (void *)g_context);
        g_retrans_timer  = HAL_Timer_Create("retrans", CoAPServer_retransmit, (void *)g_context);
        if (NULL != g_retrans_timer) {
            HAL_Timer_Start(g_retrans_timer, COAP_SERV_WAIT_TIME_MS);
        }
#endif
    } else {
        COAP_INFO("The CoAP Server already init");
    }

    HAL_MutexLock(g_coap_serv_mutex);
    g_call_num ++;
    HAL_MutexUnlock(g_coap_serv_mutex);

    return (CoAPContext *)g_context;
}

typedef void (*func_v_v)(void *);
static func_v_v coapserver_timer = NULL;
void CoAPServer_add_timer (void (*on_timer)(void *))
{
    coapserver_timer = on_timer;
}

void *CoAPServer_yield(void *param)
{
#ifndef HAL_ASYNC_API
    CoAPContext *context = (CoAPContext *)param;
    COAP_DEBUG("Enter to CoAP daemon task");
    while (g_coap_running) {
        CoAPMessage_cycle(context);
        if (coapserver_timer) {
            coapserver_timer(context);
        }
    }
#ifdef COAP_SERV_MULTITHREAD
    HAL_SemaphorePost(g_semphore);
    COAP_INFO("Exit the CoAP daemon task, Post semphore");

    HAL_ThreadDelete(NULL);
    g_coap_thread = NULL;
#endif
#endif
    return NULL;
}

void CoAPServer_deinit0(CoAPContext *context)
{
#ifdef HAL_ASYNC_API
    intptr_t  fd = -1;
#endif

    if (context != g_context) {
        COAP_INFO("Invalid CoAP Server context");
        return;
    }

    COAP_INFO("CoAP Server deinit");
    g_coap_running = 0;

#ifdef COAP_SERV_MULTITHREAD
    if (NULL != g_semphore) {
        HAL_SemaphoreWait(g_semphore, PLATFORM_WAIT_INFINITE);
        COAP_INFO("Wait Semaphore, will exit task");
        HAL_SemaphoreDestroy(g_semphore);
        g_semphore = NULL;
    }
#endif

#ifdef HAL_ASYNC_API
    fd = CoAPContextFd_get(context);
    HAL_Unregister_Recv_Callback(fd, CoAPServer_recv);
    if (NULL != g_retrans_timer) {
        HAL_Timer_Stop(g_retrans_timer);
        HAL_Timer_Delete(g_retrans_timer);
        g_retrans_timer = NULL;
    }
    if (NULL != g_deinit_timer) {
        HAL_Timer_Stop(g_deinit_timer);
        HAL_Timer_Delete(g_deinit_timer);
        g_deinit_timer = NULL;
    }
#endif

    HAL_MutexDestroy(g_coap_serv_mutex);
    g_coap_serv_mutex = NULL;

    if (NULL != context) {
        CoAPContext_free(context);
        g_context = NULL;
    }
}

void CoAPServer_deinit(CoAPContext *context)
{
    HAL_MutexLock(g_coap_serv_mutex);
    if (0 == g_call_num) {
        HAL_MutexUnlock(g_coap_serv_mutex);
#ifdef HAL_ASYNC_API
        g_deinit_timer  = HAL_Timer_Create("CoAPDeinit", CoAPServer_deinit0, (void *)context);
        HAL_Timer_Start(g_deinit_timer, 0);
#else
        CoAPServer_deinit0(context);
        HAL_SleepMs(1000);
#endif
    } else {
        g_call_num --;
        HAL_MutexUnlock(g_coap_serv_mutex);
    }

}

int CoAPServer_register(CoAPContext *context, const char *uri, CoAPRecvMsgHandler callback)
{
    if (NULL == context || g_context != context) {
        return COAP_ERROR_INVALID_PARAM;
    }

    return CoAPResource_register(context, uri, COAP_PERM_GET, COAP_CT_APP_JSON, 60, callback);
}

int CoAPServerMultiCast_send(CoAPContext *context, NetworkAddr *remote, const char *uri, unsigned char *buff,
                             unsigned short len, CoAPSendMsgHandler callback, unsigned short *msgid)
{
    int ret = COAP_SUCCESS;
#if 1
    CoAPMessage message;
    unsigned char tokenlen;
    unsigned char token[COAP_MSG_MAX_TOKEN_LEN] = {0};

    if (NULL == context || g_context != context || NULL == remote
        || NULL == uri || NULL == buff || NULL == msgid) {
        return COAP_ERROR_INVALID_PARAM;
    }


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
#endif
    return ret;
}

int CoAPServerResp_send(CoAPContext *context, NetworkAddr *remote, unsigned char *buff, unsigned short len, void *req,
                        const char *paths)
{
    int ret = COAP_SUCCESS;
    CoAPMessage response;
    unsigned int observe = 0;
    CoAPMessage *request = (CoAPMessage *)req;

    if (NULL == context || g_context != context || NULL == remote
        || NULL == buff || NULL == paths || NULL == req) {
        return COAP_ERROR_INVALID_PARAM;
    }

    CoAPMessage_init(&response);
    CoAPMessageType_set(&response, COAP_MESSAGE_TYPE_NON);
    CoAPMessageCode_set(&response, COAP_MSG_CODE_205_CONTENT);
    CoAPMessageId_set(&response, request->header.msgid);
    CoAPMessageToken_set(&response, request->token, request->header.tokenlen);

    ret = CoAPUintOption_get(request, COAP_OPTION_OBSERVE, &observe);
    if (COAP_SUCCESS == ret && 0 == observe) {
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

void CoAPServer_loop(CoAPContext *context)
{
    if (g_context != context  || 1 == g_coap_running) {
        COAP_INFO("The CoAP Server is already running");
        return;
    }

#ifndef COAP_SERV_MULTITHREAD
    g_coap_running = 1;
    CoAPServer_yield((void *)context);
#endif

}
