/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#ifdef AOS_COMP_UND

#include "und_log.h"
#include "und_types.h"
#include "und_utils.h"
#include "und_platform.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

struct und_sched_ctx_t {
    void *mutex;
    void *sched;
};

static struct und_sched_ctx_t g_und_sched_ctx = {0};


int und_sched_start(int cycle_ms)
{
    struct und_sched_ctx_t *ctx = &g_und_sched_ctx;

    UND_PTR_SANITY_CHECK(ctx->mutex, UND_ERR);

    undp_mutex_lock(ctx->mutex);
    aos_timer_stop(ctx->sched);
    aos_timer_start(ctx->sched, cycle_ms);
    undp_mutex_unlock(ctx->mutex);
    return UND_SUCCESS;
}

int und_sched_deinit()
{
    struct und_sched_ctx_t *ctx = &g_und_sched_ctx;
    void *mutex = ctx->mutex;

    UND_PTR_SANITY_CHECK(ctx->mutex, UND_ERR);

    /* clear ctx, then release mutex */
    undp_mutex_lock(mutex);
    aos_timer_stop(ctx->sched);
    aos_timer_delete(ctx->sched);
    aos_memset(ctx, 0, sizeof(*ctx));
    undp_mutex_unlock(mutex);
    undp_mutex_free(mutex);

    return UND_SUCCESS;
}

int und_sched_init(void *sched_task)
{
    struct und_sched_ctx_t *ctx = &g_und_sched_ctx;

    if (ctx->mutex) {
        und_debug("und sched ctx is ready inited\n");
        return UND_SUCCESS;
    }

    ctx->mutex = undp_mutex_new();

    UND_PTR_SANITY_CHECK(ctx->mutex, UND_MEM_ERR);

    undp_mutex_lock(ctx->mutex);
    ctx->sched = aos_timer_create("und_sched", (void (*)(void *))sched_task, NULL);
    if (ctx->sched == NULL) {
        void *mutex = ctx->mutex;
        und_err("und alloc sched fail\n");
        aos_memset(ctx, 0, sizeof(*ctx));
        undp_mutex_unlock(mutex);
        undp_mutex_free(mutex);
        return UND_MEM_ERR;
    }
    undp_mutex_unlock(ctx->mutex);

    return UND_SUCCESS;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif  /* end of AOS_COMP_UND */
