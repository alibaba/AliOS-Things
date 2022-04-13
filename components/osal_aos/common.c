/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdlib.h>

#include "aos/kernel.h"
#include "k_config.h"

#define SEED_MAGIC 0x123

extern void hal_reboot(void);

void aos_reboot(void)
{
    hal_reboot();
}

int32_t aos_get_hz(void)
{
    return RHINO_CONFIG_TICKS_PER_SECOND;
}

aos_status_t aos_version_str_get(char *buf, size_t len)
{
    const char *ver_str = SYSINFO_KERNEL_VERSION;

    if (buf == NULL) {
        return -EINVAL;
    }

    if (len <= strlen(ver_str)) {
        return -EINVAL;
    }

    memcpy(buf, ver_str, strlen(ver_str));
    return 0;
}

#if !defined (ENABLE_RNG) && defined (AOS_COMP_KV)
#include "aos/kv.h"
#endif

void aos_srand(uint32_t seed)
{
#if !defined (ENABLE_RNG) && defined (AOS_COMP_KV)
    int           ret        = 0;
    int           seed_len   = 0;
    uint32_t      seed_val   = 0;
    static char  *g_seed_key = "seed_key";

    seed_len = sizeof(seed_val);
    ret = aos_kv_get(g_seed_key, &seed_val, &seed_len);
    if (ret) {
        seed_val = SEED_MAGIC;
    }
    seed_val += seed;
    srand(seed_val);

    seed_val = rand();
    aos_kv_set(g_seed_key, &seed_val, sizeof(seed_val), 1);
#else
    srand(seed);
#endif
}

int32_t aos_rand(void)
{
    return rand();
}
