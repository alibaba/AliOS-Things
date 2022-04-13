/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <pthread.h>
#include "k_api.h"
#include "debug_api.h"
#include "aos/kernel.h"
#include "rpc.h"
#include "ipc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RPC_MAX_HANDLE_NUM     64
#define INVALID_HANDLE         0
#define RPC_HANDLE_OFFSET      1

#define RPC_TASK_DEFAULT_PRI  (45)
#define RPC_ASYNC_IPC_BUF_LEN (4)

#define OBJ_HANDLE_GEN_INCR              (1 << 16) /* 00010000 */
#define OBJ_HANDLE_GEN_MAX               (((1 << 15) - 1) <<  16)

enum {
    RPC_STATE_INIT = 0,
    RPC_STATE_RUN,
    RPC_STATE_STOP,
    RPC_STATE_RESUME,
    RPC_STATE_CLOSE,
};

typedef void (*rpc_req_arrived)(struct aos_parcel_t *);
typedef void (*rpc_event_cb)(int, void *);

struct rpc_contxt_t {
    rpc_req_arrived arrived;
    rpc_event_cb event;
    void *rx_buf;
    size_t rblen;
    int ipc_handle;
    int srpc_handle;
    pthread_t srpc;
    uint8_t run;
};

typedef struct handle_table_entry {
    void       *ctx;      /* object        */
    int32_t     handle;   /* object handle */
} handle_table_entry_t;

static aos_hdl_t g_handle_map_lock;
static int32_t  gen_number = 0; /* gen_number++ to prevent get repeat handle */
static handle_table_entry_t g_handle_map[RPC_MAX_HANDLE_NUM] = {0};

static void rpc_handle_lock_init()
{
    static int init = 0;
    int tmp = 0;

    if (__atomic_compare_exchange_n(&init, &tmp, 1, 0,
            __ATOMIC_SEQ_CST, __ATOMIC_RELAXED)) {
        if (aos_mutex_new(&g_handle_map_lock) != 0) {
            __atomic_store_n(&init, 0, __ATOMIC_SEQ_CST);
        }
    }
}

static struct rpc_contxt_t *rpc_ctx_get(int32_t handle)
{
    handle_table_entry_t *entry;
    int16_t off_set = (int16_t)((handle & 0xffff) - RPC_HANDLE_OFFSET);
    if (off_set < 0) {
        return NULL;
    }

    if (off_set >= RPC_MAX_HANDLE_NUM) {
        return NULL;
    }

    entry = &g_handle_map[off_set];
    if (entry->handle != handle) {
        return NULL;
    }
    return entry->ctx;
}

static int32_t rpc_handle_alloc(struct rpc_contxt_t *ctx)
{
    int i;
    int32_t handle = INVALID_HANDLE;

    /* need lock here */
    aos_mutex_lock(&g_handle_map_lock, AOS_WAIT_FOREVER);
    for (i = 0; i < RPC_MAX_HANDLE_NUM; i ++) {
        if (g_handle_map[i].ctx == NULL) {
            if ((gen_number += OBJ_HANDLE_GEN_INCR) == OBJ_HANDLE_GEN_MAX) {
                gen_number = OBJ_HANDLE_GEN_INCR;
            }

            handle = gen_number + i + RPC_HANDLE_OFFSET;
            g_handle_map[i].handle = handle;
            ctx->srpc_handle = handle;
            g_handle_map[i].ctx = ctx;
            break;
        }

    }
    aos_mutex_unlock(&g_handle_map_lock);
    return handle;
}

static int32_t rpc_handle_free(int32_t handle)
{
    struct rpc_contxt_t  *ctx;
    handle_table_entry_t *entry;
    int16_t off_set = (int16_t)((handle & 0xffff)) - RPC_HANDLE_OFFSET;
    if (off_set < 0) {
        return -RHINO_INV_PARAM;
    }

    if (off_set >= RPC_MAX_HANDLE_NUM) {
        return -RHINO_INV_PARAM;
    }

    aos_mutex_lock(&g_handle_map_lock, AOS_WAIT_FOREVER);
    entry = &g_handle_map[off_set];
    if (entry->handle != handle) {
        aos_mutex_unlock(&g_handle_map_lock);
        return -RHINO_RPC_NOT_EXIST;
    }

    entry->handle = INVALID_HANDLE;
    ctx = (struct rpc_contxt_t *)entry->ctx;
    free(ctx->rx_buf);
    memset(ctx, 0, sizeof(*ctx));
    free(entry->ctx);
    entry->ctx = NULL;

    aos_mutex_unlock(&g_handle_map_lock);

    return RHINO_SUCCESS;
}

static int __rpc_regist(const char *name, size_t rx_req_buf_len, int *srpc_handle)
{
    int ret;
    struct rpc_contxt_t *ctx;

    if (name == NULL || rx_req_buf_len <= RPC_ASYNC_IPC_BUF_LEN || srpc_handle == NULL) {
        return -RHINO_INV_PARAM;
    }

    ctx = (struct rpc_contxt_t *)malloc(sizeof(*ctx));
    if (ctx == NULL) {
        return -RHINO_NO_MEM;
    }
    memset(ctx, 0, sizeof(*ctx));
    ctx->rx_buf = (void *)malloc(rx_req_buf_len + 1);
    if (ctx->rx_buf == NULL) {
        free(ctx);
        return -RHINO_NO_MEM;
    }
    ctx->rblen = rx_req_buf_len;
    memset(ctx->rx_buf, 0, rx_req_buf_len + 1);

    ret = aos_ipc_create_stream(name, rx_req_buf_len, &ctx->ipc_handle);
    if (ret == RHINO_SUCCESS) {
        *srpc_handle = rpc_handle_alloc(ctx);
        if (*srpc_handle == INVALID_HANDLE) {
            ret = -RHINO_RPC_HANDLE_TAB_FULL;
            printf("rpc_handle_alloc failed,handle table full!\r\n");
            aos_ipc_del(ctx->ipc_handle);
            free(ctx->rx_buf);
            free(ctx);
        }
    } else {
        free(ctx->rx_buf);
        free(ctx);
    }

    return ret;
}

/* register service to service manager used by RSS (Remote Service Server) */
int aos_rpc_regist(const char *name, size_t rx_req_buf_len, int *srpc_handle)
{
    int ret;
    struct rpc_contxt_t *ctx;
    struct aos_ipc_opt_t opt_val;

    rpc_handle_lock_init();
    ret = __rpc_regist(name, rx_req_buf_len, srpc_handle);
    if (ret == RHINO_SUCCESS) {
        ctx = rpc_ctx_get(*srpc_handle);
        aos_ipc_getopt(ctx->ipc_handle, IPC_OPT_GETINFO, &opt_val);
        if (opt_val.owner != NULL) {
            /* the RPC service is already exist */
            aos_rpc_close(*srpc_handle);
            return -RHINO_RPC_REPEAT;
        }
        opt_val.owner = krhino_cur_task_get();
        aos_ipc_setopt(ctx->ipc_handle, IPC_OPT_SETINFO, &opt_val);
    }

    return ret;
}

int aos_rpc_close(int srpc_handle)
{
    int ret = RHINO_SUCCESS;
    struct rpc_contxt_t *ctx;
    rpc_handle_lock_init();
    aos_mutex_lock(&g_handle_map_lock, AOS_WAIT_FOREVER);

    ctx = rpc_ctx_get(srpc_handle);

    if (ctx == NULL || ctx->run == RPC_STATE_CLOSE) {
        aos_mutex_unlock(&g_handle_map_lock);
        return -RHINO_INV_PARAM;
    }

    ctx->run = RPC_STATE_CLOSE;
    if (ctx->ipc_handle) {
        ret = aos_ipc_del(ctx->ipc_handle);
        ctx->ipc_handle = 0;
    }

    if (ctx->srpc == 0) {
        aos_mutex_unlock(&g_handle_map_lock);
        rpc_handle_free(srpc_handle);
    } else {
        aos_mutex_unlock(&g_handle_map_lock);
    }

    return ret;
}

int aos_rpc_stop(int srpc_handle)
{
    struct aos_ipc_opt_t opt_val;
    struct rpc_contxt_t *ctx;

    rpc_handle_lock_init();

    aos_mutex_lock(&g_handle_map_lock, AOS_WAIT_FOREVER);
    ctx = rpc_ctx_get(srpc_handle);
    if (ctx == 0 || ctx->rblen <= RPC_ASYNC_IPC_BUF_LEN) {
        aos_mutex_unlock(&g_handle_map_lock);
        return -RHINO_INV_PARAM;
    }
    ctx->run = RPC_STATE_STOP;
    memset(&opt_val, 0, sizeof(opt_val));
    aos_ipc_setopt(ctx->ipc_handle, IPC_OPT_DISABLE, &opt_val);
    aos_mutex_unlock(&g_handle_map_lock);

    return RHINO_SUCCESS;
}

void *srpc_routine(void *param)
{
    struct rpc_contxt_t *ctx = (struct rpc_contxt_t *)param;
    struct aos_revt_param_t revt_param;
    struct aos_ipc_opt_t opt_val;
    struct aos_parcel_t pkt;
    int res = RHINO_SUCCESS;
    size_t blen;
    int eid;

    if (ctx->run == RPC_STATE_RUN || ctx->run == RPC_STATE_RESUME) {
        opt_val.owner = krhino_cur_task_get();
        res = aos_ipc_setopt(ctx->ipc_handle, IPC_OPT_SETINFO, &opt_val);
    }

    while (ctx->run == RPC_STATE_RUN || ctx->run == RPC_STATE_RESUME) {
        blen = ctx->rblen;
        res = aos_ipc_recv(ctx->ipc_handle, ctx->rx_buf, &blen, RHINO_WAIT_FOREVER);
        if (res != RHINO_SUCCESS) {
            break;
        }

        aos_parcel_init(&pkt, ctx->rx_buf, blen);
        ctx->arrived(&pkt);
    }

    if (ctx->event) {
        revt_param.srpc_handle = ctx->srpc_handle;
        revt_param.param = &res;
        if (ctx->run == RPC_STATE_CLOSE) {
            eid = RPC_EID_CLOSE;
        } else if ((ctx->run == RPC_STATE_RUN || ctx->run == RPC_STATE_RESUME) &&
            ctx->srpc == pthread_self()) {
            eid = RPC_EID_ERR;
        } else {
            eid = RPC_EID_STOP;
        }
        ctx->event(eid, &revt_param);
    }

    if (ctx->run == RPC_STATE_CLOSE) {
        rpc_handle_free(ctx->srpc_handle);
    } else if (ctx->srpc == pthread_self()) {
        ctx->srpc = 0;
    }

    pthread_exit(NULL);
    return NULL;
}

int aos_rpc_run(int srpc_handle, void (*req_arrived)(struct aos_parcel_t *), void (*event_cb)(int, void *))
{
    int res;
    int state;
    struct aos_ipc_opt_t opt_val;
    struct rpc_contxt_t *ctx;
    pthread_attr_t a;

    rpc_handle_lock_init();
    aos_mutex_lock(&g_handle_map_lock, AOS_WAIT_FOREVER);
    ctx = rpc_ctx_get(srpc_handle);

    if (ctx == NULL || req_arrived == NULL || event_cb == NULL ||
        ctx->rblen <= RPC_ASYNC_IPC_BUF_LEN) {
        aos_mutex_unlock(&g_handle_map_lock);
        return -RHINO_INV_PARAM;
    }

    ctx->arrived = req_arrived;
    ctx->event = event_cb;

    opt_val.owner = krhino_cur_task_get();
    res = aos_ipc_setopt(ctx->ipc_handle, IPC_OPT_SETINFO, &opt_val);
    if (res != RHINO_SUCCESS) {
        aos_mutex_unlock(&g_handle_map_lock);
        return res;
    }
    aos_ipc_setopt(ctx->ipc_handle, IPC_OPT_ENABLE, &opt_val);
    state = ctx->run;
    if (ctx->run == RPC_STATE_STOP && ctx->srpc != 0) {
        ctx->run = RPC_STATE_RESUME;
    } else {
        ctx->run = RPC_STATE_RUN;
    }

    pthread_attr_init(&a);
    pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);

    res = pthread_create(&ctx->srpc, &a, srpc_routine, ctx);
    if (res != RHINO_SUCCESS) {
        aos_mutex_unlock(&g_handle_map_lock);
        ctx->run = state;
        return res;
    }

    aos_ipc_getopt(ctx->ipc_handle, IPC_OPT_GETINFO, &opt_val);
    pthread_setname_np(ctx->srpc, opt_val.name);
    pthread_setschedprio(ctx->srpc, RPC_TASK_DEFAULT_PRI);

    aos_mutex_unlock(&g_handle_map_lock);

    return res;
}

/* request service from service manager used by RSC (Remote Service Client) */
int aos_rpc_open_sync(const char *name, size_t rsp_buf_len, int *crpc_handle)
{
    struct rpc_contxt_t *ctx;
    struct aos_ipc_opt_t opt_val;

    rpc_handle_lock_init();

    int ret = __rpc_regist(name, rsp_buf_len, crpc_handle);

    if (ret == RHINO_SUCCESS) {
        ctx = rpc_ctx_get(*crpc_handle);
        aos_ipc_getopt(ctx->ipc_handle, IPC_OPT_GETINFO, &opt_val);
        if (opt_val.owner == NULL) {
            /* RPC service not exist */
            aos_rpc_close((int)ctx);
            return -RHINO_RPC_NOT_EXIST;
        }
    }

    return ret;
}

int aos_rpc_open_async(const char *name, int *crpc_handle)
{
    struct rpc_contxt_t *ctx;
    struct aos_ipc_opt_t opt_val;
    rpc_handle_lock_init();
    int ret = __rpc_regist(name, RPC_ASYNC_IPC_BUF_LEN + 1, crpc_handle);
    if (ret == RHINO_SUCCESS) {
        ctx = rpc_ctx_get(*crpc_handle);
        //ctx = (struct rpc_contxt_t *)(*crpc_handle);
        aos_ipc_getopt(ctx->ipc_handle, IPC_OPT_GETINFO, &opt_val);
        if (opt_val.owner == NULL) {
            /* RPC service not exist */
            aos_rpc_close((int)ctx);
            return -RHINO_RPC_NOT_EXIST;
        }

        ctx->rblen = RPC_ASYNC_IPC_BUF_LEN;
    }

    return ret;
}

/* aos_rpc_call or aos_rpc_call_with_params call remote service
 * reply == NULL, the operation is asyn operation
 * reply != NULL && reply.
 */
int aos_rpc_call_sync(int crpc_handle, struct aos_parcel_t *reply, int nparams, ...)
{
    struct rpc_contxt_t *ctx;
    struct aos_parcel_t *rdata = NULL;
    size_t pblen = sizeof(int) + 4;
    va_list var_args;
    size_t arg_size;
    void *pbuf, *arg;
    kstat_t res;
    int i;

    ctx = rpc_ctx_get(crpc_handle);
    if (ctx == NULL || reply == NULL || ctx->rx_buf == NULL ||
        ctx->ipc_handle == 0 || ctx->rblen <= RPC_ASYNC_IPC_BUF_LEN) {
        return -RHINO_INV_PARAM;
    }

    /* compute length of proto buffer */
    i = 0;

    va_start(var_args, nparams);
    while (i ++ < nparams) {
        arg_size = va_arg(var_args, size_t);
        pblen += arg_size + 4;

        arg = va_arg(var_args, void *);
    }
    va_end(var_args);

    /* alloc rpc_data_t and rpc_buffer */
    rdata = (struct aos_parcel_t *)malloc(sizeof(*rdata));
    if (rdata == NULL) {
        return -RHINO_NO_MEM;
    }

    pbuf = (char *)malloc(pblen);
    if (pbuf == NULL) {
        free(rdata);
        return -RHINO_NO_MEM;
    }
    memset(pbuf, 0, pblen);
    aos_parcel_init(rdata, pbuf, pblen);
    aos_parcel_put(AP_UVARINT, &ctx->ipc_handle, sizeof(ctx->ipc_handle), rdata);

    /* serialize parameter list */
    i = 0;
    va_start(var_args, nparams);
    while (i ++ < nparams) {
        arg_size = va_arg(var_args, size_t);
        arg = va_arg(var_args, void *);

        aos_parcel_put(AP_BUF, arg, arg_size, rdata);
    }
    va_end(var_args);

    res = aos_ipc_send_owner(ctx->ipc_handle, rdata->state,
                             rdata->wbytes, RHINO_WAIT_FOREVER);

    aos_parcel_params_free(rdata);

    if (res == RHINO_SUCCESS) {
        pblen = ctx->rblen;
        res = aos_ipc_recv(ctx->ipc_handle, ctx->rx_buf,
                           &pblen, RHINO_WAIT_FOREVER);
        if (res == RHINO_SUCCESS) {
            aos_parcel_init(reply, ctx->rx_buf, pblen);
        }
    }
    return res;
}

int aos_rpc_call_async(int crpc_handle, int nparams, ...)
{
    struct rpc_contxt_t *ctx;
    struct aos_parcel_t *rdata = NULL;
    size_t pblen = sizeof(int) + 4;
    int reply_handle = 0;
    va_list var_args;
    size_t arg_size;
    void *pbuf, *arg;
    kstat_t res;
    int i;

    ctx = rpc_ctx_get(crpc_handle);
    if (ctx == NULL || ctx->ipc_handle == 0 ||
        ctx->rblen > RPC_ASYNC_IPC_BUF_LEN) {
        return -RHINO_INV_PARAM;
    }

    /* compute length of proto buffer */
    i = 0;
    va_start(var_args, nparams);
    while (i ++ < nparams) {
        arg_size = va_arg(var_args, size_t);
        pblen += arg_size + 4;

        arg = va_arg(var_args, void *);
    }
    va_end(var_args);

    /* alloc rpc_data_t and rpc_buffer */
    rdata = (struct aos_parcel_t *)malloc(sizeof(*rdata));
    if (rdata == NULL) {
        return -RHINO_NO_MEM;
    }

    pbuf = (char *)malloc(pblen);
    if (pbuf == NULL) {
        free(rdata);
        return -RHINO_NO_MEM;
    }

    aos_parcel_init(rdata, pbuf, pblen);
    aos_parcel_put(AP_UVARINT, &reply_handle, sizeof(reply_handle), rdata);

    /* serialize parameter list */
    i = 0;
    va_start(var_args, nparams);
    while (i ++ < nparams) {
        arg_size = va_arg(var_args, size_t);
        arg = va_arg(var_args, void *);

        aos_parcel_put(AP_BUF, arg, arg_size, rdata);
    }
    va_end(var_args);

    res = aos_ipc_send_owner(ctx->ipc_handle, rdata->state,
                             rdata->wbytes, RHINO_WAIT_FOREVER);

    aos_parcel_params_free(rdata);

    return res;
}

/* send reponse of service to request used by RSS */
int aos_rpc_resp(int crpc_handle, int nparams, ...)
{
    struct aos_parcel_t *rsp_data = NULL;
    size_t pblen = 0;
    va_list var_args;
    size_t arg_size;
    void *pbuf, *arg;
    kstat_t res;
    int i;


    if (crpc_handle == 0 || nparams <= 0) {
        return -RHINO_INV_PARAM;
    }

    /* compute length of proto buffer */
    i = 0;
    va_start(var_args, nparams);
    while (i ++ < nparams) {
        arg_size = va_arg(var_args, size_t);
        pblen += arg_size + 4;

        arg = va_arg(var_args, void *);
    }
    va_end(var_args);

    /* alloc rpc_data_t and rpc_buffer */
    rsp_data = (struct aos_parcel_t *)malloc(sizeof(*rsp_data));
    if (rsp_data == NULL) {
        return -RHINO_NO_MEM;
    }

    pbuf = (char *)malloc(pblen);
    if (pbuf == NULL) {
        free(rsp_data);
        return -RHINO_NO_MEM;
    }

    aos_parcel_init(rsp_data, pbuf, pblen);

    /* serialize parameter list */
    i = 0;
    va_start(var_args, nparams);
    while (i ++ < nparams) {
        arg_size = va_arg(var_args, size_t);
        arg = va_arg(var_args, void *);

        aos_parcel_put(AP_BUF, arg, arg_size, rsp_data);
    }
    va_end(var_args);

    res = aos_ipc_sendto(crpc_handle, rsp_data->state,
                         rsp_data->wbytes, RHINO_WAIT_FOREVER);

    aos_parcel_params_free(rsp_data);

    return res;
}

#ifdef __cplusplus
}
#endif
