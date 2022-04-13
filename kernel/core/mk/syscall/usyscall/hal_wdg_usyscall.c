/*
 * Copyright (C) 2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "syscall.h"
#include "syscall_no.h"
#include "aos/hal/wdg.h"

int32_t hal_wdg_init(wdg_dev_t *wdg)
{
    return (int32_t)SYSCALL(SYS_HAL_WDG_INIT, wdg);
}

void hal_wdg_reload(wdg_dev_t *wdg)
{
    SYSCALL(SYS_HAL_WDG_RELOAD, wdg);
}

int32_t hal_wdg_finalize(wdg_dev_t *wdg)
{
    return (int32_t)SYSCALL(SYS_HAL_WDG_FINALIZE, wdg);
}
