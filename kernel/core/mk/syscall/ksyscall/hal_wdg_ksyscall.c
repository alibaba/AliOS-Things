/*
 * Copyright (C) 2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "aos/hal/wdg.h"

int32_t sys_hal_wdg_init_stub(void *wdg)
{
    return hal_wdg_init((wdg_dev_t *)wdg);
}

void sys_hal_wdg_reload_stub(void *wdg)
{
    hal_wdg_reload((wdg_dev_t *)wdg);
}

int32_t sys_hal_wdg_finalize_stub(void *wdg)
{
    return hal_wdg_finalize((wdg_dev_t *)wdg);
}
