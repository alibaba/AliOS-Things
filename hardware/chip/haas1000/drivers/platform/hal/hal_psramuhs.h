/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_PSRAMUHS_H__
#define __HAL_PSRAMUHS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

void hal_psramuhs_sleep(void);
void hal_psramuhs_wakeup(void);
void hal_psramuhs_init(void);
void hal_psramuhs_hold();
void hal_psramuhs_release();
void psramuhsphy_sleep(void);
void hal_psramuhs_refresh_enable();
void hal_psramuhs_refresh_disable();
#ifdef __cplusplus
}
#endif

#endif

