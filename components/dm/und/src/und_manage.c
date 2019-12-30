/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#ifdef FEATURE_UND_SUPPORT

#include "und/und.h"
#include "und_hal.h"
#include "und_log.h"
#include "und_types.h"
#include "und_utils.h"
#include "und_config.h"
#include "und_manage.h"
#include "und_report.h"
#include "und_adapter.h"
#include "und_platform.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#define UND_ARRAY_SIZE(array)  (sizeof(array) / sizeof(array[0]))

struct und_cap_manage_ctx_t {
    struct und_cap_manage_t *targets[UND_CAPTURE_IDX(UND_STATIS_CAP_IDX_MAX)];
    char wb[UND_CAPTURE_IDX(UND_STATIS_CAP_IDX_MAX)];
    void *mutex;
    void *save_sched;
};

static int und_package_capture(char *buf, int buf_len);
static void und_package_wireless_info(struct und_wireless_info_t *info);

static struct und_cap_manage_ctx_t g_und_cap_ctx = {0};

int und_cap_manage_clear_kv(void)
{
    int i;
    char key[UND_KV_KEY_LEN_MAX + 1];
    struct und_cap_manage_ctx_t *ctx = &g_und_cap_ctx;

    if (ctx->mutex == NULL) {
        und_err("save, und cap is not init\n");
        return UND_ERR;
    }

    undp_mutex_lock(ctx->mutex);
    for (i = 0; i < UND_ARRAY_SIZE(ctx->targets); i ++) {
        aos_memset(key, 0, sizeof(key));
        aos_snprintf(key, sizeof(key), UND_KV_KEY, i);

        aos_kv_del(key);
    }

    for (i = 0; i < UND_ARRAY_SIZE(ctx->targets); i ++) {
        if (ctx->targets[i])
            aos_free(ctx->targets[i]);
    }
    aos_memset(ctx->targets, 0, sizeof(ctx->targets));
    aos_memset(ctx->wb, 0, sizeof(ctx->wb));
    undp_mutex_unlock(ctx->mutex);

    return UND_SUCCESS;
}

static void und_cap_manage_save_task(void *param)
{
    int i;
    int res;
    char res_sched = 0;
    char key[UND_KV_KEY_LEN_MAX + 1];
    int len = sizeof(struct und_cap_manage_t);
    struct und_cap_manage_ctx_t *ctx = &g_und_cap_ctx;

    if (ctx->mutex == NULL) {
        und_err("save, und cap is not init\n");
        return;
    }

    undp_mutex_lock(ctx->mutex);
    for (i = 0; i < UND_ARRAY_SIZE(ctx->targets); i ++) {
        if (ctx->wb[i] == 0)
            continue;
        aos_memset(key, 0, sizeof(key));
        aos_snprintf(key, sizeof(key), UND_KV_KEY, i);
        if ((res = aos_kv_set(key, ctx->targets[i], len, 0)) != 0) {
            und_err("und cap kv set %s fail(%d)\n", key, res);
            res_sched = 1;
            continue;
        }
        ctx->wb[i] = 0;
    }

    /* writeback fails, re-schedule save task. */
    if (res_sched) {
        aos_timer_stop(ctx->save_sched);
        aos_timer_start(ctx->save_sched, UND_SAVE_SCHED_CYCLE_MS);
    }
    undp_mutex_unlock(ctx->mutex);
}

int und_cap_manage_init()
{
    int i, j;
    void *mutex = NULL;
    int res = UND_SUCCESS;
    struct und_cap_manage_ctx_t *ctx = &g_und_cap_ctx;

    if (ctx->mutex) {
        und_debug("und cap is ready inited\n");
        return UND_SUCCESS;
    }

    ctx->mutex = undp_mutex_new();
    UND_PTR_SANITY_CHECK(ctx->mutex, UND_MEM_ERR);

    undp_mutex_lock(ctx->mutex);
    aos_memset(ctx->targets, 0, sizeof(ctx->targets));

    ctx->save_sched = aos_timer_create("und_save", und_cap_manage_save_task, NULL);
    if (ctx->save_sched == NULL) {
        und_err("und save init fail\n");
        res = UND_MEM_ERR;
        goto UND_CAP_INIT_FAIL;
    }

    /* read capture from kv */
    for (i = 0; i < UND_ARRAY_SIZE(ctx->targets); i ++) {
        char key[UND_KV_KEY_LEN_MAX + 1];
        struct und_cap_manage_t target;
        int len = sizeof(target);
        aos_memset(key, 0, sizeof(key));
        aos_memset(&target, 0, sizeof(target));
        aos_snprintf(key, sizeof(key), UND_KV_KEY, i);
        aos_kv_get(key, &target, &len);
        for (j = 0; j < target.idx; j ++) {
            while (0 < target.caps[j].cnt --) {
                res = und_update_cap(i, target.caps[j].reason, 0, 0);
                if (res != UND_SUCCESS)
                    goto UND_CAP_INIT_FAIL;
            }
        }
    }

    /* it's no need to writeback capture from kv to kv again */
    aos_memset(ctx->wb, 0, sizeof(ctx->wb));
    undp_mutex_unlock(ctx->mutex);

    return UND_SUCCESS;

UND_CAP_INIT_FAIL:
    mutex = ctx->mutex;
    for (i = 0; i < UND_ARRAY_SIZE(ctx->targets); i ++) {
        if (ctx->targets[i])
            aos_free(ctx->targets[i]);
    }
    if (ctx->save_sched) {
        aos_timer_stop(ctx->save_sched);
        aos_timer_delete(ctx->save_sched);
    }
    aos_memset(ctx, 0, sizeof(*ctx));
    undp_mutex_unlock(mutex);
    undp_mutex_free(mutex);
    return res;
}

int und_cap_manage_deinit()
{
    struct und_cap_manage_ctx_t *ctx = &g_und_cap_ctx;
    void *mutex = ctx->mutex;
    int i;

    UND_PTR_SANITY_CHECK(ctx->mutex, UND_ERR);

    undp_mutex_lock(mutex);
    for (i = 0; i < UND_ARRAY_SIZE(ctx->targets); i ++) {
        if (ctx->targets[i])
            aos_free(ctx->targets[i]);
    }
    if (ctx->save_sched) {
        aos_timer_stop(ctx->save_sched);
        aos_timer_delete(ctx->save_sched);
    }
    aos_memset(ctx, 0, sizeof(*ctx));
    undp_mutex_unlock(mutex);
    undp_mutex_free(mutex);

    return UND_SUCCESS;
}

int und_update_cap(int cap_idx, int reason_code, int wb, int sync)
{
    int i;
    struct und_cap_manage_ctx_t *ctx = &g_und_cap_ctx;

    UND_PARAM_RANGE_SANITY_CHECK(cap_idx,
            UND_ARRAY_SIZE(ctx->targets) - 1, 0, UND_PARAM_ERR);
    UND_PTR_SANITY_CHECK(ctx->mutex, UND_ERR);

    if (sync) undp_mutex_lock(ctx->mutex);
    if (ctx->targets[cap_idx] == NULL) {
        /* alloc buffer for targets */
        int len = sizeof(struct und_cap_manage_t);
        ctx->targets[cap_idx] = aos_malloc(len);
        if (ctx->targets[cap_idx] == NULL) {
            und_err("alloc cap %d fail\n", cap_idx);
            if (sync) undp_mutex_unlock(ctx->mutex);
            return UND_MEM_ERR;
        }
        aos_memset(ctx->targets[cap_idx], 0, len);
        ctx->targets[cap_idx]->size = UND_ELEM_MAX_CNT_PER_TARGET;
    }

    if (ctx->targets[cap_idx]->idx >= ctx->targets[cap_idx]->size) {
        und_err("no space for target %d\n", cap_idx);
        if (sync) undp_mutex_unlock(ctx->mutex);
        return UND_ERR;
    }
    for (i = 0; i < ctx->targets[cap_idx]->idx; i ++) {
        /* search captures is exist or not */
        if (ctx->targets[cap_idx]->caps[i].reason != reason_code)
            continue;
        /* update capture */
        ctx->targets[cap_idx]->caps[i].cnt ++;
        break;
    }

    if (i >= ctx->targets[cap_idx]->idx) {
        /* current capture is not exist */
        ctx->targets[cap_idx]->caps[i].cnt ++;
        ctx->targets[cap_idx]->caps[i].reason = reason_code;
        ctx->targets[cap_idx]->idx ++;
    }

    if (wb) {
        ctx->wb[cap_idx] = 1;
    }

    if (ctx->wb[cap_idx]) {
        aos_timer_stop(ctx->save_sched);
        aos_timer_start(ctx->save_sched, UND_SAVE_SCHED_CYCLE_MS);
    }
    if (sync) undp_mutex_unlock(ctx->mutex);

    return UND_SUCCESS;
}

int und_collect_package(char *buf, int buf_len, int insert_split, int with_exception)
{
    int len = 0, l = 0;
    struct und_cap_manage_ctx_t *ctx = &g_und_cap_ctx;

    UND_PTR_SANITY_CHECK(buf, UND_PARAM_ERR);
    UND_PARAM_RANGE_SANITY_CHECK(buf_len, UND_REPORT_TARGET_BUF_LEN, 1, UND_PARAM_ERR);

    UND_PTR_SANITY_CHECK(ctx->mutex, UND_ERR);

    if (with_exception == 0) {
        return len;
    }

    undp_mutex_lock(ctx->mutex);

    if (insert_split) buf[len ++] = ',';

    l += und_package_capture(buf + len, buf_len - len);
    if (l > 0) {
        len += l;
    } else if (buf[len - 1] == ',') {
        /* remove the end of ',' */
        buf[-- len] = '\0';
    }

    undp_mutex_unlock(ctx->mutex);
    return len;
}

static void und_package_wireless_info(struct und_wireless_info_t *info)
{
    /* init the default value, in cast that some platform does not realise HAL_GetWirelessInfo */
    info->rssi = -1;
    info->snr  = 80;
    info->per  = 0;

    if (HAL_GetWirelessInfo(info) < 0) {
        und_err("HAL_GetWirelessInfo not implemented, use default value\n");
    }

    und_update_report_cycle(info->per == 0 ? 0 : 1);

    return;
}

static int und_package_capture(char *buf, int buf_len)
{
    int i, j;
    int len = 0;
    int temp_len = 0;
    int not_first = 0;
    char *cur_ex = NULL;
    unsigned int time_stamp = 0;
    struct und_wireless_info_t wireless_info;
    struct und_cap_manage_ctx_t *ctx = &g_und_cap_ctx;

    UND_PTR_SANITY_CHECK(buf, UND_PARAM_ERR);
    UND_PARAM_RANGE_SANITY_CHECK(buf_len, UND_REPORT_TARGET_BUF_LEN, 1, UND_PARAM_ERR);

    cur_ex = (char *)aos_malloc(256);
    UND_PTR_SANITY_CHECK(ctx->mutex, UND_MEM_ERR);

    for (i = 1; i < UND_ARRAY_SIZE(ctx->targets); i ++) {
        if (ctx->targets[i] == NULL || ctx->targets[i]->idx <= 0)
            continue;
        und_info("targets[%d]:%p\n", i, ctx->targets[i]);
        for (j = 0; j < ctx->targets[i]->idx; j ++) {
            und_package_wireless_info(&wireless_info);
            aos_memset(cur_ex, 0, 256);

            temp_len = aos_snprintf(cur_ex, 256,
                    "{\"wifi\":{\"rssi\":%d,\"snr\":%d,\"per\":%d,\"err_stats\":\"0x%02x,%d,%u\"},\"_time\":%lu}",
                    wireless_info.rssi, wireless_info.snr, wireless_info.per,
                    UND_CAPTURE_GROUP(i), ctx->targets[i]->caps[j].reason, ctx->targets[i]->caps[j].cnt,
                    time_stamp);

            if (len + temp_len + not_first > buf_len) {
                /* remove end of ',' */
                break;
            } else {  /* make sure buffer is enough */
                len += aos_snprintf(buf + len, buf_len - len, not_first ? ",%s" : "%s", cur_ex);
                not_first = 1;
            }
        }
        und_info("pkt:%.*s\n", len, buf);

        if (j < ctx->targets[i]->idx) break;
    }
    aos_free(cur_ex);

    return len;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif
