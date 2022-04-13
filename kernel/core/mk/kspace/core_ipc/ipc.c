/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifdef RHINO_CONFIG_IPC

#include <stdatomic.h>
#include "k_api.h"
#include "utask.h"
#include "debug_api.h"

#include "ipc.h"
#include "kipc_perf.h"
#include "kipc_utils.h"
#include "kipc_queue.h"

#ifndef RHINO_CONFIG_PROC_MSG_MAX_OWNER_NUM
    #define RHINO_CONFIG_PROC_MSG_MAX_OWNER_NUM (8)
#endif

#define IPC_OBJ_FREE             (0)
#define IPC_OBJG_MAGIC           (0x49504347)  /* value of "IPCG" */
#define IPC_OBJQ_MAGIC           (0x49504351)  /* value of "IPCQ" */
#define IPC_CONN_ELEM_MAX_SIZE   RHINO_CONFIG_PROC_MSG_MAX_OWNER_NUM

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int magic;
    ipc_queue_t *queue;
    ktask_t *owner;
    void *qobj;
    void *grp;
    int *usr_handle;       /* used for futex API */
    uint32_t drop;
} kipc_queue_t;

typedef struct {
    int access;
    kipc_queue_t *qobj;
} kipc_queue_obj_t;

typedef struct {
    int magic;
    char name[IPC_CONN_NAME_MAX_SIZE + 1];
    int refs;
    kipc_queue_obj_t queue_lst[IPC_CONN_ELEM_MAX_SIZE];
    kipc_queue_t *owner;
#if IPC_PERF_STATISTIC_ENALBE
    struct ipc_perf_stats_t perf;
#endif  /* end of IPC_PERF_STATISTIC_ENALBE */
    klist_t node;
} kipc_obj_t;

struct kipc_ctx_t {
    klist_t obj_lst;  /* object list for ipc object */
    kmutex_t obj_mtx; /* object sync mutex */
};

static struct kipc_ctx_t *ipc_ctx_ptr = NULL;

static kstat_t __aos_ipc_check(kipc_obj_t *grp)
{
    int i;
    int access = 0;
    kipc_queue_t *qobj = NULL;
    ipc_queue_t *queue = NULL;

    for (i = 0; i < IPC_CONN_ELEM_MAX_SIZE; i ++) {
        if (grp->queue_lst[i].qobj == NULL) continue;

        if (grp->queue_lst[i].qobj->owner == NULL) {
            if (__atomic_compare_exchange(&grp->queue_lst[i].access, &access,
                        &grp->queue_lst[i].access, 0, __ATOMIC_RELEASE, __ATOMIC_RELAXED)) {
                qobj = grp->queue_lst[i].qobj;
                queue = qobj->queue;
                qobj->magic = 0;
                grp->queue_lst[i].qobj = NULL;
                grp->queue_lst[i].access = 0;
                krhino_mm_free(qobj);
                if (grp->owner == qobj) {
                    grp->owner = NULL;
                }
                kipc_queue_del(queue);
                grp->refs --;
            }
        }
    }

    if (grp->refs == 0) {
        grp->magic = IPC_OBJ_FREE;
        klist_rm(&grp->node);

        IPC_PERF_DEINIT(grp);
        krhino_mm_free(grp);

        return RHINO_SUCCESS;
    }

    return -RHINO_IPC_OBJ_NOT_EXIST;
}

static kstat_t __aos_ipc_create(const char *name, int type, size_t len,
                                size_t blk_size, int *obj_handle)
{
    struct kipc_ctx_t *ctx = NULL, *tmp = NULL;
    kipc_obj_t *grp = NULL;
    klist_t *head, *iter;
    kipc_queue_t *queue;
    ktask_t *cur_task;
    kstat_t ret;

    int dst = -1;
    uint8_t i, rpt;
    kipc_obj_t *ipc_obj;

    CPSR_ALLOC();

    if ((name && aos_ipc_strlen(name) > IPC_CONN_NAME_MAX_SIZE)) {
        return -RHINO_INV_PARAM;
    }

    __atomic_load(&ipc_ctx_ptr, &ctx, __ATOMIC_RELAXED);
    if (__atomic_compare_exchange(&ipc_ctx_ptr, &tmp, &ipc_ctx_ptr,
                0, __ATOMIC_SEQ_CST, __ATOMIC_RELAXED)) {
        /* fast check ipc context is ready without os-level lock */
        RHINO_CRITICAL_ENTER();
        if (ipc_ctx_ptr == NULL) {
            /* slow path */
            ipc_ctx_ptr = (struct kipc_ctx_t *)krhino_mm_alloc(sizeof(*ipc_ctx_ptr));
            if (ipc_ctx_ptr == NULL) {
                RHINO_CRITICAL_EXIT();
                return -RHINO_NO_MEM;
            }

            ctx = ipc_ctx_ptr;
            klist_init(&ctx->obj_lst);
            krhino_mutex_create(&ctx->obj_mtx, "kipc_ctx_obj_mtx");
        }
        RHINO_CRITICAL_EXIT();
    }

    ret = krhino_mutex_lock(&ctx->obj_mtx, RHINO_WAIT_FOREVER);
    if (ret != RHINO_SUCCESS && ret != RHINO_MUTEX_OWNER_NESTED) {
        return -ret;
    }

    cur_task = krhino_cur_task_get();
    if (name && aos_ipc_strlen(name) > 0) {
        head = &ctx->obj_lst;
        iter = head->next;
        while (iter != head) {
            ipc_obj = krhino_list_entry(iter, kipc_obj_t, node);
            iter = iter->next;

            if (RHINO_SUCCESS == __aos_ipc_check(ipc_obj)) {
                continue;
            }

            /* search group existed */
            if (aos_ipc_strcmp(name, ipc_obj->name) == 0) {
                grp = ipc_obj;
                rpt = 0;
                for (i = 0; i < IPC_CONN_ELEM_MAX_SIZE; i ++) {
                    if (grp->queue_lst[i].qobj == NULL) {
                        if (dst < 0) dst = i;
                        continue;
                    }

                    if (grp->queue_lst[i].qobj->owner == cur_task) {
                        if (rpt == 0 && grp->owner != NULL &&
                            grp->owner->owner == cur_task) {
                            rpt = 1;
                            continue;
                        }
                        krhino_mutex_unlock(&ctx->obj_mtx);
                        /* warning: repeat create */
                        *obj_handle = (int)grp->queue_lst[i].qobj;
                        return -RHINO_IPC_OBJ_RPT;
                    }
                }

                if (grp->refs >= IPC_CONN_ELEM_MAX_SIZE || dst < 0) {
                    krhino_mutex_unlock(&ctx->obj_mtx);
                    return -RHINO_IPC_GRP_FULL;
                }
                break;
            }
        }
    }

    queue = (kipc_queue_t *)krhino_mm_alloc(sizeof(*queue));
    if (queue == NULL) {
        krhino_mutex_unlock(&ctx->obj_mtx);
        return -RHINO_NO_MEM;
    }
    memset(queue, 0, sizeof(*queue));

    if (kipc_queue_create(&queue->queue, type, len, blk_size) != RHINO_SUCCESS) {
        krhino_mm_free(queue);
        krhino_mutex_unlock(&ctx->obj_mtx);
        return -RHINO_NO_MEM;
    }

    if (grp == NULL) {
        grp = (kipc_obj_t *)krhino_mm_alloc(sizeof(kipc_obj_t));
        if (grp == NULL) {
            kipc_queue_del(queue->queue);
            krhino_mm_free(queue);
            krhino_mutex_unlock(&ctx->obj_mtx);
            return -RHINO_NO_MEM;
        }
        memset(grp, 0, sizeof(*grp));

        if (RHINO_SUCCESS != IPC_PERF_INIT(grp, IPC_PERF_GRP_CAP_CNT)) {
            kipc_queue_del(queue->queue);
            krhino_mm_free(queue);
            krhino_mm_free(grp);
            krhino_mutex_unlock(&ctx->obj_mtx);
            return -RHINO_NO_MEM;
        }

        grp->magic = IPC_OBJG_MAGIC;
        klist_init(&grp->node);
        klist_insert(&ctx->obj_lst, &grp->node);
        if (name && aos_ipc_strlen(name) > 0) {
            aos_ipc_strcpy(grp->name, name);
        }
        dst = 0;
    }

    queue->owner = cur_task;
    queue->grp = grp;
    queue->qobj = grp->queue_lst + dst;
    queue->drop = 0;
    queue->magic = IPC_OBJQ_MAGIC;

    grp->queue_lst[dst].access = 0;
    grp->queue_lst[dst].qobj = queue;
    grp->refs ++;

    *obj_handle = (int)queue;
    krhino_mutex_unlock(&ctx->obj_mtx);

    return RHINO_SUCCESS;
}

kstat_t aos_ipc_create_stream(const char *name, size_t size, int *obj_handle)
{
    if (size == 0 || obj_handle == NULL) {
        return -RHINO_INV_PARAM;
    }

    return __aos_ipc_create(name, KIPC_QUEUE_TYP_STREAM, size, 1, obj_handle);
}

kstat_t aos_ipc_create_msg(const char *name, size_t msg_count, int *obj_handle)
{
    if (msg_count == 0 || obj_handle == NULL) {
        return -RHINO_INV_PARAM;
    }

    return __aos_ipc_create(name, KIPC_QUEUE_TYP_MSG, msg_count,
                               sizeof(struct ipc_msg_desc_t), obj_handle);
}

kstat_t aos_ipc_create_fifo(const char *name, size_t esize, size_t ecount, int *obj_handle)
{
    if (esize == 0 || ecount == 0 || obj_handle == NULL) {
        return -RHINO_INV_PARAM;
    }

    return __aos_ipc_create(name, KIPC_QUEUE_TYP_FIFO, ecount, esize, obj_handle);
}

kstat_t aos_ipc_del(int obj_handle)
{
    kipc_queue_t *queue = (kipc_queue_t *)obj_handle;
    struct kipc_ctx_t *ctx = NULL;

    kipc_obj_t *grp;
    kstat_t ret;

    if (obj_handle == 0) {
        return -RHINO_INV_PARAM;
    }
    if (__atomic_compare_exchange(&ipc_ctx_ptr, &ctx, &ipc_ctx_ptr,
                                  0, __ATOMIC_SEQ_CST, __ATOMIC_RELAXED)) {
        return -RHINO_IPC_NOT_INIT;
    }
    __atomic_load(&ipc_ctx_ptr, &ctx, __ATOMIC_RELAXED);

    grp = (kipc_obj_t *)queue->grp;
    if (grp == NULL || grp->magic != IPC_OBJG_MAGIC ||
        queue->magic != IPC_OBJQ_MAGIC) {
        return -RHINO_IPC_NOT_INIT;
    }

    ret = krhino_mutex_lock(&ctx->obj_mtx, RHINO_WAIT_FOREVER);
    if (ret != RHINO_SUCCESS && ret != RHINO_MUTEX_OWNER_NESTED) {
        return -ret;
    }

    __atomic_store_n(&queue->owner, NULL, __ATOMIC_SEQ_CST);

    kipc_queue_disable(queue->queue, KIPC_PEND_CLR_RX | KIPC_PEND_CLR_TX);

    __aos_ipc_check(grp);

    krhino_mutex_unlock(&ctx->obj_mtx);

    return RHINO_SUCCESS;
}

kstat_t aos_ipc_getopt(int handle, int optyp, void *val)
{
    kipc_queue_t *queue = (kipc_queue_t *)handle;
    struct kipc_ctx_t *ctx = NULL;

    kipc_obj_t *grp;
    kstat_t ret;

    struct aos_ipc_opt_t *opt_val = (struct aos_ipc_opt_t *)val;

    if (handle == 0) {
        return -RHINO_INV_PARAM;
    }
    if (__atomic_compare_exchange(&ipc_ctx_ptr, &ctx, &ipc_ctx_ptr,
                                  0, __ATOMIC_SEQ_CST, __ATOMIC_RELAXED)) {
        return -RHINO_IPC_NOT_INIT;
    }
    __atomic_load(&ipc_ctx_ptr, &ctx, __ATOMIC_RELAXED);

    grp = (kipc_obj_t *)queue->grp;
    if (grp == NULL || grp->magic != IPC_OBJG_MAGIC ||
        queue->magic != IPC_OBJQ_MAGIC || queue->owner == NULL) {
        return -RHINO_IPC_NOT_INIT;
    }

    if (optyp != IPC_OPT_GETINFO) {
        return -RHINO_IPC_OPT_NOT_SUPPORT;
    }

    ret = krhino_mutex_lock(&ctx->obj_mtx, RHINO_WAIT_FOREVER);
    if (ret != RHINO_SUCCESS && ret != RHINO_MUTEX_OWNER_NESTED) {
        return -ret;
    }

    opt_val->size = queue->queue->read.tsize;
    strncpy(opt_val->name, grp->name, sizeof(grp->name) - 1);
    opt_val->name[sizeof(opt_val->name) - 1] = '\0';
    opt_val->owner = grp->owner;

    __aos_ipc_check(grp);

    krhino_mutex_unlock(&ctx->obj_mtx);

    return RHINO_SUCCESS;
}

kstat_t aos_ipc_setopt(int handle, int optyp, void *val)
{
    kipc_queue_t *queue = (kipc_queue_t *)handle;
    struct kipc_ctx_t *ctx = NULL;
    kipc_obj_t *grp;

    struct aos_ipc_opt_t *opt_val = (struct aos_ipc_opt_t *)val;
    kstat_t ret = RHINO_SUCCESS;

    if (handle == 0) {
        return -RHINO_INV_PARAM;
    }
    if (__atomic_compare_exchange(&ipc_ctx_ptr, &ctx, &ipc_ctx_ptr,
                                  0, __ATOMIC_SEQ_CST, __ATOMIC_RELAXED)) {
        return -RHINO_IPC_NOT_INIT;
    }
    __atomic_load(&ipc_ctx_ptr, &ctx, __ATOMIC_RELAXED);

    grp = (kipc_obj_t *)queue->grp;
    if (grp == NULL || grp->magic != IPC_OBJG_MAGIC ||
        queue->magic != IPC_OBJQ_MAGIC || queue->owner == NULL) {
        return -RHINO_IPC_NOT_INIT;
    }

    ret = krhino_mutex_lock(&ctx->obj_mtx, RHINO_WAIT_FOREVER);
    if (ret != RHINO_SUCCESS && ret != RHINO_MUTEX_OWNER_NESTED) {
        return -ret;
    }

    switch (optyp) {
        case IPC_OPT_SETINFO:
            __atomic_store_n(&queue->owner, opt_val->owner, __ATOMIC_SEQ_CST);
            grp->owner = queue;
            break;
        case IPC_OPT_ENABLE:
            ret = kipc_queue_enable(queue->queue, KIPC_PEND_CLR_RX | KIPC_PEND_CLR_TX);
            break;
        case IPC_OPT_DISABLE:
            ret = kipc_queue_disable(queue->queue, KIPC_PEND_CLR_RX | KIPC_PEND_CLR_TX);
            break;
        default:
            ret = -RHINO_IPC_OPT_NOT_SUPPORT;
            break;
    }

    __aos_ipc_check(grp);

    krhino_mutex_unlock(&ctx->obj_mtx);

    return ret;
}

kstat_t aos_ipc_sendto(int handle, void *data, size_t dlen, tick_t ticks)
{
    kstat_t res;
    kipc_obj_t *grp;
    kipc_queue_obj_t *qobj;
    kipc_queue_t *queue = (kipc_queue_t *)handle;

    IPC_PERF_ALLOC();
    IPC_PERF_ST();

    __builtin_prefetch((void *)handle, KIPC_PREFETCH_FOR_RD,
                       KIPC_PREFETCH_CACHE_LOCAL_LVL_0);

    if (unlikely((void *)handle == NULL) ||
        unlikely(data == NULL) ||
        unlikely(dlen == 0)) {
        return -RHINO_INV_PARAM;
    }

    grp = (kipc_obj_t *)queue->grp;
    __builtin_prefetch(queue->qobj, KIPC_PREFETCH_FOR_RD,
                       KIPC_PREFETCH_CACHE_LOCAL_LVL_0);
    qobj = (kipc_queue_obj_t *)queue->qobj;

    if (unlikely(queue->grp == NULL) ||
        unlikely(queue->magic != IPC_OBJQ_MAGIC) ||
        unlikely(grp->magic != IPC_OBJG_MAGIC)) {
        return -RHINO_IPC_NOT_INIT;
    }

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(grp, IPC_PERF_GRP_SENDTO_CHK_IDX);
    IPC_PERF_UPDATE_ET(grp, IPC_PERF_GRP_SENDTO_CHK_IDX);
    IPC_PERF_ST();

    __atomic_add_fetch(&qobj->access, 1, __ATOMIC_ACQUIRE);
#if 0
    if (unlikely(__atomic_compare_exchange(&queue->owner, &owner, &queue->owner,
                                           0, __ATOMIC_RELEASE, __ATOMIC_RELAXED))) {
        __atomic_sub_fetch(&qobj->access, 1, __ATOMIC_RELAXED);
        return -RHINO_IPC_OBJ_NOT_EXIST;
    }
#else
    if (unlikely(queue->owner == NULL)) {
        __atomic_sub_fetch(&qobj->access, 1, __ATOMIC_RELAXED);
        return -RHINO_IPC_OBJ_NOT_EXIST;
    }
#endif

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(grp, IPC_PERF_GRP_ATOMCMP_IDX);
    IPC_PERF_UPDATE_ET(grp, IPC_PERF_GRP_ATOMCMP_IDX);
    IPC_PERF_ST();

    __builtin_prefetch(&queue->queue->write, KIPC_PREFETCH_FOR_RD,
                       KIPC_PREFETCH_CACHE_LOCAL_LVL_0);
    __builtin_prefetch((char *)&queue->queue->write + IPC_CACHE_LINE_SIZE,
                       KIPC_PREFETCH_FOR_RD, KIPC_PREFETCH_CACHE_LOCAL_LVL_0);

    res = kipc_queue_send(queue->queue, data, dlen, ticks);

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(grp, IPC_PERF_GRP_SENDTO_IDX);
    IPC_PERF_UPDATE_ET(grp, IPC_PERF_GRP_SENDTO_IDX);
    IPC_PERF_ST();

    if (unlikely(res != RHINO_SUCCESS) && unlikely(queue->owner != NULL)) {
        queue->drop ++;
    }
    __atomic_sub_fetch(&qobj->access, 1, __ATOMIC_RELAXED);

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(grp, IPC_PERF_GRP_TX_FIN_IDX);
    IPC_PERF_UPDATE_ET(grp, IPC_PERF_GRP_TX_FIN_IDX);

    return res;
}

kstat_t aos_ipc_send_owner(int handle, void *data, size_t dlen, tick_t ticks)
{
    kstat_t res;
    ktask_t *cur_task;

    kipc_obj_t *grp;
    kipc_queue_obj_t *qobj = NULL;
    kipc_queue_t *queue = (kipc_queue_t *)handle;

    IPC_PERF_ALLOC();
    IPC_PERF_ST();

    __builtin_prefetch((void *)handle, KIPC_PREFETCH_FOR_RD,
                       KIPC_PREFETCH_CACHE_LOCAL_LVL_0);

    if (unlikely((void *)handle == NULL) ||
        unlikely(data == NULL) ||
        unlikely(dlen == 0)) {
        return -RHINO_INV_PARAM;
    }

    cur_task = krhino_cur_task_get();
    grp = (kipc_obj_t *)queue->grp;

    if (unlikely(queue->grp == NULL) ||
        unlikely(queue->magic != IPC_OBJQ_MAGIC) ||
        unlikely(((kipc_obj_t *)queue->grp)->magic != IPC_OBJG_MAGIC)) {
        return -RHINO_IPC_NOT_INIT;
    }

    if (unlikely(queue->owner != cur_task)) {
        return -RHINO_IPC_OBJ_NOT_EXIST;
    }
    __atomic_load(&grp->owner, &queue, __ATOMIC_RELAXED);
    if (queue == NULL) {
        return -RHINO_IPC_NO_RX;
    }
    qobj = (kipc_queue_obj_t *)queue->qobj;

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(grp, IPC_PERF_GRP_SENDS_CHK_IDX);
    IPC_PERF_UPDATE_ET(grp, IPC_PERF_GRP_SENDS_CHK_IDX);
    IPC_PERF_ST();

    __atomic_add_fetch(&qobj->access, 1, __ATOMIC_ACQUIRE);
#if 0
    if (unlikely(__atomic_compare_exchange(&queue->owner, &owner, &queue->owner,
                                           0, __ATOMIC_RELEASE, __ATOMIC_RELAXED))) {
        __atomic_sub_fetch(&qobj->access, 1, __ATOMIC_RELAXED);
        return -RHINO_IPC_OBJ_NOT_EXIST;
    }
#else
    if (unlikely(queue->owner == NULL)) {
        __atomic_sub_fetch(&qobj->access, 1, __ATOMIC_RELAXED);
        return -RHINO_IPC_OBJ_NOT_EXIST;
    }
#endif

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(grp, IPC_PERF_GRP_ATOMCMP_IDX);
    IPC_PERF_UPDATE_ET(grp, IPC_PERF_GRP_ATOMCMP_IDX);
    IPC_PERF_ST();

    __builtin_prefetch(&queue->queue->write, KIPC_PREFETCH_FOR_RD,
                       KIPC_PREFETCH_CACHE_LOCAL_LVL_0);
    __builtin_prefetch((char *)&queue->queue->write + IPC_CACHE_LINE_SIZE,
                       KIPC_PREFETCH_FOR_RD, KIPC_PREFETCH_CACHE_LOCAL_LVL_0);

    res = kipc_queue_send(queue->queue, data, dlen, ticks);

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(grp, IPC_PERF_GRP_SENDTO_IDX);
    IPC_PERF_UPDATE_ET(grp, IPC_PERF_GRP_SENDTO_IDX);
    IPC_PERF_ST();

    if (unlikely(res != RHINO_SUCCESS) && unlikely(queue->owner != NULL)) {
        queue->drop ++;
    }
    __atomic_sub_fetch(&qobj->access, 1, __ATOMIC_RELAXED);

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(grp, IPC_PERF_GRP_TX_FIN_IDX);
    IPC_PERF_UPDATE_ET(grp, IPC_PERF_GRP_TX_FIN_IDX);

    return res;
}

kstat_t aos_ipc_send(int handle, void *data, size_t dlen, tick_t ticks)
{
    int i;
    kstat_t res;
    tick_t start, end;
    ktask_t *cur_task;

    kipc_obj_t *grp;
    kipc_queue_t *queue = (kipc_queue_t *)handle, *tx_queue;

    IPC_PERF_ALLOC();
    IPC_PERF_ST();

    __builtin_prefetch((void *)handle, KIPC_PREFETCH_FOR_RD,
                       KIPC_PREFETCH_CACHE_LOCAL_LVL_0);

    if (unlikely((void *)handle == NULL) ||
        unlikely(data == NULL) ||
        unlikely(dlen == 0)) {
        return -RHINO_INV_PARAM;
    }

    cur_task = krhino_cur_task_get();
    grp = (kipc_obj_t *)queue->grp;

    if (unlikely(queue->grp == NULL) ||
        unlikely(queue->magic != IPC_OBJQ_MAGIC) ||
        unlikely(((kipc_obj_t *)queue->grp)->magic != IPC_OBJG_MAGIC)) {
        return -RHINO_IPC_NOT_INIT;
    }

    if (unlikely(queue->owner != cur_task)) {
        return -RHINO_IPC_OBJ_NOT_EXIST;
    }

    __builtin_prefetch(grp->queue_lst, KIPC_PREFETCH_FOR_RD,
                       KIPC_PREFETCH_CACHE_LOCAL_LVL_0);

    if (unlikely(grp->refs <= 1)) {
        return -RHINO_IPC_NO_RX;
    }

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(grp, IPC_PERF_GRP_SEND_CHK_IDX);
    IPC_PERF_UPDATE_ET(grp, IPC_PERF_GRP_SEND_CHK_IDX);
    IPC_PERF_ST();

    for (i = 0; i < IPC_CONN_ELEM_MAX_SIZE; i ++) {
        if (grp->queue_lst[i].qobj == NULL) continue;
        /* BCAST msg to the other queue */
        if (likely(grp->queue_lst[i].qobj != queue)) {
            tx_queue = grp->queue_lst[i].qobj;
            __builtin_prefetch(grp->queue_lst[i].qobj, KIPC_PREFETCH_FOR_RD,
                               KIPC_PREFETCH_CACHE_LOCAL_LVL_0);
            __atomic_add_fetch(&grp->queue_lst[i].access, 1, __ATOMIC_ACQUIRE);
#if 0
            if (unlikely(__atomic_compare_exchange(&tx_queue->owner, &owner, &tx_queue->owner,
                                                   0, __ATOMIC_RELEASE, __ATOMIC_RELAXED))) {
                __atomic_sub_fetch(&grp->queue_lst[i].access, 1, __ATOMIC_RELAXED);
                continue;
            }
#else
            if (unlikely(tx_queue->owner == NULL)) {
                __atomic_sub_fetch(&grp->queue_lst[i].access, 1, __ATOMIC_RELAXED);
                continue;
            }
#endif
            IPC_PERF_ET();
            IPC_PERF_UPDATE_ST(grp, IPC_PERF_GRP_ATOMCMP_IDX);
            IPC_PERF_UPDATE_ET(grp, IPC_PERF_GRP_ATOMCMP_IDX);
            IPC_PERF_ST();

            __builtin_prefetch(&tx_queue->queue->write, KIPC_PREFETCH_FOR_RD,
                               KIPC_PREFETCH_CACHE_LOCAL_LVL_0);
            __builtin_prefetch((char *)&tx_queue->queue->write + IPC_CACHE_LINE_SIZE,
                               KIPC_PREFETCH_FOR_RD, KIPC_PREFETCH_CACHE_LOCAL_LVL_0);

            if (likely(ticks > 0)) {
                start = krhino_sys_tick_get();
            }

            res = kipc_queue_send(tx_queue->queue, data, dlen, ticks);

            if (likely(ticks > 0)) {
                end = krhino_sys_tick_get();
            }
            if (likely(ticks > 0)) {
                ticks = (end - start > ticks) ? 0 : ticks - (end - start);
            }

            IPC_PERF_ET();
            IPC_PERF_UPDATE_ST(grp, IPC_PERF_GRP_SEND_IDX);
            IPC_PERF_UPDATE_ET(grp, IPC_PERF_GRP_SEND_IDX);
            IPC_PERF_ST();

            if (unlikely(tx_queue->owner != NULL) && unlikely(res != RHINO_SUCCESS)) {
                tx_queue->drop ++;
            }
            __atomic_sub_fetch(&grp->queue_lst[i].access, 1, __ATOMIC_RELAXED);

            IPC_PERF_ET();
            IPC_PERF_UPDATE_ST(grp, IPC_PERF_GRP_TX_FIN_IDX);
            IPC_PERF_UPDATE_ET(grp, IPC_PERF_GRP_TX_FIN_IDX);
            IPC_PERF_ST();  /* for next loop */
        }
    }

    return RHINO_SUCCESS;
}

kstat_t aos_ipc_recv(int handle, void *data, size_t *dlen, tick_t ticks)
{
    kstat_t res;
    kipc_obj_t *grp;
    ktask_t *cur_task;
    kipc_queue_obj_t *qobj;
    kipc_queue_t *queue = (kipc_queue_t *)handle;

    IPC_PERF_ALLOC();
    IPC_PERF_ST();

    __builtin_prefetch((void *)handle, KIPC_PREFETCH_FOR_RD,
                       KIPC_PREFETCH_CACHE_LOCAL_LVL_0);

    if (unlikely((void *)handle == NULL) ||
        unlikely(data == NULL) ||
        unlikely(dlen == NULL) ||
        unlikely(*dlen == 0)) {
        return -RHINO_INV_PARAM;
    }

    cur_task = krhino_cur_task_get();

    __builtin_prefetch(queue->qobj, KIPC_PREFETCH_FOR_RD,
                       KIPC_PREFETCH_CACHE_LOCAL_LVL_0);

    grp = (kipc_obj_t *)queue->grp;
    qobj = (kipc_queue_obj_t *)queue->qobj;

    if (unlikely(queue->grp == NULL) ||
        unlikely(queue->magic != IPC_OBJQ_MAGIC) ||
        unlikely(grp->magic != IPC_OBJG_MAGIC)) {
        return -RHINO_IPC_NOT_INIT;
    }

    if (unlikely(queue->owner != cur_task)) {
        return -RHINO_IPC_OBJ_NOT_EXIST;
    }

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(grp, IPC_PERF_GRP_RECV_CHK_IDX);
    IPC_PERF_UPDATE_ET(grp, IPC_PERF_GRP_RECV_CHK_IDX);
    IPC_PERF_ST();

    __atomic_add_fetch(&qobj->access, 1, __ATOMIC_RELEASE);

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(grp, IPC_PERF_GRP_ATOM_IDX);
    IPC_PERF_UPDATE_ET(grp, IPC_PERF_GRP_ATOM_IDX);
    IPC_PERF_ST();

    __builtin_prefetch(&queue->queue->read, KIPC_PREFETCH_FOR_RD,
                       KIPC_PREFETCH_CACHE_LOCAL_LVL_0);
    __builtin_prefetch((char *)&queue->queue->read + IPC_CACHE_LINE_SIZE,
                       KIPC_PREFETCH_FOR_RD, KIPC_PREFETCH_CACHE_LOCAL_LVL_0);

    res = kipc_queue_recv(queue->queue, data, dlen, ticks);

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(grp, IPC_PERF_GRP_RECV_IDX);
    IPC_PERF_UPDATE_ET(grp, IPC_PERF_GRP_RECV_IDX);
    IPC_PERF_ST();

    __atomic_sub_fetch(&qobj->access, 1, __ATOMIC_RELAXED);

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(grp, IPC_PERF_GRP_RX_FIN_IDX);
    IPC_PERF_UPDATE_ET(grp, IPC_PERF_GRP_RX_FIN_IDX);

    return res;
}

kstat_t aos_ipc_handle_writable(int handle)
{
    void *owner = NULL;
    kipc_obj_t *grp = NULL;
    kipc_queue_obj_t *qobj = NULL;
    kipc_queue_t *queue = (kipc_queue_t *)handle;

    if (queue == NULL) {
        return -RHINO_INV_PARAM;
    }

    grp = (kipc_obj_t *)queue->grp;
    qobj = (kipc_queue_obj_t *)queue->qobj;
    if (grp == NULL || grp->magic != IPC_OBJG_MAGIC ||
        queue->magic != IPC_OBJQ_MAGIC) {
        return -RHINO_IPC_NOT_INIT;
    }

    __atomic_add_fetch(&qobj->access, 1, __ATOMIC_RELEASE);

    if (__atomic_compare_exchange(&queue->owner, &owner, &queue->owner,
                                  0, __ATOMIC_RELEASE, __ATOMIC_RELAXED)) {
        __atomic_sub_fetch(&qobj->access, 1, __ATOMIC_RELAXED);
        return -RHINO_RINGBUF_FULL;
    }

    if (kipc_queue_full(queue->queue)) {
        __atomic_sub_fetch(&qobj->access, 1, __ATOMIC_RELAXED);
        return -RHINO_RINGBUF_FULL;
    }

    __atomic_sub_fetch(&qobj->access, 1, __ATOMIC_RELAXED);

    return RHINO_SUCCESS;
}

kstat_t aos_ipc_handle_readable(int handle)
{
    void *owner = NULL;
    kipc_obj_t *grp = NULL;
    kipc_queue_obj_t *qobj = NULL;
    kipc_queue_t *queue = (kipc_queue_t *)handle;

    if (queue == NULL) {
        return -RHINO_INV_PARAM;
    }

    grp = (kipc_obj_t *)queue->grp;
    qobj = (kipc_queue_obj_t *)queue->qobj;
    if (grp == NULL || grp->magic != IPC_OBJG_MAGIC ||
        queue->magic != IPC_OBJQ_MAGIC) {
        return -RHINO_IPC_NOT_INIT;
    }

    __atomic_add_fetch(&qobj->access, 1, __ATOMIC_RELEASE);

    if (__atomic_compare_exchange(&queue->owner, &owner, &queue->owner,
                                  0, __ATOMIC_RELEASE, __ATOMIC_RELAXED)) {
        __atomic_sub_fetch(&qobj->access, 1, __ATOMIC_RELAXED);
        return -RHINO_RINGBUF_EMPTY;
    }

    if (kipc_queue_empty(queue->queue)) {
        __atomic_sub_fetch(&qobj->access, 1, __ATOMIC_RELAXED);
        return -RHINO_RINGBUF_EMPTY;
    }
    __atomic_sub_fetch(&qobj->access, 1, __ATOMIC_RELAXED);

    return RHINO_SUCCESS;
}

void aos_ipc_exit(uint32_t pid)
{
    struct kipc_ctx_t *ctx = NULL;
    klist_t *head, *iter;

    kipc_obj_t *grp;
    kstat_t res;
    uint8_t i;

    if (pid == 0) {
        return;
    }

    if (__atomic_compare_exchange(&ipc_ctx_ptr, &ctx, &ipc_ctx_ptr,
                0, __ATOMIC_RELEASE, __ATOMIC_RELAXED)) {
        return;
    }
    __atomic_load(&ipc_ctx_ptr, &ctx, __ATOMIC_RELAXED);

    res = krhino_mutex_lock(&ctx->obj_mtx, RHINO_WAIT_FOREVER);
    if (res != RHINO_SUCCESS && res != RHINO_MUTEX_OWNER_NESTED) {
        return;
    }

    head = &ctx->obj_lst;
    iter = head->next;
    while (iter != head) {
        grp = krhino_list_entry(iter, kipc_obj_t, node);
        iter = iter->next;
        /* search group existed */
        for (i = 0; i < IPC_CONN_ELEM_MAX_SIZE; i ++) {
            if (grp->queue_lst[i].qobj == NULL) continue;
            if (grp->queue_lst[i].qobj->owner &&
                grp->queue_lst[i].qobj->owner->pid == pid) {
                __atomic_store_n(&grp->queue_lst[i].qobj->owner, NULL, __ATOMIC_SEQ_CST);
            }
        }
        __aos_ipc_check(grp);
    }

    krhino_mutex_unlock(&ctx->obj_mtx);
}

tick_t aos_ipc_cycle()
{
    return soc_hr_hw_cnt_get();
}

kstat_t aos_ipc_perf()
{
#if IPC_PERF_STATISTIC_ENALBE
    uint8_t i;
    klist_t *head, *iter;
    kipc_obj_t *grp = NULL;

    struct kipc_ctx_t *ctx = NULL;
    kstat_t res;

    if (__atomic_compare_exchange(&ipc_ctx_ptr, &ctx, &ipc_ctx_ptr,
                                  0, __ATOMIC_RELEASE, __ATOMIC_RELAXED)) {
        return -RHINO_IPC_NOT_INIT;
    }
    __atomic_load(&ipc_ctx_ptr, &ctx, __ATOMIC_RELAXED);

    /* fix confusion of performance statis print */
    res = krhino_mutex_lock(&ctx->obj_mtx, RHINO_WAIT_FOREVER);
    if (res != RHINO_SUCCESS && res != RHINO_MUTEX_OWNER_NESTED) {
        return res;
    }

    head = &ctx->obj_lst;
    iter = head->next;
    while (iter != head) {
        grp = krhino_list_entry(iter, kipc_obj_t, node);
        iter = iter->next;
        printk("grp perf stats:\r\n");
        IPC_PERF_DISP(grp);
        for (i = 0; i < IPC_CONN_ELEM_MAX_SIZE; i ++) {
            if (grp->queue_lst[i].qobj == NULL) continue;
            printk("idx:%d, queue perf stats:\r\n", i);
            IPC_PERF_DISP(grp->queue_lst[i].qobj->queue);
        }
    }

    krhino_mutex_unlock(&ctx->obj_mtx);
#endif
    return RHINO_SUCCESS;
}

#ifdef __cplusplus
}
#endif

#endif
