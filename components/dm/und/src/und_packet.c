/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#ifdef AOS_COMP_UND

#include "linkkit/wrappers/wrappers.h"

#include "und_log.h"
#include "und_types.h"
#include "und_utils.h"
#include "und_config.h"
#include "und_packet.h"
#include "und_platform.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

struct und_packet_ctx_t {
    void *mutex;
    int packet_id;
};

static struct und_packet_ctx_t g_und_pkt_ctx = {0};

static int und_packet_id()
{
    int id = UND_ERR;
    struct und_packet_ctx_t *ctx = &g_und_pkt_ctx;

    UND_PTR_SANITY_CHECK(ctx->mutex, id);

    undp_mutex_lock(ctx->mutex);
    id = ctx->packet_id ++;
    if (ctx->packet_id == 0)
        ctx->packet_id = 1;
    undp_mutex_unlock(ctx->mutex);

    return id;
}

int und_packet_ctx_init()
{
    struct und_packet_ctx_t *ctx = &g_und_pkt_ctx;
    if (ctx->mutex) {
        und_debug("und pkt ctx is ready inited\n");
        return UND_SUCCESS;
    }

    ctx->mutex = undp_mutex_new();

    UND_PTR_SANITY_CHECK(ctx->mutex, UND_MEM_ERR);

    undp_mutex_lock(ctx->mutex);
    ctx->packet_id = 1;
    undp_mutex_unlock(ctx->mutex);
    return UND_SUCCESS;
}

int und_packet_ctx_deinit()
{
    struct und_packet_ctx_t *ctx = &g_und_pkt_ctx;
    void *mutex = ctx->mutex;

    UND_PTR_SANITY_CHECK(ctx->mutex, UND_ERR);

    undp_mutex_lock(mutex);
    aos_memset(ctx, 0, sizeof(*ctx));
    undp_mutex_unlock(mutex);
    undp_mutex_free(mutex);

    return UND_SUCCESS;
}

int und_build_topic(const char *fmt, char *topic_buf, int topic_buf_len)
{
    int len = 0;
    char pk[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char dn[IOTX_DEVICE_NAME_LEN + 1] = {0};

    UND_PTR_SANITY_CHECK(topic_buf, UND_PARAM_ERR);
    UND_PARAM_RANGE_SANITY_CHECK(topic_buf_len, UND_TOPIC_BUF_LEN_MAX, 1, UND_PARAM_ERR);

    undp_get_product_key(pk);
    undp_get_device_name(dn);

    len = aos_snprintf(topic_buf, topic_buf_len, fmt, pk, dn);

    return len;
}

int und_build_packet(const char *ver, const char *param, char *buf, int buf_len)
{
    int len = 0;

    UND_PTR_SANITY_CHECK(ver, UND_PARAM_ERR);
    UND_PTR_SANITY_CHECK(param, UND_PARAM_ERR);
    UND_PTR_SANITY_CHECK(buf, UND_PARAM_ERR);
    UND_PARAM_RANGE_SANITY_CHECK(buf_len, UND_REPORT_BUF_LEN_MAX, 1, UND_PARAM_ERR);

    len = aos_snprintf(buf, buf_len, UND_REPORT_FMT, und_packet_id(), UND_ALINK_VER, param);

    return len;
}

int und_build_packet_param(const char *content, char *param, int param_len)
{
    int len = 0;

    UND_PTR_SANITY_CHECK(content, UND_PARAM_ERR);
    UND_PTR_SANITY_CHECK(param, UND_PARAM_ERR);
    UND_PARAM_RANGE_SANITY_CHECK(param_len, UND_REPORT_BUF_LEN_MAX, 1, UND_PARAM_ERR);

    len = aos_snprintf(param, param_len, UND_REPORT_PARAM_FMT, content);

    return len;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif  /* end of AOS_COMP_UND */
