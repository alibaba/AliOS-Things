/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include "pwrmgmt_api.h"
#include <hal/hal.h>

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)

int wifi_enter_powersave()
{
    hal_wifi_module_t *module;
    module = hal_wifi_get_default_module();
    return hal_wifi_enter_powersave(module, WIFI_CONFIG_RECEIVE_DTIM);
}

int wifi_exit_powersave()
{
    hal_wifi_module_t *module;
    module = hal_wifi_get_default_module();
    return hal_wifi_exit_powersave(module);
}

#endif
