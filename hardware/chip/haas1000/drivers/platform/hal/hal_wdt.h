/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef WDT_HAL_H
#define WDT_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

enum HAL_WDT_ID_T {
    HAL_WDT_ID_0 = 0,
    HAL_WDT_ID_NUM,
};

enum HAL_WDT_EVENT_T {
    HAL_WDT_EVENT_FIRE = 0,
};

#define HAL_WDT_YES 1
#define HAL_WDT_NO 0

typedef void (*HAL_WDT_IRQ_CALLBACK)(enum HAL_WDT_ID_T id, uint32_t status);
/* hal api */
void hal_wdt_set_irq_callback(enum HAL_WDT_ID_T id, HAL_WDT_IRQ_CALLBACK handler);

/* mandatory operations */
int hal_wdt_start(enum HAL_WDT_ID_T id);
int hal_wdt_stop(enum HAL_WDT_ID_T id);

/* optional operations */
int hal_wdt_ping(enum HAL_WDT_ID_T id);
int hal_wdt_set_timeout(enum HAL_WDT_ID_T id, unsigned int);
unsigned int hal_wdt_get_timeleft(enum HAL_WDT_ID_T id);

#ifdef __cplusplus
}
#endif

#endif /* WDT_HAL_H */
