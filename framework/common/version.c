/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <aos/aos.h>

#ifndef AOS_EXPORT
#define AOS_EXPORT
#endif

#ifndef AOS_WEAK
#define AOS_WEAK __attribute__((weak))
#endif

#define TAG "AOS_VERSION"

AOS_EXPORT AOS_WEAK const char   *aos_get_product_model(void)
{
    return (const char *)SYSINFO_PRODUCT_MODEL;
}

AOS_EXPORT AOS_WEAK const char   *aos_get_device_name(void)
{
    return (const char *)SYSINFO_DEVICE_NAME;
}

#ifdef SYSINFO_OS_BINS
#define OS_MAX_VERSION_LEN 64
char  os_version[OS_MAX_VERSION_LEN];
#endif

AOS_EXPORT AOS_WEAK const char   *aos_get_kernel_version(void)
{
    return (const char *)aos_version_get();
}


AOS_EXPORT AOS_WEAK const char   *aos_get_app_version(void)
{
    return (const char *)SYSINFO_APP_VERSION;
}

AOS_EXPORT AOS_WEAK const char   *aos_get_os_version(void)
{
#ifdef SYSINFO_OS_BINS
    snprintf(os_version, OS_MAX_VERSION_LEN, "%s_%s", aos_get_kernel_version(), aos_get_app_version());
    return os_version;
#else
    return (const char *)SYSINFO_APP_VERSION;
#endif
}

static void show_version(char *pwbuf, int blen, int argc, char **argv)
{
    LOGI(TAG, "kernel version :%s\r\n", aos_get_kernel_version());
    LOGI(TAG, "app version :%s\r\n", aos_get_app_version());
}

static struct cli_command versioncmd = {
    .name = "version",
    .help = "show version",
    .function = show_version
};

void version_init(void)
{
    aos_cli_register_command(&versioncmd);
}

AOS_EXPORT AOS_WEAK void dump_sys_info(void)
{
    LOGI(TAG, "app_version: %s", aos_get_app_version());
#ifdef SYSINFO_OS_BINS
    LOGI(TAG, "kernel_version: %s", aos_get_kernel_version());
#endif
    LOGI(TAG, "product_model: %s", aos_get_product_model());
    LOGI(TAG, "device_name: %s", aos_get_device_name());
}

