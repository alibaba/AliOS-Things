/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "accs.h"
#include "alink_protocol.h"
#include "connectivity_manager.h"
#include "aos/aos.h"
#include "service_manager.h"
#include "wsf.h"
#include "alink_export_internal.h"
#include "os.h"
#include "service.h"

#define ACCS_DEBUG 1
#if(ACCS_DEBUG==1)
#define accs_debug LOGD
#else
#define accs_debug
#endif

static dlist_t g_accs_listener_list;
const device_t *main_device = 0;
const connectivity_t *remote_conn = 0;
uint8_t *uplink_buff = 0;
uint8_t *downlink_buff = 0;
void *link_buff_sem;

static int accs_set_state(int);
static int accs_get_state(void);
static int accs_conn_listener(int, void *, int, void *, int *);
static int accs_broadcast(int, void *, int, void *, int *);
static int accs_notify_event(service_cb, int);
static int accs_broadcast_event(int);
static int accs_notify_data(service_cb, void *, int, void *, int *);
static int accs_broadcast_data(void *, int, void *, int *);
static void accs_handshake(void *arg);
static void accs_handshake_async(void *arg);
static int accs_event_handler(int type, void *data, int dlen, void *result,
                              int *rlen);

#define MODULE_NAME_ACCS "accs"
SERVICE_DEFINE(accs);

void start_accs_work(int delay)
{
    //aos_loop_schedule_work(0,accs_handshake,NULL,NULL,NULL);

    aos_loop_schedule_work(delay, accs_handshake_async, NULL, NULL, NULL);
}

/*
 * SERVICE_STATE_INIT: cloud thread is working
 * SERVICE_STATE_PREPARE: cloud is connected
 * SERVICE_STATE_READY: accs ready
 * SERVICE_STATE_STOP: accs stoped
 */
int accs_prepare()
{
    if (accs_get_state() == SERVICE_STATE_STOP) {
        accs_set_state(SERVICE_STATE_INIT);
        main_device = get_main_dev();
        remote_conn = cm_get_conn("wsf");
        cm_bind_conn("wsf", &accs_conn_listener);
        link_buff_sem = os_semaphore_init();
        os_semaphore_post(link_buff_sem);
        remote_conn->init_buff(&uplink_buff, &downlink_buff);
        remote_conn->connect();//FIXME: what happen if connection failed
    }
    return SERVICE_RESULT_OK;
}

int accs_start()
{
    //start_accs_work(0);
    accs_add_listener(&accs_event_handler);
    return SERVICE_RESULT_OK;
}

int accs_stop()
{
    if (accs_get_state() != SERVICE_STATE_STOP) {
        LOG("accs will stop current session.");
        cm_get_conn("wsf")->disconnect();//wsf_disconnect
        cm_release_conn("wsf", &accs_conn_listener);
        accs_set_state(SERVICE_STATE_STOP);
        os_semaphore_destroy(link_buff_sem);
        accs_del_listener(&accs_event_handler);
    }
    return SERVICE_RESULT_OK;
}

int accs_put_async(void *in, int len, void *(*cb)(void *), void *arg)
{
    int ret;
    if (accs_get_state() != SERVICE_STATE_READY) {
        LOGW(MODULE_NAME_ACCS, "accs not ready!");
        return (0 - accs_get_state());
    }
    /* handle transaction */
    alink_data_t *pack = (alink_data_t *)in;
    ret = __alink_post_async(pack->method, pack->data, cb, arg);
    return (ret == ALINK_CODE_SUCCESS) ? SERVICE_RESULT_OK : ret;
}

int accs_put(void *in, int len)
{
    int ret;
    if (accs_get_state() != SERVICE_STATE_READY) {
        LOGW(MODULE_NAME_ACCS, "accs not ready!");
        return (0 - accs_get_state());
    }
    /* handle transaction */
    alink_data_t *pack = (alink_data_t *)in;
    ret = __alink_post(pack->method, pack->data);
    return (ret == ALINK_CODE_SUCCESS) ? SERVICE_RESULT_OK : ret;
}

int accs_get(void *in, int inlen, void *out, int outlen)
{
    int ret;
    if (accs_get_state() != SERVICE_STATE_READY) {
        LOGW(MODULE_NAME_ACCS, "accs not ready!");
        return (0 - accs_get_state());
    }
    if (!out || outlen <= 0) {
        LOGE(MODULE_NAME_ACCS, "accs_get fail, illegal output buffer !");
        return SERVICE_RESULT_ERR;
    }

    /* handle transaction */
    alink_data_t *p = (alink_data_t *)in;
    ret = alink_get(p->method, p->data, out, outlen);
    return (ret == ALINK_CODE_SUCCESS) ? SERVICE_RESULT_OK : ret;
}

int accs_add_listener(service_cb func)
{
    service_listener_t *listener = (service_listener_t *)os_malloc(sizeof(
                                                                       service_listener_t));
    listener->listen = func;
    dlist_add(&listener->list_head, &g_accs_listener_list);
    LOGD(MODULE_NAME_ACCS, "accs add listerner: %p", func);
    accs_notify_event(func, SERVICE_ATTACH);
    return SERVICE_RESULT_OK;
}

int accs_del_listener(service_cb func)
{
    service_listener_t *pos;
    dlist_t *tmp = NULL;
    dlist_for_each_entry_safe(&g_accs_listener_list, tmp, pos, service_listener_t,
                              list_head) {
        if (pos->listen == func) {
            LOGD(MODULE_NAME_ACCS, "accs del listerner: %p", func);
            accs_notify_event(func, SERVICE_DETACH);
            dlist_del(&pos->list_head);
            os_free(pos);
        }
    }
    return SERVICE_RESULT_OK;
}

static int accs_set_state(int st)
{
    accs_debug(MODULE_NAME_ACCS, "%s -> %s", sm_code2string(accs.state),
               sm_code2string(st));
    accs.state = st;
    accs_broadcast_event(accs.state);
    return SERVICE_RESULT_OK;
}

static int accs_get_state()
{
    return accs.state;
}

#define ALINK_RESPONSE_OK   "{\"id\":%d,\"result\":{\"msg\":\"success\",\"code\":1000}}"
static int accs_conn_listener(int type, void *data, int dlen, void *result,
                              int *rlen)
{
    if (type == CONNECT_EVENT) {
        int st = *((int *)data);
        accs_debug(MODULE_NAME_ACCS, "ACCS recv %s, %s", cm_code2string(type),
                   cm_code2string(st));
        if (st == CONNECT_STATE_OPEN) {
            ; //ignore connect open event
        } else if (st == CONNECT_STATE_READY) {
            accs_set_state(SERVICE_STATE_PREPARE);
            LOG("we will start handshake work.");
            start_accs_work(0);
        } else if (st == CONNECT_STATE_CLOSE) {
            LOG("ACCS: disconnected");
            void *cb = alink_cb_func[_ALINK_CLOUD_DISCONNECTED];
            if (cb) {
                void (*func)(void) = cb;
                func();
            }
            accs_set_state(SERVICE_STATE_INIT);
            aos_post_event(EV_SYS, CODE_SYS_ON_ALINK_OFFLINE, 0);
        }
    } else if (type == CONNECT_DATA) {
        alink_data_t pack;
        if (alink_parse_data(data, dlen, &pack) != ALINK_CODE_SUCCESS) {
            LOGE(MODULE_NAME_ACCS, "ACCS recv malformed pack");
        } else {
            char *str = result;
            accs_broadcast_data(&pack, dlen, result, rlen);
            /* if result is empty, fill it with response OK */
            if (str && str[0] == '\0') {
                *rlen = sprintf(str, ALINK_RESPONSE_OK, last_state.id);
            }
        }
    }
    return 0;
}

static int accs_broadcast(int type, void *data, int dlen, void *result,
                          int *rlen)
{
    service_listener_t *pos;
    dlist_for_each_entry_reverse(pos, &g_accs_listener_list, list_head,
                                 service_listener_t) {
        service_cb func = *pos->listen;
        if (func(type, data, dlen, result, rlen) == EVENT_CONSUMED) {
            accs_debug(MODULE_NAME_ACCS, "ACCS broadcast consumed by listener:%p", func);
            break;
        }
    }
    return SERVICE_RESULT_OK;
}

static int accs_notify_event(service_cb func, int evt)
{
    if (func) {
        func(SERVICE_EVENT, &evt, sizeof(evt), 0, 0);
    }
    return SERVICE_RESULT_OK;
}

static int accs_broadcast_event(int evt)
{
    return accs_broadcast(SERVICE_EVENT, &evt, sizeof(evt), 0, 0);
}

static int accs_notify_data(service_cb func, void *data, int dlen, void *result,
                            int *rlen)
{
    if (func) {
        func(SERVICE_DATA, data, dlen, result, rlen);
    }
    return SERVICE_RESULT_OK;
}

static int accs_broadcast_data(void *data, int dlen, void *result, int *rlen)
{
    return accs_broadcast(SERVICE_DATA, data, dlen, result, rlen);
}

static void accs_handshake_async(void *arg)
{
    LOG("handshake work started.");
    if (accs_get_state() == SERVICE_STATE_PREPARE) {
        if (alink_handshake_async() == SERVICE_RESULT_OK) {
            accs_set_state(SERVICE_STATE_READY);
            LOG("ACCS: connected");
            void *cb = alink_cb_func[_ALINK_CLOUD_CONNECTED];
            if (cb) {
                void (*func)(void) = cb;
                func();
            }
        } else {
            accs_set_state(SERVICE_STATE_PREPARE);
        }
    }
}

static void accs_handshake(void *arg)
{
    /*//tz  TODO: this code block will be removed if everything runs well for long time.
    char ssid[PLATFORM_MAX_SSID_LEN];
    memset(ssid, 0, PLATFORM_MAX_SSID_LEN);
    os_wifi_get_APInfo(ssid, NULL, NULL);
    if (strcmp(ssid, DEFAULT_SSID) == 0){
        return SERVICE_RESULT_OK;
    }
    */
    if (accs_get_state() == SERVICE_STATE_PREPARE) {
        if (alink_handshake() == SERVICE_RESULT_OK) {
            LOG("ACCS: connected");
            void *cb = alink_cb_func[_ALINK_CLOUD_CONNECTED];
            accs_set_state(SERVICE_STATE_READY);
            if (cb) {
                void (*func)(void) = cb;
                func();
            }
        } else {
            accs_set_state(SERVICE_STATE_PREPARE);
        }
    }
}

static int accs_event_handler(int type, void *data, int dlen, void *result,
                              int *rlen)
{
    if (type == SERVICE_EVENT) {
        int st = *((int *)data);
        accs_debug(MODULE_NAME_ACCS, "app recv: %s, %s", sm_code2string(type),
                   sm_code2string(st));
        return EVENT_IGNORE;
    } else if (type == SERVICE_DATA) {
        alink_data_t *p = (alink_data_t *)data;
        void *cb = NULL;
        int ret = EVENT_IGNORE;

        if (!strcmp(p->method, "getDeviceStatus")) {
            cb = alink_cb_func[_ALINK_GET_DEVICE_STATUS];
            ret = EVENT_IGNORE;//ATTR handler depend on it
            if (cb) {
                int (*func)(char *) = cb;
                func(p->data);
            }
        } else if (!strcmp(p->method, "setDeviceStatus")) {
            cb = alink_cb_func[_ALINK_SET_DEVICE_STATUS];
            ret = EVENT_IGNORE;//ATTR handler depend on it
            if (cb) {
                int (*func)(char *) = cb;
                func(p->data);
            }
        } else if (!strcmp(p->method, "getDeviceStatusByRawData")) {
            cb = alink_cb_func[_ALINK_GET_DEVICE_RAWDATA];
            ret = EVENT_CONSUMED;
            if (cb) {
                int (*func)(char *) = cb;
                func(p->data);
            }
        } else if (!strcmp(p->method, "setDeviceStatusByRawData")) {
            cb = alink_cb_func[_ALINK_SET_DEVICE_RAWDATA];
            ret = EVENT_CONSUMED;
            if (cb) {
                int (*func)(char *) = cb;
                func(p->data);
            }
        } else if (!strcmp(p->method, "setDeviceStatusArray")) {
            ret = EVENT_IGNORE;
            //TODO: setDeviceStatusArray not support
        } else if (!strcmp(p->method, "upgradeDevice")) {
            LOGW(MODULE_NAME_ACCS, "start to OTA now...%s", p->data);
            aos_cloud_trigger(_ALINK_UPGRADE_DEVICE, p->data);
            cb = alink_cb_func[_ALINK_UPGRADE_DEVICE];
            ret = EVENT_CONSUMED;
            if (cb) {
                int (*func)(char *) = cb;
                func(p->data);
            }
        } else if (!strcmp(p->method, "unUpgradeDevice")) {
            LOGW(MODULE_NAME_ACCS, "stop to OTA now...%s", p->data);
            aos_cloud_trigger(_ALINK_CANCEL_UPGRADE_DEVICE, p->data);
            cb = alink_cb_func[_ALINK_CANCEL_UPGRADE_DEVICE];
            ret = EVENT_CONSUMED;
            if (cb) {
                int (*func)(char *) = cb;
                func(p->data);
            }
        } else {
            ret = EVENT_IGNORE;
        }

        return ret;
    }
    return -1;
}

int cloud_is_connected(void)
{
    return accs.state == SERVICE_STATE_READY;
}
AOS_EXPORT(int, cloud_is_connected, void);

