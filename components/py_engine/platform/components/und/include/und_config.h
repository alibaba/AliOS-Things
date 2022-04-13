/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#ifndef UND_CONFIG_H
#define UND_CONFIG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UND_VER                            "1.0"
#define UND_KV_KEY                         "und_key#%d"
#define UND_KV_KEY_LEN_MAX                 (16)
#define UND_ELEM_MAX_CNT_PER_TARGET        (8)
#define UND_REPORT_WIRELESS_INFO_CYCLE_MS  (10 * 60 * 1000)
#define UND_REPORT_SCHED_CYCLE_MS          (5000)
#define UND_SAVE_SCHED_CYCLE_MS            (5000)
#define UND_CAPTURE_IDX(idx)               ((idx) >> 4)
#define UND_CAPTURE_GROUP(idx)             ((idx) << 4)
#define UND_REPORT_BUF_LEN_MAX             (1536)
#define UND_REPORT_TARGET_BUF_LEN          (UND_REPORT_BUF_LEN_MAX - 192)
#define UND_TOPIC_BUF_LEN_MAX              (192)

#ifdef FEATURE_UND_USE_UAGENT
#define UND_USE_UAGENT_MOD                 (UAGENT_MOD_UND)
#define UND_USE_UAGENT_MOD_NAME            "uND"
#define UND_USE_UAGENT_MOD_VER             "1.0.0"
#define UND_USE_UAGENT_FUNC_VIEW           (UAGENT_FUNC_USER_BASE)
#define UND_USE_UAGENT_FUNC_VIEW_NAME      "uNDView"
#define UND_USE_UAGENT_TX_POLICY_OBJ       (send_policy_object | send_policy_save_cloud)
#define UND_USE_UAGENT_TX_QOS1             (send_policy_trans_guarantee)
#endif

enum {
    UND_CAP_STATE_INIT = 0,
    UND_CAP_STATE_UPDATE,  /* captures is updated, needs to report to cloud */
    UND_CAP_STATE_REPORT,  /* caputres is reported to cloud, reply is not received */
    UND_CAP_STATE_RESYNC,  /* caputres is reported to cloud, while captures is updated agagin */
};

struct und_cap_elem_t {
    uint32_t cnt;
    int32_t reason;
};

struct und_cap_manage_t {
    int16_t size;
    int16_t idx;
    struct und_cap_elem_t caps[UND_ELEM_MAX_CNT_PER_TARGET];
};

#ifdef __cplusplus
}
#endif

#endif  /* end of UND_CONFIG_H*/
