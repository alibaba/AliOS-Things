/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifdef RHINO_CONFIG_IPC

#include "k_api.h"
#include "debug_api.h"

#include "kipc_perf.h"

#if IPC_PERF_STATISTIC_ENALBE

#ifdef __cplusplus
extern "C" {
#endif

kstat_t ipc_perf_init(struct ipc_perf_stats_t *ctx, int cap_cnt)
{
    if (ctx == NULL) {
        return -RHINO_INV_PARAM;
    }

    ctx->plst = (struct ipc_perf_elem_t *)krhino_mm_alloc(
                            sizeof(*ctx->plst) * cap_cnt);
    if (ctx->plst == NULL) {
        return -RHINO_NO_MEM;
    }

    memset(ctx->plst, 0, sizeof(*ctx->plst) * cap_cnt);
    ctx->cnt = cap_cnt;

    return RHINO_SUCCESS;
}

kstat_t ipc_perf_deinit(struct ipc_perf_stats_t *ctx)
{
    if (ctx == NULL) {
        return -RHINO_INV_PARAM;
    }

    if (ctx->plst != NULL) {
        krhino_mm_free(ctx->plst);
        ctx->plst = NULL;
    }
    ctx->cnt = 0;

    return RHINO_SUCCESS;
}

kstat_t ipc_perf_update(struct ipc_perf_stats_t *ctx, int idx, int type, tick_t time)
{
    kstat_t res = RHINO_SUCCESS;

    if (ctx == NULL || ctx->plst == NULL ||
        idx >= ctx->cnt) {
        return -RHINO_INV_PARAM;
    }

    if (type == IPC_PERF_START &&
        __atomic_test_and_set(&ctx->plst[idx].busy, __ATOMIC_ACQUIRE)) {
        return -RHINO_IPC_PERF_BUSY;
    }

    switch (type) {
        case IPC_PERF_START:
            ctx->plst[idx].start = time;
            break;
        case IPC_PERF_END:
            ctx->plst[idx].ctime += (tick_t)(time - ctx->plst[idx].start);
            ctx->plst[idx].count ++;
            break;
        default:
            res = -RHINO_IPC_PERF_NOT_SUPPORT;
            break;
    }
    __atomic_clear(&ctx->plst[idx].busy, __ATOMIC_RELEASE);

    return res;
}

kstat_t ipc_perf_cancel(struct ipc_perf_stats_t *ctx, int idx)
{
    if (ctx == NULL || ctx->plst == NULL ||
        idx >= ctx->cnt) {
        return -RHINO_INV_PARAM;
    }

    __atomic_clear(&ctx->plst[idx].busy, __ATOMIC_RELEASE);

    return RHINO_SUCCESS;
}

kstat_t ipc_perf_disp(struct ipc_perf_stats_t *ctx)
{
    int i;
    int tlen;
    int esze = 64, len = 0;
    char *print_buf = NULL;

    if (ctx == NULL || ctx->plst == NULL ||
        ctx->cnt == 0) {
        return -RHINO_INV_PARAM;
    }

    tlen = esze * (ctx->cnt + 2);
    print_buf = (char *)krhino_mm_alloc(tlen);
    if (print_buf != NULL) {
        memset(print_buf, 0, tlen);
        len += snprintf(print_buf + len, tlen - len, "%s",
                        "============Performance Statistics============\r\n");
        for (i = 0; i < ctx->cnt; i ++) {
            len += snprintf(print_buf + len, tlen - len, "idx:%d, ctime:%llu, cnt:%llu, mean:%llu\r\n",
                            i, ctx->plst[i].ctime, ctx->plst[i].count,
                            ctx->plst[i].count == 0 ? 0 : ctx->plst[i].ctime / ctx->plst[i].count);
        }
        len += snprintf(print_buf + len, tlen - len, "%s",
                        "==============================================\r\n");
        printk("%s\r\n", print_buf);
        krhino_mm_free(print_buf);
    }

    return RHINO_SUCCESS;
}

#ifdef __cplusplus
}
#endif

#endif /* end of PC_PERF_STATISTIC_ENALBE */

#endif
