/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <string.h>
#include <aos/kernel.h>
#include <aos/debug.h>

#include "wifi_provisioning.h"

typedef struct {
    uint32_t method_id;
    wifi_prov_event_t event;
    wifi_prov_result_t result;
} wifi_prov_task_ctx_t;


static slist_t g_prov_list;
static aos_mutex_t g_wifi_prov_mutex;
static wifi_prov_cb g_wiri_prov_cb;
static aos_timer_t g_wifi_prov_timer;
static uint32_t g_wifi_prov_started_method = 0;

#define WIFI_PROV_LOCK()                                     \
    do {                                                     \
        if (!aos_mutex_is_valid(&g_wifi_prov_mutex))              \
            aos_mutex_new(&g_wifi_prov_mutex);                    \
        aos_mutex_lock(&g_wifi_prov_mutex, AOS_WAIT_FOREVER);     \
    } while (0)

#define WIFI_PROV_UNLOCK()                                    \
    do {                                                      \
        aos_mutex_unlock(&g_wifi_prov_mutex);                      \
    } while (0)

static void _stop_method(uint32_t method)
{
    wifi_prov_t *tmp_prov;

    WIFI_PROV_LOCK();
    slist_for_each_entry(&g_prov_list, tmp_prov, wifi_prov_t, next) {
        if (method & tmp_prov->method_id) {
            tmp_prov->stop();
        }
    }
    g_wifi_prov_started_method = 0;
    WIFI_PROV_UNLOCK();
}

static void result_task(void *args) {
    wifi_prov_task_ctx_t *ctx = args;

    aos_timer_stop(&g_wifi_prov_timer);

    /* stop all method here if we got the result */
    _stop_method(g_wifi_prov_started_method);

    g_wifi_prov_started_method = 0;

    if (g_wiri_prov_cb) {
        WIFI_PROV_LOCK();
        g_wiri_prov_cb(ctx->method_id, ctx->event, &ctx->result);
        WIFI_PROV_UNLOCK();
    }

    free(ctx);
}

static void wifi_prov_callback(uint32_t method_id, wifi_prov_event_t event, wifi_prov_result_t *result)
{
    if (event == WIFI_RPOV_EVENT_GOT_RESULT && g_wifi_prov_started_method) {

        wifi_prov_task_ctx_t *ctx = malloc(sizeof(wifi_prov_task_ctx_t));

        if (ctx == NULL) {
            return;
        }

        ctx->method_id = method_id;
        ctx->event = event;
        memcpy(&ctx->result, result, sizeof(wifi_prov_result_t));

        /* create a task to avoid deallock */
        aos_task_new("prov_result", result_task, ctx, 64*1024);
    }
}

static void prov_timer_entry(void *timer, void *arg)
{
    wifi_prov_task_ctx_t *ctx = malloc(sizeof(wifi_prov_task_ctx_t));

    if (ctx == NULL) {
        return;
    }

    ctx->method_id = 0;
    ctx->event = WIFI_PROV_EVENT_TIMEOUT;

    /* create a task to avoid deallock */
    aos_task_new("prov_result", result_task, ctx, 64*1024);
}

int wifi_prov_method_register(wifi_prov_t *prov)
{
    wifi_prov_t *tmp_prov;
    uint32_t i = 0;

    aos_assert(prov);

    WIFI_PROV_LOCK();
    slist_for_each_entry(&g_prov_list, tmp_prov, wifi_prov_t, next) {
        /* if we got some name retuen fail */
        if (strcmp(tmp_prov->name, prov->name) == 0) {
            WIFI_PROV_UNLOCK();
            return -1;
        }
        i ++;
    }

    prov->method_id = (1 << i);

    slist_add_tail(&prov->next, &g_prov_list);
    WIFI_PROV_UNLOCK();

    return prov->method_id;
}

int wifi_prov_start(uint32_t method_id, wifi_prov_cb cb, uint32_t timeout_s)
{
    wifi_prov_t *tmp_prov;
    int res = 0;

    aos_assert(method_id);
    aos_assert(!g_wifi_prov_started_method);
    aos_assert(cb);

    g_wiri_prov_cb = cb;

    WIFI_PROV_LOCK();
    slist_for_each_entry(&g_prov_list, tmp_prov, wifi_prov_t, next) {
        if (tmp_prov->method_id & method_id) {
            method_id &= ~tmp_prov->method_id;
            if (tmp_prov->start(wifi_prov_callback)) {
                res |= tmp_prov->method_id;
            } else {
                g_wifi_prov_started_method |= tmp_prov->method_id;
            }
        }
    }
    WIFI_PROV_UNLOCK();

    /* if we has method_id start success */
    if (g_wifi_prov_started_method) {
        if (!aos_timer_is_valid(&g_wifi_prov_timer)) {
            aos_timer_new(&g_wifi_prov_timer, prov_timer_entry, NULL, timeout_s * 1000, 0);
        } else {
            aos_timer_change_once(&g_wifi_prov_timer, timeout_s * 1000);
            aos_timer_start(&g_wifi_prov_timer);
        }
    }

    return res | method_id;
}

void wifi_prov_stop()
{
    aos_assert(g_wifi_prov_started_method);

    g_wiri_prov_cb = NULL;

    _stop_method(g_wifi_prov_started_method);

    g_wifi_prov_started_method = 0;
}

uint32_t wifi_prov_get_method_id(char *method_name)
{
    wifi_prov_t *tmp_prov;

    aos_assert(method_name);

    WIFI_PROV_LOCK();
    slist_for_each_entry(&g_prov_list, tmp_prov, wifi_prov_t, next) {
        /* if we got some name retuen fail */
        if (strcmp(tmp_prov->name, method_name) == 0) {
            WIFI_PROV_UNLOCK();
            return tmp_prov->method_id;
        }
    }
    WIFI_PROV_UNLOCK();

    return 0;
}
