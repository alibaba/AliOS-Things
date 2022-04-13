/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef KIPC_PERF_H
#define KIPC_PERF_H

#include "k_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IPC_PERF_STATISTIC_ENALBE  (0)
#define IPC_PERF_GRP_CAP_CNT       (11)
#define IPC_PERF_QUEUE_CAP_CNT     (30)

struct ipc_perf_elem_t {
    char busy;
    tick_t start;
    tick_t ctime;
    tick_t count;
};

enum {
    IPC_PERF_START = 0,
    IPC_PERF_END,
};

enum {
    IPC_PERF_GRP_SENDTO_CHK_IDX = 0,
    IPC_PERF_GRP_SEND_CHK_IDX,
    IPC_PERF_GRP_SENDS_CHK_IDX,
    IPC_PERF_GRP_RECV_CHK_IDX,
    IPC_PERF_GRP_ATOMCMP_IDX,
    IPC_PERF_GRP_ATOM_IDX,
    IPC_PERF_GRP_SENDTO_IDX,
    IPC_PERF_GRP_TX_FIN_IDX,
    IPC_PERF_GRP_RX_FIN_IDX,
    IPC_PERF_GRP_SEND_IDX,
    IPC_PERF_GRP_RECV_IDX,

    IPC_PERF_QUEUE_SEND_STEP1_IDX = 0,
    IPC_PERF_QUEUE_SEND_STEP2_IDX,
    IPC_PERF_QUEUE_SEND_STEP3_IDX,
    IPC_PERF_QUEUE_SEND_STEP4_IDX,
    IPC_PERF_QUEUE_SEND_STEP5_IDX,
    IPC_PERF_QUEUE_SEND_STEP6_IDX,
    IPC_PERF_QUEUE_SEND_STEP7_IDX,
    IPC_PERF_QUEUE_SEND_STEP8_IDX,
    IPC_PERF_QUEUE_SEND_STEP9_IDX,
    IPC_PERF_QUEUE_SEND_STEPA_IDX,
    IPC_PERF_QUEUE_SEND_STEPB_IDX,
    IPC_PERF_QUEUE_SEND_STEPC_IDX,
    IPC_PERF_QUEUE_SEND_STEPD_IDX,
    IPC_PERF_QUEUE_SEND_STEPE_IDX,
    IPC_PERF_QUEUE_SEND_STEPF_IDX,

    IPC_PERF_QUEUE_RECV_STEP1_IDX,
    IPC_PERF_QUEUE_RECV_STEP2_IDX,
    IPC_PERF_QUEUE_RECV_STEP3_IDX,
    IPC_PERF_QUEUE_RECV_STEP4_IDX,
    IPC_PERF_QUEUE_RECV_STEP5_IDX,
    IPC_PERF_QUEUE_RECV_STEP6_IDX,
    IPC_PERF_QUEUE_RECV_STEP7_IDX,
    IPC_PERF_QUEUE_RECV_STEP8_IDX,
    IPC_PERF_QUEUE_RECV_STEP9_IDX,
    IPC_PERF_QUEUE_RECV_STEPA_IDX,
    IPC_PERF_QUEUE_RECV_STEPB_IDX,
    IPC_PERF_QUEUE_RECV_STEPC_IDX,
    IPC_PERF_QUEUE_RECV_STEPD_IDX,
    IPC_PERF_QUEUE_RECV_STEPE_IDX,
    IPC_PERF_QUEUE_RECV_STEPF_IDX
};

struct ipc_perf_stats_t {
    struct ipc_perf_elem_t *plst;
    size_t cnt;
};

kstat_t ipc_perf_init(struct ipc_perf_stats_t *ctx, int cap_cnt);
kstat_t ipc_perf_deinit(struct ipc_perf_stats_t *ctx);

kstat_t ipc_perf_update(struct ipc_perf_stats_t *ctx, int idx, int type, tick_t time);
kstat_t ipc_perf_cancel(struct ipc_perf_stats_t *ctx, int idx);
kstat_t ipc_perf_disp(struct ipc_perf_stats_t *ctx);

#if IPC_PERF_STATISTIC_ENALBE
#define IPC_PERF_ALLOC()  tick_t perf_start, perf_end;
#define IPC_PERF_INIT(ctx, cap_cnt) ipc_perf_init(&ctx->perf, cap_cnt)
#define IPC_PERF_DEINIT(ctx) ipc_perf_deinit(&ctx->perf)
#define IPC_PERF_ST() perf_start = soc_hr_hw_cnt_get()
#define IPC_PERF_ET() perf_end = soc_hr_hw_cnt_get()
#define IPC_PERF_UPDATE_ST(ctx, idx) ipc_perf_update(&ctx->perf, idx, IPC_PERF_START, perf_start)
#define IPC_PERF_UPDATE_ET(ctx, idx) ipc_perf_update(&ctx->perf, idx, IPC_PERF_END, perf_end)
#define IPC_PERF_CANCEL(ctx, idx) ipc_perf_cancel(&ctx->perf, idx)
#define IPC_PERF_DISP(ctx) ipc_perf_disp(&ctx->perf)
#else
#define IPC_PERF_ALLOC()
#define IPC_PERF_INIT(ctx, cap_cnt) RHINO_SUCCESS
#define IPC_PERF_DEINIT(ctx)
#define IPC_PERF_ST()
#define IPC_PERF_ET()
#define IPC_PERF_UPDATE_ST(ctx, idx)
#define IPC_PERF_UPDATE_ET(ctx, idx)
#define IPC_PERF_CANCEL(ctx, idx)
#define IPC_PERF_DISP(ctx)
#endif


#ifdef __cplusplus
}
#endif

#endif  /* end of KIPC_PERF_H */
