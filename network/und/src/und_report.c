/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#ifdef FEATURE_UND_SUPPORT

#include "linkkit/mqtt_api.h"

#include "und/und.h"
#include "und_hal.h"
#include "und_log.h"
#include "und_types.h"
#include "und_utils.h"
#include "und_sched.h"
#include "und_config.h"
#include "und_packet.h"
#include "und_manage.h"
#include "und_adapter.h"
#include "und_platform.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

struct und_report_ctx_t {
    void *mutex;
    char *buf;
    char update;
    char fast;
};

static struct und_report_ctx_t g_und_report_ctx = {0};

static void und_target_report_reply(void *pcontext, void *pclient, void *mesg)
{
    struct und_report_ctx_t *ctx = &g_und_report_ctx;

    iotx_mqtt_event_msg_pt msg = (iotx_mqtt_event_msg_pt)mesg;
    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            break;
        default:
            return;
    }

    if (ctx->mutex == NULL) {
        und_err("report relpy, not init\n");
        return;
    }

    /* success, kv clean */
    und_cap_manage_clear_kv();

    und_platform_mutex_lock(ctx->mutex);
    if (ctx->update == UND_CAP_STATE_RESYNC) {
        ctx->update = UND_CAP_STATE_UPDATE;
    } else if (ctx->update == UND_CAP_STATE_REPORT) {
        ctx->update = UND_CAP_STATE_INIT;
    }

    /* re-schedule to retransmit report pkt when capture is needed to update */
    if (ctx->fast != 0 || ctx->update != UND_CAP_STATE_INIT) {
        und_sched_start(UND_REPORT_SCHED_CYCLE_MS);
    } else {
        und_sched_start(UND_REPORT_WIRELESS_INFO_CYCLE_MS);
    }
    und_platform_mutex_unlock(ctx->mutex);
}

int und_report_init()
{
    struct und_report_ctx_t *ctx = &g_und_report_ctx;

    if (ctx->mutex) {
        und_debug("und report is ready inited\n");
        return UND_SUCCESS;
    }

    ctx->mutex = und_platform_mutex_create();

    UND_PTR_SANITY_CHECK(ctx->mutex, UND_MEM_ERR);

    und_platform_mutex_lock(ctx->mutex);
    ctx->update = UND_CAP_STATE_INIT;
    und_platform_mutex_unlock(ctx->mutex);

    return UND_SUCCESS;
}

int und_report_deinit()
{
    struct und_report_ctx_t *ctx = &g_und_report_ctx;
    void *mutex = ctx->mutex;

    UND_PTR_SANITY_CHECK(ctx->mutex, UND_ERR);

    und_platform_mutex_lock(mutex);
    if (ctx->buf) und_platform_free(ctx->buf);
    und_platform_memset(ctx, 0, sizeof(*ctx));
    und_platform_mutex_unlock(mutex);
    und_platform_mutex_destroy(mutex);

    return UND_SUCCESS;
}

static int und_build_package()
{
    int len = UND_REPORT_TARGET_BUF_LEN;
    struct und_report_ctx_t *ctx = &g_und_report_ctx;
    if (ctx->buf == NULL) {
        ctx->buf = (char *)und_platform_malloc(len);
        if (ctx->buf) und_platform_memset(ctx->buf, 0, len);
    }

    UND_PTR_SANITY_CHECK(ctx->buf, UND_MEM_ERR);

    len = und_platform_strlen(ctx->buf);

    if (len + 128 > UND_REPORT_TARGET_BUF_LEN) {
        /* buf is not enough, drop real-time capture and collect capture */
        und_platform_memset(ctx->buf, 0, UND_REPORT_TARGET_BUF_LEN);
        ctx->update = UND_CAP_STATE_UPDATE;
        len = 0;
    }

    return und_collect_package(ctx->buf + len,
            UND_REPORT_TARGET_BUF_LEN - len, len,
            ctx->update == UND_CAP_STATE_INIT ? 0 : 1);
}

int und_update_report(int cap_idx)
{
    struct und_report_ctx_t *ctx = &g_und_report_ctx;

    UND_PTR_SANITY_CHECK(ctx->mutex, UND_ERR);

    und_platform_mutex_lock(ctx->mutex);

    if (ctx->update == UND_CAP_STATE_REPORT)
        ctx->update = UND_CAP_STATE_RESYNC;

    if (ctx->update == UND_CAP_STATE_INIT)
        ctx->update = UND_CAP_STATE_UPDATE;

    /* combine wireless information with network failure or exception */
#if 0
    if (cap_idx == UND_CAPTURE_IDX(UND_STATIS_NETWORK_FAIL_IDX) ||
        cap_idx == UND_CAPTURE_IDX(UND_STATIS_NETWORK_EXCEPTION_IDX)) {
        und_build_package();
    }
#endif

    und_platform_mutex_unlock(ctx->mutex);

    return UND_SUCCESS;
}

int und_update_report_cycle(int fast)
{
    struct und_report_ctx_t *ctx = &g_und_report_ctx;

    UND_PTR_SANITY_CHECK(ctx->mutex, UND_ERR);

    ctx->fast = fast;

    return UND_SUCCESS;
}

void und_targets_report(void *param)
{
    int res;
    int plen = 0;
    char *topic_buf = NULL;
    char *pkt_buf = NULL, *target_buf = NULL;
    struct und_report_ctx_t *ctx = &g_und_report_ctx;

    if (ctx->mutex == NULL) {
        und_err("report, und cap is not inited\n");
        return;
    }

    if (und_conn_is_connected() <= 0) {
        und_warn("CMP not ready\n");
        und_sched_start(UND_REPORT_SCHED_CYCLE_MS);
        return;
    }

    topic_buf = (char *)und_platform_malloc(UND_TOPIC_BUF_LEN_MAX + 1);
    pkt_buf = (char *)und_platform_malloc(UND_REPORT_BUF_LEN_MAX + 1);
    target_buf = (char *)und_platform_malloc(UND_REPORT_BUF_LEN_MAX + 1);

    und_platform_mutex_lock(ctx->mutex);
    do {
        if (pkt_buf == NULL || target_buf == NULL || topic_buf == NULL) {
            und_err("report, alloc buf fail\n");
            break;
        }
        und_platform_memset(pkt_buf, 0, UND_REPORT_BUF_LEN_MAX + 1);

        und_build_package();

        if (ctx->buf == NULL || und_platform_strlen(ctx->buf) == 0) {
            und_info("no capture\n");
            break;
        }

        plen += und_platform_snprintf(pkt_buf + plen, UND_REPORT_BUF_LEN_MAX - plen, "%s", ctx->buf);

        if (plen <= 3) {
            und_debug("no capture\n");
            break;
        }

        /* build parameters of und report */
        und_platform_memset(target_buf, 0, UND_REPORT_BUF_LEN_MAX + 1);
        res = und_build_packet_param(pkt_buf, target_buf, UND_REPORT_BUF_LEN_MAX);
        if (res < 0) {
            und_err("build param fail:%d\n", res);
            break;
        }

        /* build packet of und report */
        und_platform_memset(pkt_buf, 0, UND_REPORT_BUF_LEN_MAX + 1);
        plen = und_build_packet(UND_ALINK_VER, target_buf, pkt_buf, UND_REPORT_BUF_LEN_MAX);
        if (plen < 0) {
            und_err("build pkt fail:%d\n", res);
            break;
        }
        und_platform_memset(topic_buf, 0, UND_TOPIC_BUF_LEN_MAX + 1);
        res = und_build_topic(UND_TOPIC_REPORT_REPLY, topic_buf, UND_TOPIC_BUF_LEN_MAX);
        if (res < 0) {
            und_err("build topic reply fail:%d\n", res);
            break;
        }

        res = und_conn_register_cb(topic_buf, und_target_report_reply);
        und_debug("reg topic res:%d\n", res);

        und_platform_memset(topic_buf, 0, UND_TOPIC_BUF_LEN_MAX + 1);
        res = und_build_topic(UND_TOPIC_REPORT, topic_buf, UND_TOPIC_BUF_LEN_MAX);
        if (res < 0) {
            und_err("build topic fail:%d\n", res);
            break;
        }

        if ((res = und_conn_send(topic_buf, pkt_buf, plen, 0)) == 0) {
            ctx->update = UND_CAP_STATE_REPORT;
            if (ctx->buf) {
                /* remove collection about wireless info & capture reported */
                und_platform_free(ctx->buf);
                ctx->buf = NULL;
            }
        }

        und_debug("topic:%s, packet:%s\n", topic_buf, pkt_buf);
        und_debug("report send %s(%d)\n", res == 0 ? "success" : "fail", res);
    } while (0);

    /* re-schedule to retransmit report pkt when capture is needed to update */
    if (ctx->update != UND_CAP_STATE_INIT) {
#ifdef AOS_UND_USE_MQTT
        und_sched_start(UND_REPORT_SCHED_CYCLE_MS);
#elif defined(AOS_UND_USE_UAGENT)
        if (ctx->update == UND_CAP_STATE_REPORT) {
            ctx->update = UND_CAP_STATE_INIT;
            und_sched_start(UND_REPORT_WIRELESS_INFO_CYCLE_MS);
        } else {
            und_sched_start(UND_REPORT_SCHED_CYCLE_MS);
        }
#endif
    } else {
        und_sched_start(UND_REPORT_WIRELESS_INFO_CYCLE_MS);
    }
    und_platform_mutex_unlock(ctx->mutex);
    if (pkt_buf) und_platform_free(pkt_buf);
    if (topic_buf) und_platform_free(topic_buf);
    if (target_buf) und_platform_free(target_buf);
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif
