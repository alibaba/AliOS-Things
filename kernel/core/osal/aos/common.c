/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdint.h>
#include "aos/kv.h"
#include "k_config.h"
#include <string.h>

#define SEED_MAGIC 0x123

extern void hal_reboot(void);

const char *sysinfo_kernel_version = "AOS-R-4.0.0";

void aos_reboot(void)
{
    hal_reboot();
}

int aos_get_hz(void)
{
    return RHINO_CONFIG_TICKS_PER_SECOND;
}

const char *aos_version_get(void)
{
    return sysinfo_kernel_version;
}

int aos_sysname(char *name, size_t length)
{
    if (name == NULL || length == 0) {
        return -1;
    }

    memset(name, 0, length);
    strncpy(name, "AliOS Things", length - 1);
    return 0;
}

char *aos_version_str_get(char *buf, size_t len)
{
    const char *ver_str;
    int ver_str_len, copy_len;
    if (buf == NULL || len == 0) {
        return NULL;
    }

    ver_str = aos_version_get();
    ver_str_len = strlen(ver_str);

    copy_len = (ver_str_len > (len - 1)) ? len - 1 : ver_str_len;
    memcpy(buf, ver_str, copy_len);
    return buf;
}
