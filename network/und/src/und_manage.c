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
static int und_package_wireless_info(char *buf, int buf_len);

static struct und_cap_manage_ctx_t g_und_cap_ctx = {0};

static void und_cap_manage_save_task(void *param)
{
    int i;
    char res_sched = 0;
    char key[UND_KV_KEY_LEN_MAX + 1];
    int len = sizeof(struct und_cap_manage_t);
    struct und_cap_manage_ctx_t *ctx = &g_und_cap_ctx;

    if (ctx->mutex == NULL) {
        und_err("save, und cap is not init\n");
        return;
    }

    und_platform_mutex_lock(ctx->mutex);
    for (i = 0; i < UND_ARRAY_SIZE(ctx->targets); i ++) {
        if (ctx->wb[i] == 0)
            continue;
        und_platform_memset(key, 0, sizeof(key));
        und_platform_snprintf(key, sizeof(key), UND_KV_KEY, i);
        if (und_platform_kv_set(key, ctx->targets[i], len, 0) != 0) {
            und_err("und cap kv set fail\n");
            res_sched = 1;
            continue;
        }
        ctx->wb[i] = 0;
    }

    /* writeback fails, re-schedule save task. */
    if (res_sched) {
        und_platform_timer_stop(ctx->save_sched);
        und_platform_timer_start(ctx->save_sched, UND_SAVE_SCHED_CYCLE_MS);
    }
    und_platform_mutex_unlock(ctx->mutex);
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

    ctx->mutex = und_platform_mutex_create();
    UND_PTR_SANITY_CHECK(ctx->mutex, UND_MEM_ERR);

    und_platform_mutex_lock(ctx->mutex);
    und_platform_memset(ctx->targets, 0, sizeof(ctx->targets));

    ctx->save_sched = und_platform_timer_create("und_save", und_cap_manage_save_task, NULL);
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
        und_platform_memset(key, 0, sizeof(key));
        und_platform_memset(&target, 0, sizeof(target));
        und_platform_snprintf(key, sizeof(key), UND_KV_KEY, i);
        und_platform_kv_get(key, &target, &len);
        for (j = 0; j < target.idx; j ++) {
            while (0 < target.caps[j].cnt --) {
                res = und_update_cap(i, target.caps[j].reason, 0, 0);
                if (res != UND_SUCCESS)
                    goto UND_CAP_INIT_FAIL;
            }
        }
    }

    /* it's no need to writeback capture from kv to kv again */
    und_platform_memset(ctx->wb, 0, sizeof(ctx->wb));
    und_platform_mutex_unlock(ctx->mutex);

    return UND_SUCCESS;

UND_CAP_INIT_FAIL:
    mutex = ctx->mutex;
    for (i = 0; i < UND_ARRAY_SIZE(ctx->targets); i ++) {
        if (ctx->targets[i])
            und_platform_free(ctx->targets[i]);
    }
    if (ctx->save_sched) {
        und_platform_timer_stop(ctx->save_sched);
        und_platform_timer_delete(ctx->save_sched);
    }
    und_platform_memset(ctx, 0, sizeof(*ctx));
    und_platform_mutex_destroy(mutex);
    und_platform_mutex_unlock(mutex);
    return res;
}

int und_cap_manage_deinit()
{
    struct und_cap_manage_ctx_t *ctx = &g_und_cap_ctx;
    void *mutex = ctx->mutex;
    int i;

    UND_PTR_SANITY_CHECK(ctx->mutex, UND_ERR);

    und_platform_mutex_lock(mutex);
    for (i = 0; i < UND_ARRAY_SIZE(ctx->targets); i ++) {
        if (ctx->targets[i])
            und_platform_free(ctx->targets[i]);
    }
    if (ctx->save_sched) {
        und_platform_timer_stop(ctx->save_sched);
        und_platform_timer_delete(ctx->save_sched);
    }
    und_platform_memset(ctx, 0, sizeof(*ctx));
    und_platform_mutex_unlock(mutex);
    und_platform_mutex_destroy(mutex);

    return UND_SUCCESS;
}

int und_update_cap(int cap_idx, int reason_code, int wb, int sync)
{
    int i;
    struct und_cap_manage_ctx_t *ctx = &g_und_cap_ctx;

    UND_PARAM_RANGE_SANITY_CHECK(cap_idx,
            UND_ARRAY_SIZE(ctx->targets) - 1, 0, UND_PARAM_ERR);
    UND_PTR_SANITY_CHECK(ctx->mutex, UND_ERR);

    if (sync) und_platform_mutex_lock(ctx->mutex);
    if (ctx->targets[cap_idx] == NULL) {
        /* alloc buffer for targets */
        int len = sizeof(struct und_cap_manage_t);
        ctx->targets[cap_idx] = und_platform_malloc(len);
        if (ctx->targets[cap_idx] == NULL) {
            und_err("alloc cap %d fail\n", cap_idx);
            if (sync) und_platform_mutex_unlock(ctx->mutex);
            return UND_MEM_ERR;
        }
        und_platform_memset(ctx->targets[cap_idx], 0, len);
        ctx->targets[cap_idx]->size = UND_ELEM_MAX_CNT_PER_TARGET;
    }

    if (ctx->targets[cap_idx]->idx >= ctx->targets[cap_idx]->size) {
        und_err("no space for target %d\n", cap_idx);
        if (sync) und_platform_mutex_unlock(ctx->mutex);
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
        und_platform_timer_stop(ctx->save_sched);
        und_platform_timer_start(ctx->save_sched, UND_SAVE_SCHED_CYCLE_MS);
    }
    if (sync) und_platform_mutex_unlock(ctx->mutex);

    return UND_SUCCESS;
}

int und_collect_package(char *buf, int buf_len, int insert_split, int with_exception)
{
    int len = 0, l = 0;
    struct und_cap_manage_ctx_t *ctx = &g_und_cap_ctx;

    UND_PTR_SANITY_CHECK(buf, UND_PARAM_ERR);
    UND_PARAM_RANGE_SANITY_CHECK(buf_len, UND_REPORT_TARGET_BUF_LEN, 1, UND_PARAM_ERR);

    UND_PTR_SANITY_CHECK(ctx->mutex, UND_ERR);

    und_platform_mutex_lock(ctx->mutex);

    if (insert_split)
        buf[len ++] = ',';

    len += und_platform_snprintf(buf + len, buf_len - len, "\"");
    l += und_package_wireless_info(buf + len, buf_len - len);
    if (l <= 0) {
        buf[len - 2] = 0;  /* remove ',' */
        buf[len - 1] = 0;  /* remove \" */
        len -= 2;
        und_platform_mutex_unlock(ctx->mutex);
        return len;
    }

    len += l;

    if (with_exception) {
        len += und_platform_snprintf(buf + len, buf_len - len, " ");
        l = 0;
        l += und_package_capture(buf + len, buf_len - len);
        if (l > 0) {
            len += l;
        } else if (buf[len - 1] == ' ') {
            /* remove the end of space */
            len --;
        }
    }

    len += und_platform_snprintf(buf + len, buf_len - len, "\"");

    und_platform_mutex_unlock(ctx->mutex);
    return len;
}

static int und_package_wireless_info(char *buf, int buf_len)
{
    int len = 0, l = 0;
    char tmp_buf[32] = {0};
    struct und_wireless_info_t info = {-1, 80, 0};

    UND_PTR_SANITY_CHECK(buf, UND_PARAM_ERR);
    UND_PARAM_RANGE_SANITY_CHECK(buf_len, UND_REPORT_TARGET_BUF_LEN, 32, UND_PARAM_ERR);

    if (HAL_GetWirelessInfo(&info) < 0) {
        und_err("HAL_GetWirelessInfo is not implemented\n");
        return UND_ERR;
    }

#if 0
    if (info.rssi == 0 && info.snr == 0 && info.per == 0) {
        und_err("implmentation of HAL_GetWirelessInfo is not right\n");
        return UND_ERR;
    }
#endif

    /* check buf is enough to save wireless information or not */
    l += und_platform_snprintf(tmp_buf, sizeof(tmp_buf) - 1, "%lu %d %d %d",
            (unsigned long)und_platform_uptime_ms(), info.rssi, info.snr, info.per);
    und_update_report_cycle(info.per == 0 ? 0 : 1);
    if (buf_len < l)
        return len;

    len += und_platform_snprintf(buf + len, buf_len - len, "%s", tmp_buf);

    return len;
}

static int und_package_capture(char *buf, int buf_len)
{
    int i, j;
    int len = 0;
    char tmp_buf[32] = {0};
    struct und_cap_manage_ctx_t *ctx = &g_und_cap_ctx;

    UND_PTR_SANITY_CHECK(buf, UND_PARAM_ERR);
    UND_PARAM_RANGE_SANITY_CHECK(buf_len, UND_REPORT_TARGET_BUF_LEN, 1, UND_PARAM_ERR);

    for (i = 1; i < UND_ARRAY_SIZE(ctx->targets); i ++) {
        if (ctx->targets[i] == NULL || ctx->targets[i]->idx <= 0)
            continue;
        for (j = 0; j < ctx->targets[i]->idx; j ++) {
            int tmp_len = 0;
            und_platform_memset(tmp_buf, 0, sizeof(tmp_buf));
            tmp_len = und_platform_snprintf(tmp_buf, sizeof(tmp_buf), "0x%02x,%d,%u;", UND_CAPTURE_GROUP(i),
                    ctx->targets[i]->caps[j].reason, ctx->targets[i]->caps[j].cnt);
            if (buf_len < len + tmp_len) /* no space for package */
                break;
            len += und_platform_snprintf(buf + len, buf_len - len, "%s", tmp_buf);
        }
    }
    /* remove the end of ';' */
    if (len > 0 && buf[len - 1] == ';') len --;

    return len;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif
