/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "sdk-impl_internal.h"

void IOT_OpenLog(const char *ident)
{
    const char         *mod = ident;

    if (NULL == mod) {
        mod = "---";
    }
    LITE_openlog(mod);
}

void IOT_CloseLog(void)
{
    LITE_closelog();
}

void IOT_SetLogLevel(IOT_LogLevel level)
{
    LOGLEVEL lvl = (LOGLEVEL)level;

    if ((int)lvl < LOG_EMERG_LEVEL || lvl > LOG_DEBUG_LEVEL) {
        log_err("Invalid input level: %d out of [%d, %d]", level,
                LOG_EMERG_LEVEL,
                LOG_DEBUG_LEVEL);
        return;
    }

    LITE_set_loglevel(lvl);
}

void IOT_DumpMemoryStats(IOT_LogLevel level)
{
    LOGLEVEL lvl = (LOGLEVEL)level;

    if ((int)lvl < LOG_EMERG_LEVEL || lvl > LOG_DEBUG_LEVEL) {
        lvl = LOG_DEBUG_LEVEL;
        log_warning("Invalid input level, using default: %d => %d", level, lvl);
    }

    LITE_dump_malloc_free_stats(lvl);
}

int IOT_SetupConnInfo(const char *product_key,
                      const char *device_name,
                      const char *device_secret,
                      void **info_ptr)
{
    int                 rc = -1;

    if (!info_ptr) {
        log_err("Invalid argument, info_ptr = %p", info_ptr);
        return -1;
    }

    STRING_PTR_SANITY_CHECK(product_key, -1);
    STRING_PTR_SANITY_CHECK(device_name, -1);
    STRING_PTR_SANITY_CHECK(device_secret, -1);

    iotx_device_info_init();
    iotx_device_info_set(product_key, device_name, device_secret);

    rc = iotx_guider_authenticate();
    if (rc == 0) {
        *info_ptr = (void *)iotx_conn_info_get();
        return 0;
    } else {
        *info_ptr = NULL;
        return -1;
    }

}
