/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include "connectivity_manager.h"
#include "connectivity.h"
#include "aos/aos.h"
#include "wsf.h"
#include "wsf_client.h"
#include "wsf_config.h"
#include "wsf_defines.h"
#include "os.h"

#define WSF_DEBUG 1
#if(WSF_DEBUG==1)
#define wsf_debug LOGD
#else
#define wsf_debug ;
#endif

#define STR_LONG_LEN                    (128)

static dlist_t g_wsf_listener_list;
static uint8_t wsf_uplink_buff[ALINK_BUF_SIZE];
static uint8_t wsf_downlink_buff[ALINK_BUF_SIZE];
static uint32_t last_rtt = 0;
static uint32_t average_rtt = 0;
static uint32_t send_time = 0;

static int wsf_prepare();
static int wsf_broadcast(int, void *, int, void *, int *);
static int wsf_notify_event(connectivity_cb, int);
static int wsf_broadcast_event(int);
static int wsf_broadcast_data( void *, int, void *, int *);
static void wsf_clear_message(uint8_t *, int);

CONNECTIVITY_DEFINE(wsf);

void wsf_init_buff(uint8_t **pp_uplink, uint8_t **pp_downlink)
{
    *pp_uplink = wsf_uplink_buff;
    *pp_downlink = wsf_downlink_buff;
}

int wsf_connect()
{
    LOGI(MODULE_NAME, "wsf_connect");
    //Note: wsf_worker thread will handle network down case
    return wsf_prepare();
}

int wsf_disconnect()
{
    LOGI(MODULE_NAME, "wsf_disconnect");
    return wsf_shutdown();
}

typedef void *(*_wsf_send_cb)(void *, void *);
typedef struct {
    _wsf_send_cb cb;
    void *arg;
    uint32_t time;
} __wsf_send_cb_t;

static void *__analy_resp(wsf_response_t *wsf_rsp, __wsf_send_cb_t *cb)
{
    __wsf_send_cb_t *_cb = cb;
    uint32_t time_after_send;
    connectivity_rsp_t *conn_rsp = NULL;
    if (wsf_rsp) {
        if (wsf_rsp->result == 0) {
            conn_rsp = (connectivity_rsp_t *)wsf_uplink_buff;
            memset(conn_rsp, 0, sizeof(wsf_uplink_buff));
            conn_rsp->result = CONNECT_RESULT_OK;
            conn_rsp->len = wsf_rsp->length;
            memcpy(conn_rsp->data, wsf_rsp->data, wsf_rsp->length);
            wsf_clear_message(conn_rsp->data, conn_rsp->len);
            wsf_debug(MODULE_NAME, "response: %s", conn_rsp->data);
            time_after_send = os_get_time_ms();
            last_rtt =  time_after_send - cb->time;
            average_rtt = (average_rtt * send_time + last_rtt) / (send_time + 1);
            send_time ++;
        } else {
            LOGE(MODULE_NAME, "wsf_invoke:%d", wsf_rsp->result);
        }
        wsf_response_destroy(wsf_rsp, 1);
    } else {
        LOGE(MODULE_NAME, "wsf_invoke timeout!!!");
    }

    wsf_debug(MODULE_NAME, "conn_rsp cb done: %p", _cb->cb);
    if (_cb->cb) {
        _cb->cb(conn_rsp, _cb->arg);
        os_free(_cb);
    }
    return conn_rsp;
}

int wsf_send_async(void *pdata, int len, void *(*cb)(connectivity_rsp_t *rsp,
                                                     void *), void *arg)
{
    connectivity_rsp_t *conn_rsp = NULL;
    char *service_name = "devService.request";
    uint32_t time_before_send = os_get_time_ms();
    int reqtimeout_interval = config_get_reqtimeout_interval();
    wsf_debug(MODULE_NAME, "wsf_send: %s", (char *)pdata);
    wsf_list_t arg_list;
    __wsf_send_cb_t *_cb = NULL;

    if (cb) {
        _cb = os_malloc(sizeof(__wsf_send_cb_t));
        if (!_cb) {
            return 0;
        }
        _cb->cb = (_wsf_send_cb)cb;
        _cb->arg = arg;
        _cb->time = time_before_send;
    }

    wsf_list_init(&arg_list);
    wsf_list_push_back(&arg_list, pdata, len);
    int ret = wsf_invoke_async(service_name, &arg_list, __analy_resp, _cb);
    wsf_list_os_free(&arg_list);

    return ret;
}


/* wsf_invoke response result
0 调用成功
1 登录成功
2 服务需要登录才能调用
3 请求上下文出错
4 包解析错误
5 服务名错误
6 服务端错误
7 消息类型错误
8 Session无效
9 服务端忙，调用被拒绝
*/
connectivity_rsp_t *wsf_send(void *pdata, int len)
{
    connectivity_rsp_t *conn_rsp = 0;
    char *service_name = "devService.request";
    uint32_t time_before_send = os_get_time_ms();
    int reqtimeout_interval = config_get_reqtimeout_interval();
    wsf_debug(MODULE_NAME, "wsf_send: %s", (char *)pdata);
    wsf_list_t arg_list;

    wsf_list_init(&arg_list);
    wsf_list_push_back(&arg_list, pdata, len);
    wsf_response_t *wsf_rsp = wsf_invoke(service_name, &arg_list,
                                         reqtimeout_interval);
    wsf_list_os_free(&arg_list);
    __wsf_send_cb_t _cb;
    _cb.cb = NULL;
    _cb.time = time_before_send;
    return __analy_resp(wsf_rsp, &_cb);
}

int wsf_add_listener(connectivity_cb func)
{
    connectivity_listener_t *listener = (connectivity_listener_t *) os_malloc(
                                            sizeof(connectivity_listener_t));
    listener->listen = func;
    dlist_add(&listener->list_head, &g_wsf_listener_list);
    wsf_notify_event(func, CONNECT_BIND);
    return CONNECT_RESULT_OK;
}

int wsf_del_listener(connectivity_cb func)
{
    connectivity_listener_t *pos;
    dlist_t *tmp = NULL;
    dlist_for_each_entry_safe(&g_wsf_listener_list, tmp, pos,
                              connectivity_listener_t, list_head) {
        if (((connectivity_listener_t *) pos)->listen == func) {
            dlist_del(&pos->list_head);
            os_free(pos);
            wsf_notify_event(func, CONNECT_RELEASE);
        }
    }
    return CONNECT_RESULT_OK;
}

int wsf_set_state(int st)
{
    wsf_debug(MODULE_NAME, "%s -> %s", (char *)cm_code2string(wsf.state),
              (char *)cm_code2string(st));
    if (st != wsf.state) {
        wsf.state = st;
        wsf_broadcast_event(wsf.state);
    }
    return CONNECT_RESULT_OK;
}

static int wsf_broadcast(int type, void *data, int dlen, void *result,
                         int *rlen)
{
    connectivity_listener_t *pos;
    dlist_for_each_entry_reverse(pos, &g_wsf_listener_list, list_head,
                                 connectivity_listener_t) {
        connectivity_cb func = *((connectivity_listener_t *) pos)->listen;
        func(type, data, dlen, result, rlen);
    }
    return CONNECT_RESULT_OK;
}

static int wsf_notify_event(connectivity_cb func, int evt)
{
    if (func) {
        func(CONNECT_EVENT, &evt, sizeof(evt), 0, 0);
    }
    return CONNECT_RESULT_OK;
}

static int wsf_broadcast_event(int evt)
{
    return wsf_broadcast(CONNECT_EVENT, &evt, sizeof(evt), 0, 0);
}

static int wsf_broadcast_data(void *data, int dlen, void *result, int *rlen)
{
    return wsf_broadcast(CONNECT_DATA, data, dlen, result, rlen);
}

static wsf_response_t *wsf_data_listener(void *data, int dlen)
{
    void *result = wsf_downlink_buff;
    int rlen = sizeof(wsf_downlink_buff);
    wsf_clear_message(data, dlen);
    wsf_debug(MODULE_NAME, "wsf_recv: %s", (char *)data);

    memset(wsf_downlink_buff, 0, sizeof(wsf_downlink_buff));
    wsf_broadcast_data(data, dlen, result, &rlen);

    /* rsp will be os_free by wsf callback thread */
    wsf_response_t *rsp = (wsf_response_t *)os_malloc(sizeof(wsf_response_t));
    rsp->result = INVOKE_RIGHT;
    rsp->length = rlen;
    rsp->data = result;
    wsf_debug(MODULE_NAME, "wsf_recv response: %s", (char *)rsp->data);
    return rsp;
}

extern const char *default_server;
extern const char *default_ca_str;
extern const char *debug_ca_str;

static int wsf_prepare()
{
    int reqtimeout_interval = config_get_reqtimeout_interval();
    int heartbeat_interval = config_get_heartbeat_interval();
    char *server = (char *)os_malloc(STR_LONG_LEN);
    const char *ca_str = NULL;
    int port = 0;

    OS_CHECK_MALLOC(server);

    config_get_server_info(server, &port);

    wsf_config(SERVER_NAME_STRING, server);
    wsf_config(SERVER_PORT_INT, &port);

    if (strstr(server, default_server)) {
        ca_str = default_ca_str;
    } else {
        ca_str = debug_ca_str;
    }

    wsf_config(CERTIFICATE_INT, (void *)ca_str);
    os_free(server);

    wsf_config(REQUEST_TIMEOUT_INT, &reqtimeout_interval);
    wsf_config(HEARTBEAT_INTERVAL_INT, &heartbeat_interval);
    wsf_config(USER_HEARTBEAT_INTERVAL_INT, &heartbeat_interval);

    wsf_register_push_callback(wsf_data_listener);

    //Note: wsf_worker thread will handle network down case
    return ((wsf_init() == WSF_SUCCESS) ? CONNECT_RESULT_OK : CONNECT_RESULT_ERR);
}

static void wsf_clear_message(uint8_t *msg, int len)
{
    if (!msg || len == 0) {
        return;
    }
    if (msg[0] == '"') {
        msg[0] = '\\';
    }
    if (msg[len - 1] == '"') {
        msg[len - 1] = '\\';
    }
    os_squeeze(msg, '\\', len);
}

void get_last_rtt(char *dev_stats, int len)
{
    sprintf(dev_stats, "%dms", last_rtt);
}

void get_average_rtt(char *dev_stats, int len)
{
    sprintf(dev_stats, "%dms", average_rtt);
}
