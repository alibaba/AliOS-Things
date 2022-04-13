/*
 * Copyright (C) 2020 Alibaba Group Holding Limited
 */

#ifndef HAL_WDG_KSYSCALL_H
#define HAL_WDG_KSYSCALL_H
#include <stdint.h>

int32_t sys_hal_wdg_init_stub(void *wdg);

void sys_hal_wdg_reload_stub(void *wdg);

int32_t sys_hal_wdg_finalize_stub(void *wdg);

#endif /* HAL_WDG_KSYSCALL_H */
