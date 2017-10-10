/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aos/network.h>

#include "wsf_client.h"
#include "wsf_config.h"
#include "wsf_defines.h"
#include "wsf_log.h"
#include "wsf_msg_queue.h"
#include "wsf_msg.h"
#include "wsf_network.h"
#include "wsf_worker.h"
#include "os.h"


extern void *msg_id_lock;
extern void *sess_lock;
extern void *device_lock;

wsf_msg_t *__wsf_invoke_sync(wsf_msg_t *req);
extern wsf_request_queue_t *global_request_queue;

static wsf_response_t *wsf_respone_create();
static void wsf_os_free_resource(void);

static volatile int initialized = 0;

wsf_push_callback push_callback = NULL;

wsf_error_callback error_callback = NULL;
#define MODULE_NAME "wsf_client"
char *global_secret_key = NULL;
static wsf_code wsf_allocate_resource(void)
{
    msg_id_lock = os_mutex_init();
    sess_lock = os_mutex_init( );
    device_lock = os_mutex_init( );
    init_req_glist();

    wsf_msg_queue_init(&global_request_queue);

    if ( (NULL == msg_id_lock)
         || (NULL == sess_lock)
         || (NULL == device_lock)
         || (NULL == global_request_queue)) {
        wsf_os_free_resource();

        return WSF_FAIL;
    }

    return WSF_SUCCESS;
}


static void wsf_os_free_resource(void)
{
    if ( NULL != msg_id_lock ) {
        os_mutex_destroy(msg_id_lock);
        msg_id_lock = NULL;
    }

    if ( NULL != sess_lock ) {
        os_mutex_destroy(sess_lock);
        sess_lock = NULL;
    }

    if ( NULL != device_lock ) {
        os_mutex_destroy(device_lock);
        device_lock = NULL;
    }

    if ( NULL != global_request_queue) {
        wsf_msg_queue_destroy(global_request_queue);
        global_request_queue = NULL;
    }
    deinit_req_glist();
}


wsf_code wsf_init()
{

    if (!initialized) {
        LOGI(MODULE_NAME, "wsf initializing...\n");

        wsf_code result = wsf_init_config();
        if (result != WSF_SUCCESS) {
            LOGE(MODULE_NAME, "failed to init wsf config \n");
            return result;
        }

        if ( WSF_FAIL == wsf_allocate_resource()) {
            LOGF(MODULE_NAME, "faild to allocate resource ");
            return WSF_FAIL;
        }

        wsf_config_t *global_config = wsf_get_config();

        result = wsf_start_worker(global_config);
        if (result != WSF_SUCCESS) {
            LOGE(MODULE_NAME, "failed to start worker thread");
            return result;
        }

        result = wsf_open_connection(global_config);
        if (result != WSF_SUCCESS) {
            LOGE(MODULE_NAME, "failed to open wsf connection");
            //return result;
        }
        initialized = 1;
    }

    return WSF_SUCCESS;
}



wsf_code wsf_shutdown()
{
    //stop worker, close connection
    wsf_stop_worker();
    wsf_destroy_connections();

    wsf_os_free_resource();

    wsf_destroy_config();

    initialized = 0;

    //LOGT();
    return WSF_SUCCESS;
}
typedef struct {
    wsf_async_cb_t  cb;
    wsf_msg_t       *req ;
    void            *extra;
} __wsf_client_cb_t;
static void *__analy_resp(wsf_msg_t *response, void *arg)
{
    wsf_response_t *ret = wsf_respone_create();
    __wsf_client_cb_t *cb = (__wsf_client_cb_t *)arg;
    //LOG("get response back.\n");
    //timeout as server error
    if (!response) {
        if (ret) {
            ret->result = INVOKE_SERVER_ERROR;
        }
    } else {
        uint8_t *pp = response->payload;
        // result code
        ret->result = *pp;
        pp++;
        //skip session id & length
        pp += ((uint8_t) * pp);
        pp++;
        //skip device id & length
        pp += ((uint8_t) * pp);
        pp++;
        //response data length
        memcpy(&ret->length, pp, sizeof(int32_t));
        ret->length = ntohl(ret->length);

        pp += sizeof(int32_t);

        ret->data = (char *)os_malloc(ret->length + 1);
        if (ret->data) {
            memset(ret->data, 0, ret->length + 1);
            memcpy(ret->data, pp, ret->length);
        } else {
            LOGE(MODULE_NAME, "failed to copy response data, out of memory");
        }
    }

    if (response) {
        os_free(response);
    }
    if (cb) {
        cb->cb(ret, cb->extra);
        os_free(cb->req);
        os_free(cb);
    }
    return ret;
}


int wsf_invoke_async(const char *service_name, wsf_list_t *parameters,
                     wsf_async_cb_t cb, void *arg)
{
    if (!initialized) {
        LOGI(MODULE_NAME, "wsf is uninitialized.\n");
        return -1;
    }

    wsf_msg_t *request = wsf_msg_request_create(service_name, parameters);
    if (NULL == request) {
        return -1;
    }
    __wsf_client_cb_t *_cb = NULL;
    if (cb) {
        _cb = (__wsf_client_cb_t *)os_malloc(sizeof(__wsf_client_cb_t));
        if (!_cb) {
            os_free(request);
            return -1;
        }
        _cb->cb = cb;
        _cb->extra = arg;
        _cb->req = request;
    }
    int ret = __wsf_invoke_async(request, __analy_resp, _cb);
    if (ret == 0) {
        return ret;
    }

    if (cb) {
        cb(NULL, arg);
    }
    os_free(request);
    os_free(_cb);

    return ret;
}

wsf_response_t *wsf_invoke(const char *service_name, wsf_list_t *parameters,
                           int timeout)
{
    if (!initialized) {
        LOGI(MODULE_NAME, "wsf is uninitialized.\n");
        return NULL;
    }

    wsf_msg_t *request = wsf_msg_request_create(service_name, parameters);
    if (NULL == request) {
        return NULL;
    }

    wsf_msg_t *response = __wsf_invoke_sync(request);

    wsf_response_t *ret = __analy_resp(response, NULL);
    os_free(request);
    return ret;
}

wsf_code wsf_register_push_callback(wsf_push_callback callback)
{
    push_callback = callback;
    return WSF_SUCCESS;
}

static wsf_response_t *wsf_respone_create()
{
    wsf_response_t *ret = (wsf_response_t *) os_malloc(sizeof(wsf_response_t));
    if (ret) {
        ret->result = INVOKE_RIGHT;
        ret->length = 0;
        ret->data   = NULL;
    } else {
        LOGE(MODULE_NAME, "failed to create response, out of memory");
    }

    return ret;
}


wsf_code wsf_response_destroy(wsf_response_t *response, int os_free_data)
{
    if (os_free_data && response->data) {
        os_free(response->data);
    }
    os_free(response);
    return WSF_SUCCESS;
}

void wsf_wait()
{
    wsf_wait_worker();
}

wsf_code wsf_set_secret_key(const char *secret_key)
{
    if (!secret_key) {
        if (global_secret_key) {
            os_free(global_secret_key);
        }
        global_secret_key = NULL;
    } else {
        size_t len = strlen(secret_key) + 1;
        char *tmp = (char *)os_malloc(len);
        if (!tmp) {
            LOGE(MODULE_NAME, "failed to copy secret key, out of memory");
            return WSF_FAIL;
        }
        memset(tmp, 0, len);
        strcpy(tmp, secret_key);
        global_secret_key = tmp;
    }

    return WSF_SUCCESS;
}

wsf_code wsf_register_error_callback(wsf_error_callback callback)
{
    error_callback = callback;
    return WSF_SUCCESS;
}
