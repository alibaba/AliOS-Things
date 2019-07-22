/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_LRWAN_H
#define HAL_LRWAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "system/timer.h"

/* the struct is for changing the device working mode */
typedef struct {
    void (*enter_stop_mode)(void);
    void (*exit_stop_mode)(void);
    void (*enter_sleep_mode)(void);
} hal_lrwan_dev_chg_mode_t;

typedef struct {
    uint8_t  (*get_battery_level)(void);
    void     (*get_unique_id)(uint8_t *id);
    uint32_t (*get_random_seed)(void);
} hal_lrwan_sys_t;

/* LoraWan manufactory interface*/
typedef struct {
    uint32_t (*get_mft_id)(void);
    uint32_t (*get_mft_model)(void);
    uint32_t (*get_mft_rev)(void);
    uint32_t (*get_mft_sn)(void);
    bool     (*set_mft_baud)(uint32_t baud);
    uint32_t (*get_mft_baud)(void);
} hal_manufactory_itf_t;

#ifdef __cplusplus
}
#endif

#endif /* HAL_LRWAN_H */

