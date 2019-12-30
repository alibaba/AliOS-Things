/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#ifdef FEATURE_UND_SUPPORT

#include "und/und.h"
#include "und_log.h"
#include "und_types.h"
#include "und_utils.h"
#include "und_sched.h"
#include "und_config.h"
#include "und_packet.h"
#include "und_report.h"
#include "und_manage.h"
#include "und_platform.h"

#ifdef AOS_COMP_DEBUG
#include "debug_api.h"
#endif

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

struct und_ctx_t {
    void *mutex;
};

static struct und_ctx_t g_und_ctx = {0};

int und_init()
{
    int res;
    void *mutex = NULL;
    struct und_ctx_t *ctx = &g_und_ctx;

    if (ctx->mutex) {
        und_debug("und is ready inited\n");
        return UND_SUCCESS;
    }
    ctx->mutex = undp_mutex_new();

    UND_PTR_SANITY_CHECK(ctx->mutex, UND_MEM_ERR);

    undp_mutex_lock(ctx->mutex);
    mutex = ctx->mutex;

    if (UND_SUCCESS != (res = und_packet_ctx_init())) {
        und_err("und pkt ctx init fail\n");
        goto UND_INIT_FAIL;
    }

    if (UND_SUCCESS != (res = und_cap_manage_init())) {
        und_err("und cap ctx init fail\n");
        goto UND_INIT_FAIL;
    }

    if (UND_SUCCESS != (res = und_sched_init(und_targets_report))) {
        und_err("und sched init fail\n");
        goto UND_INIT_FAIL;
    }

    if (UND_SUCCESS != (res = und_report_init())) {
        und_err("und report init fail\n");
        goto UND_INIT_FAIL;
    }

    undp_mutex_unlock(ctx->mutex);

#ifdef AOS_COMP_DEBUG
    und_update_statis(UND_STATIS_DEV_EXCEPTION_IDX, (int)debug_reboot_reason_get());
#else
    und_update_statis(UND_STATIS_DEV_EXCEPTION_IDX, UND_STATIS_DEV_REPOWER_REASON);
#endif

    return UND_SUCCESS;

UND_INIT_FAIL:
    und_sched_deinit();
    und_cap_manage_deinit();
    und_packet_ctx_deinit();
    und_report_deinit();
    aos_memset(ctx, 0, sizeof(*ctx));
    undp_mutex_unlock(mutex);
    undp_mutex_free(mutex);

    return res;
}

int und_deinit()
{

    struct und_ctx_t *ctx = &g_und_ctx;
    void *mutex = ctx->mutex;
    int res;

    UND_PTR_SANITY_CHECK(ctx->mutex, UND_ERR);

    undp_mutex_lock(mutex);
    res = und_sched_deinit();
    res = und_cap_manage_deinit();
    res = und_packet_ctx_deinit();
    aos_memset(ctx, 0, sizeof(*ctx));
    undp_mutex_unlock(mutex);
    undp_mutex_free(mutex);

    return res;
}

int und_update_statis(int cap_idx, int reason_code)
{
    int res;
    struct und_ctx_t *ctx = &g_und_ctx;
    if (ctx->mutex == NULL) {
        res = und_init();
        if (res != UND_SUCCESS) {
            und_err("update, und init fail\n");
            return res;
        }
    }
    undp_mutex_lock(ctx->mutex);
    cap_idx = UND_CAPTURE_IDX(cap_idx);
    res = und_update_cap(cap_idx, reason_code, 1, 1);
    if (res == UND_SUCCESS) {
        und_update_report(cap_idx);
        res = und_sched_start(UND_REPORT_SCHED_CYCLE_MS);
    }
    undp_mutex_unlock(ctx->mutex);

    return res;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif
